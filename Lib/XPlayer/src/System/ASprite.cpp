#include "stdafx.h"
#include "ASprite.h"
//#include "define.h"
//#include "DEF.h"
#include "system/Image.h"
#include "Game.h"
//#include "ResourceHeader/dict.h"
//#if IPHONE > 0
#include "system/ResourceManager.h"
//#endif

typedef char * String ;


//#define DEFAULT_LINE_SPACING -100 (using wu xiao xi define)
#define ARGB_2_RGB565(clr)    ((clr >> 8 ) & 0xf800) | ((clr >> 5 ) & 0x7e0) | ((clr >> 3 ) & 0x1f)

//change the color as you need
const	RGBVAL BrewStringColor[] = { 0xFFFFFF00, //White
									 0xFF000000, //Blue
									 0x00FF0000, //Green
									 0x0000FF00, //Red
									 0x00000000, //Black
									 0xDDDDEE00, //PINK
									 0x11112200, //BROWN
									 0x00CFFF00, //GOLD
									 0x00005200  //GOLDBROWN
									}; //the color is BGR+Reserved

const char img_prox[][10] =
{
	"",
	"_Black",
	"_red",
	"_green",
};

void arraycopy( char * src, int srcPos, char * dest, int destPos, int length )
{
	MEMCPY( &dest[destPos], &src[srcPos], length * sizeof(char) );
}

ASprite::ASprite()
: _palettes(0)
, _frames_coll(NULL)
, _map(NULL)
, _pal(NULL)
, _modules_data_off(NULL)
, _modules_data(NULL)
, _precalc_stack(NULL)
, _anims_naf(NULL)
, _anims_af_start(NULL)
, _aframes(NULL)
, _need_release(false)
, _image(NULL)
{
	_image_array[0] = _image_array[1] = _image_array[2] = _image_array[3] = NULL;
	_line_spacing = 1;
	_char_spacing = 0;
	_crt_pal = 0;
	_current_animation = NEW int[MAX_ANIMATION_INSTANCES];  
	_current_frame = NEW int[MAX_ANIMATION_INSTANCES];
	_current_frame_time = NEW int[MAX_ANIMATION_INSTANCES];
	_is_looping = NEW bool[MAX_ANIMATION_INSTANCES];
	//m_cGame = (( CGame* )(GetGameApp()->m_engine)) ;
}

ASprite::ASprite(char file[], int offset)
: _palettes(0)
, _frames_coll(NULL)
, _map(NULL)
, _pal(NULL)
, _modules_data_off(NULL)
, _modules_data(NULL)
, _precalc_stack(NULL)
, _anims_naf(NULL)
, _anims_af_start(NULL)
, _aframes(NULL)
, _need_release(false)
, _image(NULL)
{
	String_InPage = true;
	_crt_pal = 0;
	_line_spacing = 1;
	_char_spacing = 0;
	_current_animation = NEW int[MAX_ANIMATION_INSTANCES];  
	_current_frame = NEW int[MAX_ANIMATION_INSTANCES];
	_current_frame_time = NEW int[MAX_ANIMATION_INSTANCES];
	_is_looping = NEW bool[MAX_ANIMATION_INSTANCES];
	//m_cGame = (( CGame* )(GetGameApp()->m_engine)) ;
	Load(file, offset);
}

ASprite::ASprite(const char filename[], int imgNum,bool FLIPV)
: _palettes(0)
, _frames_coll(NULL)
, _map(NULL)
, _pal(NULL)
, _modules_data_off(NULL)
, _modules_data(NULL)
, _precalc_stack(NULL)
, _anims_naf(NULL)
, _anims_af_start(NULL)
, _aframes(NULL)
, _need_release(false)
, _image(NULL)
{
	String_InPage = true;
	_crt_pal = 0;
	_line_spacing = 1;
	_char_spacing = 0;
	_current_animation = NEW int[MAX_ANIMATION_INSTANCES];  
	_current_frame = NEW int[MAX_ANIMATION_INSTANCES];
	_current_frame_time = NEW int[MAX_ANIMATION_INSTANCES];
	_is_looping = NEW bool[MAX_ANIMATION_INSTANCES];
	//m_cGame = (( CGame* )(GetGameApp()->m_engine)) ;
	for( int i = 0; i < 4; i++)
	{
		_image_array[i] = NULL;
	}
	Load(filename, imgNum,FLIPV);
}

ASprite::~ASprite()
{
	SAFE_DELETE_ARRAY( _current_animation ) ;
	SAFE_DELETE_ARRAY(_current_frame);
	SAFE_DELETE_ARRAY( _current_frame_time );
	SAFE_DELETE_ARRAY( _is_looping ) ;

    SAFE_DELETE_ARRAY( _modules_w );
    SAFE_DELETE_ARRAY(_modules_h );
    SAFE_DELETE_ARRAY(_frames_nfm );
    SAFE_DELETE_ARRAY( _frames_fm_start );
    SAFE_DELETE_ARRAY( _frames_rc );
    SAFE_DELETE_ARRAY(_frames_coll);
    SAFE_DELETE_ARRAY(_fmodules);
    SAFE_DELETE_ARRAY(_anims_naf);
    SAFE_DELETE_ARRAY( _anims_af_start);
    SAFE_DELETE_ARRAY(_aframes);

	SAFE_DELETE_ARRAY(_modules_x);
	SAFE_DELETE_ARRAY(_modules_y);

	for( int i = 0; i < 4; i++)
	{
		SAFE_DELETE(_image_array[i]);
	}

	if(_map)
	{
		for(int i = 0 ; i < MAX_MODULE_MAPPINGS ; i++ )
		{
			SAFE_DELETE_ARRAY(_map[i]);
		}
		SAFE_DELETE_ARRAY( _map );
	}
	
	if(_pal)
	{
		 for(int i = 0 ; i < MAX_SPRITE_PALETTES ; i++ )
		{
			SAFE_DELETE(_pal[i]);
		}
		SAFE_DELETE_ARRAY( _pal );
	}

    SAFE_DELETE_ARRAY( _modules_data_off );
    SAFE_DELETE_ARRAY( _modules_data );

	//if(_modules_image)
	//{
	//	for( int i = 0 ; i < _palettes*_nModules ; i++ )
	//		SAFE_DELETE(_modules_image[i] ) ;
	//	 SAFE_DELETE_ARRAY(_modules_image);
	//}


    SAFE_DELETE_ARRAY( _precalc_stack);
    //char *  _map_char;
	if (_need_release)
	{
		for( int i = 0; i < 4; i++)
		{
			if(_image_array[i] != NULL)
				SAFE_DELETE(_image_array[i]);
		}	
	}
}


