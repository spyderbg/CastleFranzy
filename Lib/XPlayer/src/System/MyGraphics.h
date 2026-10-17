// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min
// Author: Du Hai Tao	Add transform support to the DrawRegion function that support rotate

#ifndef __MYGRAPHICS_H__
#define __MYGRAPHICS_H__

#include "image.h"
#include "FMath.h"

typedef struct {
	unsigned short width;
	unsigned short height;
	unsigned short opacity;
	short srcYPitch;
	short dstXPitch;
	short dstYPitch;
	void *palette;
	void *alpha;
	unsigned int colorKey;
	int	brightness;
}BlitParam;

/// @brief Provides simple 2D geometric rendering capability.
///
/// Most functions are identical to the APIs in J2ME MIDP Graphics class.\n\n
/// This class can't be instantiated, use CGraphicsT instead. Please refer to the code of
/// \code CMutableImage::CMutableImage(unsigned int w, unsigned int h, int bpp, MemoryType memType) \endcode
/// for how to instantiate a CGraphicsT class and how to attach to a mutable image. In most
/// cases there's no need to create a stand-along CGraphicsT object, using CMutableImage
/// returned from CMutableImage::GetGraphics is enough.\n\n
/// Each CMutableImage object contains a CGraphics object, you can use CMutableImage::GetGraphics
/// to get the CGraphics of the mutable image.\n\n
/// Function Name,Clip,Translate,8Bit Dest,12Bit Dest,16Bit Dest,Opacity
/// ToNativeColor,,,,Y,Y,
/// ToRGB24Color,,,,Y,Y,
/// SetPixel,Y,Y,Y,Y,Y,
/// DrawRegion_NoClip_NoTranslat,,,Y,Y,Y,
/// DrawRegion,Y,Y,Y,Y,Y,
/// FillRect,Y,Y,,,Y,
/// @todo Support RLE image
class CGraphics
{
public:
	typedef struct {
		CFMath::Point2D screen;
		CFMath::Point2D texture;
	}Vertex2D;

	typedef struct {
		short xMin, xMax;
		F12 uMin, uMax;
		F12 vMin, vMax;
	}SpanT;

	/// @brief constant for transform, same as MIDP2.0
	enum Transform{
		///causes the specified image region to be copied unchanged
		TRANS_NONE = 0,
		///causes the specified image region to be rotated clockwise by 90 degrees
		TRANS_MIRROR_ROT180 = 1,
		///causes the specified image region to be rotated clockwise by 180 degrees
		TRANS_MIRROR = 2,
		///causes the specified image region to be rotated clockwise by 270 degrees
		TRANS_ROT180 = 3,
		///causes the specified image region to be reflected about its vertical center
		TRANS_MIRROR_ROT270 = 4,
		///causes the specified image region to be reflected about its vertical center
		///and then rotated clockwise by 90 degrees
		TRANS_ROT90 = 5,
		///causes the specified image region to be reflected about its vertical center
		///and then rotated clockwise by 180 degrees
		TRANS_ROT270 = 6,
		///causes the specified image region to be reflected about its vertical center
		///and then rotated clockwise by 270 degrees
		TRANS_MIRROR_ROT90 = 7
	};

	/// @brief flags of transform translated by TransformToFlag
	enum {
		///The transform contains x flip
		FLIP_X = 1,
		///The transform contains y flip
		FLIP_Y = 2,
		///The transform contains rotation by 90 degrees
		ROTATE_90 = 4
	};


	enum{
		HCENTER = 1 << 0,
		VCENTER = 1 << 1,
		LEFT = 1 << 2,
		RIGHT = 1 << 3,
		TOP = 1 << 4,
		BASELINE = 1 << 5,
		DEFAULT = 1 << 6,
	};

	/// @brief default constructor.
	///
	/// No image is attached. Set color to white and default opacity to 255.\n
	/// Must call SetImage before any graphics operation.
	CGraphics(){
		m_image = NULL;
		m_brightness = 0;;
		SetColor(0);
		SetOpacity(255);
		m_translate[0] = m_translate[1] = 0;
	}

	/// @brief destructor.
	virtual ~CGraphics();

	/// @brief Set the destination image to which CGraphics functions will render. 
	///	@param[in]	image	pointer to CMutableImage that will be used for rendering.
	void SetImage(CMutableImage *image);

	/// @brief Get the destination image to which CGraphics functions will render. 
	///	@return	Pointer to the current destination image.
	CMutableImage* GetImage();

	/// @brief Set the current clip to the rectangle specified by the given coordinates.
	///
	/// Rendering operations have no effect outside of the clipping area.
	/// @attention Some functions like DrawLine_NoClip_NoTranslat ignore the clip.
	///	@param[in]	x	the x coordinate of the new clip rectangle.
	///	@param[in]	y	the y coordinate of the new clip rectangle.
	///	@param[in]	width	the width of the new clip rectangle.
	///	@param[in]	height	the height of the new clip rectangle.
	virtual void SetClip(int x, int y, int width, int height);

	/// @brief Reset the current clip to the entire destination image.
	virtual void ResetClip();

	/// @brief Get the Y offset of the current clipping area, relative to the coordinate system origin of this graphics context. 
	///	@return	Y offset of the current clipping area.
	int GetClipX();

	/// @brief Get the X offset of the current clipping area, relative to the coordinate system origin of this graphics context. 
	///	@return	X offset of the current clipping area.
	int GetClipY();

	/// @brief Get the width of the current clipping area. 
	///	@return	width of the current clipping area.
	int GetClipWidth();

	/// @brief Get the height of the current clipping area. 
	///	@return	height of the current clipping area.
	int GetClipHeight();

	/// @brief Translate the origin of the graphics context to the point (x, y) in the current coordinate system.
	///
	/// All coordinates used in subsequent rendering operations on this graphics context will be relative to this new origin.\n
	/// The effect of calls to translate() are cumulative. For example, calling translate(1, 2) and then translate(3, 4) results in a translation of (4, 6).\n 
	/// The application can set an absolute origin (ax, ay) using the following technique:
	/// \code g.translate(ax - g.getTranslateX(), ay - g.getTranslateY()) \endcode
	///	@param[in]	x	the x coordinate of the new translation origin
	///	@param[in]	y	the y coordinate of the new translation origin
	void Translate(int x, int y);
	
