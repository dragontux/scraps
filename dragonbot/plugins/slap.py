class plugin:
	handle	= "slap"
	method	= "args"
	do_init = False;
	cron_time = False
	help_str = False
	join_hook = False

	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else:			reply_to = nick

		slap = ""

		if len( args ) > 1:
			slap = args[1]
		else:
			slap = nick

		server.send_message( reply_to, "\x01ACTION slaps " + slap + "\x01" )