void ASprite::SetImage(CIndexedImage * pImage, bool bEnableColorKey)
{
	_image = pImage;
	
	if (bEnableColorKey)
	{
		pImage->EnableColorKey(true);
		pImage->SetColorKey(pImage->GetPaletteData()[0]);
	}
}

void ASprite::Load(const char * szFileName, int imgNum,bool FLIPV)
{
	if (_image != NULL)
	{
		SAFE_DELETE(_image);
	}
	for( int i = 0; i < 4; i++)
	{
		if(_image_array[i] != NULL)
			SAFE_DELETE(_image_array[i]);
	}

	LoadSprite(szFileName);

	LoadImage(szFileName, imgNum,FLIPV);
}

void ASprite::LoadSprite(const char * szFileName)
{
	char szFullFileName[256] = {0};
	strcpy(szFullFileName, szFileName);
	strcat(szFullFileName, ".bsprite");

	CStream* pFS = NEW CResourceStream(szFullFileName);

	char* pData1 = NEW char[pFS->GetLength()];

	pFS->Read(pData1, pFS->GetLength());
	Load(pData1, 0);

	SAFE_DELETE(pData1);
	pFS->Close();
	SAFE_DELETE(pFS);
}

void ASprite::LoadImage(const char * szFileName, int imgNum,bool FLIPV)
{
	_need_release = true;

	for(int i = 0; i < imgNum; i++)
	{
		char szFullFileName[256] = {0};
		strcpy(szFullFileName, szFileName);
		strcat(szFullFileName, img_prox[i]);
		strcat(szFullFileName, ".gtx");

		//void*		pRecord = NULL;
		//CResFile*	pResFile = NULL;
		//int			length = 0;
		
		//GetResourceManager().FindRecord(szFullFileName, &pRecord, &pResFile, length);
		//
		//if(pRecord == NULL)
		//{
		//	continue;
		//}

		CStream* pFS = NEW CResourceStream(szFullFileName);

	#if GRAPHIC_OPENGL > 0
		//#ifdef WIN32

		//CImage::ImageType type = CImage::GetImageType(*pFS);

		//if(type == CImage::TYPE_INDEXED)
		//{
		//	_image_array[i] = NEW CIndexedImage(*pFS);
		//	_image_array[i]->EnableColorKey(true);
		//	_image_array[i]->SetColorKey(KEY_COLOR_VALUE);
		//}
		//else if(type == CImage::TYPE_RGB)
		//{
		//	_image_array[i] = NEW CRgbImage(*pFS);
		//}

		//#else

		_image_array[i] = NEW CGLImage(*pFS,FLIPV);

		//#endif
	#else

		_image_array[i] = NEW CIndexedImage(*pFS);
		//int nColor = _modules_index_image->GetPaletteData()[0];
		_image_array[i]->EnableColorKey(true);
		_image_array[i]->SetColorKey(KEY_COLOR_VALUE);
	#endif

		pFS->Close();
		SAFE_DELETE(pFS);
	}
	_image = _image_array[0];
}

