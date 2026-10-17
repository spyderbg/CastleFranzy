#include "stdafx.h"
#include "WiiTextureConv.h"

#ifdef _IRR_USE_WII_DEVICE_

#include <string.h> // for memset

namespace irr
{

void ConvertRGBAtoRGBA8( u8* imgData,  u32 width, u32 height )
{
	// TODO: use shared working buffer?
	int iPaddedSize = ((width + 3) & ~3) * ((height + 3) & ~3) * 4;
	u8* tmpbuff = irrnew u8[ iPaddedSize ];  
	memset( tmpbuff, 0, iPaddedSize );

	TCWriteTplImage_RGBA8 ( imgData, width, height, tmpbuff );

	memcpy( imgData, tmpbuff, iPaddedSize );

	delete[] tmpbuff;
}


/*>*******************************(*)*******************************<*/
// TCPackTile_RGBA8 
// 
// 4x4 tile, 16-bit texels
// x and y represent starting texel position of this tile
// pack AR in low half, GB in high half dest. buffer 
/*>*******************************(*)*******************************<*/ 
//static void TCPackTile_RGBA8 ( TCLayer* colorLayer, TCLayer* alphaLayer, u32 x, u32 y, u8* dstPtr )
void TCPackTile_RGBA8 ( u8* rgba_data, u32 width, u32 height, u32 x, u32 y, u8* dstPtr )
{
	u16 ria, a;
	u32 row, col;
	u32 realRows, realCols;
	u8* arPtr, *gbPtr;
	u8 g, b;
	

	// dstPtr is already zeroed out, so this will take care of padding issue
	// 'realRows', 'realCols' represent actual source image texels remaining
	//realRows = colorLayer->height - y;
	//realCols = colorLayer->width  - x;
	realRows = height - y;
	realCols = width  - x;
	
	if( realRows > 4)    
		realRows = 4;
		
	if(realCols > 4)
		realCols = 4;
			
	// pack 2 32B tiles
	for(row=0; row<realRows; row++)
	{	
															// pack 2 cache lines at once
		arPtr = dstPtr  +      (row * 8);                   // move 8 bytes (4 16-bit texels) per row
		gbPtr = dstPtr  + 32 + (row * 8);                   // need to reset ptr each row to account for
		                                                    // column padding

		for(col=0; col<realCols; col++)
		{               
			//TCGetLayerValue( colorLayer, (x+col), (y+row), &ria, &g, &b );
			//
			//if(alphaLayer)                                  
			//{
			//	TCGetLayerValue( alphaLayer, (x+col), (y+row), &a, 0, 0 );			
			//}	
			//else  // set to max. alpha
			//{
			//	a = 0xFF;
			//}
			u8* c = rgba_data + ( (y+row) * width + (x+col) ) * 4 ;
#ifdef __BIG_ENDIAN__
			ria = c[1];
			g   = c[2];
			b   = c[3];
			a   = c[0];
#else
			ria = c[2];
			g   = c[1];
			b   = c[0];
			a   = c[3];
#endif
				
			*arPtr       = (u8)a;                           // alpha is byte 0, red is byte 1
			*(arPtr + 1) = (u8)ria;
			
			*gbPtr       = g;                               // green is byte 0, blue is byte 1
			*(gbPtr + 1) = b;
			
			arPtr += 2;
			gbPtr += 2;

		} // end for col loop			
	} // end for row loop						
}

/*>*******************************(*)*******************************<*/ 
// WriteTplImage_RGBA8
//
// convert from layer to final hw format
// 2 4x4 texel tiles @ 8B per tile row, 32B per tile, 2 cache lines (64B) total
//
// AR tiles are stored contiguously in the low half of image->tplBuffer;
// GB tiles are stored contiguously in the high half of image->tplBuffer;
//
// note:  source color layer must be in the format LY_IMAGE_COLOR_RGB24.
//        if no alpha layer found, set all alphas to max. value
/*>*******************************(*)*******************************<*/ 
//void TCWriteTplImage_RGBA8 ( TCLayer* colorLayer, TCLayer* alphaLayer, u8* tplBuffer )
void TCWriteTplImage_RGBA8 ( u8* rgba_data, u32 width, u32 height, u8* tplBuffer )
{	
	u32 numTileRows, tileRow;	
	u32 numTileCols, tileCol;
	u8* dstPtr;
	//u32 width, height;


	//width  = colorLayer->width;
	//height = colorLayer->height;
 	
	// number of 4x4 texel tile cols, rows including any partial tiles
	numTileCols = ((width  + 3) >> 2);
	numTileRows = ((height + 3) >> 2);
	
	dstPtr  = tplBuffer;
	
	// numTileRows, numTileCols includes any partial tiles
	for( tileRow=0; tileRow<numTileRows; tileRow++ )
	{
		for(tileCol=0; tileCol<numTileCols; tileCol++)
		{						
			//TCPackTile_RGBA8( colorLayer, alphaLayer, (tileCol * 4), (tileRow * 4), dstPtr );
			TCPackTile_RGBA8( rgba_data, width, height, (tileCol * 4), (tileRow * 4), dstPtr );
			dstPtr  += 64;                  // move to next 2 (32B) cache lines
		}			
	} 
}
	///
	///
	///
	///

}
#endif //_IRR_COMPILE_WITH_WII_