	/// @brief Get the X coordinate of the translated origin of this graphics context.
	///	@return	X of current origin.
	short GetTranslateX();

	/// @brief Get the Y coordinate of the translated origin of this graphics context.
	///	@return	Y of current origin.
	short GetTranslateY();


	/// @brief Get the translated current clipping area. 
	/// Specially added for font drawing
	///	@param[out]	pClip	the point to store current clipping area
	void GetTranslatedClip(short *pClip);

	/// @brief Set the current color and opacity to the specified ARGB values.
	///
	/// All subsequent rendering operations will use this specified color and opacity level.\n
	/// The RGB value passed in is interpreted with the least significant eight bits giving the blue component,
	/// the next eight more significant bits giving the green component, the next eight more significant bits giving the red component,
	/// and the last eight most significant bits giving the opacity level.
	/// That is to say, the color component is specified in the form of 0xAARRGGBB.
	///	@param[in]	argb	the color being set
	void SetARGBColor(unsigned int argb);

	/// @brief Set the current color to the specified RGB values.
	///
	/// All subsequent rendering operations will use this specified color.\n
	/// The RGB value passed in is interpreted with the least significant eight bits giving the blue component,
	/// the next eight more significant bits giving the green component, and the next eight more significant bits giving the red component.
	/// That is to say, the color component is specified in the form of 0x00RRGGBB. The high order byte of this value is ignored.
	///	@param[in]	rgb	the color being set
	void SetColor(unsigned int rgb);

	/// @brief Get the current color.
	///	@return	an integer in form 0x00RRGGBB
	unsigned int GetColor();

	/// @brief Set the current opacity level.
	///
	/// All subsequent rendering operations will use this specified opacity level.\n
	/// 0 means totally transparent, while 255 means totally opaque.
	///	@param[in]	a	opacity level from 0 to 255.
	void SetOpacity(unsigned int a);

	/// @brief Get the current opacity level.
	///	@return	an integer from 0 to 255
	unsigned int GetOpacity();

	/// @brief Convert a RGB color to native color.
	///	@param[in]	rgb	RGB value in 0x00RRGGBB.
	///	@return	an integer of converted color in native format
	virtual unsigned int ToNativeColor(unsigned int rgb) = 0;

	/// @brief Convert a native color to RGB color in 0x00RRGGBB.
	///	@param[in]	color	color in native pixel format
	///	@return	an integer of native color to be converted.
	virtual unsigned int ToRGB24Color(unsigned int color) = 0;

	/// @brief Draw a pixel in specified position.
	///	@param[in]	x	the x coordinate of the pixel to be drawn
	///	@param[in]	y	the y coordinate of the pixel to be drawn
	///	@param[in]	nativeColor	the color of the pixel to be drawn in native color format
	virtual void SetPixel(int x, int y, unsigned int nativeColor) = 0;

	/// @brief Copies a region of the specified source image to a location within the destination,
	/// possibly transforming (rotating and reflecting) the image data using the chosen transform function.
	/// @warning This function is almost the same as the DrawRegion function in J2ME MIDP API, except the last
	/// parameter. In MIDP API, it's anchor point. But here it's the number of palette shift.
	///	@param[in]	pSrc	the source image to copy from
	///	@param[in]	x_src	the x coordinate of the upper left corner of the region within the source image to copy
	///	@param[in]	y_src	the y coordinate of the upper left corner of the region within the source image to copy
	///	@param[in]	width	the width of the region to copy
	///	@param[in]	height	the height of the region to copy
	///	@param[in]	transform	the desired transformation for the selected region being copied
	///	@param[in]	x_dest	the x coordinate of the upper left corner in the destination drawing area
	///	@param[in]	y_dest	the y coordinate of the upper left corner in the destination drawing area
	///	@param[in]	palette	the number of palette entry to shift in source image
	virtual void DrawRegion(CImage *pSrc, int x_src, int y_src, int width, int height, int transform,
		int x_dest, int y_dest, unsigned int palette = 0);


	// add by YZ
	virtual void DrawImage(CImage *pSrc, int x, int y, int anchor);
	//

	/// @brief Same as DrawRegion but no clip and translate.
	///
	/// This function is slightly faster than DrawRegion. If there're many small image clips to be drawn in screen,
	/// using this one can be faster.
	virtual void DrawRegion_NoClip_NoTranslat(CImage *pSrc, int x_src, int y_src, int width, int height, int transform,
		int x_dest, int y_dest, unsigned int palette) = 0;

	/// @brief Fills the specified rectangle with the current color.
	///
	/// If either width or height is zero or less, nothing is drawn.
	///	@param[in]	x	the x coordinate of the rectangle to be filled
	///	@param[in]	y	the y coordinate of the rectangle to be filled
	///	@param[in]	width	the width of the rectangle to be filled
	///	@param[in]	height	the height of the rectangle to be filled
	virtual void FillRect(int x, int y, int width, int height) = 0;
	
	/// @brief Draws the specified rectangle with the current color.
	///
	///	@param[in]	x	the x coordinate of the rectangle to be drawn
	///	@param[in]	y	the y coordinate of the rectangle to be drawn
	///	@param[in]	width	the width of the rectangle to be drawn
	///	@param[in]	height	the height of the rectangle to be drawn
	void DrawRect(int x, int y, int width, int height);

