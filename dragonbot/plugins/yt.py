import urllib
import urllib.request as url

class plugin:
	handle	= "yt"
	method	= "args"
	do_init = False;
	cron_time = False
	join_hook = False
	help_str = "Usage: " + handle + " [search]. Search youtube for a video."

	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else:			reply_to = nick

		msg = False

		if len( args ) <= 1:
			msg = self.help_str

		if len( args ) > 1:
			search = "http://youtube.com/results?search_query=" + " ".join( args[1:] )
			search = search.replace( " ", "+" )
			print( search )
			youtube = url.urlopen( search )
			data = str( youtube.read( ))
			vids = []

			title = ""
			views = ""
			link  = ""
			vid_id = ""
	
			for thing in data.split( "\\n" ):
				if "yt-lockup2-video" in thing and "event=ad" not in thing:
					vids.append( thing )

			if len( vids ) < 1:
				return

			i = 0
			metadata = vids[0].split("\"")
			for thing in metadata:
				if thing == " data-context-item-title=":
					title = metadata[i+1]
				elif thing == " data-context-item-id=":
					vid_id = metadata[i+1]
				elif thing == " data-context-item-views=":
					views = metadata[i+1]
				if ( i < len( metadata ) - 1 ):
					i+=1
					
			link = "http://youtube.com/watch?v=" + vid_id
	
			title = title.replace( "&", "" )
			title = title.replace( "amp;", "&" )
			title = title.replace( "quot;", "\"")

			msg = title + ": " + link + " (" + views + ")"

		if msg:	
			server.send_message( reply_to, msg )

	hooks = { }