void ASprite::Load(char * file, int offset)
{
	//if (DEF.bDbgS) cout << "ASprite.Load(" << file.length << " bytes, " << offset << ")...";

	short bs_version = (short)((file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8));
	offset +=2 ;
	//if (DEF::bDbgS) cout << "bs_version = 0x" << Integer.toHexString(bs_version);
	/*if (DEF::bErr)
	{
		if (bs_version != BSPRITE_v003)
			cout << "ERROR: Invalid BSprite version !";
	}*/

	int bs_flags =  ((file[offset]&0xFF)    ) +
		((file[offset+1]&0xFF)<< 8) +
		((file[offset+2]&0xFF)<<16) +
		((file[offset+3]&0xFF)<<24);
			offset += 4 ;

	_nModules = (file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8);
		offset += 2 ;
	//if (DEF::bDbgS) System.out.println("nModules = " + _nModules);
	if (_nModules > 0)
	{
		if ((bs_flags & BS_MODULES_XY_SHORT) != 0)
		{
			_modules_x  = NEW short[_nModules];
			_modules_y  = NEW short[_nModules];
		}
		else
		{
			ASSERT(false);
		}
		if ((bs_flags & BS_MODULES_WH_SHORT) != 0)
		{
			_modules_w  = NEW short[_nModules];
			_modules_h  = NEW short[_nModules];
		} 
		else
		{
			ASSERT(false);
		}
		for (int i = 0; i < _nModules; i++)
		{
			if ((bs_flags & BS_MODULES_XY_SHORT) != 0)
			{
				_modules_x[i] = (file[offset]&0xFF) + ((file[offset + 1]&0xFF)<<8);
				offset += 2;
				_modules_y[i] = (file[offset]&0xFF) + ((file[offset + 1]&0xFF)<<8);
				offset += 2;
			}
			else
			{
				ASSERT(false);
			}
			if ((bs_flags & BS_MODULES_WH_SHORT) != 0)
			{
				_modules_w[i] = (file[offset]&0xFF) + ((file[offset + 1]&0xFF)<<8);
				offset += 2;
				_modules_h[i] = (file[offset]&0xFF) + ((file[offset + 1]&0xFF)<<8);
				offset += 2;
			}
			else
			{
				ASSERT(false);
			}
		}
	}

	int nFModules = (file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8);
		offset += 2 ;
	//if (DEF::bDbgS) System.out.println("nFModules = " + nFModules);
	if (nFModules > 0)
	{
		int len = nFModules<<2;
		if ((bs_flags & BS_FM_OFF_SHORT) != 0)
		{
			ASSERT(USE_FM_OFF_SHORT);
			len += nFModules<<1;
		}
		_fmodules = NEW signed char[len];
		arraycopy(file, offset, (char*)_fmodules, 0, len);
		offset += len;
	}

	int nFrames = (file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8);
		offset += 2 ;
	//if (DEF::bDbgS) System.out.println("nFrames = " + nFrames);
	if (nFrames > 0)
	{
		_frames_nfm      = NEW signed char[nFrames + 1];
		_frames_fm_start = NEW short[nFrames];

		for (int i = 0; i < nFrames; i++)
		{
			_frames_nfm[i]      = file[offset++];
			if (!ALWAYS_BS_NFM_1_BYTE) offset++;
				_frames_fm_start[i] = (short)((file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8));
			offset += 2 ;
		}

		if (!ALWAYS_BS_SKIP_FRAME_RC)
		{
			if ((bs_flags & BS_SKIP_FRAME_RC) != BS_SKIP_FRAME_RC) {
				if (USE_PRECOMPUTED_FRAME_RECT)
				{
					int nFrames4 = nFrames<<2;
					_frames_rc = NEW signed char[nFrames4];
					for (int i = 0; i < nFrames4; i++)
						_frames_rc[i] = file[offset++];
				}
				else
					offset += (nFrames<<2);
			}
		}
		else
		{
			if (USE_PRECOMPUTED_FRAME_RECT)
			{
			}
		}

		if (!ALWAYS_BS_FRAME_COLL_RC)
		{
			if ((bs_flags & BS_FRAME_COLL_RC) == BS_FRAME_COLL_RC) {
				//if (DEF::bDbgS) System.out.println("use BS_FRAME_COLL_RC");

				int nFrames4 = nFrames<<2;
				_frames_coll = NEW signed char[nFrames4];
				for (int i = 0; i < nFrames4; i++)
					_frames_coll[i] = file[offset++];
			}
		}
	}

	int nAFrames = (file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8);
		offset += 2 ;
	//if (DEF::bDbgS) System.out.println("nAFrames = " + nAFrames);
	if (nAFrames > 0)
	{
		int len = nAFrames*5 ;
		_aframes = NEW short[len];
		for (int i = 0; i < len; i++)
		{
			int j = i%5;
			if (j>=2 && j<=3)
			{
				_aframes[i] = (short)((file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8));
				offset += 2 ;
			} else {
				_aframes[i] = (short)(file[offset]&0xFF) ;
				offset += 1 ;
			}
		}
	}

	int nAnims = (file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8);
		offset += 2 ;
	//if (DEF::bDbgS) System.out.println("nAnims = " + nAnims);
	if (nAnims > 0)
	{
		_anims_naf      = NEW  signed char[nAnims];
		_anims_af_start = NEW short[nAnims];

		for (int i = 0; i < nAnims; i++)
		{
			_anims_naf[i] = file[offset++]; 
			if (!ALWAYS_BS_NAF_1_BYTE) 
				offset++;

			_anims_af_start[i] = (short)((file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8));
			offset += 2 ;
		}
	}

	if (_nModules <= 0)
	{
		// if (DEF::bErr) System.out.println("WARNING: sprite with num modules = "+_nModules);
		return;
	}
//////////////////////////////////////////////////////////////////////////
/// return for no image
//////////////////////////////////////////////////////////////////////////
		return;

	short _pixel_format = (short)((file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8));
		offset += 2 ;
	// if (DEF::bDbgS) System.out.println("_pixel_format = 0x" + Integer.toHexString(_pixel_format));

	_palettes = file[offset++]&0xFF;
	// if (DEF::bDbgS) System.out.println("_palettes = " + _palettes);

	int colors = file[offset++]&0xFF;
		_m_nColors = colors;
	if (USE_ENCODE_FORMAT_I256)
		if (colors == 0) colors = 256;
	// if (DEF::bDbgS) System.out.println("colors = " + colors);

	_pal = NEW int * [MAX_SPRITE_PALETTES];

	for (int p = 0; p < _palettes; p++)
	{
		_pal[p] = NEW int[colors];

		if ((_pixel_format == PIXEL_FORMAT_8888) && USE_PIXEL_FORMAT_8888)
		{
			for (int c = 0; c < colors; c++)
			{
				int _8888  = ((file[offset++]&0xFF) );
				_8888 += ((file[offset++]&0xFF)<< 8);
				_8888 += ((file[offset++]&0xFF)<<16);
				_8888 += ((file[offset++]&0xFF)<<24);

				if ((_8888 & 0xFF000000) != 0xFF000000)
					_alpha = true;

				_pal[p][c] = _8888;
			}
		}
		else
			if ((_pixel_format == PIXEL_FORMAT_4444) && USE_PIXEL_FORMAT_4444)
			{
				for (int c = 0; c < colors; c++)
				{
					int _4444  = ((file[offset++]&0xFF)   );
					_4444 += ((file[offset++]&0xFF)<<8);

					if ((_4444 & 0xF000) != 0xF000)
						_alpha = true;

					_pal[p][c] = ((_4444 & 0xF000) << 16) | ((_4444 & 0xF000) << 12) |
						((_4444 & 0x0F00) << 12) | ((_4444 & 0x0F00) <<  8) |
						((_4444 & 0x00F0) <<  8) | ((_4444 & 0x00F0) <<  4) |
						((_4444 & 0x000F) <<  4) | ((_4444 & 0x000F)      );
				}
			}
			else
				if ((_pixel_format == PIXEL_FORMAT_1555) && USE_PIXEL_FORMAT_1555)
				{
					for (int c = 0; c < colors; c++)
					{
						int _1555  = ((file[offset++]&0xFF)   );
						_1555 += ((file[offset++]&0xFF)<<8);

						int a = 0xFF000000;
						if ((_1555 & 0x8000) != 0x8000)
						{
							a = 0;
							_alpha = true;
						}

						_pal[p][c] = a |
							((_1555 & 0x7C00) << 9) |
							((_1555 & 0x03E0) << 6) |
							((_1555 & 0x001F) << 3);
					}
				}
				else
					if ((_pixel_format == PIXEL_FORMAT_0565) && USE_PIXEL_FORMAT_0565)
					{
						for (int c = 0; c < colors; c++)
						{
							int _0565  = ((file[offset++]&0xFF)   );
							_0565 += ((file[offset++]&0xFF)<<8);

						int a = 0xFF000000;
						if (_0565 == 0xF81F)
						{
							a = 0;
							_alpha = true;
						}

						_pal[p][c] = a |
						    ((_0565 & 0xF800) << 8) |
						    ((_0565 & 0x07E0) << 5) |
						    ((_0565 & 0x001F) << 3);
					}
				}
		}

		_data_format = (short)((file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8));
		offset += 2 ;
		// if (DEF::bDbgS) System.out.println("_data_format = 0x" + Integer.toHexString(_data_format));


		if ((_data_format == ENCODE_FORMAT_I64RLE) && USE_ENCODE_FORMAT_I64RLE)
		{
			int clrs = colors - 1;
			_i64rle_color_mask = 1;
			_i64rle_color_bits = 1;
			while (clrs != 0)
			{
				clrs >>= 1;
				_i64rle_color_mask <<= 1;
				_i64rle_color_bits++;
			}
			_i64rle_color_mask--;
		}

        if (_nModules > 0)
        {
            _modules_data_off = NEW short[_nModules];
            int len = 0;
            int off = offset;

            int m;
			for ( m = 0; m < _nModules; m++)
            {
                int size = (file[off]&0xFF) + ((file[off+1]&0xFF)<<8);
				off += 2 ;

                _modules_data_off[m] = (short)len;
                off += size;
                len += size;
            }

            _modules_data = NEW char[len];
            for ( m = 0; m < _nModules; m++)
            {
                int size = (file[offset]&0xFF) + ((file[offset+1]&0xFF)<<8);
				offset += 2;
               // if (DEF::bDbgS) System.out.println("frame["+m+"] size = " + size);
                arraycopy(file, offset, _modules_data, _modules_data_off[m]&0xFFFF, size);
                offset += size;
            }
        }

        if (USE_MODULE_MAPPINGS)
        {
            _map = NEW int * [MAX_MODULE_MAPPINGS];

            _cur_map = -1;
        }

        //if (DEF::bDbgS) System.out.println("--- ok");
        //Gc();
}

