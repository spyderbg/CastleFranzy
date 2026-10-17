///////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Core/irrlicht.h"
#include "Core/Application.h"

///////////////////////////////////////////////////////////////////////

#define DEBUG_SYSTEM			1

#define ESCAPE_CHARACTER_PREFIX L'^'

const f32 SPRITE_SCALE_3D		= 1/43.0f;

//////////////////////////////////////////////////

#if DEBUG_SYSTEM

	extern void SpriteList_Dump();

#endif

//////////////////////////////////////////////////
// Frames/Anims flags...

#define FLAG_ALPHA( a )			( (0xFF-((a)&0xFF)) << 16 )
#define FLAG_ALPHA_REV( a )		(0xFF-(((a)>>16)&0xFF))
#define DEFAULT_FRAME_ALPHA		0xFF

class ASprite
{
	// ASprite_Constants.hxx - phone specific constants
	////////////////////////////////////////////////////////////////////////////////////////////////////

	static const s32 MAX_SPRITE_PALETTES	= 16;
	static const s32 MAX_MODULE_MAPPINGS	= 16;
	static const s32 MAX_FLIP_COUNT			= 8;

	static const s32 TMP_BUFFER_SIZE		= 10 * 1024;
	static const s32 PNG_BUFFER_SIZE		= 10 * 1024;

public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	static const bool USE_HYPER_FM = true;				// use Hyper Frame Modules
	static const bool USE_INDEX_EX_FMODULES = true;		// true|false -> max. 1024|256 modules refs. from a FModule
	static const bool USE_INDEX_EX_AFRAMES = true;		// true|false -> max. 1024|256 frames refs. from an Anim

	//////////////////////////////////////////////////
private:
	static const short BSPRITE_v003	= (short)0x03DF; // supported version
	static const short BSPRITE_v004	= (short)0x04DF; // supported version
	static const short BSPRITE_v005	= (short)0x05DF; // supported version

	//////////////////////////////////////////////////
	// BSprite flags

	static const s32 BS_MODULES			= (1 << 0);
	static const s32 BS_MODULES_XY		= (1 << 1);
	static const s32 BS_MODULES_IMG		= (1 << 2);
	static const s32 BS_MODULES_WH_SHORT	= (1 << 4);		// export w, h for each module as short
	static const s32 BS_MODULES_XY_SHORT	= (1 << 5);		// export x, y for each module as short
	static const s32 BS_FRAMES			= (1 << 8);
	static const s32 BS_FM_OFF_SHORT	= (1 << 10);    // export fm offsets as shorts
	static const s32 BS_NFM_1_BYTE		= (1 << 11);    // export nfm as byte
	static const s32 BS_SKIP_FRAME_RC	= (1 << 12);    // do not export frame rect
	static const s32 BS_FRAME_COLL_RC	= (1 << 13);	// export collision rect
	static const s32 BS_FM_PALETTE		= (1 << 14);	// export palette used by the module
	static const s32 BS_FRAME_RECTS		= (1 << 15);	// export frame rects
	static const s32 BS_ANIMS			= (1 << 16);
	static const s32 BS_AF_OFF_SHORT	= (1 << 18);    // export af offsets as shorts
	static const s32 BS_NAF_1_BYTE		= (1 << 19);    // export naf as byte
	static const s32 BS_MODULE_IMAGES	= (1 << 24);
	static const s32 BS_PNG_CRC			= (1 << 25);
	static const s32 BS_KEEP_PAL		= (1 << 26);
	static const s32 BS_TRANSP_FIRST	= (1 << 27);
	static const s32 BS_TRANSP_LAST		= (1 << 28);
	static const s32 BS_SINGLE_IMAGE	= (1 << 29);
	static const s32 BS_MODULE_USAGE	= (1 << 30);

