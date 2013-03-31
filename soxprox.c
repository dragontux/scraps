/* soxprox.c - a small socks server 
 *
 * -0x0 - includes
 * -0x1 - typedefs, enums, structs
 * -0x2 - global vars
 * -0x3 - function defs
 * -0x4 - callbacks
 * -0x5 - main
 * -0x6 - EOF
 *
 * supports:
 * 	- tcp streams
 * 	- name lookup through socks
 *
 * 	socks v5:
 * 		- no authentication only
 *
 * TODO
 * 	- add support for tcp bind and udp
 *
 *
 * I didn't add many comments, because the wikipedia page explains
 * what's happening pretty well.
 *
 * 	https://en.wikipedia.org/wiki/SOCKS
 *  
 * */

// -0x0
#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <error.h>
#include <unistd.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <time.h>

#include <signal.h>

// -0x1
#define BUF_SIZE 0x1000
#define MAX( a, b ) (a > b? a : b)
#define _debug_printf( ... ) (verbose? printf( __VA_ARGS__ ) : 1)
#define _debug_fprintf( ... ) (logging? ( fprintf( debug_fp, __VA_ARGS__ ) && !fflush( debug_fp )) : 1)
#define pdebugf( ... ) ( _debug_printf( __VA_ARGS__ ) && _debug_fprintf( __VA_ARGS__ ))

// socks4
typedef struct socks4_client_greeting {
	uint8_t 	version;
	uint8_t 	command;
	uint16_t 	port;
	uint32_t	ipv4_addr;
	char 		userid;
} s4_cli_greet_t;

typedef struct socks4_server_reply {
	uint8_t		null;
	uint8_t		status;
	uint8_t		a_data_1[2]; // "arbitrary bytes which should be ignored"
	uint8_t		a_data_2[4];
} s4_srv_reply_t;

// socks5
typedef struct socks5_client_greeting {
	uint8_t		version;
	uint8_t		num;
	uint8_t 	authen;
} s5_cli_greet_t;

typedef struct socks5_server_reply {
	uint8_t		version;
	uint8_t		authen;
} s5_srv_reply_t;

typedef struct socks5_packet {
	uint8_t		version;
	uint8_t		command;
	uint8_t		null;
	uint8_t		addr_type;
	uint8_t		ipaddr;
} s5_packet_t;

// general
typedef enum {
	COM_TCP_STREAM = 0x1,
	COM_TCP_BIND   = 0x2,
	COM_UDP_STREAM = 0x3
} cli_command_t;

typedef enum {
	ADDR_IPV4	= 0x1,
	ADDR_DOMAIN	= 0x3,
	ADDR_IPV6	= 0x4
} socks_addr_t;

typedef enum {
	SRV_RQST_GRANTED	= 0x5a,
	SRV_RQST_NOPE		= 0x5b,
	SRV_RQST_NEED_INETD	= 0x5c,
	SRV_RQST_USERID_INETD	= 0x5d,

	SRV_5_GRANTED		= 0x0,
	SRV_5_NOPE		= 0x1,
	SRV_5_NOTALLOWED	= 0x2,
	SRV_5_N_UNRCH		= 0x3,
	SRV_5_H_UNRCH		= 0x4,
	SRV_5_DEST_REF		= 0x5,
	SRV_5_TTL_EXP		= 0x6,
	SRV_5_BADCOMMAND	= 0x7,
	SRV_5_BADADDRT		= 0x8
} srv_status_t;

typedef enum {
	AUTH_NONE 	= 0x0,
	AUTH_GSSAP	= 0x1,
	AUTH_USERPASS	= 0x2,
	AUTH_IANA	= 0x3,
	AUTH_PRIVATE	= 0x80
} auth_t;

// -0x2
unsigned long thresh = 0;
int verbose = 0;
int show_bandwidth = 0;
int logging = 0;
FILE *debug_fp = NULL;

// -0x3
void start_serv( char *port );
void handle_connection( int s );
void handle_v4_connection( int s, s4_cli_greet_t *client );
void handle_v5_connection( int s, s5_cli_greet_t *client );
void client_loop( int reader, int writer );
int end_child( int i );

// -0x4
// bind to port, listen for clients, and fork each connection to it's own process.
void start_serv( char *port ) {
	struct addrinfo hints, *servinfo;
	struct sockaddr_in c_addr;
	socklen_t addr_size;
	int s, temp, t = 1;
	pid_t c;

	memset( &hints, 0, sizeof( hints ));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	signal( SIGCHLD, (__sighandler_t)end_child );

	temp = verbose;
	verbose = 1;
	pdebugf( "Starting server... " );

	if 	(( getaddrinfo( NULL, port, &hints, &servinfo ) && 
			pdebugf( "getaddrinfo() failed, could not look up address.\n" )) ||
		
		((( s = socket( servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol )) < 0 ) && 
			pdebugf( "socket() failed.\n" )) ||

		(( setsockopt( s, SOL_SOCKET, SO_REUSEADDR, &t, sizeof( int )) == -1 ) &&
			pdebugf( "setsockopt() failed\n" )) ||

		(( bind( s, servinfo->ai_addr, servinfo->ai_addrlen ) < 0 ) &&
			pdebugf( "bind() on %s failed. (Is there a server already running?)\n", port ))) {

			exit( 1 );
	}


	listen( s, 10 );
	pdebugf( "done, listening\n" );

	fflush( stdout );
	if ( debug_fp )
		fflush( debug_fp );

	verbose = temp;

	while( 1 ){
		temp = accept( s, (struct sockaddr *)&c_addr, &addr_size );

		c = fork( );
		if ( c == 0 ){
			pdebugf( "[ ] Got client: %s ", inet_ntoa( c_addr.sin_addr ));
			handle_connection( temp );
		} else {
			close( temp );
		}
	}
	
	return;
}

