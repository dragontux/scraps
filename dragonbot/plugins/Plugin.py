class plugin:
	handle    = "plugin"
	method 	  = "string"
	do_init   = True

	def init( self ):
		print( "Plugin initialised" )
	def run( server, nick, channel, message ):
		if channel[0] == "#":
			reply_to = channel
		else:
			reply_to = nick
		server.send_message( reply_to, "You posted \"%s\" to %s." % ( message[:-1], channel ))