	static const s32 BS_DEFAULT_DOJA	= (BS_MODULES | BS_FRAMES | BS_ANIMS);
	static const s32 BS_DEFAULT_MIDP2	= (BS_MODULES | BS_FRAMES | BS_ANIMS | BS_MODULE_IMAGES);
	static const s32 BS_DEFAULT_NOKIA	= BS_DEFAULT_MIDP2;
	static const s32 BS_DEFAULT_MIDP1	= (BS_MODULES | BS_MODULES_XY | BS_FRAMES | BS_ANIMS);
	static const s32 BS_DEFAULT_MIDP1b	= (BS_MODULES | BS_FRAMES | BS_ANIMS | BS_MODULE_IMAGES | BS_PNG_CRC);
	static const s32 BS_DEFAULT_MIDP1c	= (BS_MODULES | BS_MODULES_XY | BS_FRAMES | BS_ANIMS | BS_SINGLE_IMAGE);

	static const s32 BS_FM_FREE_ROTATE_SCALE    = (1 << 0);
	//////////////////////////////////////////////////

	static const short PIXEL_FORMAT_8888		= (short)0x8888;
	static const short PIXEL_FORMAT_4444		= (short)0x4444;
	static const short PIXEL_FORMAT_1555		= (short)0x5515;
	static const short PIXEL_FORMAT_0565		= (short)0x6505;

	//////////////////////////////////////////////////

	static const short ENCODE_FORMAT_I2			= 0x0200;
	static const short ENCODE_FORMAT_I4			= 0x0400;
	//	const short ENCODE_FORMAT_I8			= 0x0800;
	static const short ENCODE_FORMAT_I16		= 0x1600;
	//	const short ENCODE_FORMAT_I16MP		= 0x16??;
	//	const short ENCODE_FORMAT_I32		= 0x3200;
	//	const short ENCODE_FORMAT_I64		= 0x6400;
	//	const short ENCODE_FORMAT_I128		= 0x2801;
	static const short ENCODE_FORMAT_I256		= 0x5602;
	//	const short ENCODE_FORMAT_I127_		= 0x2701;
	static const short ENCODE_FORMAT_I64RLE		= 0x64F0;
	static const short ENCODE_FORMAT_I127RLE	= 0x27F1;
	static const short ENCODE_FORMAT_I256RLE	= 0x56F2;
public:
	//////////////////////////////////////////////////
	// Frames/Anims flags...

	static const u8 FLAG_FLIP_X	= 0x01;
	static const u8 FLAG_FLIP_Y	= 0x02;
	static const u8 FLAG_ROT_90	= 0x04;
	static const u8 FLAG_FREE_ROT	= 0x08;

	static const u8 FLAG_USER0	= 0x10; // user flag 0
	static const u8 FLAG_USER1	= 0x20; // user flag 1

	static const u8 FLAG_HYPER_FM	= 0x10; // Hyper FModule, used by FModules

	// Index extension...
	static const s32 FLAG_INDEX_EX_MASK = 0xC0; // 11000000, bits 6, 7
	static const s32 INDEX_MASK			= 0x03FF; // max 1024 values
	static const s32 INDEX_EX_MASK		= 0x0300;
	static const s32 INDEX_EX_SHIFT 	= 2;

	//////////////////////////////////////////////////
	// flags passed as params...

	static const u8 FLAG_OFFSET_FM = 0x10;
	static const u8 FLAG_OFFSET_AF = 0x20;

	//////////////////////////////////////////////////

	static const s32 	BOTTOM 	=	32;
	static const s32 	HCENTER =	1;
	static const s32 	LEFT 	=	4;
	static const s32 	RIGHT 	=	8;
	static const s32 	TOP 	=	16;
	static const s32 	VCENTER =	2;