	void DrawRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight);

	virtual void DrawArc(int x, int y, int width, int height) = 0;

	/// @brief Draws a line between the coordinates (x1,y1) and (x2,y2) using the current color.
	///	@param[in]	x0	the x coordinate of the start of the line
	///	@param[in]	y0	the y coordinate of the start of the line
	///	@param[in]	x1	the x coordinate of the end of the line
	///	@param[in]	y1	the y coordinate of the end of the line
	virtual void DrawLine(int x0, int y0, int x1, int y1);

	/// @brief Same as DrawLine but no clip and translate.
	///
	/// This function is slightly faster than DrawLine.
	virtual void DrawLine_NoClip_NoTranslat(int x0, int y0, int x1, int y1) = 0;

	/// @brief Draws the outline of a circular.
	///	@param[in]	x	the x coordinate of the center of the circular.
	///	@param[in]	y	the y coordinate of the center of the circular.
	///	@param[in]	r	the radius of the circular to be drawn
	virtual void DrawCircle(int x, int y, int r) = 0;

	// add by YZ
	virtual void FillCircle(int x, int y, int r) = 0;

	/// @brief Copies a region of the specified source image to a location within the destination,
	/// possibly transforming (rotating, reflecting and scaling) the image data using the chosen transform function.
	///	@param[in]	pSrc	the source image to copy from
	///	@param[in]	x_src	the x coordinate of the upper left corner of the region within the source image to copy
	///	@param[in]	y_src	the y coordinate of the upper left corner of the region within the source image to copy
	///	@param[in]	w_src	the width of the source region to copy
	///	@param[in]	h_src	the height of the source region to copy
	///	@param[in]	transform	the desired transformation for the selected region being copied
	///	@param[in]	x_dest	the x coordinate of the upper left corner in the destination drawing area
	///	@param[in]	y_dest	the y coordinate of the upper left corner in the destination drawing area
	///	@param[in]	w_dest	the width of the destination region
	///	@param[in]	h_dest	the height of the destination region
	///	@param[in]	x_pivot	the x coordinate of the rotation pivot pointe within the destination drawing area
	///	@param[in]	y_pivot	the y coordinate of the rotation pivot pointe within the destination drawing area
	///	@param[in]	angle	the angle to rotate in 4096ths of a circle (2PI radian = 4096). Can be set to any signed integer.
	///	@param[in]	palette	the number of palette entry to shift in source image
	void DrawRegion(CImage *pSrc, int x_src, int y_src, int w_src, int h_src, int transform,
		int x_dest, int y_dest, int w_dest, int h_dest, int x_pivot, int y_pivot, F12 angle,
		unsigned int palette = 0);

	/// @brief Fills the specified triangle will the current color.
	///
	/// The lines connecting each pair of points are included in the filled triangle.
	///	@param[in]	x1	the x coordinate of the first vertex of the triangle
	///	@param[in]	y1	the y coordinate of the first vertex of the triangle
	///	@param[in]	x2	the x coordinate of the second vertex of the triangle
	///	@param[in]	y2	the y coordinate of the second vertex of the triangle
	///	@param[in]	x3	the x coordinate of the third vertex of the triangle
	///	@param[in]	y3	the y coordinate of the third vertex of the triangle
	virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3);

	/// @brief Draw a textured quad with affine texture mapping.
	///	@param[in]	point	pointer to the four vertex of the quad
	///	@param[in]	texture	the texture of the quad
	///	@param[in]	palette	the number of palette entry to shift in texture image
	void DrawPoly4T(Vertex2D *point, CImage *texture, unsigned int palette);

	/// @brief Draw a textured triangle with affine texture mapping.
	///	@param[in]	point	pointer to the three vertex of the triangle
	///	@param[in]	texture	the texture of the triangle
	///	@param[in]	palette	the number of palette entry to shift in texture image
	void DrawPoly3T(Vertex2D *point, CImage *texture, unsigned int palette);

	static unsigned int TransformToFlag(int transform);


	inline void SetBrightness(int in){m_brightness = in;}
	

	virtual void BeginDraw2D(int width, int height){};
	virtual void EndDraw2D(){};
	virtual void EnableBatch(bool in){};

protected:
	class CSpanGenerator 
	{
	public:
		enum {
			MAX_SPAN = 320
		};
		bool Prepare(int yMin, int yMax, int clipY0, int clipY1);
		void EmitEdgeT(Vertex2D *p1, Vertex2D *p2);
		void EmitEdge(Vertex2D *p1, Vertex2D *p2);

		int		m_yMin, m_yMax;
		int		m_clipY0, m_clipY1;
		SpanT	m_span[MAX_SPAN];
		
	};


	int ToRawX(int logicX);
	int ToRawY(int logicY);
	void DrawSpanT(int y, CGraphics::SpanT *span, CImage *texture, unsigned int palette, CFMath *math);
	virtual void FillHLineT(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette) = 0;

	///Clip rectangle {l, t, r, b}. r, b is exclusive
	short m_clipRect[4];
	short m_translate[2];

	unsigned int m_color;
	CMutableImage *m_image;
	int	  m_brightness;
};

inline CMutableImage* CGraphics::GetImage()
{
	return m_image;
}

inline void CGraphics::ResetClip()
{
	m_clipRect[0] = m_clipRect[1] = 0;
	m_clipRect[2] = m_image->GetWidth();
	m_clipRect[3] = m_image->GetHeight();
}

inline int CGraphics::GetClipX() {
	return m_clipRect[0] - m_translate[0];
}
inline int CGraphics::GetClipY() {
	return m_clipRect[1] - m_translate[1];
}
inline int CGraphics::GetClipWidth() {
	return m_clipRect[2] - m_clipRect[0] - m_translate[0];
}
inline int CGraphics::GetClipHeight() {
	return m_clipRect[3] - m_clipRect[1] - m_translate[1];
}

inline void CGraphics::Translate(int x, int y) {
	m_translate[0] += x;
	m_translate[1] += y;
}

inline short CGraphics::GetTranslateX() {
	return m_translate[0];
}
inline short CGraphics::GetTranslateY() {
	return m_translate[1];
}

inline void CGraphics::GetTranslatedClip(short *pClip) {
	pClip[0] = m_clipRect[0];
	pClip[1] = m_clipRect[1];
	pClip[2] = m_clipRect[2];
	pClip[3] = m_clipRect[3];
};

inline int CGraphics::ToRawX(int logicX) {
	return logicX + m_translate[0];
}

inline int CGraphics::ToRawY(int logicY) {
	return logicY + m_translate[1];
}

inline void CGraphics::SetARGBColor(unsigned int argb) {
	m_color = argb;
}

inline void CGraphics::SetColor(unsigned int rgb) {
	//ASSERT((rgb >> 24) == 0);
	//if((rgb >> 24) != 0)
	//	int i = 0 ;
	m_color = (m_color & 0xff000000) | rgb;
}
inline unsigned int CGraphics::GetColor() {
	return m_color & 0xffffff;
}
inline void CGraphics::SetOpacity(unsigned int a) {
	ASSERT(a < 256);
	m_color = (m_color & 0xffffff) | (a << 24);
}
inline unsigned int CGraphics::GetOpacity() {
	return m_color >> 24;
}

