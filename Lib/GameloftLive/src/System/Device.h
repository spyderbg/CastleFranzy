#ifndef __DEVICE_H__
#define __DEVICE_H__

#define	PLATFORM_NGAGE 				1
#define	PLATFORM_WIN32 				2
#define PLATFORM_SYMBIAN			3
#define PLATFORM_WINDOWS_MOBILE		21
#define PLATFORM_MAC_OS_X			4
#define PLATFORM_KDDI				22

//#define RP_DEBUG_INFO           

#define	DEVICE_UNDIFINED			0
#define	DEVICE_N91				1
#define	DEVICE_N70				2
#define DEVICE_I550				3			//for samsung
#define DEVICE_Z8				4			//UIQ, Motorola Z8
#define DEVICE_N77				5			//UIQ, Motorola Z8


#define TOUCH_NONE				0
#define TOUCH_SINGLE			1
#define TOUCH_MULTIPLY			2


#define SYMBIAN_OS_S60_3RD	1
#define SYMBIAN_OS_S60_2ND	2

//not changed
#define SCREEN_240_320		0
#define SCREEN_176_208		1
#define SCREEN_320_320		2
#define SCREEN_240_296		3
#define SCREEN_320_480		4
#define SCREEN_SIZE_COUNT	5

#define SCREEN_WIDTH	480 //240
#define SCREEN_HEIGHT	320

#define SOUND_WAVE_MIDI 0
#define SOUND_WAVE_ONLY 1

#define KEYPAD_NORMAL_MODE		0
#define KEYPAD_ONE_TOUCH_MODE	1
//#define SCREEN_WIDTH	640
//#define SCREEN_HEIGHT	480


#define NATIVE_COLOR_DEPTH 16

#define KEY_COLOR_VALUE 0X0000F81F

#define NO_NETWORK			0
#define WIN32_NETWORK		1
#define SYMBIAN_BT			2
#define NGAGE_BT			3	
#define IPHONE_WIFI			4
#define WINMOBILE_BT		5


///for ASprite's defines

#define COMMON_FONT_MIDDLE

/// ///////////////////////////////////////////////////////////////////////

#define			bErr				!false
#define			DbgDrawFPS			false
#define			DbgDrawMem			false
#define			DbgDrawFrameNr		false
#define			bDbgO				!false
#define			MAX_FPS				14
#define			SCR_W				128
#define			SCR_H				140
#define			FIXED_PRECISION		8


#define			ZOOM_X				1 
#define			ZOOM_Y				1
#define			ZOOM_X_DIV 			1  
#define			ZOOM_Y_DIV			1


#define			DEF_SV_W 				SCR_W			 
#define			DEF_SV_H 				SCR_H			 
#define			DEF_SV_X 				0				 
#define			DEF_SV_Y 				0	

/// ///////////////////////////////////////////////////////////////////////

//***************************************************************************
//** Brew String Color Defines
//***************************************************************************

#define	BREW_FONT_WHITE							0
#define BREW_FONT_BLUE							1
#define BREW_FONT_GREEN							2
#define BREW_FONT_RED							3
#define BREW_FONT_BLACK							4
#define BREW_FONT_PINK							5
#define BREW_FONT_BROWN							6
#define BREW_FONT_GOLD							7
#define BREW_FONT_GOLDBROWN						8

#define MAX_TEXT_LENGTH							1024
#define MAX_TEXT_LINES							512
#define TEMP_TEXT_LENGTH						512

#define TEXT_ALIGN_RIGHT						(1 << 1)
#define TEXT_ALIGN_HCENTER						(1 << 2)
#define TEXT_ALIGN_TOP							(1 << 3)
#define TEXT_ALIGN_VCENTER						(1 << 4)
#define TEXT_ALIGN_BOTTOM						(1 << 5)
#define	TEXT_ALIGN_LEFT							(1 << 6)

#define LINESPACE								1

#define CGame_k_iFont_Pal_0						(1 << 1)
#define CGame_k_iFont_Pal_1						(1 << 2)
#define CGame_k_iFont_Pal_2						(1 << 3)
#define CGame_k_iFont_Pal_3						(1 << 4)
#define CGame_k_iFont_Pal_4						(1 << 5)
#define CGame_k_iFont_Pal_5						(1 << 6)
#define CGame_k_iFont_Pal_6						(1 << 7)				


#define FONT_003_0_FRM_INDEX				16
#define FONT_003_0_FRM_LEFT					0
#define FONT_003_0_FRM_TOP					0
#define FONT_003_0_FRM_RIGHT				8
#define FONT_003_0_FRM_BOTTOM				8
#define FONT_003_0_FRM_WIDTH				8
#define FONT_003_0_FRM_HEIGHT				8
#define FONT_003_0_FRM_COLL_LEFT			0
#define FONT_003_0_FRM_COLL_TOP				0
#define FONT_003_0_FRM_COLL_RIGHT			0
#define FONT_003_0_FRM_COLL_BOTTOM			0
#define FONT_003_0_FRM_COLL_WIDTH			0
#define FONT_003_0_FRM_COLL_HEIGHT			0