// wrapper to pass the greeting to the proper function
void handle_connection( int s ){
	s4_cli_greet_t *client = malloc( BUF_SIZE );

	recv( s, client, BUF_SIZE, 0 );

	if ( client->version == 4 ){
		pdebugf( "(v4) " );
		handle_v4_connection( s, client );
	} else if ( client->version == 5 ){ 
		pdebugf( "(v5) " );
		handle_v5_connection( s, (s5_cli_greet_t *)client );
	} else {
		pdebugf( "- weird version number \"%u\", dropping...\n", client->version );
	}

	exit( 0 );
	return;
}

// Do stuff for version 4 connection
void handle_v4_connection( int s, s4_cli_greet_t *client ){
	s4_srv_reply_t *reply = malloc( sizeof( s4_srv_reply_t ));
	struct addrinfo hints, *servinfo;
	uint8_t *ip_buf;
	char *ip_str;
	char port_str[32];
	int remote;

	ip_buf = (void *)&client->ipv4_addr;

	snprintf( port_str, 32, "%u", ntohs( client->port ));

	if ( !ip_buf[0] && !ip_buf[1] && !ip_buf[2] ){
		int i;
		char *dns = &client->userid;
		for ( i = 0; dns[i] != 0; i++ );
		dns = dns + i + 1;
		ip_str = malloc( strlen( dns ));
		strcpy( ip_str, dns );
	} else {
		ip_str = malloc( 32 );
		snprintf( ip_str, 32, "%u.%u.%u.%u", ip_buf[0], ip_buf[1], ip_buf[2], ip_buf[3] );
	}

	pdebugf( "-> %s:%s... ", ip_str, port_str );

	memset( &hints, 0, sizeof( hints ));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( getaddrinfo( ip_str, port_str, &hints, &servinfo )){
		pdebugf( " handle_connection getaddrinfo() failed\n" );
		exit( 1 );
	}

	free( ip_str );

	remote = socket( servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol );
	if ( connect( remote, servinfo->ai_addr, servinfo->ai_addrlen ) < 0 ){
		pdebugf( "handle_connection connect() failed\n" );
		reply->status = SRV_RQST_NOPE;
	} else {
		reply->status = SRV_RQST_GRANTED;
		pdebugf( "connected\n" );
	}

	reply->null = 0;
	memset( reply->a_data_1, 0, 2 );
	memset( reply->a_data_2, 0, 4 );

	send( s, reply, sizeof( s4_srv_reply_t ), 0 );

	free( client );
	free( reply );

	if ( reply->status == SRV_RQST_NOPE )
		exit( 1 );

	client_loop( s, remote );

	exit( 0 );
	return;
}