inline unsigned int CGraphics::TransformToFlag(int transform) {
	static const unsigned char flag[] = {
		0, FLIP_Y, FLIP_X, FLIP_X | FLIP_Y,
		FLIP_Y | ROTATE_90, ROTATE_90, FLIP_X | FLIP_Y | ROTATE_90, FLIP_X | ROTATE_90
	};
	return flag[transform];
}

typedef void (*BlitFunc)(void *dst, void *src, BlitParam *par);

void BlitD8S8(void *dst, void *src, BlitParam *par);
void BlitD12S8PO(void *dst, void *src, BlitParam *par);
void BlitD16S8PO(void *dst, void *src, BlitParam *par);
void BlitD24S8PO(void *dst, void *src, BlitParam *par);
void BlitD8S8K(void *dst, void *src, BlitParam *par);
void BlitD12S8PKO(void *dst, void *src, BlitParam *par);
void BlitD16S8PKO(void *dst, void *src, BlitParam *par);
void BlitD24S8PKO(void *dst, void *src, BlitParam *par);
void BlitD16S8P(void *dst, void *src, BlitParam *par);
void BlitD16S8A(void *dst, void *src, BlitParam *par);
void BlitD24S8P(void *dst, void *src, BlitParam *par);
void BlitD16S8PK(void *dst, void *src, BlitParam *par);
void BlitD24S8PK(void *dst, void *src, BlitParam *par);
void BlitD16S16(void *dst, void *src, BlitParam *par);
void BlitD24S24(void *dst, void *src, BlitParam *par);
void BlitD12S12O(void *dst, void *src, BlitParam *par);
void BlitD16S16O(void *dst, void *src, BlitParam *par);
void BlitD24S24O(void *dst, void *src, BlitParam *par);
void BlitD16S16K(void *dst, void *src, BlitParam *par);
void BlitD16S16A(void *dst, void *src, BlitParam *par);
void BlitD24S24K(void *dst, void *src, BlitParam *par);
void BlitD12S12KO(void *dst, void *src, BlitParam *par);
void BlitD16S16KO(void *dst, void *src, BlitParam *par);
void BlitD24S24KO(void *dst, void *src, BlitParam *par);
void BlitD12S32A(void *dst, void *src, BlitParam *par);
void BlitD16S32A(void *dst, void *src, BlitParam *par);
void BlitD24S32A(void *dst, void *src, BlitParam *par);
void BlitD12S32AO(void *dst, void *src, BlitParam *par);
void BlitD16S32AO(void *dst, void *src, BlitParam *par);
void BlitD24S32AO(void *dst, void *src, BlitParam *par);
void FillRect16(void *dst, int w, int h, int pitch, unsigned int nativeRgb);
void FillRect24(void *dst, int w, int h, int pitch, unsigned int nativeRgb);
void FillRectARGB12(void *dst, int w, int h, int pitch, unsigned int argb);
void FillRectARGB16(void *dst, int w, int h, int pitch, unsigned int argb);
void FillRectARGB24(void *dst, int w, int h, int pitch, unsigned int argb);
void DrawLine16(unsigned short *dst, int dx, int dy, int pitch, unsigned int nativeColor);
void DrawLine24(unsigned int *dst, int dx, int dy, int pitch, unsigned int nativeColor);
void FillHLineTD16S8PK(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette);
void FillHLineTD24S8PK(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette);
void FillHLineTD12S8PKO(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette, int opacity);
void FillHLineTD16S8PKO(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette, int opacity);
void FillHLineTD24S8PKO(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette, int opacity);
void FillHLineTD16S32A(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length);
void FillHLineTD16S32AO(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, int opacity);

template <typename PT, int bpp>
class CGraphicsT: public CGraphics
{
public:
	//This dummy line remove ADS compiler warning
	typedef CGraphicsT<PT, bpp> FOOTYPE;
	CGraphicsT() {
		// a: opacity, 0 - no, 1 - yes
		// b: src color key, 0 - no, 1 - yes
		// c: src bpp, 0 - 8bit, 1 - 24 bit, 2 - Native
//todo:		memset(m_blitFuncs, 0, sizeof(m_blitFuncs));
		if(bpp == 8)
		{
			m_blitFuncs[0][0][0] = BlitD8S8;
			m_blitFuncs[0][1][0] = BlitD8S8K;
		}
		else if(bpp == 12)
		{
			m_blitFuncs[0][0][0] = BlitD16S8P;
			m_blitFuncs[0][1][0] = BlitD16S8PK;
			m_blitFuncs[1][0][0] = BlitD12S8PO;
			m_blitFuncs[1][1][0] = BlitD12S8PKO;
			m_blitFuncs[0][0][1] = BlitD12S32A;
			m_blitFuncs[0][1][1] = BlitD12S32A;
			m_blitFuncs[1][0][1] = BlitD12S32AO;
			m_blitFuncs[1][1][1] = BlitD12S32AO;
			m_blitFuncs[0][0][2] = BlitD16S16;
			m_blitFuncs[0][1][2] = BlitD16S16K;
			m_blitFuncs[1][0][2] = BlitD12S12O;
			m_blitFuncs[1][1][2] = BlitD12S12KO;
		}
		else if(bpp == 16)
		{
			m_blitFuncs[0][0][0] = BlitD16S8P;
			m_blitFuncs[0][1][0] = BlitD16S8PK;
			m_blitFuncs[1][0][0] = BlitD16S8PO;
			m_blitFuncs[1][1][0] = BlitD16S8PKO;
			m_blitFuncs[0][0][1] = BlitD16S32A;
			m_blitFuncs[0][1][1] = BlitD16S32A;
			m_blitFuncs[1][0][1] = BlitD16S32AO;
			m_blitFuncs[1][1][1] = BlitD16S32AO;
			m_blitFuncs[0][0][2] = BlitD16S16;
			m_blitFuncs[0][1][2] = BlitD16S16K;
			m_blitFuncs[1][0][2] = BlitD16S16O;
			m_blitFuncs[1][1][2] = BlitD16S16KO;
		}
		else if(bpp == 24)
		{
			m_blitFuncs[0][0][0] = BlitD24S8P;
			m_blitFuncs[0][1][0] = BlitD24S8PK;
			m_blitFuncs[1][0][0] = BlitD24S8PO;
			m_blitFuncs[1][1][0] = BlitD24S8PKO;
			m_blitFuncs[0][0][1] = BlitD24S32A;
			m_blitFuncs[0][1][1] = BlitD24S32A;
			m_blitFuncs[1][0][1] = BlitD24S32AO;
			m_blitFuncs[1][1][1] = BlitD24S32AO;
			m_blitFuncs[0][0][2] = BlitD24S24;
			m_blitFuncs[0][1][2] = BlitD24S24K;
			m_blitFuncs[1][0][2] = BlitD24S24O;
			m_blitFuncs[1][1][2] = BlitD24S24KO;
		}
	}
	
