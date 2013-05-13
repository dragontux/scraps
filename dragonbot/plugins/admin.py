class plugin:
	handle    = "admin"
	method 	  = "args"
	do_init   = True
	cron_time = False
	help_str  = "Usage: " + handle + " [action] [...]. Manage bot plugins."

	def init( self ):
		print( "        Plugin manager initialised" )
	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else: 			reply_to = nick
		msg = ""

		nicks = pman.get_trusted();

		if nick in nicks:
			if len( args ) > 2:
				if args[1] == "load":
					loaded = pman.load_plugin( args[2] )
					if loaded:
						msg = "Loaded successfully.";
					else:
						msg = "Could not load " + args[2] + "."
				elif args[1] == "unload":
					unloaded = pman.unload_plugin_handle( args[2] )
					if unloaded:
						msg = "Unloaded " + args[2] + "."
					else:
						msg = "Plugin does not exist."
				elif args[1] == "allow":
					pman.add_trusted( args[2] )
					msg = "Granted access to " + args[2] + "."
				elif args[1] == "deny":
					pman.remove_trusted( args[2] )
					msg = "Denying access to " + args[2] + "."
				elif args[1] == "list":
					msg = "plugins: " + ", ".join( pman.get_handles());
			else:
				msg = "Need arguments: [action] [arg]"
		else:
			msg = "You are not authorized to use the bot."

		server.send_message( reply_to, msg );
