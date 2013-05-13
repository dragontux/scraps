import string

class plugin:
	handle    = "bot"
	method 	  = "args"
	do_init   = False
	cron_time = False
	join_hook = False
	help_str  = "Usage: " + handle + " [action] [...]. Manage bot actions."

	def init( self ):
		print( "[+] Bot plugin loaded" )

	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else: 			reply_to = nick
		msg = False

		nicks = pman.get_trusted();

		high_priv = [ "join", "part", "quit", "say", "nick", "prefix" ]
		med_priv  = [ ]
		low_priv  = [ ]

		if len( args ) > 1:
			if args[1] in high_priv:
				if nick in nicks:
					if args[1] == "join":
						if len( args ) > 2:
							server.join( args[2:] )
							msg = "Joined channels " + str( args[2:] )
						else:
							msg = "Usage: " + self.handle + " join [channels] [...]"
					elif args[1] == "part":
						if len( args ) > 2:
							server.part( args[2:] )
							msg = "parted channels " + str( args[2:] )
						else:
							msg = "Usage: " + self.handle + " part [channels] [...]"
					elif args[1] == "quit":
						if len( args ) > 2:
							server.quit( " ".join( args[2:] ))
						else:
							msg = "Usage: " + self.handle + " quit [message]"
					elif args[1] == "say":
						if len( args ) > 3:
							reply_to = args[2]
							msg = " ".join( args[3:] )
						else:
							msg = "Usage: " + self.handle + " say [message]"
					elif args[1] == "nick":
						if len( args ) > 2:
							server.nick( args[2] )
							msg = False
						else:
							msg = "Usage: " + self.handle + " nick [nickname]"
					elif args[1] == "prefix":
						if len( args ) > 2:
							pman.set_prefix( args[2] );
							msg = "Command prefix changed to \"" + args[2] + "\""
						else:
							msg = "Usage: " + self.handle + " prefix [command prefix]"
				else:
					msg = "You are not authorized to use the bot."
			elif args[1] in med_priv:
				msg = "Wut"
			elif args[1] in low_priv:
				msg = "tuW"
			else:
				msg = "Action \"" + args[1] + "\" not found"
		else:
			msg = False
			server.send_message( reply_to, self.help_str );
			server.send_message( reply_to, "Actions: admin: " + str( high_priv ) + 
				", mod: " + str( med_priv ) + ", user: " + str( low_priv ))

		if msg:
			server.send_message( reply_to, msg );

	hooks = { }
