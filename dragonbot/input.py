import irc
import threading

class in_thread( threading.Thread ):
	def __init__( self, server, p_man ):
		threading.Thread.__init__(self)
		self.server = server;
		self.p_man  = p_man

	def run( self ):
		server = self.server
		while True:
			response = server.recv()
			lines	= response.split("\n");
			if len( response ) < 4:
				exit( 0 )
			else:
				pass

				
			for line in lines:
				if "PING :" in line[0:7]:
					server.send( line.replace( "I", "O", 3 ))
					#print( "-----[ Sent pong ]-----" )

				parsed = irc.parse_privmsg( line )
				if parsed:
					if parsed["action"] == "PRIVMSG":
						if parsed["message"][:len(self.p_man.prefix)] == self.p_man.prefix:
							self.p_man.exec_cmd( parsed )

					self.p_man.exec_action( parsed )
					"""
					elif parsed["action"] == "JOIN":
						self.p_man.join_cmd( parsed )
					"""
				"""
				elif "PRIVMSG" in line:
					parsed = irc.parse_privmsg( line );
				
					if parsed:
						if parsed["channel"][0] == "#":
							reply_to = parsed["channel"]
						else:
							reply_to = parsed["nick"]

						if parsed["message"][:len(self.p_man.prefix)] == self.p_man.prefix:
							self.p_man.exec_cmd( parsed )
				elif "JOIN" in line:
					parsed = irc.parse_join( line )

					if parsed:
						self.p_man.join_cmd( parsed )
				"""