	inline static unsigned int ToNativeColorS(int r, int g, int b) {
		if(bpp == 24)
		{
			return (r << 16) | (g << 8) | b;
		}
		else if(bpp == 16)
		{
			return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
		}
		else if(bpp == 12)
		{
			return ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4);
		}
		else
		{
			//not implemented
			ASSERT(false);
			return 0;
		}
	}

	inline static unsigned int ToNativeColorS(unsigned int argb) {
		if(bpp == 24)
			return argb;
		else if(bpp == 16)
		{
			unsigned int fr = (argb >> 8) & 0xF800;
			unsigned int fg = (argb >> 5) & 0x7E0;
			unsigned int fb = (argb >> 3) & 0x1F;
			return fr | fg | fb;
		}
		else if(bpp == 12)
		{
			unsigned int fr = (argb >> 12) & 0xF00;
			unsigned int fg = (argb >> 8) & 0xF0;
			unsigned int fb = (argb >> 4) & 0xF;
			return fr | fg | fb;
		}
		else
		{
			ASSERT(false);
			return 0;
		}
	}

	virtual unsigned int ToNativeColor(unsigned int rgb) {
		return ToNativeColorS(rgb);
	}

	inline static unsigned int ToRGB24ColorS(unsigned int color) {
		unsigned int r, g, b;
		if(bpp == 24)
		{
			return color;
		}
		else if(bpp == 16)
		{
			r = (color & 0xF800) << 8;
			g = (color & 0x7E0) << 5;
			b = (color & 0x1F) << 3;
		}
		else if(bpp == 12)
		{
			r = (color & 0xF00) << 12;
			g = (color & 0xF0) << 8;
			b = (color & 0xF) << 4;
		}
		else
		{
			//not implemented
			ASSERT(false);
			r = g = b = 0;
		}
		return r | g | b;
	}

	virtual unsigned int ToRGB24Color(unsigned int color) {
		return ToRGB24ColorS(color);
	}

	virtual void SetPixel(int x, int y, unsigned int nativeColor) {
		SetPixel_NoTranslat(ToRawX(x), ToRawY(y), nativeColor);
	}
	
	virtual void DrawRegion_NoClip_NoTranslat(CImage *pSrc, int x_src, int y_src, int width, int height, int transform,
		int x_dest, int y_dest, unsigned int palette);

	virtual void FillRect(int x, int y, int width, int height) {
		x = ToRawX(x);
		y = ToRawY(y);

		if(x < m_clipRect[0])
		{
			width -= m_clipRect[0] - x;
			x = m_clipRect[0];
		}
		if (width <= 0)
			return;
		//if(x < m_clipRect[0])
		//	x = m_clipRect[0];
		if(x + width > m_clipRect[2])
			width = m_clipRect[2] - x;
		if(width <= 0)
			return;
		if(y < m_clipRect[1])
		{
			height -= m_clipRect[1] - y;
			y = m_clipRect[1];
		}
		if (height <= 0)
			return;
//		if(y < m_clipRect[1])
//			y = m_clipRect[1];
		if(y + height > m_clipRect[3])
			height = m_clipRect[3] - y;
		if(height <= 0)
			return;
		if(bpp == 12)
		{
			if((m_color >> 24) == 0xff)
			{
				FillRect16(GetPixelPointer(x, y), width, height, m_image->GetPitch(), ToNativeColorS(m_color));
			}
			else
			{
				FillRectARGB12(GetPixelPointer(x, y), width, height, m_image->GetPitch(), m_color);
			}
		}
		else if(bpp == 16)
		{
			if((m_color >> 24) == 0xff)
			{
				FillRect16(GetPixelPointer(x, y), width, height, m_image->GetPitch(), ToNativeColorS(m_color));
			}
			else
			{
				FillRectARGB16(GetPixelPointer(x, y), width, height, m_image->GetPitch(), m_color);
			}
		}
		else if(bpp == 24)
		{
			if((m_color >> 24) == 0xff)
			{
				FillRect24(GetPixelPointer(x, y), width, height, m_image->GetPitch(), ToNativeColorS(m_color));
			}
			else
			{
				FillRectARGB24(GetPixelPointer(x, y), width, height, m_image->GetPitch(), m_color);
			}
		}
		else
			ASSERT(false);
	}

	virtual void DrawLine_NoClip_NoTranslat(int x0, int y0, int x1, int y1) {
		ASSERT(x0 >= m_clipRect[0] && x0 < m_clipRect[2]);
		ASSERT(x1 >= m_clipRect[0] && x1 < m_clipRect[2]);
		ASSERT(y0 >= m_clipRect[1] && y0 < m_clipRect[3]);
		ASSERT(y1 >= m_clipRect[1] && y1 < m_clipRect[3]);
		if(bpp > 8 && bpp <= 16)
		{
			if((m_color >> 24) == 0xff)
				DrawLine16((unsigned short*)GetPixelPointer(x0, y0), x1 - x0, y1 - y0, m_image->GetPitch(), ToNativeColorS(m_color));
			else
				ASSERT(false); //not implemented yet
		}
		else if(bpp == 24)
		{
			if((m_color >> 24) == 0xff)
				DrawLine24((unsigned int*)GetPixelPointer(x0, y0), x1 - x0, y1 - y0, m_image->GetPitch(), ToNativeColorS(m_color));
			else
				ASSERT(false); //not implemented yet
		}
		else
			ASSERT(false); ////not implemented yet
	}

	virtual void DrawArc(int x, int y, int width, int height)
	{
		ASSERT(width >= 0);
		ASSERT(height >= 0);
		ASSERT((m_color >> 24) == 0xff);
		x = ToRawX(x);
		y = ToRawY(y);
		unsigned int nativeColor = ToNativeColorS(m_color);
		int xx,yy,x1,x2,x3,x4,x5,x6,x7,x8,y1,y2,y3,y4,y5,y6,y7,y8,pk;
		xx = 0;	yy = width;
		x1 = x; y1 = y + width;
		x2 = x; y2 = y + width;
		x3 = x; y3 = y - width;
		x4 = x; y4 = y - width;
		x5 = x + height; y5 = y;
		x6 = x - height; y6 = y;
		x7 = x + height; y7 = y;
		x8 = x - height; y8 = y;
		pk = 1 - height;
		while(xx <= yy)
		{
			SetPixel_NoTranslat(x1, y1, nativeColor);
			SetPixel_NoTranslat(x2, y2, nativeColor);
			SetPixel_NoTranslat(x3, y3, nativeColor);
			SetPixel_NoTranslat(x4, y4, nativeColor);
			SetPixel_NoTranslat(x5, y5, nativeColor);
			SetPixel_NoTranslat(x6, y6, nativeColor);
			SetPixel_NoTranslat(x7, y7, nativeColor);
			SetPixel_NoTranslat(x8, y8, nativeColor);
			xx++;
			x1++; x2--; x3++; x4--;
			y5++; y6++; y7--; y8--;
			if(pk < 0)
				pk+= 2*xx + 1;
			else
			{
				yy--;
				y1--; y2--; y3++; y4++;
				x5--; x6++; x7--; x8++;
				pk += 2 * (xx - yy) + 1;
			}
		}
	}

	virtual void DrawCircle(int x, int y, int r)
	{
		ASSERT(r >= 0);
		ASSERT((m_color >> 24) == 0xff);
		x = ToRawX(x);
		y = ToRawY(y);
		unsigned int nativeColor = ToNativeColorS(m_color);
		int xx,yy,x1,x2,x3,x4,x5,x6,x7,x8,y1,y2,y3,y4,y5,y6,y7,y8,pk;
		xx = 0;	yy = r;
		x1 = x; y1 = y + r;
		x2 = x; y2 = y + r;
		x3 = x; y3 = y - r;
		x4 = x; y4 = y - r;
		x5 = x + r; y5 = y;
		x6 = x - r; y6 = y;
		x7 = x + r; y7 = y;
		x8 = x - r; y8 = y;
		pk = 1 - r;
		while(xx <= yy)
		{
			SetPixel_NoTranslat(x1, y1, nativeColor);
			SetPixel_NoTranslat(x2, y2, nativeColor);
			SetPixel_NoTranslat(x3, y3, nativeColor);
			SetPixel_NoTranslat(x4, y4, nativeColor);
			SetPixel_NoTranslat(x5, y5, nativeColor);
			SetPixel_NoTranslat(x6, y6, nativeColor);
			SetPixel_NoTranslat(x7, y7, nativeColor);
			SetPixel_NoTranslat(x8, y8, nativeColor);
			xx++;
			x1++; x2--; x3++; x4--;
			y5++; y6++; y7--; y8--;
			if(pk < 0)
				pk+= 2*xx + 1;
			else
			{
				yy--;
				y1--; y2--; y3++; y4++;
				x5--; x6++; x7--; x8++;
				pk += 2 * (xx - yy) + 1;
			}
		}
	}


	void FillCircle(int x, int y, int r)
	{
		int xx,yy,x1,x2,x3,x4,x5,x6,x7,x8,y1,y2,y3,y4,y5,y6,y7,y8,pk;
		int xx1,xx2,xx3,xx4,xx5,xx6,xx7,xx8;
		long widthScr = m_image->GetWidth() - 1, heightScr = m_image->GetHeight() - 1;
		PT* p= (PT*)m_image->GetPixelData();
		PT* pt;
		long pitch = m_image->GetWidth();
		
		x = ToRawX(x);
		y = ToRawY(y);
		unsigned int nativeColor = ToNativeColorS(m_color);
		
		xx = 0;
		yy = r;
		x1 = x , y1 = y + r;
		x2 = x , y2 = y + r;
		x3 = x , y3 = y - r;
		x4 = x , y4 = y - r;
		x5 = x + r , y5 = y;
		x6 = x - r , y6 = y;
		x7 = x + r , y7 = y;
		x8 = x - r , y8 = y;
		pk = 1 - r;
		SetPixel_NoTranslat(x1, y1, nativeColor);
		SetPixel_NoTranslat(x2, y2, nativeColor);
		SetPixel_NoTranslat(x3, y3, nativeColor);
		SetPixel_NoTranslat(x4, y4, nativeColor);
		xx5 = x5;
		xx6 = x6;
		if(xx5 >= 0 && x6 <= widthScr)
		{
			if(xx5 > widthScr)
				xx5=xx7= widthScr;
			if(xx6 < 0)
				xx6=xx8= 0;
			if(y5>=0&&y5<=heightScr)
			{	
				pt = p + pitch*y6 + xx6;
				*pt = nativeColor;
				while(xx6<xx5)
				{	
					pt++;
					*pt = nativeColor;
					xx6++;
				}
			}
		}
		bool b1,b2;
		while(xx < yy)
		{	
			xx++;
			x1++,x2--,x3++,x4--;
			y5++,y6++,y7--,y8--;
			b2 = true;
			if(pk < 0)
			{	
				b1 = false;
				pk+= 2*xx + 1;
				SetPixel_NoTranslat(x1, y1, nativeColor);
				SetPixel_NoTranslat(x2, y2, nativeColor);
				SetPixel_NoTranslat(x3, y3, nativeColor);
				SetPixel_NoTranslat(x4, y4, nativeColor);
			}
			else
			{	
				yy--;
				b1 = true;
				y1--,y2--,y3++,y4++;
				x5--,x6++,x7--,x8++;
				pk+= 2*(xx - yy) + 1;
			}
			xx1=x1,xx2=x2,xx3=x3,xx4=x4,xx5=x5,xx6=x6,xx7=x7,xx8=x8;
			if(xx1 < 0)
			{	
				xx1=xx3= 0;
				b1 = false;
			}
			if(xx1 > widthScr)
				xx1=xx3= widthScr;
			if(xx2 < 0)
				xx2=xx4= 0;
			if(xx2 > widthScr)
			{	
				xx2=xx4= widthScr;
				b1 = false;
			}
			if(xx5 < 0)
			{	
				xx5=xx7= 0;
				b2 = false;
			}
			if(xx5 > widthScr)
				xx5=xx7= widthScr;
			if(xx6 < 0)
				xx6=xx8= 0;
			if(xx6 > widthScr)
			{	
				xx6=xx8= widthScr;
				b2 = false;
			}
			if(b1)
			{	
				if(y1>=0&&y1<=heightScr)
				{	
					pt = p + pitch*y2 + xx2;
					*pt = nativeColor;
					while(xx2<xx1)
					{	
						pt++;
						*pt = nativeColor;
						xx2++;
					}
				}
				if(y3>=0&&y3<=heightScr)
				{	
					pt = p + pitch*y4 + xx4;
					*pt = nativeColor;
					while(xx4<xx3)
					{	
						pt++;
						*pt = nativeColor;
						xx4++;
					}
				}
			}
			if(b2)
			{	
				if(y5>=0&&y5<=heightScr)
				{	
					pt = p + pitch*y6 + xx6;
					*pt = nativeColor;
					while(xx6<xx5)
					{	
						pt++;
						*pt = nativeColor;
						xx6++;
					}
				}
				if(y7>=0&&y7<=heightScr)
				{	
					pt = p + pitch*y8 + xx8;
					*pt = nativeColor;
					while(xx8<xx7)
					{	
						pt++;
						*pt = nativeColor;
						xx8++;
					}
				}
			}
		}
	}

	virtual void FillHLineT(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette)
	{
		if(texture->GetType() == CImage::TYPE_INDEXED)
		{
			ASSERT(texture->GetBitsPerPixel() == 8);
			if(GetOpacity() == 255)
			{
				if(bpp > 8 && bpp <= 16)
					FillHLineTD16S8PK(uStart, dux, vStart, dvx, texture, pStart, x, length, palette);
				else if(bpp == 24)
					FillHLineTD24S8PK(uStart, dux, vStart, dvx, texture, pStart, x, length, palette);
				else	
					ASSERT(false);
			}
			else
			{
				if(bpp == 12)
					FillHLineTD12S8PKO(uStart, dux, vStart, dvx, texture, pStart, x, length, palette, GetOpacity());
				else if(bpp == 16)
					FillHLineTD16S8PKO(uStart, dux, vStart, dvx, texture, pStart, x, length, palette, GetOpacity());
				else if(bpp == 24)
					FillHLineTD24S8PKO(uStart, dux, vStart, dvx, texture, pStart, x, length, palette, GetOpacity());
				else
					ASSERT(false);
			}
		}
		else if(texture->GetType() == CImage::TYPE_RGB)
		{
			if(GetOpacity() == 255)
			{
				if(bpp > 8 && bpp <= 16)
					FillHLineTD16S32A(uStart, dux, vStart, dvx, texture, pStart, x, length);
				else	
					ASSERT(false);
			}
			else
			{
				if(bpp == 16)
					FillHLineTD16S32AO(uStart, dux, vStart, dvx, texture, pStart, x, length, GetOpacity());
				else
					ASSERT(false);
			}
		}
		else
			ASSERT(false);
	}