int ASprite::GetAFrameTime(int anim, int aframe)
{
    return _aframes[(_anims_af_start[anim] + aframe) * 5 + 1] & 0xFFFF;
}


int ASprite::GetAFrames(int anim)
{
    return _anims_naf[anim]&0xFF;
}


int ASprite::GetFModules(int frame)
{
    return _frames_nfm[frame]&0xFF;
}

int ASprite::GetModuleWidth(int module)
{
	//BS_MODULES_WH_SHORT
    return _modules_w[module]&0xFFFF;
}

int ASprite::GetModuleHeight(int module)
{
    return _modules_h[module]&0xFF;
}

int ASprite::GetFrameWidth(int frame)
{
    return _frames_rc[frame*4 + 2]&0xFF;
}

int ASprite::GetFrameHeight(int frame)
{
    return _frames_rc[frame*4 + 3]&0xFF;
}

 
int ASprite::GetFrameCollisionLeft(int frame)
{
    return _frames_coll[frame*4 + 0];
}

int ASprite::GetFrameCollisionTop(int frame)
{
    return _frames_coll[frame*4 + 1];
}

int ASprite::GetFrameCollisionRight(int frame)
{
    return _frames_coll[frame*4 + 2];
}

int ASprite::GetFrameCollisionBottom(int frame)
{
    return _frames_coll[frame*4 + 3];
}

 
int ASprite::GetFrameModuleX(int frame, int fmodule)
{
    int off = (_frames_fm_start[frame] + fmodule) << 2;
	if (USE_FM_OFF_SHORT)
	{
		off += (_frames_fm_start[frame] + fmodule)<<1;
		return (_fmodules[off + 1] & 0xff) | (_fmodules[off + 2] << 8);
	}
	else
		return _fmodules[off+1];
}

int ASprite::GetFrameModuleY(int frame, int fmodule)
{
    int off = (_frames_fm_start[frame] + fmodule) << 2;
	if (USE_FM_OFF_SHORT)
	{
		off += (_frames_fm_start[frame] + fmodule)<<1;
		return (_fmodules[off + 3] & 0xff) | (_fmodules[off + 4] << 8);
	}
	else
		return _fmodules[off+2];
}

int ASprite::GetFrameModuleWidth(int frame, int fmodule)
{
    int off = (_frames_fm_start[frame] + fmodule) << 2;
	if (USE_FM_OFF_SHORT)
	{
		off += (_frames_fm_start[frame] + fmodule)<<1;
	}
    int index = _fmodules[off]&0xFF;
	//BS_MODULES_WH_SHORT
    return _modules_w[index]&0xFFFF;
}

