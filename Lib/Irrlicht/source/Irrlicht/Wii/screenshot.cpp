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

//#ifdef _IRR_USE_WII_DEVICE_

#include <revolution.h>
#include <revolution/hio2.h>
#include <string.h>
#include "screenshot.h"

typedef enum
{ 
    GRAB_NOT_TRANSFERRING, 
    GRAB_TRANSFERRING 
}
GRAB_STATUS;


//Globals
BOOL            g_connected = FALSE;
BOOL            g_ping_received = FALSE;
BOOL            g_mail_waiting = FALSE;
s32             g_usb_channel = -1;
BOOL            g_hio2Initilized = FALSE;
HIO2DeviceType  g_usb_type = HIO2_DEVICE_INVALID;
HIO2Handle      g_usb_handle = HIO2_INVALID_HANDLE_VALUE;

typedef struct st_ScreenSize
{
    u16 width;
    u16 height;
} ScreenSize;

// Static
static u16  s_ScreenEFBWidth    = SCRNSHOT_SCREEN_WIDTH;
static u16  s_ScreenEFBHeight   = SCRNSHOT_SCREEN_HEIGHT;
static u32  s_ScreenEFBSize     = SCRNSHOT_CALC_EFB_SIZE( SCRNSHOT_SCREEN_WIDTH, SCRNSHOT_SCREEN_HEIGHT);

static u16  s_ScreenXFBWidth    = SCRNSHOT_SCREEN_WIDTH;
static u16  s_ScreenXFBHeight   = SCRNSHOT_SCREEN_HEIGHT;
static u32  s_ScreenXFBSize     = SCRNSHOT_CALC_XFB_SIZE( SCRNSHOT_SCREEN_WIDTH, SCRNSHOT_SCREEN_HEIGHT);



static BOOL HostIOEnumCallback( HIO2DeviceType type )
{
    g_usb_type = type;
    return( FALSE );
}


static void CopyoutPortionEFB( u8* image_buffer, u32 width, u32 startByte, u32 totalBytes )
{
    u32 color;
    
    u16 x = (u16)((u32)(startByte/3) % width);
    u16 y = (u16)((u32)(startByte/3) / width);
    u32 component = startByte % 3;
    
    u32 count = 0;
    while( count < totalBytes )
    {
        GXPeekARGB( x, y, &color );
        
        if( component == 0 )
        {
            count++;
            image_buffer[0] = (u8)((color & 0x00FF0000) >> 16);
        }
        if( component <= 1 && count < totalBytes )
        {
            count++;
            image_buffer[1 - component] = (u8)((color & 0x0000FF00) >> 8);
        }
        if( component <= 2 && count < totalBytes )
        {
            count++;
            image_buffer[2 - component] = (u8)(color & 0x000000FF);
        }
        
        image_buffer += 3 - component;
        component = 0;
        
        x++;
        if( x >= width )
        {
            x = 0;
            y++;
        }
    }
}


static void WriteScreenshotPortionEFBtoUSB( u32 chunk, void* workingBuffer )
{
    u32 size;

    if( ( s_ScreenEFBSize - chunk * SCRNSHOT_SHARE_MEMORY_SIZE) > SCRNSHOT_SHARE_MEMORY_SIZE )
    {
        size = SCRNSHOT_SHARE_MEMORY_SIZE;
    }
    else
    {
        size = s_ScreenEFBSize - chunk * SCRNSHOT_SHARE_MEMORY_SIZE;
    }
    CopyoutPortionEFB( (u8*)workingBuffer, s_ScreenEFBWidth, SCRNSHOT_SHARE_MEMORY_SIZE * chunk, size );
    DCFlushRange( workingBuffer, SCRNSHOT_SHARE_MEMORY_SIZE );
    while( !HIO2Write( g_usb_handle, SCRNSHOT_SHARE_ADDR, workingBuffer, (s32)size ) )
    {
        //Spin until communication is successful
    }
}




