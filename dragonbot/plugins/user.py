import time
import hashlib

class plugin:
	handle	= "user"
	method	= "args"
	do_init = False;
	cron_time = False;
	join_hook = False
	help_str = "Usage: " + handle + " [action] [...]. Manage user data.";

	passfile = "bot_passwd"

	def in_passfile( self, nick, password ):
		try:
			fp = open( self.passfile, "r" )
		except IOError as e:
			print( "password file error:", e )
			return False

		phash = hashlib.md5()
		phash.update(bytes( password, "UTF-8" ))

		buf = fp.readline()
		while len( buf ) > 0:
			key = buf.split()
			if ( len( key ) > 1 ):
				if nick == key[0] and phash.hexdigest() == key[1]:
					return True
			buf = fp.readline()
		return False
	
	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else:			reply_to = nick
		msg = False
		nicks = pman.get_trusted()

		high_priv = [ "add", "delete" ]
		med_priv  = [ ]
		low_priv  = [ "login", "logout", "list" ]

		if len( args ) < 2:
			msg = False
			server.send_message( reply_to, self.help_str )
			server.send_message( reply_to, "Actions: admin: " + str( high_priv ) + 
				", mod: " + str( med_priv ) + ", user: " + str( low_priv ))

		elif args[1] in high_priv:
			if nick in pman.get_trusted():
				if args[1] == "add":
					if len( args ) > 2:
						if args[2] == "user":	
							if len( args ) > 3:
								pman.add_user( args[3] )
							else:
								msg = "Usage: " + self.handle + " add user [username]"

						elif args[2] == "mod":
							if len( args ) > 3:
								pman.add_mod( args[3] )
							else:
								msg = "Usage: " + self.handle + " add mod [username]"

						elif args[2] == "trusted":
							if len( args ) > 3:
								pman.add_trusted( args[3] )
							else:
								msg = "Usage: " + self.handle + " add trusted [username]"
						else:
							msg = "Can't add \"" + args[2] + "\"."
					else:
						msg = "Usage: " + self.handle + " add [user|mod|trusted]"

				elif args[1] == "delete":
					if len( args ) > 2:
						if args[2] == "user":	
							if len( args ) > 3:
								pman.remove_user( args[3] )
							else:
								msg = "Usage: " + self.handle + " delete user [username]"

						elif args[2] == "mod":
							if len( args ) > 3:
								pman.remove_mod( args[3] )
							else:
								msg = "Usage: " + self.handle + " delete mod [username]"

						elif args[2] == "trusted":
							if len( args ) > 3:
								pman.remove_trusted( args[3] )
							else:
								msg = "Usage: " + self.handle + " delete trusted [username]"
						else:
							msg = "Can't delete \"" + args[2] + "\"."
					else:
						msg = "Usage: " + self.handle + " delete [user|mod|trusted]"
			else:
				msg = "You are not authorised to use this action."

		elif args[1] in med_priv:
			if nick in pman.get_mods():
				msg = "wut"
			else:
				msg = "You are not authorised to use this action."

		elif args[1] in low_priv:
			if args[1] == "login":
				if nick in nicks:
					msg = "You are already logged in!"
				elif len( args ) < 3:
					msg = "Usage: " + self.handle + " login [password]"
				elif self.in_passfile( self, nick, args[2] ):
					pman.add_trusted( nick )
					msg = "You are now logged in."
				else:
					msg = "Invalid username/password."

			elif args[1] == "logout":
				if nick in nicks:
					pman.remove_trusted( nick )
					msg = "You are now logged out."
				else:
					msg = "You are not logged in."

			elif args[1] == "list":
				msg = "trusted: " + str( pman.get_trusted( )) + ", mods: " + str( pman.get_mods( )) + \
					", users: " + str( pman.get_users( ))

		else:
			msg = "Unknown action \"" + args[1] + "\""

		if msg:
			server.send_message( reply_to, msg );

	hooks = { }