int ASprite::GetFrameModuleHeight(int frame, int fmodule)
{
    int off = (_frames_fm_start[frame] + fmodule) << 2;
	if (USE_FM_OFF_SHORT)
	{
		off += (_frames_fm_start[frame] + fmodule)<<1;
	}
    int index = _fmodules[off]&0xFF;
    return _modules_h[index]&0xFF;
}

 
int ASprite::GetAnimFrame(int anim, int aframe)
{
    int off = (_anims_af_start[anim] + aframe) * 5;
    return _aframes[off]&0xFFFF;
}

void ASprite::GetAFrameRect(int * rc, int anim, int aframe, int posX, int posY, int flags, int hx, int hy)
{
    int off = (_anims_af_start[anim] + aframe) * 5;
    int frame = _aframes[off]&0xFFFF;
    if (USE_INDEX_EX_AFRAMES)
        frame |= ((_aframes[off+4]&FLAG_INDEX_EX_MASK)<<INDEX_EX_SHIFT);
    if ((flags & FLAG_OFFSET_AF) != 0)
    {
        if ((flags & FLAG_FLIP_X) != 0)	hx += _aframes[off+2];
        else							hx -= _aframes[off+2];
        if ((flags & FLAG_FLIP_Y) != 0)	hy += _aframes[off+3];
        else							hy -= _aframes[off+3];
    }

    GetFrameRect(rc, frame, posX, posY, flags ^ (_aframes[off+4]&0x000F), hx, hy);
}

void ASprite::GetFrameRect(int * rc, int frame, int posX, int posY, int flags, int hx, int hy)
{
    if (USE_PRECOMPUTED_FRAME_RECT)
    {
        int frame4 = frame<<2;
        int fx = _frames_rc[frame4++];
        int fy = _frames_rc[frame4++];
        int fw = _frames_rc[frame4++]&0xFF;
        int fh = _frames_rc[frame4++]&0xFF;

        if ((flags & FLAG_FLIP_X) != 0)	
			hx += fx + fw;
        else
			hx -= fx;
        if ((flags & FLAG_FLIP_Y) != 0)	
			hy += fy + fh;
        else
			hy -= fy;

        rc[0] = posX - (hx << FIXED_PRECISION);
        rc[1] = posY - (hy << FIXED_PRECISION);
        rc[2] = rc[0] + (fw << FIXED_PRECISION);
        rc[3] = rc[1] + (fh << FIXED_PRECISION);
    }
    else
    {
        int fx = 0;
        int fy = 0;
        int fw = 0;
        int fh = 0;

        int nFModules = _frames_nfm[frame]&0xFF;
         
        for (int fmodule = 0; fmodule < nFModules; fmodule++)
        {
            GetFModuleRect(rc, frame, fmodule, posX, posY, flags, hx, hy);

            if (rc[0] < fx)
            {
                fw = (fx+fw) - rc[0];
                fx = rc[0];
            }

            if (rc[1] < fy)
            {
                fh = (fy+fh) - rc[1];
                fy = rc[1];
            }

            if (rc[2] > fx + fw)
            {
                fw = rc[2] - fx;
            }

            if (rc[3] > fy + fh)
            {
                fh = rc[3] - fy;
            }
        }

        hx <<= FIXED_PRECISION;
        hy <<= FIXED_PRECISION;

        if ((flags & FLAG_FLIP_X) != 0)	
			hx += fx + fw;
        else
			hx -= fx;
        if ((flags & FLAG_FLIP_Y) != 0)	
			hy += fy + fh;
        else
			hy -= fy;

        rc[0] = posX - hx;
        rc[1] = posY - hy;
        rc[2] = rc[0] + fw;
        rc[3] = rc[1] + fh;
    }
}


void ASprite::GetFModuleRect(int * rc, int frame, int fmodule, int posX, int posY, int flags, int hx, int hy)
{
    rc[0] = GetFrameModuleX(frame, fmodule) << FIXED_PRECISION;
    rc[1] = GetFrameModuleY(frame, fmodule) << FIXED_PRECISION;
    rc[2] = rc[0] + (GetFrameModuleWidth(frame, fmodule) << FIXED_PRECISION);
    rc[3] = rc[1] + (GetFrameModuleHeight(frame, fmodule) << FIXED_PRECISION);      
}

void ASprite::GetModuleRect(int * rc, int module, int posX, int posY, int flags)
{
    rc[0] = posX;
    rc[1] = posY;
	//BS_MODULES_WH_SHORT
    rc[2] = posX + ((_modules_w[module]&0xFFFF) << FIXED_PRECISION);
    rc[3] = posY + ((_modules_h[module]&0xFFFF) << FIXED_PRECISION);
}

void ASprite::SetCurrentAnimation(int anim, bool loop)
{
    SetCurrentAnimation(0, anim, loop);
}


void ASprite::SetCurrentAnimation(int animIndex, int anim, bool loop)
{
    if (animIndex >= MAX_ANIMATION_INSTANCES) {
        //if (DEF::bDbgS) System.out.println("MAX_ANIMATION_INSTANCES is too small!");
    }

    _current_animation[animIndex] = anim;
    _current_frame[animIndex] = 0;
    _current_frame_time[animIndex] = 0;
    _is_looping[animIndex] = loop;
}

int ASprite::GetCurrentAnimation()
{
    return GetCurrentAnimation(0);
}

 
int ASprite::GetCurrentAnimation(int animIndex)
{
    if (animIndex >= MAX_ANIMATION_INSTANCES) {
        //if (DEF::bDbgS) System.out.println("MAX_ANIMATION_INSTANCES is too small!");
    }
    return _current_animation[animIndex];
}