protected:
	BlitFunc m_blitFuncs[2][2][3];

	inline PT* GetPixelPointer(int x, int y) {
		return (PT*)(m_image->GetPixelData() + y * m_image->GetPitch()) + x;
	}

	void SetPixel_NoTranslat(int x, int y, int nativeColor) {
		if(x < m_clipRect[0] || x >= m_clipRect[2] || y < m_clipRect[1] || y >= m_clipRect[3])
			return;
		GetPixelPointer(x, y)[0] = nativeColor;
	}
};

template <typename PT, int bpp>
void CGraphicsT<PT, bpp>::DrawRegion_NoClip_NoTranslat(CImage *pSrc, int x_src, int y_src, int width, int height, int transform,
		int x_dest, int y_dest, unsigned int palette)
{
	ASSERT(pSrc != NULL);
	ASSERT(x_src >= 0);
	ASSERT(x_src + width <= pSrc->GetWidth());
	ASSERT(y_src >= 0);
	ASSERT(y_src + height <= pSrc->GetHeight());
	BlitParam par;
	par.brightness = m_brightness;
	
	int dstXPitch = m_image->GetPitch();
	switch(transform)
	{
	case TRANS_NONE:
		par.dstXPitch = sizeof(PT);
		par.dstYPitch = dstXPitch;
		break;
	case TRANS_MIRROR_ROT180:
		y_dest += height - 1;
		par.dstXPitch = sizeof(PT);
		par.dstYPitch = -dstXPitch;
		break;
	case TRANS_MIRROR:
		x_dest += width - 1;
		par.dstXPitch = -(int)sizeof(PT);
		par.dstYPitch = dstXPitch;
		break;
	case TRANS_ROT180:
		x_dest += width - 1;
		y_dest += height - 1;
		par.dstXPitch = -(int)sizeof(PT);
		par.dstYPitch = -dstXPitch;
		break;
	case TRANS_MIRROR_ROT270:
		par.dstXPitch = dstXPitch;
		par.dstYPitch = sizeof(PT);
		break;
	case TRANS_ROT90:
		x_dest += height - 1;
		par.dstXPitch = dstXPitch;
		par.dstYPitch = -(int)sizeof(PT);
		break;
	case TRANS_ROT270:
		y_dest += width - 1;
		par.dstXPitch = -dstXPitch;
		par.dstYPitch = sizeof(PT);
		break;
	case TRANS_MIRROR_ROT90:
		x_dest += height - 1;
		y_dest += width - 1;
		par.dstXPitch = -dstXPitch;
		par.dstYPitch = -(int)sizeof(PT);
		break;
	default:
		ASSERT(false);
	}
	
	par.srcYPitch = pSrc->GetPitch();
	par.width = width;
	par.height = height;

	int id3;
	int opacity = m_color >> 24;
	if(opacity == 255)
		id3 = 0;
	else
	{
		par.opacity = opacity;
		id3 = 1;
	}

	BlitFunc *curBlitFunc;
	if(pSrc->GetFlag() & BBM_COLOR_KEY)
	{
		curBlitFunc = m_blitFuncs[id3][1];
		par.colorKey = pSrc->GetColorKey();
	}
	else
		curBlitFunc = m_blitFuncs[id3][0];
	
	if(pSrc->GetType() == CImage::TYPE_INDEXED)
	{
		CIndexedImage *image = DYNAMIC_CAST(CIndexedImage*, pSrc);
		ASSERT(palette < image->GetPaletteEntryNumber());
		par.palette = image->GetPaletteData() + palette * 256;
		
		if(image->GetAlphaData() == NULL)
		{
			BlitFunc blitFunc = curBlitFunc[0];
			ASSERT(blitFunc != NULL);
			(*blitFunc)(GetPixelPointer(x_dest, y_dest), image->GetPixelData() + image->GetPitch() * y_src + x_src, &par);
		}
		else
		{				
			par.alpha = image->GetAlphaData() + image->GetPitch() * y_src + x_src;
			BlitD16S8A(GetPixelPointer(x_dest, y_dest), image->GetPixelData() + image->GetPitch() * y_src + x_src, &par);	
		}
	}
	else if(pSrc->GetType() == CImage::TYPE_RGB || pSrc->GetType() == CImage::TYPE_MUTABLE)
	{
		CRgbImage *image = DYNAMIC_CAST(CRgbImage*, pSrc);
		
		if(image->GetBitsPerPixel() == 32)
		{
			BlitFunc blitFunc = curBlitFunc[1];
			ASSERT(blitFunc != NULL);
			(*blitFunc)(GetPixelPointer(x_dest, y_dest), image->GetPixelData() + image->GetPitch() * y_src + x_src * 4, &par);
		}
		else if(image->GetBitsPerPixel() == bpp)
		{
			BlitFunc blitFunc = curBlitFunc[2];
			ASSERT(blitFunc != NULL);
			if(bpp > 8 && bpp <= 16)
			{
				if(image->HasAlpha())
				{
					BlitD16S16A(GetPixelPointer(x_dest, y_dest), image->GetPixelData() + image->GetPitch() * y_src + x_src * 2, &par);
				}
				else
					(*blitFunc)(GetPixelPointer(x_dest, y_dest), image->GetPixelData() + image->GetPitch() * y_src + x_src * 2, &par);
			}
			else if(bpp == 24)
				(*blitFunc)(GetPixelPointer(x_dest, y_dest), image->GetPixelData() + image->GetPitch() * y_src + x_src * 2, &par);
			else
				ASSERT(false);
		}
		else
			ASSERT(false);
	}
	else
	{
		ASSERT(false);
	}
};

