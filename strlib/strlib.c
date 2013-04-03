#ifndef _strlib_c
#define _strlib_c

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "strlib.h"

#define new( n ) (memset(malloc(sizeof(n)),0,sizeof(n)))
#define RAY_BUF 16

// strings 
string_t *newstr( ){
	string_t *thing = new( string_t );

	thing->str = NULL;
	thing->size = 0;
	thing->alloced = 0;
	thing->encoding = E_UTF8;

	return thing;
}

string_t *mkstr( char *str ){
	string_t *thing = new( string_t );

	thing->str = str;
	thing->size = strlen( str );
	thing->alloced = 0;
	thing->encoding = E_UTF8;

	return thing;
}

string_t *cpcstr( char *str ){
	string_t *thing = new( string_t );

	thing->size = strlen( str );
	thing->str = new( char[thing->size + 1] );
	thing->alloced = 1;
	thing->encoding = E_UTF8;

	strncpy( thing->str, str, thing->size );

	return thing;
}

string_t *cpstr( string_t *str ){
	string_t *thing = new( string_t );

	thing->size = str->size;
	thing->str = new( char[thing->size] );
	thing->alloced = 1;
	thing->encoding = str->encoding;

	strncpy( thing->str, str->str, thing->size );

	return thing;
}

string_t *setstr( string_t *str, char *set ){
	if ( str == NULL )
		return mkstr( set );

	if ( str->str && str->alloced ){
		free( str->str );
		str->alloced = 0;
	}

	str->str = set;
	str->size = strlen( set );

	return str;
}

string_t *joinstr( string_t *str1, string_t *str2 ){
	string_t *thing = newstr( );
	int i, j;

	thing->size = str1->size + str2->size;
	thing->encoding = (str1->encoding > str2->encoding)? str1->encoding : str2->encoding;
	thing->alloced = 1;
	thing->str = new( char[ thing->size ]);

	for ( i = 0; i < str1->size; i++ )
		thing->str[i] = str1->str[i];

	for ( j = 0; j < str2->size; j++, i++ )
		thing->str[i] = str2->str[j];

	thing->str[i] = 0;

	return thing;
}

char *rawstr( string_t *str ){
	return str->str;
}

void freestr( string_t **str ){
	if ( *str == NULL )
		return;

	if ((*str)->alloced )
		free( (*str)->str );

	free( *str );
	*str = NULL;
}

// arrays
array_t *newarray( ){
	array_t *ray = new( array_t );

	ray->ray = NULL;
	ray->size = 0;

	return ray;
}

array_t *pushray( array_t *ray, void *thing ){
	if ( ray->ray == NULL || ray->size % RAY_BUF == 0 ){
		ray->ray = realloc( ray->ray, sizeof(void *) * ( ray->size + RAY_BUF ));
	}

	ray->ray[ ray->size ] = thing;

	ray->size++;

	return ray;
}

void *popray( array_t *ray ){
	if ( ray->size && ray->size % RAY_BUF == 0 ){
		ray->ray = realloc( ray->ray, sizeof(void *) * ( ray->size + RAY_BUF ));
	}

	if ( ray->size )
		ray->size--;

	return ray->ray[ ray->size ];
}

array_t *splitstr( string_t *str, char tok ){
	array_t *ray = newarray( );
	string_t *thing = cpstr( str );
	char *buf = thing->str;
	int i;

	for ( i = 0; i < thing->size; i++ )
		if ( buf[i] == tok )
			buf[i] = 0;

	pushray( ray, mkstr( buf ));
	for ( i = 0; i < thing->size; i++ ){
		if ( buf[i] == 0 )
			pushray( ray, mkstr( buf + i + 1 ));
	}

	return ray;
}

unsigned int lenray ( array_t *ray ){
	return ray->size;
}

void freeray( array_t **ray ){
	if ( !*ray )
		return;
	while ((*ray)->size )
		popray( *ray );

	free((*ray)->ray );
	free( *ray );
	*ray = NULL;
}

void dumpray( array_t *ray ){
	int i;
	printf( "array = [ " );
	for ( i = 0; i < ray->size; i++ ){
		printf( "0x%x, ", ray->ray[i] );
	}
	printf( "]\n" );
}

// dictionaries
dict_t *newdict( ){
	dict_t *dict 	= new( dict_t );
	dict->key	= newarray( );
	dict->index	= newarray( );
	dict->size 	= 0;
}

dict_t *adddict( dict_t *dict, void *key, void *hash ){
	pushray( dict->key, key );
	pushray( dict->index, hash ); 
	
	dict->size++;
}

dict_t *addraydict( dict_t *dict, array_t *key, array_t *hash ){
	int i;
	for ( i = 0; i < key->size && i < hash->size; i++ )
		adddict( dict, key->ray[i], hash->ray[i] );

	return dict;
}

void *searchdict( dict_t *dict, void *key ){
	int i;
	for ( i = 0; i < dict->size; i++ ){
		if ( key == dict->key->ray[i] )
			return dict->index->ray[i];
	}

	return NULL;
}

void freedict( dict_t **dict ){
	if ( !*dict )
		return;
	freeray( &(*dict)->key );
	freeray( &(*dict)->index );

	free( *dict );
	*dict = NULL;
}

// linked lists
llist_t *newllist( ){
	llist_t *list = new( llist_t );
	
	list->root = new( _link_node_t );
	list->root->prev = NULL;
	list->root->next = NULL;
	list->root->value = NULL;
	list->size = 1;

	return list;
}

llist_t *addnode( llist_t *list, void *thing ){
	_link_node_t *end = list->root;
	while ( end->next )
		end = end->next;

	end->next = new( _link_node_t );
	end->value = thing;
	list->size++;

	return list;
}

llist_t *addrayllist( llist_t *list, array_t *ray ){
	int i;
	for ( i = 0; i < ray->size; i++ )
		addnode( list, ray->ray[i] );

	return list;
}

void *listindex( llist_t *list, unsigned int index ){
	unsigned int i;
	_link_node_t *move = list->root;
	for ( i = 0; i < index && i < list->size; i++ )
		move = move->next;

	return move->value;
}

void _free_llist_tree( _link_node_t *node ){
	if ( node->prev )
		_free_llist_tree( node->prev );
	if ( node->next )
		_free_llist_tree( node->next );

	free( node );
}

void freellist( llist_t **list ){
	if ( !*list )
		return;
	_free_llist_tree((*list)->root );

	free( *list );
	*list = NULL;
}










#endif
