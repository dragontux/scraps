import urllib
import urllib.request as url
import re

class plugin:
	handle	= "ddg"
	method	= "args"
	do_init = False;
	cron_time = False
	join_hook = False
	help_str = "Usage: " + handle + " [search]. Search duckduckgo for whatever."

	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else:			reply_to = nick

		msg = False

		if len( args ) <= 1:
			msg = self.help_str
			#msg = "Usage: " + handle + " [search]"

		if len( args ) > 1:
			search = "http://www.duckduckgo.com/html/?q=" + " ".join( args[1:] )
			search = search.replace( " ", "+" )
			print( search )
			ddg = url.urlopen( search )
			data = str( ddg.read( ))
			links = []

			title = ""
			link  = ""
	
			for thing in data.split( "\\n" ):
				if "<a rel=\"nofollow\" class" in thing:
					links.append( thing )

			if len( links ) < 1:
				return

			metadata = links[0].split("\"")
			link = metadata[5]
			title = metadata[6][1:]
			title = re.sub( "<.*?>", "", title )

			msg = title + ": " + link

		if msg:	
			server.send_message( reply_to, msg )

	hooks = { }
