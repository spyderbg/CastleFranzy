/*---------------------------------------------------------------------------*
  Project:  onetri
  File:     screenshot.h

  Copyright 1998, 1999, 2000 Nintendo.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  Created 05-25-01 by Steve Rabin
  
 *---------------------------------------------------------------------------*/


#ifndef __SCREENSHOT_H__
#define __SCREENSHOT_H__

#define	SCRNSHOT_SHARE_MEMORY_SIZE		0x2000	//	share memory size 8KByte
#define	SCRNSHOT_SHARE_ADDR				0x00000000
#define	SCRNSHOT_SCREEN_WIDTH			640
#define	SCRNSHOT_SCREEN_HEIGHT			480
#define	SCRNSHOT_SCREEN_SAVE_HEIGHT		480
//#define	SCRNSHOT_SCREEN_SAVE_HEIGHT		448
#define	SCRNSHOT_CALC_XFB_SIZE( w, h )	((w) * (h) * (u32)2)
#define	SCRNSHOT_CALC_EFB_SIZE( w, h )	((w) * (h) * (u32)3)

//#define	SCRNSHOT_XFB_SIZE				(SCRNSHOT_SCREEN_WIDTH * SCRNSHOT_SCREEN_HEIGHT * 2)
//#define	SCRNSHOT_EFB_SIZE				(SCRNSHOT_SCREEN_WIDTH * SCRNSHOT_SCREEN_HEIGHT * 3)

#define	SCRNSHOT_PC_TIMEOUT				3		// PC Screenshot app timeout [sec]

//-----------------------------------------------
//       Mailbox bit define
//
// |31   24|23   16|15                            0|
// +-------+-------+-------------------------------+
// | Resrv |  CMD  |           CMD Option          |
// +-------+-------+-------------------------------+
//
#define	SCRNSHOT_MAIL_CMD_SHIFT		16
#define	SCRNSHOT_MAIL_CMD_MASK		0x00FF0000
#define	SCRNSHOT_MAIL_OPTION_SHIFT	0
#define	SCRNSHOT_MAIL_OPTION_MASK	0x0000FFFF
#define	SCRNSHOT_MAIL_GET_CMD( mail ) \
	(((mail)  & SCRNSHOT_MAIL_CMD_MASK) >> SCRNSHOT_MAIL_CMD_SHIFT )
#define	SCRNSHOT_MAIL_GET_OPTION( mail ) \
	(((mail)  & SCRNSHOT_MAIL_OPTION_MASK) >> SCRNSHOT_MAIL_OPTION_SHIFT )
#define	SCRNSHOT_MAIL_MAKECMD( cmd, option) 								\
  	((((u32)(cmd) << SCRNSHOT_MAIL_CMD_SHIFT) & SCRNSHOT_MAIL_CMD_MASK) 	\
	| (((u32)(option) & SCRNSHOT_MAIL_OPTION_MASK) << SCRNSHOT_MAIL_OPTION_SHIFT))

// cmd define
#define	SCRNSHOT_CMD_NONE				0x00	// no op
#define	SCRNSHOT_CMD_PING				0x01	// Ping
#define	SCRNSHOT_CMD_XFB				0x02	// Grap XFB Request
#define	SCRNSHOT_CMD_EFB				0x03	// Grap EFB Request
#define	SCRNSHOT_CMD_SIZE				0x04	// Grap EFB Request

// ping cmd option
#define	SCRNSHOT_CMD_PING_OPT_MAGICVALUE_MASK	0xFFFF	// Magic value(Only use Host(PC))
#define	SCRNSHOT_CMD_PING_TIMEOUT				1000	// [msec](Only use Host(PC))


#define	SCRNSHOT_CALC_BLOCK_NUM( size )					\
		(( (size) + SCRNSHOT_SHARE_MEMORY_SIZE - 1)/ SCRNSHOT_SHARE_MEMORY_SIZE)


// EFB/XFB SIZE
#define	SCRNSHOT_CMD_SIZE_EFB_WIDTH_INDEX	0
#define	SCRNSHOT_CMD_SIZE_EFB_HEIGHT_INDEX	1
#define	SCRNSHOT_CMD_SIZE_XFB_WIDTH_INDEX	2
#define	SCRNSHOT_CMD_SIZE_XFB_HEIGHT_INDEX	3

#ifdef _IRR_USE_WII_DEVICE_
#include <revolution.h>

void SCREENSHOTSetEFBSize(u16 efbHeight, u16 efbWidth);
void SCREENSHOTSetXFBSize(u16 xfbHeight, u16 xfbWidth);
void SCREENSHOTSetRenderModeObj(GXRenderModeObj* obj);
void SCREENSHOTService( void* bufferXFB,
						void* workingBuffer8K,
						u32 workingBufferSize );

#endif	// _IRR_USE_WII_DEVICE_

#endif	// __SCREENSHOT_H__