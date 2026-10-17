/** Example 006 2D Graphics

This Tutorial shows how to do 2d graphics with the Irrlicht Engine.
It shows how to draw images, keycolor based sprites,
transparent rectangles, and different fonts. You may consider
this useful if you want to make a 2d game with the engine, or if
you want to draw a cool interface or head up display for your 3d game.

As always, I include the header files, use the irr namespace,
and tell the linker to link with the .lib file.
*/
#include <irrlicht.h>
#include <iostream>
#include <IImage.h>
#include "RawImage.h"
#include "CReadFile.h"
#include "theora\theora.h"
using namespace irr;

#ifdef _MSC_VER
#pragma comment(lib, "Irrlicht.lib")
#endif

IrrlichtDevice *device = 0;
int frameDecodeCount = 0;
int frameDecodeTimerStart = 0;
int frameDecodeTimerStop  = 0;
int frameDecodeTimerDelaySum = 0;
double frameDecodeTimerAvg = 0.0;


bool decodeYUVtoRGB(irr::video::ITexture* texture, yuv_buffer* yuvFrameData, theora_info ti) {

	// Retrieve information used for indexing
	int imgLumaHeight = yuvFrameData->y_height;
	int imgLumaWidth = yuvFrameData->y_width;
	int imgLumaStride = yuvFrameData->y_stride;
	int imgLumaStrideLength = imgLumaStride - imgLumaWidth;

	int imgChromaStride = yuvFrameData->uv_stride;
	int imgChromaStrideLength = imgChromaStride - yuvFrameData->uv_width;
	
	int buffStride = texture->getPitch() / sizeof(int);
	int buffStrideLength = buffStride - imgLumaWidth;

	unsigned char* lumaPlane = yuvFrameData->y;
	unsigned char* uPlane = yuvFrameData->u;
	unsigned char* vPlane = yuvFrameData->v;

	u32* textureData = (u32*)texture->lock();

	// YUV Values
	int iLumaValue;
	int iUChromaValue;
	int iVChromaValue;

	// Colors
	int iColorR;
	int iColorG;
	int iColorB;

	// Populate the image data depending on pixel format.
	bool bResult = false;
	switch (ti.pixelformat) {

		// Horizontal and vertical sub-sampling
		case OC_PF_420:
			// 4:2:0 -- horizontal and vertical
			bResult = true;
			break;

	//	// Horizontal sub-sampling only
	//	case OC_PF_422:
	//		// In 4:2:2 sampling, both U and V are subsampled
	//		bFrameFound = true;
	//		break;

	//	// No subsampling
	//	case OC_PF_444:
	//		// In 4:2:0 sampling, both U and V are subsampled
	//		bFrameFound = true;
	//		break;
	//	
		// Not supported.
		default:
			//bResult = false;
			return bResult;
			break;
	}


frameDecodeTimerStart = device->getTimer()->getRealTime();
	for (int row = 1; row <= imgLumaHeight; ++row) {
		for (int column = 0; column < imgLumaWidth; ++column) {

			// Retrieve values
			iLumaValue		= (*lumaPlane)	- 16;
			iUChromaValue	= (*uPlane)		- 128;
			iVChromaValue	= (*vPlane)		- 128;

			// Calculate colors
			iColorR = ((298 * iLumaValue					   + 409 * iVChromaValue + 128) >> 8);
			iColorG = ((298 * iLumaValue - 100 * iUChromaValue - 208 * iVChromaValue + 128) >> 8);
			iColorB = ((298 * iLumaValue + 516 * iUChromaValue						 + 128) >> 8);



			// Clamp the values to something [0, 255]
			iColorR &= (~iColorR) >> 31; iColorR -= 255; iColorR &= iColorR >> 31; iColorR += 255;
			iColorG &= (~iColorG) >> 31; iColorG -= 255; iColorG &= iColorG >> 31; iColorG += 255;
			iColorB &= (~iColorB) >> 31; iColorB -= 255; iColorB &= iColorB >> 31; iColorB += 255;
			//iColorR = (iColorR > 255) ? 255 : ((iColorR < 0) ? 0 : iColorR);
			//iColorG = (iColorG > 255) ? 255 : ((iColorG < 0) ? 0 : iColorG);
			//iColorB = (iColorB > 255) ? 255 : ((iColorB < 0) ? 0 : iColorB);
			
			// Write pixel data
			(*textureData) = 0xFF000000 | (iColorR << 16) | (iColorG << 8) | iColorB;

			++textureData;
			++lumaPlane;

			uPlane += (column & 0x01);
			vPlane += (column & 0x01);

		}

		lumaPlane += imgLumaStrideLength;
		textureData += buffStrideLength;
		uPlane += (imgChromaStrideLength) - ((row & 0x01) * imgChromaStride);
		vPlane += (imgChromaStrideLength) - ((row & 0x01) * imgChromaStride);
	}
frameDecodeTimerStop  = device->getTimer()->getRealTime();
	texture->unlock();

	return bResult;
}