// Do stuff for version 5 connection
void handle_v5_connection( int s, s5_cli_greet_t *client ){
	s5_srv_reply_t *reply = malloc( sizeof( s5_srv_reply_t ));
	s5_packet_t *s_buf;
	struct addrinfo hints, *servinfo;

	uint8_t *ip_buf;
	uint8_t name_size;
	uint16_t port;
	int remote, status = SRV_5_GRANTED,
		buf_size, i;
	char *ip_str = NULL;
	char port_str[32];

	reply->version = 5;
	reply->authen = 0xff;
	for ( i = 0; i < client->num; i++ ){
		if (*(uint8_t *)(&client->authen + i) == 0 ){
			reply->authen = 0;
			break;
		}
	}

	send( s, reply, sizeof( s5_srv_reply_t ), 0);

	if ( reply->authen == 0xff ){
		pdebugf( "can't authenticate, closing\n" );
		free( reply );
		close( s );
		exit( 0 );
	}

	pdebugf( "(auth: %u) ", *(uint8_t *)(&client->authen + i));

	s_buf = malloc( BUF_SIZE );
	buf_size = recv( s, s_buf, BUF_SIZE, 0 );

	if ( s_buf->version != 5 ){
		status = SRV_5_NOPE;
		goto send_reply;
	}

	if ( s_buf->command != COM_TCP_STREAM ){
		status = SRV_5_BADCOMMAND;
		goto send_reply;
	}

	if ( s_buf->addr_type == ADDR_IPV4 ){
		name_size = 4;
		ip_str = malloc( 32 );
		ip_buf = &s_buf->ipaddr;

		snprintf( ip_str, 32, "%u.%u.%u.%u", ip_buf[0], ip_buf[1], ip_buf[2], ip_buf[3] );
	} else if ( s_buf->addr_type == ADDR_DOMAIN ){
		name_size = s_buf->ipaddr;
		ip_str = malloc( name_size + 16 );
		strncpy( ip_str, (char *)&s_buf->ipaddr + 1, name_size );
		name_size++;
	} else {
		status = SRV_5_BADADDRT;
		goto send_reply;
	}

	port = *((uint16_t *)(&s_buf->ipaddr + name_size));
	snprintf( port_str, 32, "%u", ntohs( port ));
	pdebugf( "-> %s:%s... ", ip_str, port_str );

	memset( &hints, 0, sizeof( hints ));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( getaddrinfo( ip_str, port_str, &hints, &servinfo )){
		pdebugf( "[-] handle_connection getaddrinfo() failed\n" );
		status = SRV_5_NOPE;
		goto send_reply;
	}

	remote = socket( servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol );
	if ( connect( remote, servinfo->ai_addr, servinfo->ai_addrlen ) < 0 ){
		pdebugf( "handle_connection connect() failed\n" );
		status = SRV_5_NOPE;
	} else {
		status = SRV_5_GRANTED;
		pdebugf( "connected\n" );
	}

send_reply: 
	s_buf->command = status;
	send( s, s_buf, buf_size, 0 );
	if ( ip_str )
		free( ip_str );

	free( s_buf );

	if ( status != SRV_5_GRANTED )
		exit( 0 );

	client_loop( s, remote );

	exit( 0 );
	return;
}

// main part of program. simple enough, read bytes from one host, send them to the other. 
void client_loop( int client, int remote ){
	unsigned long 	data_sent = 0,
 			adjust = 0,
			rate = 0;

	char *buf = malloc( BUF_SIZE );
	int 	len_sent, len_read, connected = 1, 
		s = 0, r = 0, maxfd;
	fd_set master, fds;
	time_t 	start = time( NULL ),
		running = 0;

	FD_ZERO( &master );
	FD_SET( client, &master );
	FD_SET( remote, &master );

	maxfd = MAX( client, remote ) + 1;

	while ( connected ){
		fds = master;

		select( maxfd, &fds, NULL, NULL, NULL );
		if ( FD_ISSET( client, &fds )){
			s = client;
			r = remote;
		} else if ( FD_ISSET( remote, &fds )){
			s = remote;
			r = client;
		}

		len_read = recv( s, buf, BUF_SIZE, 0 );
		len_sent = send( r, buf, len_read, 0 );

		data_sent += len_sent;
		running = time( NULL ) - start;

		if ( thresh ){
			if ( adjust )
				usleep( adjust );

			if ( running )
				rate = data_sent / running;

			if ( rate ){
				if ( show_bandwidth )
					printf( "\tcurrent rate: %lukbps, thresh: %lukb, %lukb sent, %lu sleeping\n", 
						rate / 1024, thresh / 1024, data_sent / 1024, adjust );

				if ( rate > thresh )
					adjust += 1000;
				else if ( rate < thresh && adjust )
					adjust -= 1000;
			}
		}

		if ( !len_read || !len_sent ){
			close( client );
			close( remote );
			free( buf );
			exit( 0 );
		} 
	}

	return;
}

// kill zombies
int end_child( int signo ){
	int status;

	wait( &status );
	return 0;
}

// -0x5
int main( int argc, char *argv[] ){
	char *port = "1080";
	char *filename;
	char ch;
	int i = 0;

	while (( ch = getopt( argc, argv, "p:t:l:hvqb" )) != -1 && i++ < argc ){
		switch( ch ){
			case 't':
				thresh = (unsigned long)atoi( argv[++i] ) * 1024;
				break;

			case 'p':
				port = argv[++i];
				break;

			case 'l':
				if (( debug_fp = fopen( argv[++i], "a" )) == NULL ){
					printf( "Could not open \"%s\".", argv[i] );
					return 1;
				}
				logging = 1;
				break;

			case 'h':
				printf( 
					"usage: soxprox	[-p port] [-t threshold] [-l file] [-hvqb]\n" 
					"	-p:	specify port to listen on (default 1080)\n"
					"	-t:	specify bandwidth threshold in KB (default 0 (off))\n"
					"	-l:	specify log file (disables verbose output, if enabled)\n"
					"	-h:	show this help and exit\n"
					"	-v:	show verbose info\n"
					"	-q:	be quiet (default)\n"
					"	-b:	show live bandwidth info\n"
				);
				exit( 0 );
				break;

			case 'v':
				verbose = 1;
				break;
			case 'q':
				verbose = 0;
				break;
			case 'b':
				show_bandwidth = 1;
				break;

			default:
				exit( 1 );
		}
	}

	start_serv( port );

	return 0;
}

// -0x6
