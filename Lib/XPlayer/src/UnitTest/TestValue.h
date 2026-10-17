#include "../../include/XPlayer_defines.h"

#define	TEST_USER_NAME		"zzz"
#define	TEST_USER_NUMBER	"2404"
#define	TEST_PASSWORD		"zzz"
#define	TEST_LOGIN_TYPE		XPLAYER_USER_STATE_ONLINE
#define	TEST_DEV_TOKEN		"111111111"
#define	TEST_RETURN_USER_NUMBER	true
#define	TEST_RETURN_USER_NAME	true

static char USER_NAME[32] = TEST_USER_NAME;

#ifdef USE_USER_NUMBER
	#define TEST_OTHER_USER_NAME		"123"
	#define TEST_OTHER_USER				"qqq"
#else
	#define TEST_OTHER_USER_NAME		"qqq"
#endif
	#define TEST_OTHER_USER_PASSWORD	"qqq"
	
#define TEST_NEW_USER_NAME	"zzzaaazzz"
#define TEST_NEW_PASSWORD	"zzzaaazzz"
#define TEST_NEW_EMAIL		"tested@gameloft.com"
#define TEST_NEW_COUNTRY	"vn"

#define TEST_REGISTER_NAME			"test_"
#define TEST_REGISTER_COUNTRY		"vn"
#define TEST_REGISTER_EMAIL			"test_register@gameloft.com"
#define TEST_REGISTER_EMAIL_FLAG	false
#define TEST_REGISTER_GLLIVE		true

#define TEST_DAY	30
#define TEST_LIMIT	100

#define TEST_STORE_NAME			"color"
#define TEST_STORE_VALUE		"Yellow"

#define TEST_POINT_REPUTATION	5

#define TEST_AVATAR_PATH		"avatar.bmp"
#define TEST_TROPHY_VALUE		50

#define TEST_USER_EMAIL			"test_tester@gameloft.com"
#define TEST_USER_EMAIL_FLAG	false
#define TEST_USER_COUNTRY		"vn"
#define TEST_USER_FIRST_NAME	"First"
#define TEST_USER_LAST_NAME		"Last"
#define TEST_USER_SEX			1
#define TEST_USER_BIRTHDAY		"10/10/2000"

#define TEST_TROPHY_ID			11
#define TEST_PRODUCT_ID			15851
#define TEST_ATTACHMENT_ID		1

#define TEST_SCORE_VALUE		300
#define TEST_SCORE_LEVEL		1
#define TEST_SCORE_TYPE			SCORE_TYPE_POINTS
#define TEST_NUMBER_OF_PLAYER	10
#define TEST_NUMBER_OF_PLAYER_AROUND	10

#define TEST_MSG_TYPE			USER_MESSAGE_TYPE_GAMEMESSAGE|USER_MESSAGE_TYPE_USERMESSAGE|USER_MESSAGE_TYPE_USERMESSAGE_FRIENDINVITATION|USER_MESSAGE_TYPE_USERMESSAGE_GAMEINVITATION|USER_MESSAGE_TYPE_USERMESSAGE_CHATINVITATION
#define TEST_MSG_READ_TYPE		USER_MESSAGE_READTYPE_ALL

#define TEST_MSG_INDEX			0
static int TEST_MSG_COUNT = 0;
static int TEST_MSG_ID = 0;
#define TEST_MSG_POSITION		0
#define TEST_SEND_MSG_TYPE		USER_MESSAGE_TYPE_USERMESSAGE
#define TEST_SEND_MSG_TITLE		"Test send message function."
#define TEST_SEND_MSG_BODY		"This is a message, use to test send message function."

#define TEST_PRODUCT_LIST		"16422,16433,16444"
#define TEST_USER_STATE			XPLAYER_USER_STATE_ONLINE
#define TEST_PRODUCT_LANGUAGE	"FR"
static char TEST_AVATAR_KEY[32]	= {0};

#define TEST_VIDEO_TITLE		"Gameloft video test."
#define TEST_VIDEO_COMMENTS		"Gameloft video test comments."
static char TEST_VIDEO_ID[32] = {0};
#define TEST_VIDEO_STATUS		1
#define TEST_VID				"1"

#define TEST_YOUTUBE_NAME		"guisu.huang@gmail.com"
#define TEST_YOUTUBE_PASSWORD	"huang123"