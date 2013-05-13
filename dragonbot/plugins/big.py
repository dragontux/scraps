class plugin:
	handle	= "big"
	method	= "args"
	do_init = False;
	cron_time = False
	help_str = False

	letters = { 
		"a":	[	"  ___ ",
				" | _ |",
				" |   |",
				" |_|_|"],

		"b":	[ 	"  _   ",
				" | |_ ",
				" | _ |",
				" |___|"],

		"c":	[	"  ___ ",
				" |  _|",
				" | |_ ",
				" |___|"],

		"d":	[	"    _ ",
				"  _| |",
				" | _ |",
				" |___|"],

		"e":	[	"  ___ ",
				" |  _|",
				" |  _|",
				" |___|"],

		"f":	[	"  ___ ",
				" |  _|",
				" |  _|",
				" |_|  "],

		"g":	[	"  ___ ",
				" | _ |",
				" \__ |",
				" |___|"],

		"h":	[	"  _   ",
				" | |_ ",
				" |   |",
				" |_|_|"],

		"i":	[	"  _ ",
				" |_|",
				" | |",
				" |_|"],

		"j":	[	"    _ ",
				"   | |",
				"  _| |",
				" |__/ "],

		"k":	[	"  _ _ ",
				" | | /",
				" |  - ",
				" |_|_|"],

		"l":	[	"  _   ",
				" | |  ",
				" | |_ ",
				" |___|"],

		"m":	[	"  _____ ",
				" | ` ` \\",
				" | |_| |",
				" |_| |_|"],

		"n":	[	"  ____  ",
				" |  _ \ ",
				" | | | |",
				" |_| |_|"],

		"o":	[	"  ___ ",
				" |   |",
				" | | |",
				" |___|"],

		"p":	[	"  ___ ",
				" | _ |",
				" |  _|",
				" |_|  "],

		"q":	[	"  ___ ",
				" | _ |",
				" |_  |",
				"   |_|"],

		"r":	[	" ,___ ",
				" /  _|",
				" | |  ",
				" |_|  "],

		"s":	[	"  ___ ",
				" |  _|",
				" |__ |",
				" |___|"],

		"t":	[	"  ___ ",
				" |_ _|",
				"  | | ",
				"  |_| "],

		"u":	[	"  ___ ",
				" | | |",
				" | | |",
				" \\___|"],

		"v":	[	"  ___ ",
				" | | |",
				" | | |",
				" \\__/ "],

		"w":	[	"  _   _ ",
				" | |_| |",
				" | | | |",
				" \\_____|"],

		"x":	[	"  ___ ",
				" | | |",
				"  - - ",
				" |_|_|"],

		"y":	[	"  ___ ",
				" | | |",
				" |__ |",
				" |___|"],

		"z":	[	"  ___ ",
				" |_  |",
				" | __|",
				" |___|"],

		" ":	[	"     ",
				"     ",
				"     ",
				"     "]

	}

	def run( self, pman, server, nick, host, channel, args ):
		if channel[0] == "#": 	reply_to = channel
		else:			reply_to = nick

		msg = False

		if len( args ) < 2: 
			msg = "Usage: " + self.handle + " [message]. Say something big."

		else:
			if not ( nick in pman.get_trusted() or nick in pman.get_mods( )):
				msg = "You are not authorized to use the bot."
			else: 
				string = " ".join( args[1:] )
				string = string.lower()

				i = 0
				for line in self.letters["a"]:
					send = ""
					for char in string:
						if char in self.letters.keys():
							send += self.letters[ char ][i]

					server.send_message( reply_to, send )
					i += 1

		if msg:
			server.send_message( reply_to, msg )

	hooks = { }

