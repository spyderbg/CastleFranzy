#ifndef __ASPRITE_H__
#define __ASPRITE_H__

#include "Predefine.h"
#include "system/Image.h"
#include "system/MyGraphics.h"
#include "system/Stream.h"
#include "Game.h"
class CGame ;
typedef char * String ;

#if PLATFORM == PLATFORM_NGAGE
using namespace ngi;
#endif

class ASprite
{
public:
	ASprite();
	ASprite(const char filename[], int imgNum = 1,bool FlIPV = false);
	ASprite(char file[], int offset);
	~ASprite();
	void Load(char * file, int offset);

	void Load(const char * szFileName, int imgNum = 1,bool FLIPV =false);
	void SetImage(CIndexedImage *, bool = true);
	void LoadSprite(const char * szFileName);
	void LoadImage(const char * szFileName, int imgNum = 1,bool FLIPV =false);


    int GetAFrameTime(int anim, int aframe);
    int GetAFrames(int anim);
    int GetFModules(int frame);
    int GetModuleWidth(int module);
    int GetModuleHeight(int module);
    int GetFrameWidth(int frame);
    int GetFrameHeight(int frame);
    int GetFrameCollisionLeft(int frame);
    int GetFrameCollisionTop(int frame);
    int GetFrameCollisionRight(int frame);
    int GetFrameCollisionBottom(int frame);
    int GetFrameModuleX(int frame, int fmodule);
    int GetFrameModuleY(int frame, int fmodule);
    int GetFrameModuleWidth(int frame, int fmodule);
    int GetFrameModuleHeight(int frame, int fmodule);
    int GetAnimFrame(int anim, int aframe);
    void GetAFrameRect(int * rc, int anim, int aframe, int posX, int posY, int flags, int hx, int hy);
    void GetFrameRect(int * rc, int frame, int posX, int posY, int flags, int hx, int hy);
    void GetFModuleRect(int * rc, int frame, int fmodule, int posX, int posY, int flags, int hx, int hy);
    void GetModuleRect(int * rc, int module, int posX, int posY, int flags);
    void SetCurrentAnimation(int anim, bool loop);
    void SetCurrentAnimation(int animIndex, int anim, bool loop);
    int GetCurrentAnimation();
    int GetCurrentAnimation(int animIndex);
    int GetCurrentAnimationFrame() ;
    int GetCurrentAnimationFrame(int animIndex) ;
    int PaintAndUpdateCurrentAnimation(CGraphics * g, int posX, int posY, int flags, int hx, int hy);
    int PaintAndUpdateCurrentAnimation(CGraphics * g, int animIndex, int posX, int posY, int flags, int hx, int hy);
    bool IsAnimationDone();
    bool IsAnimationDone(int animIndex);
    void PaintAFrame(CGraphics * g, int anim, int aframe, int posX, int posY, int flags, int hx, int hy);
    void PaintFrame(CGraphics * g, int frame, int posX, int posY, int flags, int hx, int hy);
    void PaintFModule(CGraphics * g, int frame, int fmodule, int posX, int posY, int flags, int hx, int hy);
    void PaintModule(CGraphics * g, int module, int posX, int posY, int flags);
    char * DecodeImage(int module, int flags);
    void SetCurrentPalette(int pal);
    int GetCurrentPalette();

	int * GenPalette(int type, int * pal, int length); //length == pal.length;

// Variables for DrawBrewString, should in .h file
//MAX_TEXT_LENGTH is defined as the MAX length of your sentence
	AECHAR	m_AEStrText[MAX_TEXT_LENGTH];
	char	m_stringWrapped[MAX_TEXT_LENGTH];
	char	m_stringDelWrap[MAX_TEXT_LENGTH];
	int		m_off[MAX_TEXT_LINES];
	RGBVAL	m_TextForeColor;
	RGBVAL	m_TextBackColor;
	int		_text_w;
	int		_text_h;

	bool	String_InPage;


public:
 
    int			_nModules;
	short *		_modules_x;
	short *		_modules_y;
    short *		_modules_w ;
    short *		_modules_h;
    signed char *	_frames_nfm;
    short *			_frames_fm_start;
    signed char  *	_frames_rc;
    signed char  *	_frames_coll;
    signed char*	_fmodules;
    signed char *	_anims_naf;
    short *	    _anims_af_start;
    short *	_aframes;
    int * *		_map;
    int * *		_pal;
    int			_palettes;
    int	        _crt_pal;
    bool		_alpha;

    short 		_data_format;
    int			_i64rle_color_mask;
    int			_i64rle_color_bits;
    char *		_modules_data;
    short *		_modules_data_off;

	 //CImage  ** 	_modules_image;
    CImage *	 _image;
	CImage *	 _image_array[4];

//#if GRAPHIC_OPENGL > 0
//#ifdef WIN32
//	CImage		*_modules_gl_image;
//#else
//	CGLImage	*_modules_gl_image;
//#endif
//#endif
    int *         _current_animation;
    int *         _current_frame;
    int *         _current_frame_time ;
    bool *        _is_looping ;

    int*          _precalc_stack;
    int           _precalc_stack_index;
    int           _precalc_stack_max;
    int _m_nColors;
	//int _newSizeX;
    //int _newSizeY;
    
    signed char *  _map_char;
    int _line_spacing ;
    int _char_spacing ;

    //int  s_drawPageOffset[200] ;
    //int  s_drawPage_nLine;
    //bool s_isDrawPageInit ; 
    CGame * m_cGame ;

private:
	int	_cur_map;
	bool _need_release;
};

#endif
