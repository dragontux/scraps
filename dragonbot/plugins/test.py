import time

class plugin:
	handle	= "test"
	method	= "args"
	do_init = False;
	count	= 0
	cron_time = "00"
	
	def cron( self ):
		print( "Did cron stuff, 2" )

	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else:			reply_to = nick

		#time.sleep( 10 )
		self.count += 1
		server.send_message( reply_to, "Times this has been called: " + str( self.count ))
