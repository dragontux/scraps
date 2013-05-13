import socket

def parse_privmsg( message ):
	spliced = message.split()
	try:
		channel = spliced[2]
		action  = spliced[1]
		host = spliced[0][1:]
		nick = message[message.index(":")+1 : message.index("!")]
		if message.count( ":" ) > 1:
			message  = message[message.index(":")+2:]
			message  = message[message.index(":")+1:]
		else:
			message = ""

		return { "nick":nick, "host":host, "action":action, "channel":channel, "message":message }
	except ValueError:
		return False

	except IndexError:
		return False

class irc_server( ):
	def __init__( self, server ):
		self.sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM );
		self.sock.connect(( socket.gethostbyname( server ), 6667 ))

	def send( self, message ):
		self.sock.send(bytes( message, "UTF-8" ))

	def recv( self ):
		ret = 0
		while ret == 0:
			try:
				ret = self.sock.recv( 1024 ).decode()
			except UnicodeDecodeError:
				pass

		return ret

	def send_message( self, channel, message ):
		self.send( "PRIVMSG %s :%s\r\n" % ( channel, message ))

	def identify( self, nick ):
		self.send( "USER %s %s %s :%s\r\n" % ( nick, nick, nick, nick ))
		self.send( "NICK %s\r\n" % ( nick ))

	def nick( self, nick ):
		self.send( "NICK %s\r\n" % ( nick ))

	def join( self, channels ):
		for thing in channels:
			self.send( "JOIN %s\r\n" % ( thing ))

	def part( self, channels ):
		for thing in channels:
			self.send( "PART %s\r\n" % ( thing ))

	def quit( self, message ):
		self.send( "QUIT :" + message + "\r\n" )
		self.sock.close()
		exit(0)
