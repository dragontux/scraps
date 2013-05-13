class plugin:
	handle	  = "help"
	method	  = "args"
	do_init   = False
	cron_time = False
	join_hook = False
	help_str  = "Get help on this bot and it's plugins"

	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else:			reply_to = nick

		if len( args ) < 2:
			server.send_message( reply_to, nick + ": Loaded plugins: " + ", ".join( pman.get_handles( )))
		else:
			string = pman.get_plugin_help( args[1] )
			if string:
				server.send_message( reply_to, string )
			else:
				server.send_message( reply_to, "I have no help for %s. :(" % ( args[1] ))

	hooks = { }