#define FONT_003_MINUS_FRM_INDEX 13
#define FONT_003_MINUS_FRM_LEFT 0
#define FONT_003_MINUS_FRM_TOP 2
#define FONT_003_MINUS_FRM_RIGHT 5
#define FONT_003_MINUS_FRM_BOTTOM 5
#define FONT_003_MINUS_FRM_WIDTH 5
#define FONT_003_MINUS_FRM_HEIGHT 3
#define FONT_003_MINUS_FRM_COLL_LEFT 0
#define FONT_003_MINUS_FRM_COLL_TOP 0
#define FONT_003_MINUS_FRM_COLL_RIGHT 0
#define FONT_003_MINUS_FRM_COLL_BOTTOM 0
#define FONT_003_MINUS_FRM_COLL_WIDTH 0
#define FONT_003_MINUS_FRM_COLL_HEIGHT 0

//#define FONT_HIGHT_JP						m_iFontH
#define FONT_HIGHT_JP						14
#define FONT_WIDTH_JP						9

/// //////////////////////////////////////////////////////////////////////////

#define TEMP_BUFFER_SIZE  10 * 1024  
#define USE_MODULE_MAPPINGS		 false 
#define USE_HYPER_FM				 false//true  by liwei2
#define USE_INDEX_EX_FMODULES	 true  
#define USE_INDEX_EX_AFRAMES	 false 
#define USE_FM_OFF_SHORT		true
#define USE_PRECOMPUTED_FRAME_RECT  true
#define ALWAYS_BS_NFM_1_BYTE		 false  
#define ALWAYS_BS_SKIP_FRAME_RC	 false  
#define ALWAYS_BS_FRAME_COLL_RC	 false  
#define ALWAYS_BS_NAF_1_BYTE	 false  
#define USE_PIXEL_FORMAT_8888		 true
#define USE_PIXEL_FORMAT_4444		 true
#define USE_PIXEL_FORMAT_1555		 true
#define USE_PIXEL_FORMAT_0565		 true
#define USE_ENCODE_FORMAT_I2		 true
#define USE_ENCODE_FORMAT_I4		 true
#define USE_ENCODE_FORMAT_I16		 true
#define USE_ENCODE_FORMAT_I256		 true
#define USE_ENCODE_FORMAT_I64RLE	 true
#define USE_ENCODE_FORMAT_I127RLE	 true
#define USE_ENCODE_FORMAT_I256RLE	 true
#define MAX_SPRITE_PALETTES			 16
#define MAX_MODULE_MAPPINGS			 16
#define MAX_ANIMATION_INSTANCES		 12  
#define BSPRITE_v003	 (short)0x03DF      
#define BS_MODULES			 (1 << 0)
#define BS_MODULES_XY		 (1 << 1)
#define BS_MODULES_IMG		 (1 << 2)
#define BS_MODULES_WH_SHORT		(1 << 4)
#define BS_MODULES_XY_SHORT		(1 << 5)		// export x, y for each module as short
#define BS_FRAMES			 (1 << 8)
#define BS_FM_OFF_SHORT	 (1 << 10)     
#define BS_NFM_1_BYTE		 (1 << 11)     
#define BS_SKIP_FRAME_RC	 (1 << 12)     
#define BS_FRAME_COLL_RC    (1 << 13)     
#define BS_ANIMS			 (1 << 16)
#define BS_AF_OFF_SHORT	 (1 << 18)    
#define BS_NAF_1_BYTE		 (1 << 19)     
#define BS_MODULE_IMAGES	 (1 << 24)
#define BS_PNG_CRC			 (1 << 25)
#define BS_KEEP_PAL		 (1 << 26)
#define BS_TRANSP_FIRST	 (1 << 27)
#define BS_TRANSP_LAST		 (1 << 28)
#define BS_DEFAULT_DOJA	= (BS_MODULES | BS_FRAMES | BS_ANIMS)
#define BS_DEFAULT_MIDP2	 (BS_MODULES | BS_FRAMES | BS_ANIMS | BS_MODULE_IMAGES)
#define BS_DEFAULT_NOKIA	 BS_DEFAULT_MIDP2
#define BS_DEFAULT_MIDP1	 (BS_MODULES | BS_MODULES_XY | BS_FRAMES | BS_ANIMS)
#define BS_DEFAULT_MIDP1b	 (BS_MODULES | BS_FRAMES | BS_ANIMS | BS_MODULE_IMAGES | BS_PNG_CRC)
#define PIXEL_FORMAT_8888		 (short)0x8888
#define PIXEL_FORMAT_4444		 (short)0x4444
#define PIXEL_FORMAT_1555		 (short)0x5515
#define PIXEL_FORMAT_0565		 (short)0x6505  
#define ENCODE_FORMAT_I2			 0x0200
#define ENCODE_FORMAT_I4			 0x0400   
#define ENCODE_FORMAT_I16		 0x1600        
#define ENCODE_FORMAT_I256		 0x5602    
#define ENCODE_FORMAT_I64RLE		 0x64F0
#define ENCODE_FORMAT_I127RLE	 0x27F1
#define ENCODE_FORMAT_I256RLE	 0x56F2
#define  FLAG_FLIP_X	 0x01
#define  FLAG_FLIP_Y	 0x02
#define  FLAG_ROT_90	 0x04
#define  FLAG_USER0	 0x10  
#define  FLAG_USER1	 0x20  
#define  FLAG_HYPER_FM	 0x10       
#define FLAG_INDEX_EX_MASK  0xC0  
#define INDEX_MASK			 0x03FF  
#define INDEX_EX_MASK		 0x0300
#define INDEX_EX_SHIFT 	 2
#define  FLAG_OFFSET_FM  0x10
#define  FLAG_OFFSET_AF  0x20   
#define STACK_SIZE  200 
#define SCROLL_DELAY_TIME  40 
//end of sprite
#endif