#if GRAPHIC_OPENGL > 0

#include "glt_math_alg.h"

#define TEX_SIZE	32
#ifdef RP_DEBUG_INFO
#define REGION_SIZE	1024
#else
#define REGION_SIZE	512
#endif

typedef struct{
	GLTvec3D	_vertex[REGION_SIZE * 4];
	GLTvertexUV _uv[REGION_SIZE * 4];
	short		_tex;
	short		_region_count;
	int			_col;
}TexRegions;

class CGraphicsGL: public CGraphics
{
public:
	CGraphicsGL();
	~CGraphicsGL();

	void DrawRegion(CImage *pSrc, int x_src, int y_src, int width, int height, int transform,
							int x_dest, int y_dest, unsigned int palette = 0);
	
	void DrawRegion_NoClip_NoTranslat(CImage *pSrc, int x_src, int y_src, int width, int height, int transform,
											  int x_dest, int y_dest, unsigned int palette);

	virtual void DrawLine_NoClip_NoTranslat(int x0, int y0, int x1, int y1) ;
	
	virtual unsigned int ToNativeColor(unsigned int rgb);
	
	/// @brief Convert a native color to RGB color in 0x00RRGGBB.
	///	@param[in]	color	color in native pixel format
	///	@return	an integer of native color to be converted.
	virtual unsigned int ToRGB24Color(unsigned int color);
	
