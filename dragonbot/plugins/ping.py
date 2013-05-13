class plugin:
	handle	= "ping"
	method	= "args"
	do_init = False;
	cron_time = False
	help_str = False

	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else:			reply_to = nick

		server.send_message( reply_to, nick + ": pong! host: \"" + host + "\", channel: \"" + channel + "\"" )

	hooks = { }
