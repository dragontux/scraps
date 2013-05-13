class plugin:
	handle	= "msg"
	method	= "args"
	do_init = False;
	cron_time = False
	help_str = 	"Usage: " + handle + " [action] [...] Manage user messages. " + \
			"Actions = { send [nick] [message] }"

	msglist = {}
	# msg entry = { "nick": [[sender, message]] }

	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else:			reply_to = nick

		if args[1] == "send" and len( args ) > 3:
			sendto = args[2]
			message = " ".join( args[3:] )
			if sendto in self.msglist:
				self.msglist[sendto].append([ nick, message ])
			else:
				self.msglist.update({ sendto:[[nick, message]]})

			server.send_message( reply_to, nick + ": message queued." )
		else:
			server.send_message( reply_to, nick + ": invalid action \"" + args[1] + "\"" )

	def join_cmd( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else:			reply_to = nick

		if nick in self.msglist:
			ray = self.msglist[nick]
			if len( ray ) > 0:
				server.send_message( reply_to, nick + ": you have " + str(len( ray )) + " messages (check pms)" )
				for msg in ray:
					server.send_message( nick, "(" + msg[0] + ") " + msg[1] )

				self.msglist[nick] = []

	hooks = { "JOIN":join_cmd, "PRIVMSG":join_cmd }