static GRAB_STATUS GrabChunk( u32 mail,
                              void* bufferXFB,
                              void* workingBuffer )
{
    
    GRAB_STATUS status = GRAB_NOT_TRANSFERRING;
    u32 cmd    = SCRNSHOT_MAIL_GET_CMD( mail );
    u32 option = SCRNSHOT_MAIL_GET_OPTION( mail );
    
    switch( cmd )
    {
        case SCRNSHOT_CMD_XFB:
            {
                void* buffer = NULL;
                
                //transfer chunk
                u8* fb = (u8*)bufferXFB;
                buffer = (void*)( &fb[ option * SCRNSHOT_SHARE_MEMORY_SIZE ] );

                while( !HIO2Write( g_usb_handle, SCRNSHOT_SHARE_ADDR, buffer, SCRNSHOT_SHARE_MEMORY_SIZE ) )
                {
                    //Spin until communication is successful
                }
                
                while( !HIO2WriteMailbox( g_usb_handle,  mail ) )
                {
                    //Spin until communication is successful
                }
                
                if( option >= ( SCRNSHOT_CALC_BLOCK_NUM( s_ScreenXFBSize ) - 1 ) )
                {
                    status = GRAB_NOT_TRANSFERRING;
                }
                else
                {
                    status = GRAB_TRANSFERRING;
                }
            }
            break;
        case SCRNSHOT_CMD_EFB:
            {
                void* buffer = NULL;

                //transfer chunk
                WriteScreenshotPortionEFBtoUSB( option, workingBuffer );
                

                while( !HIO2WriteMailbox(g_usb_handle,  mail ) )
                {
                    //Spin until communication is successful
                }

                if( option >= ( SCRNSHOT_CALC_BLOCK_NUM( s_ScreenEFBSize ) -1 ) )
                {
                    status = GRAB_NOT_TRANSFERRING;
                }
                else
                {
                    status = GRAB_TRANSFERRING;
                }
                
            }
            break;
        case SCRNSHOT_CMD_PING: // Ping request
            while( !HIO2WriteMailbox( g_usb_handle, mail ) )
            {
                //Spin until communication is successful
            }
            break;
        case SCRNSHOT_CMD_SIZE: // SIZE request
            {
                u16 buf[32/sizeof(u16)] ATTRIBUTE_ALIGN(32);

                buf[SCRNSHOT_CMD_SIZE_EFB_WIDTH_INDEX]  = s_ScreenEFBWidth;
                buf[SCRNSHOT_CMD_SIZE_EFB_HEIGHT_INDEX] = s_ScreenEFBHeight;
                buf[SCRNSHOT_CMD_SIZE_XFB_WIDTH_INDEX]  = s_ScreenXFBWidth;
                buf[SCRNSHOT_CMD_SIZE_XFB_HEIGHT_INDEX] = s_ScreenXFBHeight;
                DCFlushRange( buf, sizeof(buf) );
                
                while( !HIO2Write( g_usb_handle, SCRNSHOT_SHARE_ADDR, buf, sizeof( buf ) ) )
                {
                    //Spin until communication is successful
                }
                while( !HIO2WriteMailbox( g_usb_handle, mail ) )
                {
                    //Spin until communication is successful
                }
                break;
            }
        default:
            break;
    }

    return status;
}



static void CheckMail( void* bufferXFB, void* workingBuffer )
{
    u32 temp = 0;
    GRAB_STATUS status = GRAB_TRANSFERRING;
    OSTime escapeCount = OSGetTime();

    do
    {
        u32 mail;

        if(  g_mail_waiting == TRUE)
        {
            if( HIO2ReadMailbox( g_usb_handle, &mail ) == TRUE )
            {
                g_mail_waiting = FALSE;

                status = GrabChunk( mail, bufferXFB, workingBuffer );

                escapeCount = OSGetTime();

            }
        }

        if( OSTicksToSeconds( OSGetTime() - escapeCount ) > SCRNSHOT_PC_TIMEOUT )
        {   //The PC screenshot app has died - escape
            break;
        }
    }
    while( status == GRAB_TRANSFERRING );

}

static void ReceiveMailCallback( HIO2Handle handle )
{
    (void) handle;
    
    //Mail is waiting for us - don't look at it yet
    g_mail_waiting = TRUE;
}

static void DisconnectCallback( HIO2Handle handle )
{
    (void) handle;
    g_connected = FALSE;
}


static BOOL ConnectToUSB( void )
{
    if( !g_connected )
    {
        if( HIO2EnumDevices( HostIOEnumCallback ) == TRUE )
        {
            if( g_usb_type != HIO2_DEVICE_INVALID )
            {
                g_usb_handle = HIO2Open( g_usb_type, ReceiveMailCallback, DisconnectCallback );
                if( g_usb_handle != HIO2_INVALID_HANDLE_VALUE )
                {
                    u32 temp = 0;
                    HIO2ReadMailbox( g_usb_handle, &temp );
                    g_connected = TRUE;
                }
            }
        }
    }
    
    return( g_connected );
}


void SCREENSHOTSetEFBSize(u16 efbHeight, u16 efbWidth)
{
    s_ScreenEFBHeight   = efbHeight;
    s_ScreenEFBWidth    = efbWidth;
    s_ScreenEFBSize     = SCRNSHOT_CALC_EFB_SIZE( s_ScreenEFBWidth,  s_ScreenEFBHeight );
}

void SCREENSHOTSetXFBSize(u16 xfbHeight, u16 xfbWidth)
{
    s_ScreenXFBHeight   = xfbHeight;
    s_ScreenXFBWidth    = xfbWidth;
    s_ScreenXFBSize     = SCRNSHOT_CALC_XFB_SIZE( s_ScreenXFBWidth,  s_ScreenXFBHeight );   
}

void SCREENSHOTSetRenderModeObj(GXRenderModeObj* obj)
{
    SCREENSHOTSetXFBSize( obj->xfbHeight, obj->fbWidth );
    SCREENSHOTSetEFBSize( obj->efbHeight, obj->fbWidth );
}

void SCREENSHOTService( void* bufferXFB, 
                        void* workingBuffer,
                        u32 workingBufferSize )
{
    if( workingBufferSize < SCRNSHOT_SHARE_MEMORY_SIZE )
    {   //Stop program execution regardless of Debug/Release
        OSHalt("SCREENSHOTService: Argument workingBufferSize must be >= SCRNSHOT_SHARE_MEMORY_SIZE\n");
    }
    
    if( !g_hio2Initilized )
    {
        if( HIO2Init() == TRUE )
        {
            g_hio2Initilized = TRUE;
        }
    }
    
    if( !g_connected )
    {
        ConnectToUSB();
    }
    else if( g_mail_waiting == TRUE)
    {
        CheckMail( bufferXFB, workingBuffer );
    }

}

//#endif // _IRR_USE_WII_DEVICE_