int ASprite::GetCurrentAnimationFrame()
{
    return GetCurrentAnimationFrame(0);
}

int ASprite::GetCurrentAnimationFrame(int animIndex) {
    return _aframes[(_anims_af_start[_current_animation[animIndex]] + _current_frame[animIndex]) * 5];
}

int ASprite::PaintAndUpdateCurrentAnimation(CGraphics * g, int posX, int posY, int flags, int hx, int hy)
{
    return PaintAndUpdateCurrentAnimation(g, 0, posX, posY, flags, hx, hy);
}

 
int ASprite::PaintAndUpdateCurrentAnimation(CGraphics * g, int animIndex, int posX, int posY, int flags, int hx, int hy)
{
    if (animIndex >= MAX_ANIMATION_INSTANCES) {
        //if (DEF::bDbgS) System.out.println("MAX_ANIMATION_INSTANCES is too small!");
    }
    int off = (_anims_af_start[_current_animation[animIndex]] + _current_frame[animIndex]) * 5;
    int frame = _aframes[off] & 0xFFFF;

    if(USE_INDEX_EX_AFRAMES)
    {
        frame |= ((_aframes[off + 4] & FLAG_INDEX_EX_MASK) << INDEX_EX_SHIFT);
    }

    if ((flags & FLAG_FLIP_X) != 0)
		hx += _aframes[off + 2];
    else
		hx -= _aframes[off + 2];
    if ((flags & FLAG_FLIP_Y) != 0)
		hy += _aframes[off + 3];
    else
		hy -= _aframes[off + 3];

    PaintFrame(g, frame, posX - hx, posY - hy, flags ^ (_aframes[off + 4] & 0x000F), hx, hy);

    if(++_current_frame_time[animIndex] >= (_aframes[off + 1] & 0xFFFF))
    {
        if(++_current_frame[animIndex] >= (_anims_naf[_current_animation[animIndex]] & 0xFF))
        {
            if(_is_looping[animIndex])
            {
                _current_frame_time[animIndex] = 0;
                _current_frame[animIndex] = 0;
            }
            else
            {  
                _current_frame[animIndex]--;
            }
        } else {
            _current_frame_time[animIndex] = 0;
        }
    }

    return _current_frame[animIndex];
}

bool ASprite::IsAnimationDone()
{
    return IsAnimationDone(0);
}

bool ASprite::IsAnimationDone(int animIndex) {
    if (animIndex >= MAX_ANIMATION_INSTANCES) {
       // if (DEF::bDbgS) System.out.println("MAX_ANIMATION_INSTANCES is too small!");
    }

    int off = (_anims_af_start[_current_animation[animIndex]] + _current_frame[animIndex]) * 5;
    return !_is_looping[animIndex] && (_current_frame[animIndex] + 1 == (_anims_naf[_current_animation[animIndex]] & 0xFF)) && (_current_frame_time[animIndex] >= (_aframes[off + 1] & 0xFFFF));
}


void ASprite::PaintAFrame(CGraphics * g, int anim, int aframe, int posX, int posY, int flags, int hx, int hy)
{
    int off = (_anims_af_start[anim] + aframe) * 5;
    int frame = _aframes[off] & 0xFFFF;

    if(USE_INDEX_EX_AFRAMES)
    {
        frame |= ((_aframes[off + 4] & FLAG_INDEX_EX_MASK) << INDEX_EX_SHIFT);
    }

    if ((flags & FLAG_FLIP_X) != 0)
		hx += _aframes[off + 2];
    else
		hx -= _aframes[off + 2];
    if ((flags & FLAG_FLIP_Y) != 0)	
		hy += _aframes[off + 3];
    else
		hy -= _aframes[off + 3];

    PaintFrame(g, frame, posX - hx, posY - hy, flags ^ (_aframes[off + 4] & 0x000F), hx, hy);
}

void ASprite::PaintFrame(CGraphics * g, int frame, int posX, int posY, int flags, int hx, int hy)
{
    int nFModules = _frames_nfm[frame]&0xFF;
     
    for (int fmodule = 0; fmodule < nFModules; fmodule++)
        PaintFModule(g, frame, fmodule, posX, posY, flags, hx, hy);
}

void ASprite::PaintFModule(CGraphics * g, int frame, int fmodule, int posX, int posY, int flags, int hx, int hy)
{
    int off = (_frames_fm_start[frame] + fmodule) << 2;
	if (USE_FM_OFF_SHORT)
	{
		off += (_frames_fm_start[frame] + fmodule)<<1;
	}
    int fm_flags = _fmodules[off+3]&0xFF;
	if (USE_FM_OFF_SHORT)
	{
		fm_flags = _fmodules[off+5]&0xFF;
	}
    int index = _fmodules[off]&0xFF;
    if (USE_INDEX_EX_FMODULES)
        index |= ((fm_flags&FLAG_INDEX_EX_MASK)<<INDEX_EX_SHIFT);
    {
        if ((flags & FLAG_FLIP_X) != 0)
			posX -= GetFrameModuleX(frame, fmodule);
        else						
			posX += GetFrameModuleX(frame, fmodule);
        if ((flags & FLAG_FLIP_Y) != 0)	
			posY -= GetFrameModuleY(frame, fmodule);
        else							
			posY += GetFrameModuleY(frame, fmodule);
    }

    if (USE_HYPER_FM && ((fm_flags & FLAG_HYPER_FM) != 0))
    {
        PaintFrame(g, index, posX, posY, flags ^ (fm_flags&0x0F), hx, hy);
    }
    else
    {
		//BS_MODULES_WH_SHORT
        if ((flags & FLAG_FLIP_X) != 0)	posX -= _modules_w[index]&0xFFFF;
        if ((flags & FLAG_FLIP_Y) != 0)	posY -= _modules_h[index]&0xFFFF;

        PaintModule(g, index, posX, posY, flags ^ (fm_flags&0x0F));
    }
}

 