	/// @brief Draw a pixel in specified position.
	///	@param[in]	x	the x coordinate of the pixel to be drawn
	///	@param[in]	y	the y coordinate of the pixel to be drawn
	///	@param[in]	nativeColor	the color of the pixel to be drawn in native color format
	virtual void SetPixel(int x, int y, unsigned int nativeColor);
	
	/// @brief Fills the specified rectangle with the current color.
	///
	/// If either width or height is zero or less, nothing is drawn.
	///	@param[in]	x	the x coordinate of the rectangle to be filled
	///	@param[in]	y	the y coordinate of the rectangle to be filled
	///	@param[in]	width	the width of the rectangle to be filled
	///	@param[in]	height	the height of the rectangle to be filled
	virtual void FillRect(int x, int y, int width, int height);
	
	virtual void DrawArc(int x, int y, int width, int height);
	
	virtual void DrawCircle(int x, int y, int r);
	
	// add by YZ
	virtual void FillCircle(int x, int y, int r);
	
	virtual void FillHLineT(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette);
	virtual void FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3);

	void BeginDraw2D(int width, int height);
	void EndDraw2D();
	void SetClip(int x, int y, int width, int height);
	void ResetClip();


	inline void EnableBatch(bool in){m_batch = in;};
	
private:
	

	void ResetRegions();
	void RenderRegions();
	void AddRegion(int tex, int col, GLTvec3D* pVertex, GLTvertexUV* pUV);


	TexRegions	m_texRegions[TEX_SIZE];
	int			m_texCount;
	u16			m_index[REGION_SIZE * 6];
	GLfixed		m_depth;

	bool		m_batch;
};

#endif

#if NATIVE_COLOR_DEPTH == 16
typedef CGraphicsT<NativePixel, 16> CNativeGraphics;
#elif NATIVE_COLOR_DEPTH == 12
typedef CGraphicsT<NativePixel, 12> CNativeGraphics;
#elif NATIVE_COLOR_DEPTH == 24
typedef CGraphicsT<NativePixel, 24> CNativeGraphics;
#else
#error NATIVE_COLOR_DEPTH is not supported
#endif



#endif