	//USE_EXTERN_IMAGE
private:
	bool Load(byte * rawdata);
	bool LoadSprite(const char * spriteName);

public:
	bool Load(const char * name);
	bool LoadTexture(const char * textureName);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	s32 GetAFrameTime(s32 anim, s32 aframe)
	{
		//	s32 af = (_anims_af_start[anim] + aframe);
		//	return _aframes[((af<<2) + af + 1)] & 0xFF;

		return _aframes[(_anims_af_start[anim] + aframe) * 5 + 1] & 0xFF;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	s32 GetAFrames(s32 anim)
	{
		return _anims_naf[anim]&0xFF;
		//	return _anims_naf[anim]&0xFFFF;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	s32 GetFModules(s32 frame)
	{
		//return _frames_nfm[frame]&0xFF;
		return _frames_nfm[frame]&0xFFFF;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	s32 GetModuleWidth(s32 module)
	{
		return _modules_w[module]&0xFF;
	}

	s32 GetModuleHeight(s32 module)
	{
		return _modules_h[module]&0xFF;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	short GetFrameLeft(s32 frame)
	{
		if ((bs_flags & BS_FM_OFF_SHORT) != 0) 
			return (_frames_rc[(frame<<3) + 0] & 0xFF) | (_frames_rc[(frame<<3) + 1] << 8);
		else
			return _frames_rc[(frame<<2) + 0];
	}

	short GetFrameTop(s32 frame)
	{
		if ((bs_flags & BS_FM_OFF_SHORT) != 0) 
			return (_frames_rc[(frame<<3) + 2] & 0xFF) | (_frames_rc[(frame<<3) + 3] << 8);
		else
			return _frames_rc[(frame<<2) + 1];
	}
    s32 GetFontFrameLeft(s32 frame);
    s32 GetFontFrameWidth(s32 frame);
	s32 GetFrameWidth(s32 frame)
	{
		if ((bs_flags & BS_FM_OFF_SHORT) != 0) 
			return (_frames_rc[(frame<<3) + 4] & 0xFF) | (_frames_rc[(frame<<3) + 5] << 8);
		else
			return _frames_rc[(frame<<2) + 2]&0xFF;
	}

	s32 GetFrameHeight(s32 frame)
	{
		if ((bs_flags & BS_FM_OFF_SHORT) != 0) 
			return (_frames_rc[(frame<<3) + 6] & 0xFF) | (_frames_rc[(frame<<3) + 7] << 8);
		else
			return _frames_rc[(frame<<2) + 3]&0xFF;
	}

	p2i GetFramePos (s32 frame)
	{
		return p2i(GetFrameLeft(frame), GetFrameTop(frame));
	}

	d2i GetFrameSize (s32 frame)
	{
		return d2i(GetFrameWidth(frame), GetFrameHeight(frame));
	}

	r2i GetFrameRect (s32 frame)
	{
		return r2i(GetFramePos(frame), GetFrameSize(frame));
	}



    s32 GetFrameColLeft(s32 frame)
    {
        return _frames_col[frame*4 + 0];
    }

    s32 GetFrameColTop(s32 frame)
    {
        return _frames_col[frame*4 + 1];
    }
    s32 GetFrameColWidth(s32 frame)
    {
        return _frames_col[frame*4 + 2]&0xFF;
    }
    s32 GetFrameColHeight(s32 frame)
    {
        return _frames_col[frame*4 + 3]&0xFF;
    }

	////////////////////////////////////////////////////////////////////////////////////////////////////

    u8 GetFrameRectsNum( u32 frame )
    {
		if( (bs_flags & BS_FRAME_RECTS) == 0 )
			return 0;
		
		if( frame > GetFramesNum() )
			return 0;

        return _frames_frect_num[frame];
    }

    bool GetFrameRect( r2i &rect, u32 frame, u8 rect_idx )
    {
		if( (bs_flags & BS_FRAME_RECTS) == 0 )
			return false;
		
		u8 rects_num = GetFrameRectsNum( frame );
		if( rects_num == 0 || rects_num <= rect_idx )
			return false;				
		
		s16* frame_rect_ptr;
		
		frame_rect_ptr = &( _frame_rects[(_frames_frect_start[frame] + rect_idx) << 2] );

		rect.UpperLeftCorner.X = frame_rect_ptr[0];
		rect.UpperLeftCorner.Y = frame_rect_ptr[1];
		rect.LowerRightCorner.X = frame_rect_ptr[2] + frame_rect_ptr[0];
		rect.LowerRightCorner.Y = frame_rect_ptr[3] + frame_rect_ptr[1];

        return true;
    }

	////////////////////////////////////////////////////////////////////////////////////////////////////

	s32 GetFrames()
	{
		return _nFrames;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	s32 GetAnimFrame(s32 anim, s32 aframe)
	{
		if( anim < 0 || aframe < 0 )
			return -1;

		s32 off = (_anims_af_start[anim] + aframe) * 5;
		return _aframes[off]&0xFF;
		//USE_SINGLE_ARRAY_FOR_FM_AF
		//USE_SINGLE_ARRAY_FOR_FM_AF
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	s32 GetFModuleOffX(s32 frame, s32 fmodule);
	s32 GetFModuleOffY(s32 frame, s32 fmodule);

	s32 GetFModuleWidth(s32 frame, s32 fmodule);
	s32 GetFModuleHeight(s32 frame, s32 fmodule);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	void PaintAFrame(s32 anim, s32 aframe, s32 posX, s32 posY, s32 flags, s32 hx, s32 hy, s32 alpha = 255, bool is3d = false);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	void PaintFrame(s32 frame, s32 posX, s32 posY, s32 flag_alpha = 255, bool is3d = false)
	{
		PaintFrame( frame, posX, posY, 0, 0, 0, flag_alpha, is3d );
	}

	void PaintFrame(s32 frame, s32 posX, s32 posY, s32 flags, s32 hx, s32 hy, s32 alpha = 255, s32 rotate = 0, s32 xscale = 100, s32 yscale = 100, bool is3d = false)
	{
		if (alpha == 0) return;
		//	System.out.println("PaintFrame(g, "+frame+", "+posX+", "+posY+", 0x"+Integer.toHexString(flags)+", "+hx+", "+hy+")");
		//s32 nFModules = _frames_nfm[frame]&0xFF;

		s32 nFModules = _frames_nfm[frame]&0xFFFF;
		for (s32 fmodule = 0; fmodule < nFModules; fmodule++)
			PaintFModule(/*g,*/ frame, fmodule, posX, posY, flags, hx, hy,alpha,rotate,xscale,yscale, is3d);
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	void PaintFModule(s32 frame, s32 fmodule, s32 posX, s32 posY, s32 flags, s32 hx, s32 hy, s32 alpha = 255, s32 rotate = 0, s32 xscale = 100, s32 yscale = 100, bool is3d = false);
	
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void PaintModule(/*Graphics g,*/ s32 module, s32 posX, s32 posY, s32 flags, s32 rotate = 0, s32 xscale = 100, s32 yscale = 100, s32 alpha = 255, bool is3d = false);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 3d stuff

	void PaintAFrame3D(s32 anim, s32 aframe, const core::vector3df& pos, s32 flags, s32 hx = 0, s32 hy = 0, f32 xscale = SPRITE_SCALE_3D, f32 yscale = SPRITE_SCALE_3D);
	void PaintFrame3D(s32 frame, const core::vector3df& pos, s32 flags, s32 hx = 0, s32 hy = 0, f32 xscale = SPRITE_SCALE_3D, f32 yscale = SPRITE_SCALE_3D);
	//void PaintFModule3D(s32 frame, s32 fmodule, const core::vector3df& pos, s32 flags, s32 hx = 0, s32 hy = 0, f32 xscale = SPRITE_SCALE_3D, f32 yscale = SPRITE_SCALE_3D);
	//void PaintModule3D(s32 module, const core::vector3df& pos, s32 flags, s32 hx = 0, s32 hy = 0, f32 xscale = SPRITE_SCALE_3D, f32 yscale = SPRITE_SCALE_3D);


	inline s32 GetModulesNum()				{ return _nModules; }
	inline s32 GetFramesNum()				{ return _nFrames; }
	inline s32 GetAnimsNum()				{ return _nAnims; }

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Draw String System...

private:
	void DRAW_STRINGS_SYSTEM___() {}
	s32 _space_width;
	s32 _line_height;
	s32 _char_spacing;
	s32 _line_spacing;
	////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	//	_modules_w[0] -> w  -> width of the space character (' ')
	//	_modules_h[0] -> h  -> height of a text line
	//	_fmodules[0*4+1] -> ox -> space between two adiacent chars
	//	_fmodules[0*4+2] -> oy -> base line offset

	// Used to gather dimensions of a string...
	// (call UpdateStringSize() to update these values)
	static s32 _text_w;
	static s32 _text_h;

	// Maps an ASCII char to a sprite FModule...
//	byte[] _map_char;// = new byte[256]; NEEDS TO BE LOADED FROM RESOURCES !!!

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Space between two lines of text...

	s32  GetLineSpacing()				{ return _line_spacing; }
	void SetLineSpacing(s32 spacing)	{ _line_spacing = spacing; }
	void SetLineSpacingToDefault()		{ _line_spacing = ((_modules_h[0]&0xFF) >> 1); }
	void SetCharSpacing(s32 spacing) { _char_spacing = spacing; }
    s32 GetCharSpacing(s32 index);
	////////////////////////////////////////////////////////////////////////////////////////////////////

/*
	static short[] _sizes = new short[100];

	short[] WraptextB(String s, s32 width, s32 height);
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void DrawPageB(Graphics g, String s, short[] sizes, s32 x, s32 y, s32 startLine, s32 maxLines, s32 anchor);
	////////////////////////////////////////////////////////////////////////////////////////////////////
*/
	static s32 _index1;
	static s32 _index2;

	static void SetSubString(s32 i1, s32 i2)
	{
		_index1 = i1;
		_index2 = i2;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	s32 UpdateStringSize(TChar* s);
	s32 GetStringLength(TChar* s)
	{
		UpdateStringSize(s);
		return _text_w;
	}

	s32 GetPageHeight(TChar* s,s32 width);
	////////////////////////////////////////////////////////////////////////////////////////////////////

	s32 _old_pal;

	s32 DrawString(/*Graphics g, */TChar* s, s32 x, s32 y, s32 anchor ,s32 alpha = 255,s32 rotate = 0, s32 x_scale = 100 , s32 y_scale = 100, bool WB = false,bool WH = false);
	////////////////////////////////////////////////////////////////////////////////////////////////////

/*
	static s32 StringTokenize(String s, s32 index1, s32 index2, char token, s32[] indices)
	{
		s32 lines = 0;
		indices[0] = index1 - 1;
		for (s32 i = index1; i < index2; i++)
			if (s.charAt(i) == token)
				indices[++lines] = i;
		indices[++lines] = index2;
		return lines;
	}
*/
	static const s32  MAX_LINES_PER_PAGE = 20;
	static s32 lines;
	static s32 off[MAX_LINES_PER_PAGE];

	void UpdatePage(TChar* s, s32 width);
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void DrawPage(/*Graphics g,*/ TChar* s, s32 x, s32 y, s32 anchor,bool WB = false)
	{
		DrawPage(/*g,*/ s, x, y, /*SCREEN_WIDTH*/480, 0, 0, -1, anchor, 0, true,WB);
	}
	s32 GetCharacterNumber(s32 start, s32 line_number)
	{
		s32 iReturn = 0;
		for (s32 j = start; j < (start + line_number > lines ? lines : start + line_number); j++)
		{
			iReturn += off[j];
		}
		return iReturn;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////

	s32 DrawPage(/*Graphics g, */TChar* s, s32 x, s32 y, s32 width, s32 start, s32 line_number, s32 character_number, s32 anchor, s32 type, bool bUpdate, bool WB = false);


	////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	void SetCurrentPalette(s32 pal)		{ /*if (pal < _palettes)*/ _crt_pal = pal; }
	s32 GetCurrentPalette()				{ return _crt_pal; }

	////////////////////////////////////////////////////////////////////////////////////////////////////

	/*
	void SetColor(s32 index, short color)
	{
	//	_font_color =
	_pal[_crt_pal][index] = color;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	void SetColor(s32 index, s32 color)
	{
	//	_font_color =
	_pal[_crt_pal][index] = (short)(((((color & 0xFF000000)>>24) & 0xF0)<<8) |
	((((color & 0x00FF0000)>>16) & 0xF0)<<4) |
	((((color & 0x0000FF00)>>8 ) & 0xF0)   ) |
	((((color & 0x000000FF)    ) & 0xF0)>>4));
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	void SetColor(s32 index, s32 a, s32 r, s32 g, s32 b)
	{
	//	_font_color =
	_pal[_crt_pal][index] = (short)(((a & 0xF0)<<8) |
	((r & 0xF0)<<4) |
	((g & 0xF0)   ) |
	((b & 0xF0)>>4));
	}
	*/

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Palette generation based on other palette...

	// -1 - original colors
	//  0 - invisible (the sprite will be hidden)
	//  1 - red-yellow
	//  2 - blue-cyan
	//  3 - green
	//  4 - grey

/*
	static s32[] GenPalette(s32 type, s32 pal[])
	{
		if (type <  0) return pal;	// original
		if (type == 0) return null; // invisible

		s32[] new_pal = new s32[pal.length];
		switch (type)
		{
			/ *			case 1: // red - yellow
			for (s32 i = 0; i < pal.length; i++)
			new_pal[i] = (pal[i] | 0x00FF3300) & 0xFFFFFF00;
			break;

			case 2: // blue - cyan
			for (s32 i = 0; i < pal.length; i++)
			new_pal[i] = (pal[i] | 0x000033FF) & 0xFF00FFFF;
			break;

			case 3: // green
			for (s32 i = 0; i < pal.length; i++)
			new_pal[i] = (pal[i] | 0x00000000) & 0xFF00FF00;
			break;
			* /
		case 4: // grey (desaturate)
			for (s32 i = 0; i < pal.length; i++)
			{
				s32 a = (pal[i] & 0xFF000000);
				s32 r = (pal[i] & 0x00FF0000) >> 16;
				s32 g = (pal[i] & 0x0000FF00) >> 8;
				s32 b = (pal[i] & 0x000000FF);
				s32 l = ((r + b + g) / 3) & 0x000000FF;
				new_pal[i] = ((l << 16) | (l << 8) | l | a);
			}
			break;
			/ *
			case 5: // blend with black 50%
			for (s32 i = 0; i < pal.length; i++)
			{
			s32 a = (pal[i] & 0xFF000000);
			s32 r = (pal[i] & 0x00FC0000) >> 2;
			s32 g = (pal[i] & 0x0000FC00) >> 2;
			s32 b = (pal[i] & 0x000000FC) >> 2;
			new_pal[i] = (a | r | g | b);
			}
			break;
			* /		}

		return new_pal;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////

	void ModifyPaletteAlpha (s32 palNb, short alpha)
	{
		alpha = (short)((alpha<<4)/100);

		for (s32 i=0;i<_pal[palNb].length;i++)
		{
			if ((_pal[palNb][i]&0x0fff) != 0x0f0f)               
				_pal[palNb][i] = (short)( ( (alpha&0xf) << 12) | (_pal[palNb][i] &0x0fff));
		}
	}
*/

	////////////////////////////////////////////////////////////////////////////////////////////////////

// 	static s32  temp[] = new s32 [TMP_BUFFER_SIZE];

	//////////////////////////////////////////////////

	// Modules...
	s32			_nModules;			// number of modules
	short*		_modules_x;			// x for each module [BS_MODULES_XY_SHORT]
	short*		_modules_y;			// y for each module [BS_MODULES_XY_SHORT]
	short*		_modules_w;			// width for each module
	short*		_modules_h;			// height for each module

	// Frames...
	s32			_nFrames;			// number of frames
//	byte*		_frames_nfm;		// number of FModules (max 256 FModules/Frame)
	short*		_frames_nfm;		// number of FModules (max 65536 FModules/Frame)
	short*		_frames_fm_start;	// index of the first FModule
	byte*		_frames_rc;			// frame bound rect (x y width height)
	byte*		_frames_col;		// collision rect for each frame (x y width height)
	u16*		_frames_frect_start;// frect start index for frame
	u8*			_frames_frect_num;	// frect number for frame
	
	// FModules...

	s32 nFModules;
	byte*		_nFModules;				// 4 bytes for each FModule
//	s32			___nFrames;
	s16*		_frame_rects;		// frame rects for frames
	//USE_SINGLE_ARRAY_FOR_FM_AF
	//USE_SINGLE_ARRAY_FOR_FM_AF

	//byte*		_fmodules_id;		// 0 : module index
	//short*		_fmodules_ox;			// 1 : ox
	//short*		_fmodules_oy;			// 2 : oy
	//byte*		_fmodules_flags;		// 3 : flags

	// Anims...
	short*		_anims_naf;			// number of AFrames (max 256 AFrames/Anim)
	//	short[]		_anims_naf;			// number of AFrames (max 65536 AFrames/Anim)
	short*		_anims_af_start;	// index of the first AFrame

	// AFrames...

	s32	_nAFrames;
	byte*		_aframes;			// 5 bytes for each AFrame
	//USE_SINGLE_ARRAY_FOR_FM_AF
	//USE_SINGLE_ARRAY_FOR_FM_AF

	//	byte[]		_aframes_frame;			// 0 : frame index
	//	byte[]		_aframes_time;			// 1 : time
	//	byte[]		_aframes_ox;			// 2 : ox
	//	byte[]		_aframes_oy;			// 3 : oy
	//	byte[]		_aframes_flags;			// 4 : flags
	s32	_nAnims;

	// Module mappings...
// 	s32[][]		_map; 				// all mappings
// 	//	s32			_mappings;			// number of mapings
// 	private s32	_cur_map;			// current mapping

	// Palettes...
	//	short 		_pixel_format;		// always converted to 8888
	s32			bs_flags;			// sprite format
	s32			ex_bs_flags;			// sprite format

// 	s32  [][]	_pal; 				// all palettes

// 	byte[][]	_transp;			// all transparences (A)
// 	byte[]		_pal_data; 			// palettes data for BS_DEFAULT_MIDP1c
// 	s32			_palettes;			// number of palettes
// 	s32			_colors;			// number of colors
	s32 _crt_pal;			// current palette
// 	bool		_alpha;				// has transparency ?
	//	s32			_flags;				// transparency, etc.

	// Graphics data (for each module)...
	//	Image[]		_main_image;		// an image with all modules, for each palette
// 	short 		_data_format;
// 	s32			_i64rle_color_mask;	// used by ENCODE_FORMAT_I64RLE
// 	s32			_i64rle_color_bits;	// used by ENCODE_FORMAT_I64RLE
// 	byte[]		_modules_data;		// encoded image data for all modules
// 	short[]		_modules_data_off;	// offset for the image data of each module
	//	s32[][]		_modules_data2;		// cashe image data (decoded)

	//USE_DYNAMIC_PNG

/*
	Image [][]	_modules_image;		// cache image for each module / with each palette

	//USE_NOKIA_UI

	//	Image[]		_modules_image_fx;	// cache image for each module (flipped horizontally)
	Image[]		_main_image;		// an image with all modules, for each palette
*/
	//ResTex3d*	_modules_image;
	ITexture*	_modules_image;
	// CRC data for creating a valid PNG stream...
// 	s32[] _PNG_packed_PLTE_CRC;		// for each palette
// 	s32[] _PNG_packed_tRNS_CRC;		// for each palette
// 	s32[] _PNG_packed_IHDR_CRC;		// for each module
// 	s32[] _PNG_packed_IDAT_ADLER;	// for each module
// 	s32[] _PNG_packed_IDAT_CRC;		// for each module

// 	static s32			_images_count;
// 	static s32			_images_size;

	//3d stuff
	u16 indices[6];
	video::SMaterial*	_material;
	video::SColor		_color;
	video::S3DVertex	vertices[4];
	float				_fmodules_correction;

	short* _map_char;

	void LoadCharacterMap();
	s32 GetCharFrame(TChar c);
	s32 GetCharacterWidth(TChar c);
	void InitSpaceSizeWithAuroraValue();
	
	s32 StrLength(TChar* s);

	////////////////////////////////////////////////////////////////////////////////////////////////////

	ASprite();
	~ASprite();
	////////////////////////////////////////////////////////////////////////////////////////////////////
	// ASprite debug constants

	static const bool DEBUG_bDbgS			= false;
	static const bool DEBUG_bErr				= false;
	static const bool DEBUG_bDbgVM			= false;
	static const bool DEBUG_bDbgO			= false;
	static const bool DEBUG_bEmu				= false;
	static const bool DEBUG_bDbgI			= false;
	static const bool DEBUG_bDbgModuleUsage	= false;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// TODO: Spyder
	//======================================================================
	// ANIMATIONS
	s32 StartAnimation(s32 animIndex, s32 anim, bool loop) { return 0; }
	s32 StartAnimation(s32 animIndex, s32 anim, s32 loop_count) { return 0; }
	void FreeAnimation(s32 animIndex) {}

	void ChangeAnimation(s32 animIndex, s32 anim, bool loop) {}
	void ChangeAnimation(s32 animIndex, s32 anim, s32 loop_count) {}

	s32 PlayAnimation(s32 animIndex, s32 posX, s32 posY, s32 flags) { return 0; }
	s32 UpdateAnimation(s32 animIndex) { return 0; }
	s32 PaintAndUpdateAnimation(s32 animIndex, s32 posX, s32 posY, s32 flags) { return 0; }

	bool IsAnimationDone(s32 animIndex) { return true; }
	s32 GetCurrentAnimationFrame(s32 animIndex) { return 0; }
	s32 GetCurrentAnimationFrameID(s32 animIndex) { return 0; } 
	s32 GetCurrentAnimationPos(s32 animIndex) { return 0; }
	s32 GetCurrentAnimation(s32 animIndex) { return 0; }

	void SaveAnimations(s32* data_buffer, s32& data_pos) {}
	void LoadAnimations(s32* data_buffer, s32& data_pos) {}


	////////////////////////////////////////////////////////////////////////////////////////////////////
	// 2D transformation
public:
	static void PushTransform(vector3df pivot, vector3df scale, vector3df rotation = vector3df(0,0,0));
	static void PushTranslation(vector3df offset);

	static void PushTransform(matrix4 & m);
	static void PopTransform();

private:
	static array<matrix4>	s_transformStack;

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// Clip Rect
public:
	static void PushClipRect(r2i r);
	static void PopClipRect();

	static inline void PushClipRect(p2i pos, d2i size) { PushClipRect(r2i(pos, size)); }

private:
	static array<r2i>		s_clipRectStack;
	////////////////////////////////////////////////////////////////////////////////////////////////////

}; // class ASprite