void ASprite::PaintModule(CGraphics * g, int module, int posX, int posY, int flags)
{
    if (USE_MODULE_MAPPINGS)
    {
        if (_cur_map >= 0)
        {
            module = _map[_cur_map][module];
        }
    }
	//BS_MODULES_WH_SHORT
    int sizeX = _modules_w[module]&0xFFFF;
    int sizeY = _modules_h[module]&0xFFFF;
    if (sizeX <= 0 || sizeY <= 0) return;

    CImage * img = _image;
	
	///we can change color palette here
	///img->SetPalette(_crt_pal);

//    if ((_modules_image != NULL))
//        img = _modules_image[_crt_pal * _nModules + module];
//     
//    if (img == NULL)
//    { 
//        int modId = _crt_pal * _nModules + module;
//        if (_precalc_stack[_precalc_stack_index] > 0)
//		{
////			delete[] _modules_image[_precalc_stack[_precalc_stack_index]-1] ;
////            _modules_image[_precalc_stack[_precalc_stack_index]-1] = NULL;
//			SAFE_DELETE(_modules_image[_precalc_stack[_precalc_stack_index]-1]);
//		}
//        _precalc_stack[_precalc_stack_index] = modId + 1;
//        _precalc_stack_index = (_precalc_stack_index+1) % _precalc_stack_max;
//
//        char * image_data = DecodeImage(module, flags);
//        if (image_data == NULL)
//        {
//           // if (DEF::bErr) System.out.println("DecodeImage() FAILED !");
//            return;
//        }
//
//		//CMemoryStream stream(image_data, 10 + _m_nColors*sizeof(short) + sizeX*sizeY);
//		CMemoryStream stream(image_data, sizeof(BBM_HEADER) + _m_nColors*sizeof(short) + sizeX*sizeY);
//		img =  CImage::CreateImage(stream);
//		_modules_image[modId] = img;
//         
//    }
//
//    sizeX = img->GetWidth();
//    sizeY = img->GetHeight();

    const int x = _modules_x[module];
    const int y = _modules_y[module];

    if ((flags & FLAG_FLIP_X) != 0)
    {
        if ((flags & FLAG_FLIP_Y) != 0)
        {
            if((flags & FLAG_ROT_90) != 0) {
                g->DrawRegion(img, x, y, sizeX, sizeY, g->TRANS_ROT270, posX, posY, _crt_pal);
            } else {
                g->DrawRegion(img, x, y, sizeX, sizeY, g->TRANS_ROT180, posX, posY, _crt_pal);
            }
        }
        else
        {
            if((flags & FLAG_ROT_90) != 0) {
                g->DrawRegion(img, x, y, sizeX, sizeY, g->TRANS_MIRROR_ROT90, posX, posY, _crt_pal);
            } else {
                g->DrawRegion(img, x, y, sizeX, sizeY, g->TRANS_MIRROR, posX, posY, _crt_pal);
            }
        }
    }
    else if ((flags & FLAG_FLIP_Y) != 0)
    {
        if((flags & FLAG_ROT_90) != 0) {
            g->DrawRegion(img, x, y, sizeX, sizeY, g->TRANS_MIRROR_ROT90, posX, posY, _crt_pal);
        } else {
            g->DrawRegion(img, x, y, sizeX, sizeY, g->TRANS_MIRROR_ROT180, posX, posY, _crt_pal);
        }
    }
    else
    {
        if((flags & FLAG_ROT_90) != 0) {
            g->DrawRegion(img, x, y, sizeX, sizeY, g->TRANS_ROT90, posX, posY, _crt_pal);
        } else {
			int clipX = g->GetClipX();
			int clipY = g->GetClipY();
            g->DrawRegion(img, x, y, sizeX, sizeY, g->TRANS_NONE, posX, posY, _crt_pal);
        }
    }
}

 
/*char * ScaleImageI256(int module, int factor1, int factor2)
{
    char * buf_img_data = NEW char[5000];  
    if (_modules_data == NULL ||
        _modules_data_off == NULL) return NULL;

    int m = (module<<1);
    int sizeX = _modules_w[m] & 0xFF;
    int sizeY = _modules_h[m] & 0xFF;

    _NEWSizeX = factor1*sizeX/factor2;
    _NEWSizeY = factor1*sizeY/factor2;

    char * image = _modules_data;
    int si = _modules_data_off[module];
    int di = 0;
    int ds = _NEWSizeX * _NEWSizeY;
    int i = 0, j = 0;
    for (i = 0; i < _NEWSizeY; i++)
    {
        int i0 = ((i*sizeY)/_NEWSizeY)*sizeX;
        int i1 =  i*_NEWSizeX;
        for (j = 0; j < _NEWSizeX; j++)
            buf_img_data[j +i1] = image[si + ((j*sizeX)/_NEWSizeX) + i0];
    }
    return buf_img_data;
} */

 
char * ASprite::DecodeImage(int module, int flags)
{
    if (_modules_data == NULL ||
        _modules_data_off == NULL) return NULL;
	//BS_MODULES_WH_SHORT
    int sizeX = _modules_w[module]&0xFFFF;
    int sizeY = _modules_h[module]&0xFFFF;

    char * img_data = NULL;//(char*)(m_cGame->m_temp); mby remarked for CMyFont

    // header

    BBM_HEADER* pBBM_Header = (BBM_HEADER*)(img_data);

    pBBM_Header->bpp = 8;                                            // bpp

    pBBM_Header->flag = BBM_PALETTE|BBM_COLOR_KEY;                   // flag

    pBBM_Header->width = sizeX;                                      // width

    pBBM_Header->height = sizeY;                                     // height

    pBBM_Header->colorKey = 0xF81F;                                  // colorkey

    pBBM_Header->nColors = _m_nColors;                               // colors

	// palette
	int * pal = _pal[_crt_pal]; 
	if (pal == NULL) return NULL;
	unsigned short* pPalette = (unsigned short*)((char*)img_data + sizeof(BBM_HEADER));
	for(int i = 0; i < _m_nColors ; i++ )
	{
		pPalette[i] = ARGB_2_RGB565(_pal[_crt_pal][i]);
	}

	// index
	char* pIndex = (char*)img_data + sizeof(BBM_HEADER) + _m_nColors*sizeof(short);
	char * rp = img_data;
	img_data = pIndex;

    char * image = _modules_data;
    int si = _modules_data_off[module]&0xFFFF;
    int di = 0;
    int ds = sizeX * sizeY;

    if ((_data_format == ENCODE_FORMAT_I64RLE) && USE_ENCODE_FORMAT_I64RLE)
    {
        while (di < ds)
        {
            int c = image[si++]&0xFF;
            int clr =  c & _i64rle_color_mask ; // pal[c & _i64rle_color_mask];
            c >>= _i64rle_color_bits;
            while (c-- >= 0)
                img_data[di++] = clr;
        }
    }
    else
        if ((_data_format == ENCODE_FORMAT_I127RLE) && USE_ENCODE_FORMAT_I127RLE)
        {
            while (di < ds)
            {
                int c = image[si++]&0xFF;
                if (c > 127)
                {
                    int c2 = image[si++]&0xFF;
                    int clr = pal[c2];
                    c -= 128;
                    while (c-- > 0)
                        img_data[di++] = c2 ; //clr;
                }
                else
                    img_data[di++] = c ; //pal[c];
            }
        }
        else
            if ((_data_format == ENCODE_FORMAT_I256RLE) && USE_ENCODE_FORMAT_I256RLE)
            {
                while (di < ds)
                {
                    int c = image[si++]&0xFF;
                    if (c > 127)
                    {
                        c -= 128;
                        while (c-- > 0)
                            img_data[di++] = image[si++]&0xFF ; // ;pal[image[si++]&0xFF];
                    }
                    else
                    {
                        int clr = image[si++]&0xFF ; // pal[image[si++]&0xFF];
                        while (c-- > 0)
                            img_data[di++] = clr;
                    }
                }
            }
            else
                if ((_data_format == ENCODE_FORMAT_I16) && USE_ENCODE_FORMAT_I16)
                {
                    while (di < ds)
                    {
                        img_data[di++] = (image[si] >> 4) & 0x0F ;//pal[(image[si] >> 4) & 0x0F];
                        img_data[di++] = (image[si]     ) & 0x0F ;//pal[(image[si]     ) & 0x0F];
                        si++;
                    }
                }
                else
                    if ((_data_format == ENCODE_FORMAT_I4) && USE_ENCODE_FORMAT_I4)
                    {
                        while (di < ds)
                        {
                            img_data[di++] = (image[si] >> 6) & 0x03 ; //pal[(image[si] >> 6) & 0x03];
                            img_data[di++] = (image[si] >> 4) & 0x03 ; //pal[(image[si] >> 4) & 0x03];
                            img_data[di++] = (image[si] >> 2) & 0x03 ; //pal[(image[si] >> 2) & 0x03];
                            img_data[di++] = (image[si]     ) & 0x03 ;//pal[(image[si]     ) & 0x03];
                            si++;
                        }
                    }
                    else
                        if ((_data_format == ENCODE_FORMAT_I2) && USE_ENCODE_FORMAT_I2)
                        {
                            while (di < ds)
                            {
                                img_data[di++] = (image[si] >> 7) & 0x01 ;  //pal[(image[si] >> 7) & 0x01];
                                img_data[di++] = (image[si] >> 6) & 0x01 ;  //pal[(image[si] >> 6) & 0x01];
                                img_data[di++] = (image[si] >> 5) & 0x01 ;  //pal[(image[si] >> 5) & 0x01];
                                img_data[di++] = (image[si] >> 4) & 0x01 ;  //pal[(image[si] >> 4) & 0x01];
                                img_data[di++] = (image[si] >> 3) & 0x01 ;  //pal[(image[si] >> 3) & 0x01];
                                img_data[di++] = (image[si] >> 2) & 0x01 ;  //pal[(image[si] >> 2) & 0x01];
                                img_data[di++] = (image[si] >> 1) & 0x01 ;  //pal[(image[si] >> 1) & 0x01];
                                img_data[di++] = (image[si]     ) & 0x01 ;  //pal[(image[si]     ) & 0x01];
                                si++;
                            }
                        }
                        else
                            if ((_data_format == ENCODE_FORMAT_I256) && USE_ENCODE_FORMAT_I256)
                            {
                                 
                                while (di < ds)
                                    img_data[di++] = image[si++]&0xFF ; //pal[image[si++]&0xFF];
                            }

    return rp;
}


void ASprite::SetCurrentPalette(int pal)		{ _crt_pal = pal; }
int ASprite::GetCurrentPalette()				{ return _crt_pal; }


int * ASprite::GenPalette(int type, int * pal, int length) //length == pal.length
{
    if (type <  0) return pal;
    if (type == 0) return NULL;

    int* NEW_pal = NEW int[length];
    switch (type)
    {
    case 4:
        for (int i = 0; i < length; i++)
        {
            int a = (pal[i] & 0xFF000000);
            int r = (pal[i] & 0x00FF0000) >> 16;
            int g = (pal[i] & 0x0000FF00) >> 8;
            int b = (pal[i] & 0x000000FF);
            int l = ((r + b + g) / 3) & 0x000000FF;
            NEW_pal[i] = ((l << 16) | (l << 8) | l | a);
        }
        break;

	}

    return NEW_pal;
}