/*
At first, we let the user select the driver type, then start up the engine, set
a caption, and get a pointer to the video driver.
*/
int main()
{


	// State booleans
	bool bRetrieveHeader = true;
	bool bRunDecoder = true;

	// Open the video stream
	irr::io::CReadFile fileReader("test.vp3");
	int iBytesReadFromStream;

	// Theora and Ogg declarations
	// Forget about Vorbis for now
	ogg_sync_state   oy;
	ogg_page         og;
	ogg_stream_state to;
	ogg_packet		 op;

	theora_info      ti;
	theora_comment   tc;
	theora_state     td;
	int				 tpCount = 0;		// Theora packet count.

	double frameTime = 0.0;

	void* buffer = 0;

	// Read the first 4096 bytes into a buffer.
	//fileReader.read(buffer, 4096);


	// Initialise the ogg synchronization layer
	ogg_sync_init(&oy);

	// Initialise Theora structures required for header parsing.
	theora_comment_init(&tc);
	theora_info_init(&ti);

	// Do it once before reading in the buffer to avoid memory leakage
	// (AS SUGGESTED BY XIPH.ORG IN THE LIBOGG API DOCUMENTATION)
	// retrieve data from an ogg buffer and store it into a page.
	// >0 = we have a page
	//ogg_sync_pageout(&oy, &og);


	// *** LOAD THEORA HEADER ***
	while (bRetrieveHeader) {

		// Start buffering
		buffer = ogg_sync_buffer(&oy, 4096);
		
		iBytesReadFromStream = fileReader.read(buffer, 4096);
		ogg_sync_wrote(&oy, iBytesReadFromStream);

		// Pickup the first pages of the theora header.
		if (iBytesReadFromStream > 0) {
			
			// If we have a page, do something with it.
			while(ogg_sync_pageout(&oy, &og) > 0) {
				ogg_stream_state ossState;

				// Start off by testing if the page is the BOS (Beginning of Stream)
				if (ogg_page_bos(&og)) {

					ogg_stream_init(&ossState, ogg_page_serialno(&og));
					ogg_stream_pagein(&ossState, &og);
					ogg_stream_packetout(&ossState, &op);

					// Identify what codec the packet is from
					if (!tpCount && (theora_decode_header(&ti, &tc, &op) >= 0)) {
						memcpy(&to, &ossState, sizeof(ossState));
						tpCount = 1;

					// If it's not a theora packet, don't care about it. 
					} else {
						ogg_stream_clear(&ossState);
					
					}

				// If not, don't lose it, queue it up.
				} else {
					// Push the page into the theora stream.
					// Even if its not a theora page, it will just 
					//   be dropped so we don't have to worry about
					//   testing it.
					ogg_stream_pagein(&to, &og);
					bRetrieveHeader = false;
					break;		// Break the loop

				}
			}
		}
	}
			
	// When we're done, start parsing non-BOS pages
	// We are expenting more header packets to initialise the theora decoder
	while ((tpCount >= 1) && (tpCount < 3)) {

		// Decode all readily available packets
		while ((tpCount < 3) && ogg_stream_packetout(&to, &op)) {

			// Decode as a header
			theora_decode_header(&ti, &tc, &op);
			++tpCount;
		}

		// Check if a page is available.
		if (ogg_sync_pageout(&oy, &og) > 0) {

			// If there is one, push it into the theora stream.
			ogg_stream_pagein(&to, &og);

		// If not, fetch more data from the stream
		} else {
			buffer = ogg_sync_buffer(&oy, 4096);
			
			iBytesReadFromStream = fileReader.read(buffer, 4096);
			ogg_sync_wrote(&oy, iBytesReadFromStream);
		}
	}
	// *** END LOAD THEORA HEADERS ***


	// *** INITIALIZE THEORA DECODER ***
	// If we've gotten some theora packages, initialise
	// the decoder.
	if (tpCount) {		
		theora_decode_init(&td, &ti);

	// If we can't initialize the decoder,
	// tear everything down.
	} else {
		theora_info_clear(&ti);
		theora_comment_clear(&tc);
	}
	// *** END INITIALIZE THEORA DECODER ***


	//int pp_level = 0;
	////theora_control(&td,TH_DECCTL_GET_PPLEVEL_MAX,&pp_level,sizeof(pp_level));
	//theora_control(&td,TH_DECCTL_SET_PPLEVEL,&pp_level,sizeof(pp_level));





		
	// Use software rendering.
	video::E_DRIVER_TYPE driverType = video::EDT_OPENGL;

	// create device
	device = createDevice(driverType,
		core::dimension2d<irr::s32>(640, 480));

	if (device == 0)
		return 1; // could not create selected driver.

	device->setWindowCaption(L"Irrlicht Engine - 2D Graphics Demo");

	video::IVideoDriver* driver = device->getVideoDriver();

	/*
	All 2d graphics in this example are put together into one texture,
	2ddemo.bmp. Because we want to draw colorkey based sprites, we need to
	load this texture and tell the engine, which part of it should be
	transparent based on a colorkey.

	In this example, we don't tell it the color directly, we just say "Hey
	Irrlicht Engine, you'll find the color I want at position (0,0) on the
	texture.". Instead, it would be also possible to call
	driver->makeColorKeyTexture(images, video::SColor(0,0,0,0)), to make
	e.g. all black pixels transparent. Please note that
	makeColorKeyTexture just creates an alpha channel based on the color.
	*/
	//video::ITexture* images = driver->getTexture("../../media/2ddemo.bmp");
	//driver->makeColorKeyTexture(images, core::position2d<irr::s32>(0,0));

	/*
	To be able to draw some text with two different fonts, we first load
	them. Ok, we load just one. As the first font we just use the default
	font which is built into the engine. Also, we define two rectangles
	which specify the position of the images of the red imps (little flying
	creatures) in the texture.
	*/
	//gui::IGUIFont* font = device->getGUIEnvironment()->getBuiltInFont();
	//gui::IGUIFont* font2 = device->getGUIEnvironment()->getFont("../../media/fonthaettenschweiler.bmp");

	core::rect<irr::s32> imp1(349,15,385,78);
	core::rect<irr::s32> imp2(387,15,423,78);

	
	// We don't do anything to load or initialise video
	// since we won't be using overlays to display it.

	// Try to fetch the first frame.
	// Will most likely have to buffer data

	// *** DECODE FRAMES ***
	bool bFrameFound = false;
	//void* imageData = 0;

	// Create a texture large enough to fit the whole frame
	irr::video::ITexture* renderText = 0;
	irr::video::ITexture* bufferText = 0;
	if (renderText == 0) {
		//renderText = driver->addTexture(irr::core::dimension2d<s32>(yuvFrameData.y_width, yuvFrameData.y_height), "test texture", irr::video::ECF_A8R8G8B8);
		renderText = driver->addTexture(irr::core::dimension2d<s32>(512, 512), "test texture", irr::video::ECF_A8R8G8B8);
		bufferText = driver->addTexture(irr::core::dimension2d<s32>(512, 512), "test texture", irr::video::ECF_A8R8G8B8);
	}


	/*
	Everything is prepared, now we can draw everything in the draw loop,
	between the begin scene and end scene calls. In this example, we are
	just doing 2d graphics, but it would be no problem to mix them with 3d
	graphics. Just try it out, and draw some 3d vertices or set up a scene
	with the scene manager and draw it.
	*/

	//irr::u32 oldFrameTime = 0;
	irr::u32 startPlayBackTime = device->getTimer()->getRealTime();
	irr::u32 currFrameTime = startPlayBackTime;
	while(device->run() && driver)
	{
		if (device->isWindowActive())
		{
			//bRunDecoder = false;
			if (bRunDecoder) {
				currFrameTime = device->getTimer()->getRealTime() - startPlayBackTime;
				

				// Fetch Data
				// Check if we have a page
				while (!(ogg_sync_pageout(&oy, &og) > 0)) {

					// Buffer data
					buffer = ogg_sync_buffer(&oy, 1048576);
						
					iBytesReadFromStream = fileReader.read(buffer, 1048576);
					ogg_sync_wrote(&oy, iBytesReadFromStream);

					
					if (iBytesReadFromStream <= 0) {
						bRunDecoder = false;
						bFrameFound = true;
						break;
					} else {
						bRunDecoder = true;
					}
					
				}

				// *** DECODE FRAMES ***
				while (!bFrameFound) {


					ogg_stream_pagein(&to, &og);

					// If we have a packet ready
					if (ogg_stream_packetout(&to, &op) > 0) {


						int iTmp = -1;
				
						// Get packet and if valid, get video
						iTmp = theora_decode_packetin(&td, &op);

						if (iTmp == 0) {

							// Get frame time data
							// Cheap frame skip
							frameTime = theora_granule_time(&td, td.granulepos);

							if ((unsigned int)(frameTime*1000) < currFrameTime) {
								continue;
							}

							// Frame decode timer
							yuv_buffer yuvFrameData;

							theora_decode_YUVout(&td, &yuvFrameData);
				
							
							// Stop frame decode timer

							bFrameFound = decodeYUVtoRGB(bufferText, &yuvFrameData, ti);


							if (bFrameFound) {
								irr::video::ITexture* textSwapTmp = bufferText;
								bufferText = renderText;
								renderText = textSwapTmp;

							}


							frameDecodeTimerDelaySum += (frameDecodeTimerStop - frameDecodeTimerStart);
							frameDecodeTimerAvg = frameDecodeTimerDelaySum / (++frameDecodeCount);






							// If time to display the frame, display it and buffer the next one.
							//if (((unsigned int)(frameTime * 1000)) < currFrameTime) {
							//	
							//	//if (renderText != 0) {
							//	//	driver->removeTexture(renderText);
							//	//}
							//	//renderText = driver->addTexture("test texture", img);
							//	continue;
							//}


						}

					} else {

						while (!(ogg_sync_pageout(&oy, &og) > 0)) {

							// Buffer data
							buffer = ogg_sync_buffer(&oy, 1048576);
								
							iBytesReadFromStream = fileReader.read(buffer, 1048576);
							ogg_sync_wrote(&oy, iBytesReadFromStream);

							if (iBytesReadFromStream <= 0) {
								bRunDecoder = false;
								bFrameFound = true;
								break;
							} else {
								bRunDecoder = true;
							}
						}
					}
				}
				// *** END DECODE FRAMES ***
			}
			driver->beginScene(true, true, video::SColor(255,120,102,136));
			driver->draw2DImage(renderText, core::position2d<irr::s32>(10, 10), irr::core::rect<irr::s32>(core::position2d<s32>(0, 0), core::position2d<s32>(ti.frame_width, ti.frame_height)));

			
			// draw transparent rect under cursor
			if(device->getCursorControl())
			{
				core::position2d<irr::s32> m = device->getCursorControl()->getPosition();
				driver->draw2DRectangle(video::SColor(100,255,255,255),
					core::rect<irr::s32>(m.X-20, m.Y-20, m.X+20, m.Y+20));
			}
			driver->endScene();			

			bFrameFound = false;
		}
	}

	device->drop();


	//[RED] Insert video clean up code here
	// Copy/Pasted from player_example
	if(tpCount){
		ogg_stream_clear(&to);
		theora_clear(&td);
		theora_comment_clear(&tc);
		theora_info_clear(&ti);
	}
	ogg_sync_clear(&oy);

	//delete img;
	//delete imageData;

	return 0;
}

/*
That's all. I hope it was not too difficult.
**/
