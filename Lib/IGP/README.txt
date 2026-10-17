this trunk contains both IGP and the LITE IGP

to build IGP - follow instructions in HowToIntegrate.doc
		*download ads.bin	http://ingameads.gameloft.com/redir/iphone131.php?game=XXXX&file=1
		*set game name in IGPConfig.py
		*run makeIGP.bat
		*build the project in xcode

to build IGP Lite - follow instructions in HowToIntegrate_lite.doc
		*download ads.bin	http://ingameads.gameloft.com/redir/iphone131.php?game=XXXX&lite=1&file=1
		*set game name in IGPConfig_lite.py
		*run makeIGP_lite.bat
		*change IGP_LITE define from IGPGame.cpp to 1	!!!!
		*build the project in xcode