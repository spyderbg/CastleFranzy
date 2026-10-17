// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min
#include "stdafx.h"
#include "MyGraphics.h"
#include "glt_math_alg.h"

#if PLATFORM == PLATFORM_MAC_OS_X
#include <OpenGLES/ES1/gl.h>
#elif IPHONE > 0
#include "../tools/rasteroid3.1/include/gles/gl.h"
#else
#include "gl.h"
#endif

#define ALPHA_DEVISOR	256 //should be 255, but use 256 for the sake of speed
#define BRIGHTNESS_DEVISOR	16

#define ALPHA_BLEND_S12_D12(dst, bg, fg, a) {\
		const unsigned int rMask = 0xF00;\
		const unsigned int gMask = 0xF0;\
		const unsigned int bMask = 0xF;\
		int r = bg & rMask;\
		r += ((fg & rMask) - r) * a / ALPHA_DEVISOR;\
		int g = bg & gMask;\
		g += ((fg & gMask) - g) * a / ALPHA_DEVISOR;\
		int b = bg & bMask;\
		b += ((fg & bMask) - b) * a / ALPHA_DEVISOR;\
		dst = (r & rMask) | (g & gMask) | (b & bMask);\
	}

#define ALPHA_BLEND_S32_D12(dst, bg, fg, a) {\
		int t;\
		t = (bg >> 8) & 0x0f;\
		int r = t + (((fg >> 20) & 0xf) - t) * a / ALPHA_DEVISOR;\
		t = (bg >> 4) & 0xf;\
		int g = t + (((fg >> 12) & 0xf) - t) * a / ALPHA_DEVISOR;\
		t = (bg) & 0xf;\
		int b = t + (((fg >> 4) & 0xf) - t) * a / ALPHA_DEVISOR;\
		dst = (r << 8) | (g << 4) | b;\
	}

#define ALPHA_BLEND_S16_D16(dst, bg, fg, a) {\
		const unsigned int rMask = 0xF800;\
		const unsigned int gMask = 0x7E0;\
		const unsigned int bMask = 0x1f;\
		int r = bg & rMask;\
		r += ((fg & rMask) - r) * a / ALPHA_DEVISOR;\
		int g = bg & gMask;\
		g += ((fg & gMask) - g) * a / ALPHA_DEVISOR;\
		int b = bg & bMask;\
		b += ((fg & bMask) - b) * a / ALPHA_DEVISOR;\
		dst = (r & rMask) | (g & gMask) | (b & bMask);\
	}


#define ALPHA_BLEND_S32_D16(dst, bg, fg, a) {\
		int t;\
		t = (bg >> 11) & 0x1f;\
		int r = t + (((fg >> 19) & 0x1f) - t) * a / ALPHA_DEVISOR;\
		t = (bg >> 5) & 0x3f;\
		int g = t + (((fg >> 10) & 0x3f) - t) * a / ALPHA_DEVISOR;\
		t = (bg) & 0x1f;\
		int b = t + (((fg >> 3) & 0x1f) - t) * a / ALPHA_DEVISOR;\
		dst = (r << 11) | (g << 5) | b;\
	}

#define ALPHA_BLEND_S24_D24(dst, bg, fg, a) {\
		const unsigned int rMask = 0xFF0000;\
		const unsigned int gMask = 0xFF00;\
		const unsigned int bMask = 0xFF;\
		int r = bg & rMask;\
		r += ((fg & rMask) - r) * a / ALPHA_DEVISOR;\
		int g = bg & gMask;\
		g += ((fg & gMask) - g) * a / ALPHA_DEVISOR;\
		int b = bg & bMask;\
		b += ((fg & bMask) - b) * a / ALPHA_DEVISOR;\
		dst = (r & rMask) | (g & gMask) | (b & bMask);\
	}


#define CHANGE_BRIGHTNESS_D16(dst, brightness){\
	int r = (dst & 0xF800) >> 10;\
	int g = (dst & 0x7E0) >> 5;\
	int b = (dst & 0x1F) << 1;\
	if(brightness > 0)\
	{\
		r = r + (63 - r) * brightness / BRIGHTNESS_DEVISOR;\
		g = g + (63 - g) * brightness / BRIGHTNESS_DEVISOR;\
		b = b + (63 - b) * brightness / BRIGHTNESS_DEVISOR;\
	}\
	else if(brightness < 0)\
	{\
		r = r + r * brightness / BRIGHTNESS_DEVISOR;\
		g = g + g * brightness / BRIGHTNESS_DEVISOR;\
		b = b + b * brightness / BRIGHTNESS_DEVISOR;\
	}\
	dst = ((r >> 1) << 11) | (g << 5) | (b >> 1);\
}



CGraphics::~CGraphics()
{
}

void CGraphics::SetImage(CMutableImage *image)
{
	m_image = image;
	ResetClip();
	m_translate[0] = m_translate[1] = 0;
}

void CGraphics::SetClip(int x, int y, int width, int height)
{
	m_clipRect[0] = x + m_translate[0];
	m_clipRect[1] = y + m_translate[1];
	m_clipRect[2] = m_clipRect[0] + width;
	m_clipRect[3] = m_clipRect[1] + height;
	if(m_clipRect[0] < 0)
		m_clipRect[0] = 0;
	if(m_clipRect[1] < 0)
		m_clipRect[1] = 0;
	
	int n = m_image->GetWidth();
	if(m_clipRect[2] > n)
		m_clipRect[2] = n;
	n = m_image->GetHeight();
	if(m_clipRect[3] > n)
		m_clipRect[3] = n;
}

void CGraphics::DrawRegion(CImage *pSrc, int x_src, int y_src, int width, int height, int transform,
	int x_dest, int y_dest, unsigned int palette)
{
	x_dest += m_translate[0];
	y_dest += m_translate[1];

	unsigned int f = TransformToFlag(transform);

	//I derived these clipping rules by studying at a bunch of diagrams with
	//all kinds of transforms ;)
	if(!(f & ROTATE_90))
	{
		int delta = m_clipRect[0] - x_dest;
		if(delta > 0)
		{
			if(!(f & FLIP_X))
				x_src += delta;
			width -= delta;
			x_dest = m_clipRect[0];
		}
		delta = x_dest + width - m_clipRect[2];
		if(delta > 0)
		{
			if(f & FLIP_X)
				x_src += delta;
			width -= delta;
		}
		delta = m_clipRect[1] - y_dest;
		if(delta > 0)
		{
			if(!(f & FLIP_Y))
				y_src += delta;
			height -= delta;
			y_dest = m_clipRect[1];
		}
		delta = y_dest + height - m_clipRect[3];
		if(delta > 0)
		{
			if(f & FLIP_Y)
				y_src += delta;
			height -= delta;
		}
	}
	else
	{
		int delta = m_clipRect[0] - x_dest;
		if(delta > 0)
		{
			if((f & FLIP_Y))
				y_src += delta;
			height -= delta;
			x_dest = m_clipRect[0];
		}
		delta = x_dest + height - m_clipRect[2];
		if(delta > 0)
		{
			if(!(f & FLIP_Y))
				y_src += delta;
			height -= delta;
		}
		delta = m_clipRect[1] - y_dest;
		if(delta > 0)
		{
			if(!(f & FLIP_X))
				x_src += delta;
			width -= delta;
			y_dest = m_clipRect[1];
		}
		delta = y_dest + width - m_clipRect[3];
		if(delta > 0)
		{
			if(f & FLIP_X)
				x_src += delta;
			width -= delta;
		}
	}

	if(width > 0 && height > 0)
		DrawRegion_NoClip_NoTranslat(pSrc, x_src, y_src, width, height, transform, x_dest, y_dest, palette);
}

void CGraphics::DrawImage(CImage *pSrc, int x, int y, int anchor)
{
	if (anchor&CGraphics::HCENTER)
	{
		x -= pSrc->GetWidth() >> 1;
	}
	else if (anchor&CGraphics::RIGHT)
	{
		x -= pSrc->GetWidth();
	}

	if (anchor&CGraphics::VCENTER)
	{
		y -= pSrc->GetHeight() >> 1;
	}
	else if (anchor&CGraphics::BASELINE)
	{
		y -= pSrc->GetHeight();
	}

	DrawRegion(pSrc, 0, 0, pSrc->GetWidth(), pSrc->GetHeight(), 0, x, y, 0);
	
}

void BlitD8S8(void *dst, void *src, BlitParam *par)
{
	unsigned char *d = (unsigned char*)dst;
	unsigned char *s = (unsigned char*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		//Unroll the following loop in asm code
		while(j != 0)
		{
			*d = *s++;
			d += dx;
			--j;
		}
		s += sy;
		d += dy;
	}
}

void BlitD8S8K(void *dst, void *src, BlitParam *par)
{
	unsigned char *d = (unsigned char*)dst;
	unsigned char *s = (unsigned char*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		//Unroll the following loop in asm code
		while(j != 0)
		{
			unsigned char v = *s++;
			if(v != par->colorKey)
				*d = v;
			d += dx;
			--j;
		}
		s += sy;
		d += dy;
	}
}

void BlitD16S8P(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned short *pal = (unsigned short*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;

	ASSERT(par->height > 0);
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j >= 4)
		{
			*d = pal[*s++];
			d = (unsigned short*)((char*)d + dx);
			*d = pal[*s++];
			d = (unsigned short*)((char*)d + dx);
			*d = pal[*s++];
			d = (unsigned short*)((char*)d + dx);
			*d = pal[*s++];
			d = (unsigned short*)((char*)d + dx);
			j -= 4;
		}
		while(j != 0)
		{
			*d = pal[*s++];
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s += sy;
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD24S8P(void *dst, void *src, BlitParam *par)
{
	unsigned int *d = (unsigned int*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned int *pal = (unsigned int*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;
	
	ASSERT(par->height > 0);
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j >= 4)
		{
			*d = pal[*s++];
			d = (unsigned int*)((char*)d + dx);
			*d = pal[*s++];
			d = (unsigned int*)((char*)d + dx);
			*d = pal[*s++];
			d = (unsigned int*)((char*)d + dx);
			*d = pal[*s++];
			d = (unsigned int*)((char*)d + dx);
			j -= 4;
		}
		while(j != 0)
		{
			*d = pal[*s++];
			d = (unsigned int*)((char*)d + dx);
			--j;
		}
		s += sy;
		d = (unsigned int*)((char*)d + dy);
	}
}

void BlitD12S8PO(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned short *pal = (unsigned short*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;

	const int a = par->opacity;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int fg = pal[*s++];
			unsigned int bg = *d;
			ALPHA_BLEND_S12_D12(*d, bg, fg, a);
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD16S8PO(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned short *pal = (unsigned short*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;

	const int a = par->opacity;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int fg = pal[*s++];
			unsigned int bg = *d;
			ALPHA_BLEND_S16_D16(*d, bg, fg, a);
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD24S8PO(void *dst, void *src, BlitParam *par)
{
	unsigned int *d = (unsigned int*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned int *pal = (unsigned int*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;

	const int a = par->opacity;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int fg = pal[*s++];
			unsigned int bg = *d;
			ALPHA_BLEND_S24_D24(*d, bg, fg, a);
			d = (unsigned int*)((char*)d + dx);
			--j;
		}
		s = (s + sy);
		d = (unsigned int*)((char*)d + dy);
	}
}

void BlitD16S8PB(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned short *pal = (unsigned short*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;
	int key = par->colorKey;
	int brightness = par->brightness;

	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j >= 4)
		{
			int c = *s++;
			if(pal[c] != key)
			{
				*d = pal[c];
				CHANGE_BRIGHTNESS_D16(*d,brightness);		
			}

			d = (unsigned short*)((char*)d + dx);
			c = *s++;
			if(pal[c] != key)
			{
				*d = pal[c];
				CHANGE_BRIGHTNESS_D16(*d,brightness);		
			}
			d = (unsigned short*)((char*)d + dx);
			c = *s++;
			if(pal[c] != key)
			{
				*d = pal[c];
				CHANGE_BRIGHTNESS_D16(*d,brightness);		
			}
			d = (unsigned short*)((char*)d + dx);
			c = *s++;
			if(pal[c] != key)
			{
				*d = pal[c];
				CHANGE_BRIGHTNESS_D16(*d,brightness);		
			}
			d = (unsigned short*)((char*)d + dx);
			j -= 4;
		}
		while(j != 0)
		{
			int c = *s++;
			if(pal[c] != key)
			{
				*d = pal[c];
				CHANGE_BRIGHTNESS_D16(*d,brightness);		
			}
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s += sy;
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD16S8PK(void *dst, void *src, BlitParam *par)
{
	if(par->brightness != 0)
	{
		BlitD16S8PB(dst, src, par);
		return;
	}

	unsigned short *d = (unsigned short*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned short *pal = (unsigned short*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;
	int key = par->colorKey;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j >= 4)
		{
			int c = *s++;
			if(pal[c] != key)
				*d = pal[c];
			d = (unsigned short*)((char*)d + dx);
			c = *s++;
			if(pal[c] != key)
				*d = pal[c];
			d = (unsigned short*)((char*)d + dx);
			c = *s++;
			if(pal[c] != key)
				*d = pal[c];
			d = (unsigned short*)((char*)d + dx);
			c = *s++;
			if(pal[c] != key)
				*d = pal[c];
			d = (unsigned short*)((char*)d + dx);
			j -= 4;
		}
		while(j != 0)
		{
			int c = *s++;
			if(pal[c] != key)
				*d = pal[c];
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s += sy;
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD24S8PK(void *dst, void *src, BlitParam *par)
{
	unsigned int *d = (unsigned int*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned int *pal = (unsigned int*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;
	int key = par->colorKey;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j >= 4)
		{
			int c = *s++;
			if(c != key)
				*d = pal[c];
			d = (unsigned int*)((char*)d + dx);
			c = *s++;
			if(c != key)
				*d = pal[c];
			d = (unsigned int*)((char*)d + dx);
			c = *s++;
			if(c != key)
				*d = pal[c];
			d = (unsigned int*)((char*)d + dx);
			c = *s++;
			if(c != key)
				*d = pal[c];
			d = (unsigned int*)((char*)d + dx);
			j -= 4;
		}
		while(j != 0)
		{
			int c = *s++;
			if(c != key)
				*d = pal[c];
			d = (unsigned int*)((char*)d + dx);
			--j;
		}
		s += sy;
		d = (unsigned int*)((char*)d + dy);
	}
}

void BlitD12S8PKO(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned short *pal = (unsigned short*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;

	const int a = par->opacity;
	const unsigned int key = par->colorKey;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int c = *s++;
			if(c != key)
			{
				unsigned int fg = pal[c];
				unsigned int bg = *d;
				ALPHA_BLEND_S12_D12(*d, bg, fg, a);
			}
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD16S8PKO(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned short *pal = (unsigned short*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;

	const int a = par->opacity;
	const unsigned int key = par->colorKey;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int c = *s++;
			unsigned int fg = pal[c];
			
			if(fg != key)
			{
				unsigned int bg = *d;
				ALPHA_BLEND_S16_D16(*d, bg, fg, a);
			}
			
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD24S8PKO(void *dst, void *src, BlitParam *par)
{
	unsigned int *d = (unsigned int*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned int *pal = (unsigned int*)par->palette;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;

	const int a = par->opacity;
	const unsigned int key = par->colorKey;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int c = *s++;
			if(c != key)
			{
				unsigned int fg = pal[c];
				unsigned int bg = *d;
				ALPHA_BLEND_S24_D24(*d, bg, fg, a);
			}
			d = (unsigned int*)((char*)d + dx);
			--j;
		}
		s = (s + sy);
		d = (unsigned int*)((char*)d + dy);
	}
}

void BlitD16S32A(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned int *s = (unsigned int*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * 4;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int fg = *s++;
			int bg = *d;
			int a = fg >> 24;
			ALPHA_BLEND_S32_D16(*d, bg, fg, a);
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (unsigned int*)((char*)s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD24S32A(void *dst, void *src, BlitParam *par)
{
	unsigned int *d = (unsigned int*)dst;
	unsigned int *s = (unsigned int*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * 4;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int fg = *s++;
			int bg = *d;
			int a = fg >> 24;
			ALPHA_BLEND_S24_D24(*d, bg, fg, a);
			d = (unsigned int*)((char*)d + dx);
			--j;
		}
		s = (unsigned int*)((char*)s + sy);
		d = (unsigned int*)((char*)d + dy);
	}
}

void BlitD12S32AO(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned int *s = (unsigned int*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * 4;
	const unsigned int opacity = par->opacity;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int fg = *s++;
			int bg = *d;
			int a = (fg >> 24) * opacity / ALPHA_DEVISOR;
			ALPHA_BLEND_S32_D12(*d, bg, fg, a);

			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (unsigned int*)((char*)s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD16S32AO(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned int *s = (unsigned int*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * 4;
	const unsigned int opacity = par->opacity;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int fg = *s++;
			int bg = *d;
			int a = (fg >> 24) * opacity / ALPHA_DEVISOR;
			ALPHA_BLEND_S32_D16(*d, bg, fg, a);

			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (unsigned int*)((char*)s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD24S32AO(void *dst, void *src, BlitParam *par)
{
	unsigned int *d = (unsigned int*)dst;
	unsigned int *s = (unsigned int*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * 4;
	const unsigned int opacity = par->opacity;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int fg = *s++;
			int bg = *d;
			int a = (fg >> 24) * opacity / ALPHA_DEVISOR;
			ALPHA_BLEND_S24_D24(*d, bg, fg, a);
			d = (unsigned int*)((char*)d + dx);
			--j;
		}
		s = (unsigned int*)((char*)s + sy);
		d = (unsigned int*)((char*)d + dy);
	}
}

void BlitD12S32A(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned int *s = (unsigned int*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * 4;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = par->width;
		while(j != 0)
		{
			unsigned int fg = *s++;
			int bg = *d;
			int a = fg >> 24;
			ALPHA_BLEND_S32_D12(*d, bg, fg, a);
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (unsigned int*)((char*)s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD16S8AB(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned short *pal = (unsigned short*)par->palette;
	unsigned char *alpha = (unsigned char*)par->alpha;

	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;
	int brightness =  par->brightness;

	ASSERT(par->height > 0);

	for(int i = par->height; i != 0; i--)
	{
		register int j = par->width;

		while(j != 0)
		{
			const unsigned int fg = pal[*s++];
			const unsigned int bg = *d;
			const unsigned char a = *alpha++;		
			ALPHA_BLEND_S16_D16(*d, bg, fg, a);			
			CHANGE_BRIGHTNESS_D16(*d, brightness);		
			d = (unsigned short*)((char*)d + dx);

			--j;
		}
		s += sy;
		alpha += sy;
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD16S8A(void *dst, void *src, BlitParam *par)
{
	if(par->brightness != 0)
	{
		BlitD16S8AB(dst, src, par);
		return;
	}

	unsigned short *d = (unsigned short*)dst;
	unsigned char *s = (unsigned char*)src;
	unsigned short *pal = (unsigned short*)par->palette;
	unsigned char *alpha = (unsigned char*)par->alpha;

	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width;

	ASSERT(par->height > 0);
	
	for(int i = par->height; i != 0; i--)
	{
		register int j = par->width;
		
		while(j != 0)
		{
			const unsigned int fg = pal[*s++];
			const unsigned int bg = *d;
			const unsigned char a = *alpha++;
			ALPHA_BLEND_S16_D16(*d, bg, fg, a);
			//*d = fg;
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s += sy;
		alpha += sy;
		d = (unsigned short*)((char*)d + dy);
	}
}



void FillRect16(void *dst, int w, int h, int pitch, unsigned int nativeRgb)
{
	ASSERT(h > 0);
	if(((int)dst & 3) == 0 && (w & 1) == 0)
	{
		unsigned int fclr = (nativeRgb << 16) | nativeRgb;
		unsigned int *dst32 = (unsigned int*)dst;
		unsigned int dy = pitch - w * 2;
		while(h != 0)
		{
			int i = w;
			while(i >= 8)
			{
				*dst32++ = fclr;
				*dst32++ = fclr;
				*dst32++ = fclr;
				*dst32++ = fclr;
				i -= 8;
			}
			while(i != 0)
			{
				*dst32++ = fclr;
				i -= 2;
			}
			dst32 = (unsigned int*)((char*)dst32 + dy);
			--h;
		}
	}
	else
	{
		unsigned short *d = (unsigned short*)dst;
		unsigned int fclr = nativeRgb;
		while(h != 0)
		{
			unsigned short *end = d + w;
			while(d < end)
			{
				*d++ = fclr;
			}
			d = (unsigned short*)((char*)(d - w) + pitch);
			--h;
		}
	}
}

void FillRect24(void *dst, int w, int h, int pitch, unsigned int nativeRgb)
{
	unsigned int *dst32 = (unsigned int*)dst;
	unsigned int dy = pitch - w * 4;
	ASSERT(h > 0);
	while(h != 0)
	{
		int i = w;
		while(i >= 4)
		{
			*dst32++ = nativeRgb;
			*dst32++ = nativeRgb;
			*dst32++ = nativeRgb;
			*dst32++ = nativeRgb;
			i -= 4;
		}
		while(i != 0)
		{
			*dst32++ = nativeRgb;
			i--;
		}
		dst32 = (unsigned int*)((char*)dst32 + dy);
		--h;
	}
}

void FillRectARGB12(void *dst, int w, int h, int pitch, unsigned int argb)
{
	unsigned int fa = argb >> 24;
	unsigned int fr = ((argb >> 16) & 0xff) * fa / ALPHA_DEVISOR >> 4;
	unsigned int fg = ((argb >> 8) & 0xff) * fa / ALPHA_DEVISOR >> 4;
	unsigned int fb = ((argb) & 0xff) * fa / ALPHA_DEVISOR >> 4;
	unsigned int fclr = (fr << 8) | (fg << 4) | (fb);

	fa = ALPHA_DEVISOR - fa;
	unsigned short *d = (unsigned short*)dst;
	while(h != 0)
	{
		unsigned short *end = d + w;
		while(d < end)
		{
			unsigned int v = *d;
			unsigned int r = ((v >> 8) & 0xf) * fa / ALPHA_DEVISOR;
			unsigned int g = ((v >> 4) & 0xf) * fa / ALPHA_DEVISOR;
			unsigned int b = ((v) & 0xf) * fa / ALPHA_DEVISOR;
			*d++ = ((r << 8) | (g << 4) | (b)) + fclr;
		}
		d = (unsigned short*)((char*)(d - w) + pitch);
		--h;
	}
}

void FillRectARGB16(void *dst, int w, int h, int pitch, unsigned int argb)
{
	unsigned int fa = argb >> 24;
	unsigned int fr = ((argb >> 16) & 0xff) * fa / ALPHA_DEVISOR >> 3;
	unsigned int fg = ((argb >> 8) & 0xff) * fa / ALPHA_DEVISOR >> 2;
	unsigned int fb = ((argb) & 0xff) * fa / ALPHA_DEVISOR >> 3;
	unsigned int fclr = (fr << 11) | (fg << 5) | (fb);

	fa = ALPHA_DEVISOR - fa;
	unsigned short *d = (unsigned short*)dst;
	while(h != 0)
	{
		unsigned short *end = d + w;
		while(d < end)
		{
			unsigned int v = *d;
			unsigned int r = ((v >> 11) & 0x1f) * fa / ALPHA_DEVISOR;
			unsigned int g = ((v >> 5) & 0x3f) * fa / ALPHA_DEVISOR;
			unsigned int b = ((v) & 0x1f) * fa / ALPHA_DEVISOR;
			*d++ = ((r << 11) | (g << 5) | (b)) + fclr;
		}
		d = (unsigned short*)((char*)(d - w) + pitch);
		--h;
	}
}

void FillRectARGB24(void *dst, int w, int h, int pitch, unsigned int argb)
{
	unsigned int fa = argb >> 24;
	unsigned int fr = ((argb >> 16) & 0xff) * fa / ALPHA_DEVISOR;
	unsigned int fg = ((argb >> 8) & 0xff) * fa / ALPHA_DEVISOR;
	unsigned int fb = ((argb) & 0xff) * fa / ALPHA_DEVISOR;
	unsigned int fclr = (fr << 16) | (fg << 8) | (fb);

	fa = ALPHA_DEVISOR - fa;
	unsigned int *d = (unsigned int*)dst;
	while(h != 0)
	{
		unsigned int *end = d + w;
		while(d < end)
		{
			unsigned int v = *d;
			unsigned int r = ((v >> 16) & 0xff) * fa / ALPHA_DEVISOR;
			unsigned int g = ((v >> 8) & 0xff) * fa / ALPHA_DEVISOR;
			unsigned int b = ((v) & 0xff) * fa / ALPHA_DEVISOR;
			*d++ = ((r << 16) | (g << 8) | (b)) + fclr;
		}
		d = (unsigned int*)((char*)(d - w) + pitch);
		--h;
	}
}

void BlitD16S16In4Byte(void *dst, void *src, BlitParam *par)
{
	register unsigned int *d = (unsigned int*)dst;
	register unsigned int *s = (unsigned int*)src;
	
	int sw = (par->dstXPitch > 0 ? par->width : -par->width) >> 1;
	int s_space = ((par->srcYPitch >> 1) - par->width) >> 1;
	int dw = par->dstXPitch > 0 ? par->width : -par->width;
	int d_space = ((par->dstYPitch >> 1) - dw) >> 1;

	int height = par->height;

	if(dw > 0)
	{
		for(int i = height; i != 0; i--)
		{
			unsigned int *e = d + sw;
			
			if(d != e)
			{

#define BLIT1	*d++ = *s++;\
				*d++ = *s++;\
				*d++ = *s++;\
				*d++ = *s++;
			
				while(e - d > 24)
				{
					BLIT1
					BLIT1
					BLIT1
					BLIT1
					BLIT1
					BLIT1
				}

				while(e - d > 12)
				{
					BLIT1
					BLIT1
					BLIT1
				}

				while(e - d > 4)
				{
					BLIT1
				}

				while(d != e)
				{
					*d++ = *s++;
				}
#undef BLIT1
			}

			s += s_space;
			d += d_space;
		}
	}
	else
	{
		ASSERT(0);
	}	
}

void BlitD16S16In2Byte(void *dst, void *src, BlitParam *par)
{
	register unsigned short *d = (unsigned short*)dst;
	register unsigned short *s = (unsigned short*)src;

	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sw = par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	int height = par->height;
	
	if(dx > 0)
	{
		for(int i = height; i != 0; i--)
		{
			unsigned short* e = (unsigned short*)((char*)d + sw);
			
			if(d != e)
			{

#define BLIT1	*d++ = *s++;\
				*d++ = *s++;\
				*d++ = *s++;\
				*d++ = *s++;
			
				while(e - d > 24)
				{
					BLIT1
					BLIT1
					BLIT1
					BLIT1
					BLIT1
					BLIT1
				}

				while(e - d > 12)
				{
					BLIT1
					BLIT1
					BLIT1
				}

				while(e - d > 4)
				{
					BLIT1
				}

				while(d != e)
				{
					*d++ = *s++;
				}
#undef BLIT1
			}

			s = (unsigned short*)((char*)s + sy);
			d = (unsigned short*)((char*)d + dy);
		}
	}
	else
	{
		for(int i = height; i != 0; i--)
		{
			unsigned short* e = (unsigned short*)((char*)d + sw);
			
			while(d != e)
			{	
#define BLIT2	*d-- = *s++;\
				*d-- = *s++;\
				*d-- = *s++;\
				*d-- = *s++;\
			
				while(d - e > 24)
				{
					BLIT2
					BLIT2
					BLIT2
					BLIT2
					BLIT2
					BLIT2
				}

				while(d - e> 12)
				{
					BLIT2
					BLIT2
					BLIT2
				}

				while(d - e> 4)
				{
					BLIT2		
				}
				
				while(d != e)
				{
					*d++ = *s++;
				}
#undef BLIT2
			}

			s = (unsigned short*)((char*)s + sy);
			d = (unsigned short*)((char*)d + dy);
		}
	}
}

void BlitD16S16(void *dst, void *src, BlitParam *par)
{
	//if(((int)dst & 0x03) == 0 
	//	&& ((int)src & 0x03) == 0 
	//	&& (par->width & 0x01) == 0 
	//	&& (par->dstYPitch & 0x03) == 0
	//	&& par->dstXPitch > 0)
	//{
	//	BlitD16S16In4Byte(dst, src, par);		
	//}
	//else
	{
		BlitD16S16In2Byte(dst, src, par);
	}
}

void BlitD16S16A(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned short *s = (unsigned short*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	const int w = par->width;

	for(int i = par->height; i != 0; i--)
	{
		int j = w;
		while(j != 0)
		{
			int fg = *s;
			int bg = *d;
			int a = *s & 0xF;

			int t;
			t = (bg >> 11) & 0x1f;
			int r = t + (((fg >> 11) & 0x1e) - t) * a / 15;
			t = (bg >> 5) & 0x3f;
			int g = t + (((fg >> 6) & 0x3c) - t) * a / 15;
			t = (bg) & 0x1f;
			int b = t + (((fg >> 3) & 0x1e) - t) * a / 15;
			
			*d = (r << 11) | (g << 5) | b;

			++s;

			d = (unsigned short*)((char*)d + dx);
		
			--j;
		}
		s = (unsigned short*)((char*)s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD24S24(void *dst, void *src, BlitParam *par)
{
	unsigned int *d = (unsigned int*)dst;
	unsigned int *s = (unsigned int*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	const int w = par->width;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = w;
		while(j != 0)
		{
			*d = *s++;
			d = (unsigned int*)((char*)d + dx);
			--j;
		}
		s = (unsigned int*)((char*)s + sy);
		d = (unsigned int*)((char*)d + dy);
	}
}

void BlitD12S12O(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned short *s = (unsigned short*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	const int w = par->width;
	const int a = par->opacity;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = w;
		while(j != 0)
		{
			unsigned int fg = *s++;
			unsigned int bg = *d;
			ALPHA_BLEND_S12_D12(*d, bg, fg, a);
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (unsigned short*)((char*)s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD16S16O(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned short *s = (unsigned short*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	const int w = par->width;
	const int a = par->opacity;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = w;
		while(j != 0)
		{
			unsigned int fg = *s++;
			unsigned int bg = *d;
			ALPHA_BLEND_S16_D16(*d, bg, fg, a);
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (unsigned short*)((char*)s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD24S24O(void *dst, void *src, BlitParam *par)
{
	unsigned int *d = (unsigned int*)dst;
	unsigned int *s = (unsigned int*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	const int w = par->width;
	const int a = par->opacity;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = w;
		while(j != 0)
		{
			unsigned int fg = *s++;
			unsigned int bg = *d;
			ALPHA_BLEND_S24_D24(*d, bg, fg, a);
			d = (unsigned int*)((char*)d + dx);
			--j;
		}
		s = (unsigned int*)((char*)s + sy);
		d = (unsigned int*)((char*)d + dy);
	}
}

void BlitD16S16K(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned short *s = (unsigned short*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	const int w = par->width;
	const unsigned int key = par->colorKey;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = w;
		while(j != 0)
		{
			unsigned int c = *s++;
			if(c != key)
				*d = c;
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (unsigned short*)((char*)s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD24S24K(void *dst, void *src, BlitParam *par)
{
	unsigned int *d = (unsigned int*)dst;
	unsigned int *s = (unsigned int*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	const int w = par->width;
	const unsigned int key = par->colorKey;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = w;
		while(j != 0)
		{
			unsigned int c = *s++;
			if(c != key)
				*d = c;
			d = (unsigned int*)((char*)d + dx);
			--j;
		}
		s = (unsigned int*)((char*)s + sy);
		d = (unsigned int*)((char*)d + dy);
	}
}

void BlitD12S12KO(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned short *s = (unsigned short*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	const int w = par->width;
	const int a = par->opacity;
	const unsigned int key = par->colorKey;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = w;
		while(j != 0)
		{
			unsigned int fg = *s++;
			if(fg != key)
			{
				unsigned int bg = *d;
				ALPHA_BLEND_S12_D12(*d, bg, fg, a);
			}
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (unsigned short*)((char*)s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD16S16KO(void *dst, void *src, BlitParam *par)
{
	unsigned short *d = (unsigned short*)dst;
	unsigned short *s = (unsigned short*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	const int w = par->width;
	const int a = par->opacity;
	const unsigned int key = par->colorKey;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = w;
		while(j != 0)
		{
			unsigned int fg = *s++;
			if(fg != key)
			{
				unsigned int bg = *d;
				ALPHA_BLEND_S16_D16(*d, bg, fg, a);
			}
			d = (unsigned short*)((char*)d + dx);
			--j;
		}
		s = (unsigned short*)((char*)s + sy);
		d = (unsigned short*)((char*)d + dy);
	}
}

void BlitD24S24KO(void *dst, void *src, BlitParam *par)
{
	unsigned int *d = (unsigned int*)dst;
	unsigned int *s = (unsigned int*)src;
	int dx = par->dstXPitch;
	int dy = par->dstYPitch - par->width * dx;
	int sy = par->srcYPitch - par->width * sizeof(*s);

	const int w = par->width;
	const int a = par->opacity;
	const unsigned int key = par->colorKey;
	
	for(int i = par->height; i != 0; i--)
	{
		int j = w;
		while(j != 0)
		{
			unsigned int fg = *s++;
			if(fg != key)
			{
				unsigned int bg = *d;
				ALPHA_BLEND_S24_D24(*d, bg, fg, a);
			}
			d = (unsigned int*)((char*)d + dx);
			--j;
		}
		s = (unsigned int*)((char*)s + sy);
		d = (unsigned int*)((char*)d + dy);
	}
}

void DrawLine16(unsigned short *dst, int dx, int dy, int pitch, unsigned int nativeColor)
{
	int	dx2, dy2, x_inc, y_inc;

	// test which direction the line is going in i.e. slope angle
	if(dx >= 0)
	{
		x_inc = sizeof(*dst);
	} // end if line is moving right
	else
	{
		x_inc = -(int)sizeof(*dst);
		dx    = -dx;  // need absolute value
	} // end else moving left
	
	// test y component of slope
	if(dy >= 0)
	{
		y_inc = pitch;
	} // end if line is moving down
	else
	{
		y_inc = -pitch;
		dy    = -dy;  // need absolute value
	} // end else moving up
	
	if(dx < dy)
	{
		int tmp = x_inc;
		x_inc = y_inc;
		y_inc = tmp;

		tmp = dx;
		dx = dy;
		dy = tmp;
	}

	// compute (dx,dy) * 2
	dx2 = dx << 1;
	dy2 = dy << 1;

	int error = dy2 - dx;
	while(dx >= 0)
	{
		*dst = nativeColor; // set the pixel
		if (error >= 0)
		{
			error -= dx2;
			dst = (unsigned short*)((char*)dst + y_inc); // move to next line
		} // end if error overflowed
		dst = (unsigned short*)((char*)dst + x_inc); // move to the next pixel
		error += dy2; // adjust the error term
		--dx;
	}
}

void DrawLine24(unsigned int *dst, int dx, int dy, int pitch, unsigned int nativeColor)
{
	int	dx2, dy2, x_inc, y_inc;

	// test which direction the line is going in i.e. slope angle
	if(dx >= 0)
	{
		x_inc = sizeof(*dst);
	} // end if line is moving right
	else
	{
		x_inc = -(int)sizeof(*dst);
		dx    = -dx;  // need absolute value
	} // end else moving left
	
	// test y component of slope
	if(dy >= 0)
	{
		y_inc = pitch;
	} // end if line is moving down
	else
	{
		y_inc = -pitch;
		dy    = -dy;  // need absolute value
	} // end else moving up
	
	if(dx < dy)
	{
		int tmp = x_inc;
		x_inc = y_inc;
		y_inc = tmp;

		tmp = dx;
		dx = dy;
		dy = tmp;
	}

	// compute (dx,dy) * 2
	dx2 = dx << 1;
	dy2 = dy << 1;

	int error = dy2 - dx;
	while(dx >= 0)
	{
		*dst = nativeColor; // set the pixel
		if (error >= 0)
		{
			error -= dx2;
			dst = (unsigned int*)((char*)dst + y_inc); // move to next line
		} // end if error overflowed
		error += dy2; // adjust the error term
		dst = (unsigned int*)((char*)dst + x_inc); // move to the next pixel
		--dx;
	}
}

static int findRegion(int x, int y, int w, int h)
{
    int code=0;
    if(y < 0)
    code |= 1; //top
    else if(y >= h)
    code |= 2; //bottom
    if(x >= w)
    code |= 4; //right
    else if (x < 0)
    code |= 8; //left
    return code;
}

/**
 * Clip a line by a rectangle.
 * @clipRect	pointer to the rectangle(x, y, w, h)
 * @line		pointer to the line to be clipped(x0, y0, x1, y1)
 * @return		ture if have a trivial accept or false if a trivial reject
*/
bool ClipLine(int *clipRect, int* line)
{
    int code1, code2, codeout;
    bool accept = false, done = false;
	int x1 = line[0] - clipRect[0];
	int x2 = line[2] - clipRect[0];
	int y1 = line[1] - clipRect[1];
	int y2 = line[3] - clipRect[1];

    code1 = findRegion(x1, y1, clipRect[2], clipRect[3]); //the region outcodes for the endpoints
    code2 = findRegion(x2, y2, clipRect[2], clipRect[3]);
    do  //In theory, this can never end up in an infinite loop, it'll always come in one of the trivial cases eventually
    {
        if(!(code1 | code2))
			accept = done = true;  //accept because both endpoints are in screen or on the border, trivial accept
        else if(code1 & code2)
			done = true; //the line isn't visible on screen, trivial reject
		else  //if no trivial reject or accept, continue the loop
        {
            int x, y;
            codeout = code1 ? code1 : code2;
            if(codeout & 1) //top
            {
                x = x1 - (x2 - x1) * y1 / (y2 - y1);
                y = 0;
            }
            else if(codeout & 2) //bottom
            {
                x = x1 + (x2 - x1) * (clipRect[3] - y1) / (y2 - y1);
                y = clipRect[3] - 1;
            }
            else if(codeout & 4) //right
            {
                y = y1 + (y2 - y1) * (clipRect[2] - x1) / (x2 - x1);
                x = clipRect[2] - 1;
            }
            else //left
            {
                y = y1 - (y2 - y1) * x1 / (x2 - x1);
                x = 0;
            }
			if(codeout == code1) //first endpoint was clipped
            {
                x1 = x; y1 = y;
                code1 = findRegion(x1, y1, clipRect[2], clipRect[3]);
            }
            else //second endpoint was clipped
            {
                x2 = x; y2 = y;
                code2 = findRegion(x2, y2, clipRect[2], clipRect[3]);
            }
        }
    }
    while(done == false);
	if(accept)
    {
		line[0] = x1 + clipRect[0];
		line[2] = x2 + clipRect[0];
		line[1] = y1 + clipRect[1];
		line[3] = y2 + clipRect[1];
        return true;
    }
    else
    {
		return false;
    }
}

void CGraphics::DrawRect(int x, int y, int width, int height)
{
	DrawLine(x, y, x + width, y);
	DrawLine(x, y + height, x + width+1, y + height);//
	DrawLine(x, y, x, y + height);
	DrawLine(x + width, y, x + width, y + height);
}

void CGraphics::DrawRoundRect(int x, int y, int width, int height, int arcWidth, int arcHeight)
{
	ASSERT(arcWidth == arcHeight);

	short clipRct[4];
	memcpy(clipRct, m_clipRect, sizeof(clipRct));

	DrawLine(x + arcWidth, y, x + width - arcWidth, y);
	DrawLine(x + arcWidth, y + height, x + width - arcWidth, y + height);
	DrawLine(x,	y + arcHeight,	x,	y + height - arcHeight);
	DrawLine(x + width, y + arcHeight,	x + width, y + height - arcHeight);

	int r = arcWidth;

	SetClip(x, y, r + 1, r + 1);
	DrawCircle(x + r, y + r, r);

	SetClip(x + width - r, y, r + 1, r + 1);
	DrawCircle(x + width - r, y + r, r);

	SetClip(x , y + height - r, r + 1, r + 1);
	DrawCircle(x + r,y + height - r, r);

	SetClip(x + width - r , y + height - r, r + 1, r + 1);
	DrawCircle(x + width - r,y + height - r, r);

	memcpy(m_clipRect, clipRct, sizeof(m_clipRect));
}

void CGraphics::DrawLine(int x0, int y0, int x1, int y1)
{
	int r[4];
	r[0] = m_clipRect[0];
	r[1] = m_clipRect[1];
	r[2] = m_clipRect[2] - m_clipRect[0];
	r[3] = m_clipRect[3] - m_clipRect[1];
	int l[4] = {
		ToRawX(x0), ToRawX(y0), ToRawY(x1), ToRawY(y1)
	};

	if(ClipLine(r, l))
		DrawLine_NoClip_NoTranslat(l[0], l[1], l[2], l[3]);
}

void CGraphics::DrawRegion(CImage *pSrc, int x_src, int y_src, int w_src, int h_src, int transform,
	int x_dest, int y_dest, int w_dest, int h_dest, int x_pivot, int y_pivot, int angle, unsigned int palette)
{
	Vertex2D v[4];

    int i;
    unsigned int f = TransformToFlag(transform);

    if (f & FLIP_X)
    {
        v[0].screen.x = ToRawX(x_dest + w_dest);
       	v[1].screen.x = ToRawX(x_dest);
        v[2].screen.x = ToRawX(x_dest);
        v[3].screen.x = ToRawX(x_dest + w_dest);
    }
    else
    {
        v[0].screen.x = ToRawX(x_dest);
        v[1].screen.x = ToRawX(x_dest + w_dest);
        v[2].screen.x = ToRawX(x_dest + w_dest);
        v[3].screen.x = ToRawX(x_dest);
    }

    if (f & FLIP_Y)
    {
        v[0].screen.y = ToRawY(y_dest + h_dest);
        v[1].screen.y = ToRawY(y_dest + h_dest);
        v[2].screen.y = ToRawY(y_dest);
        v[3].screen.y = ToRawY(y_dest);
    }
	else
    {
        v[0].screen.y = ToRawY(y_dest);
        v[1].screen.y = ToRawY(y_dest);
        v[2].screen.y = ToRawY(y_dest + h_dest);
        v[3].screen.y = ToRawY(y_dest + h_dest);
    }

    if (f & ROTATE_90)
    {
        // rotate the specified image region clockwise by 90 degrees
        int xcenter = (v[0].screen.x + v[1].screen.x) / 2;
        int ycenter = (v[0].screen.y + v[3].screen.y) / 2;
        int x_coord;
        for(i = 0; i < 4; i++)
        {
            x_coord = v[i].screen.x;
            v[i].screen.x = xcenter - (v[i].screen.y - ycenter);
            v[i].screen.y = ycenter + (x_coord - xcenter);
        }
    }

	v[0].texture.x = (x_src);
	v[0].texture.y = (y_src);
	v[1].texture.x = (x_src + w_src);
	v[1].texture.y = (y_src);
	v[2].texture.x = (x_src + w_src);
	v[2].texture.y = (y_src + h_src);
	v[3].texture.x = (x_src);
	v[3].texture.y = (y_src + h_src);

    CFMath::Point2D pivot;
	pivot.x = ToRawX(x_pivot);
	pivot.y = ToRawY(y_pivot);
    
	for(i = 0; i < 4; i++)
	{
		CFMath::Rotate(v[i].screen, v[i].screen, pivot, angle);
	}
	DrawPoly4T(v, pSrc, palette);
//	Vertex2D test[4];
//	test[0] = v[2];
//	test[1] = v[3];
//	test[2] = v[0];
//	DrawPoly3T(v, pSrc, palette);
//	DrawPoly3T(test, pSrc, palette);
}

bool CGraphics::CSpanGenerator::Prepare(int yMin, int yMax, int clipY0, int clipY1)
{
	m_clipY0 = clipY0;
	m_clipY1 = clipY1;
	m_yMin = yMin >= m_clipY0 ? yMin : m_clipY0;
	m_yMax = yMax <= m_clipY1 ? yMax : m_clipY1;
	int n = m_yMax - m_yMin;
	ASSERT(n < MAX_SPAN);
	if(n <= 0 || n >= MAX_SPAN)
		return false;
	int i;
	for(i = 0; i < n; i++)
	{
		m_span[i].xMax = -32768;
		m_span[i].xMin = 32767;
	}
	//m_math = &GetMath();
	return true;
}

void CGraphics::CSpanGenerator::EmitEdgeT(Vertex2D *p1, Vertex2D *p2)
{
	Vertex2D *vTop, *vBottom;
	//sort by Y
	if(p1->screen.y <= p2->screen.y)
	{
		vTop = p1;
		vBottom = p2;
	}
	else
	{
		vTop = p2;
		vBottom = p1;
	}

	int yStart = vTop->screen.y;
	int ySkip = 0;
	if(yStart < m_yMin)
	{
		ySkip = m_yMin - yStart;
		yStart = m_yMin;
	}

	int h = vBottom->screen.y > m_yMax ? m_yMax - yStart : vBottom->screen.y - yStart;
	if(h <= 0)
		return;

	int deltaY = vBottom->screen.y - vTop->screen.y;
	int deltaX = vBottom->screen.x - vTop->screen.x;
	int xStart, xStep, numerator;
	int errorTerm;
	CFMath::FloorDivMod(deltaX * (yStart - vTop->screen.y) - 1, deltaY, xStart, errorTerm);
	xStart += vTop->screen.x + 1;
	CFMath::FloorDivMod(deltaX, deltaY, xStep, numerator);

	F12 u = IntToF12(vTop->texture.x);
	ASSERT(0);
	F12 du = 0;//GetGlobal().m_pMath->FastDiv(IntToF12(vBottom->texture.x - vTop->texture.x), deltaY);
	//see the comment in DrawSpanT for the reason to decrease u du < 0
	if(du < 0)
		--u;

	F12 v = IntToF12(vTop->texture.y);
	F12 dv = 0;//GetGlobal().m_pMath->FastDiv(IntToF12(vBottom->texture.y - vTop->texture.y), deltaY);
	if(dv < 0)
		--v;

	if(ySkip)
	{
		u += du * ySkip;
		v += dv * ySkip;
	}

	SpanT *span = m_span + yStart - m_yMin;
	do {
		ASSERT(m_span <= span && m_span + MAX_SPAN > span);
		if(xStart > span->xMax)
		{
			span->xMax = xStart;
			span->uMax = u;
			span->vMax = v;
		}
		if(xStart < span->xMin)
		{
			span->xMin = xStart;
			span->uMin = u;
			span->vMin = v;
		}
		xStart += xStep;
		errorTerm += numerator;
		if(errorTerm >= deltaY)
		{
			xStart++;
			errorTerm -= deltaY;
		}
		u += du;
		v += dv;
		span++;
	}while(--h != 0);
}

void CGraphics::CSpanGenerator::EmitEdge(Vertex2D *p1, Vertex2D *p2)
{
	Vertex2D *vTop, *vBottom;
	//sort by Y
	if(p1->screen.y <= p2->screen.y)
	{
		vTop = p1;
		vBottom = p2;
	}
	else
	{
		vTop = p2;
		vBottom = p1;
	}

	int yStart = vTop->screen.y;
	int ySkip = 0;
	if(yStart < m_yMin)
	{
		ySkip = m_yMin - yStart;
		yStart = m_yMin;
	}

	int h = vBottom->screen.y > m_yMax ? m_yMax - yStart : vBottom->screen.y - yStart;
	if(h <= 0)
		return;

	int deltaY = vBottom->screen.y - vTop->screen.y;
	int deltaX = vBottom->screen.x - vTop->screen.x;
	int xStart, xStep, numerator;
	int errorTerm;
	CFMath::FloorDivMod(deltaX * (yStart - vTop->screen.y) - 1, deltaY, xStart, errorTerm);
	xStart += vTop->screen.x + 1;
	CFMath::FloorDivMod(deltaX, deltaY, xStep, numerator);

	SpanT *span = m_span + yStart - m_yMin;
	do {
		ASSERT(m_span <= span && m_span + MAX_SPAN > span);
		if(xStart > span->xMax)
		{
			span->xMax = xStart;
		}
		if(xStart < span->xMin)
		{
			span->xMin = xStart;
		}
		xStart += xStep;
		errorTerm += numerator;
		if(errorTerm >= deltaY)
		{
			xStart++;
			errorTerm -= deltaY;
		}
		span++;
	}while(--h != 0);
}

void CGraphics::DrawPoly4T(Vertex2D *point, CImage *texture, unsigned int palette)
{
	int yMin, yMax;
	yMin = yMax = point[0].screen.y;
	if(point[1].screen.y < yMin)
		yMin = point[1].screen.y;
	else if(point[1].screen.y > yMax)
		yMax = point[1].screen.y;
	if(point[2].screen.y < yMin)
		yMin = point[2].screen.y;
	else if(point[2].screen.y > yMax)
		yMax = point[2].screen.y;
	if(point[3].screen.y < yMin)
		yMin = point[3].screen.y;
	else if(point[3].screen.y > yMax)
		yMax = point[3].screen.y;
	CSpanGenerator sg;
	if(!sg.Prepare(yMin, yMax, m_clipRect[1], m_clipRect[3]))
		return;
	sg.EmitEdgeT(point, point + 1);
	sg.EmitEdgeT(point + 1, point + 2);
	sg.EmitEdgeT(point + 2, point + 3);
	sg.EmitEdgeT(point + 3, point);
	int i, n = sg.m_yMax - sg.m_yMin;
	ASSERT(0);
	for(i = 0; i < n; i++)
	{
		//DrawSpanT(i + sg.m_yMin, sg.m_span + i, texture, palette, GetGlobal().m_pMath);
	}
}

void CGraphics::DrawPoly3T(Vertex2D *point, CImage *texture, unsigned int palette)
{
	int yMin, yMax;
	yMin = yMax = point[0].screen.y;
	if(point[1].screen.y < yMin)
		yMin = point[1].screen.y;
	else if(point[1].screen.y > yMax)
		yMax = point[1].screen.y;
	if(point[2].screen.y < yMin)
		yMin = point[2].screen.y;
	else if(point[2].screen.y > yMax)
		yMax = point[2].screen.y;
	CSpanGenerator sg;
	if(!sg.Prepare(yMin, yMax, m_clipRect[1], m_clipRect[3]))
		return;
	sg.EmitEdgeT(point, point + 1);
	sg.EmitEdgeT(point + 1, point + 2);
	sg.EmitEdgeT(point + 2, point);
	int i, n = sg.m_yMax - sg.m_yMin;
	ASSERT(0);
	for(i = 0; i < n; i++)
	{
		//DrawSpanT(i + sg.m_yMin, sg.m_span + i, texture, palette, GetGlobal().m_pMath);
	}
}

void FillHLineTD16S8PK(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette)
{
	unsigned short *pos = (unsigned short*)pStart + x;
	unsigned short *end = pos + length;
	CIndexedImage *timage = DYNAMIC_CAST(CIndexedImage*, texture);
	unsigned char *pixel = timage->GetPixelData();
	unsigned short *pal = (unsigned short*)timage->GetPaletteData() + palette;
	int key = timage->GetColorKey();
	const int tpitch = timage->GetPitch();

	while (pos < end)
	{
		int tu = F12ToInt(uStart);
		int tv = F12ToInt(vStart);
#if DEBUG_LEVEL > 1
		ASSERT(tu >= 0 && tu < timage->GetWidth());
  		ASSERT(tv >= 0 && tv < timage->GetHeight());
#endif
		int v = pixel[tv * tpitch + tu];
		if(v != key)
			*pos = pal[v];
		++pos;
		uStart += dux;
		vStart += dvx;
	}
}

void FillHLineTD24S8PK(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette)
{
	unsigned int *pos = (unsigned int*)pStart + x;
	unsigned int *end = pos + length;
	CIndexedImage *timage = DYNAMIC_CAST(CIndexedImage*, texture);
	unsigned char *pixel = timage->GetPixelData();
	unsigned int *pal = (unsigned int*)timage->GetPaletteData() + palette;
	int key = timage->GetColorKey();
	const int tpitch = timage->GetPitch();

	while (pos < end)
	{
		int tu = F12ToInt(uStart);
		int tv = F12ToInt(vStart);
#if DEBUG_LEVEL > 1
		ASSERT(tu >= 0 && tu < timage->GetWidth());
  		ASSERT(tv >= 0 && tv < timage->GetHeight());
#endif
		int v = pixel[tv * tpitch + tu];
		if(v != key)
			*pos = pal[v];
		++pos;
		uStart += dux;
		vStart += dvx;
	}
}

void FillHLineTD12S8PKO(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette, int opacity)
{
	unsigned short *pos = (unsigned short*)pStart + x;
	unsigned short *end = pos + length;
	CIndexedImage *timage = DYNAMIC_CAST(CIndexedImage*, texture);
	unsigned char *pixel = timage->GetPixelData();
	unsigned short *pal = (unsigned short*)timage->GetPaletteData() + palette;
	int key = timage->GetColorKey();
	const int tpitch = timage->GetPitch();

	while (pos < end)
	{
		int tu = F12ToInt(uStart);
		int tv = F12ToInt(vStart);
#if DEBUG_LEVEL > 1
		ASSERT(tu >= 0 && tu < timage->GetWidth());
		ASSERT(tv >= 0 && tv < timage->GetHeight());
#endif
		int v = pixel[tv * tpitch + tu];
		if(v != key)
		{
			int bg = *pos;
			int fg = pal[v];
			ALPHA_BLEND_S12_D12(*pos, bg, fg, opacity);
		}
		++pos;
		uStart += dux;
		vStart += dvx;
	}
}

void FillHLineTD16S8PKO(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette, int opacity)
{
	unsigned short *pos = (unsigned short*)pStart + x;
	unsigned short *end = pos + length;
	CIndexedImage *timage = DYNAMIC_CAST(CIndexedImage*, texture);
	unsigned char *pixel = timage->GetPixelData();
	unsigned short *pal = (unsigned short*)timage->GetPaletteData() + palette;
	int key = timage->GetColorKey();
	const int tpitch = timage->GetPitch();

	while (pos < end)
	{
		int tu = F12ToInt(uStart);
		int tv = F12ToInt(vStart);
#if DEBUG_LEVEL > 1
		ASSERT(tu >= 0 && tu < timage->GetWidth());
		ASSERT(tv >= 0 && tv < timage->GetHeight());
#endif
		int v = pixel[tv * tpitch + tu];
		if(v != key)
		{
			int bg = *pos;
			int fg = pal[v];
			ALPHA_BLEND_S16_D16(*pos, bg, fg, opacity);
		}
		++pos;
		uStart += dux;
		vStart += dvx;
	}
}

void FillHLineTD24S8PKO(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette, int opacity)
{
	unsigned int *pos = (unsigned int*)pStart + x;
	unsigned int *end = pos + length;
	CIndexedImage *timage = DYNAMIC_CAST(CIndexedImage*, texture);
	unsigned char *pixel = timage->GetPixelData();
	unsigned int *pal = (unsigned int*)timage->GetPaletteData() + palette;
	int key = timage->GetColorKey();
	const int tpitch = timage->GetPitch();

	while (pos < end)
	{
		int tu = F12ToInt(uStart);
		int tv = F12ToInt(vStart);
#if DEBUG_LEVEL > 1
		ASSERT(tu >= 0 && tu < timage->GetWidth());
		ASSERT(tv >= 0 && tv < timage->GetHeight());
#endif
		int v = pixel[tv * tpitch + tu];
		if(v != key)
		{
			int bg = *pos;
			int fg = pal[v];
			ALPHA_BLEND_S24_D24(*pos, bg, fg, opacity);
		}
		++pos;
		uStart += dux;
		vStart += dvx;
	}
}

void FillHLineTD16S32A(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length)
{
	unsigned short *pos = (unsigned short*)pStart + x;
	unsigned short *end = pos + length;
	CIndexedImage *timage = DYNAMIC_CAST(CIndexedImage*, texture);
	unsigned int *pixel = (unsigned int*)timage->GetPixelData();
	const int tpitch = timage->GetPitch() / 4;

	while (pos < end)
	{
		int tu = F12ToInt(uStart);
		int tv = F12ToInt(vStart);
#if DEBUG_LEVEL > 1
		ASSERT(tu >= 0 && tu < timage->GetWidth());
		ASSERT(tv >= 0 && tv < timage->GetHeight());
#endif
		unsigned int fg = pixel[tv * tpitch + tu];
		unsigned int bg = *pos;
		unsigned int a = fg >> 24;
		ALPHA_BLEND_S32_D16(*pos, bg, fg, a);
		++pos;
		uStart += dux;
		vStart += dvx;
	}
}

void FillHLineTD16S32AO(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, int opacity)
{
	unsigned short *pos = (unsigned short*)pStart + x;
	unsigned short *end = pos + length;
	CIndexedImage *timage = DYNAMIC_CAST(CIndexedImage*, texture);
	unsigned int *pixel = (unsigned int*)timage->GetPixelData();
	const int tpitch = timage->GetPitch() / 4;

	while (pos < end)
	{
		int tu = F12ToInt(uStart);
		int tv = F12ToInt(vStart);
#if DEBUG_LEVEL > 1
		ASSERT(tu >= 0 && tu < timage->GetWidth());
		ASSERT(tv >= 0 && tv < timage->GetHeight());
#endif
		unsigned int fg = pixel[tv * tpitch + tu];
		unsigned int bg = *pos;
		unsigned int a = (fg >> 24) * opacity / ALPHA_DEVISOR;
		ALPHA_BLEND_S32_D16(*pos, bg, fg, a);
		++pos;
		uStart += dux;
		vStart += dvx;
	}
}

void CGraphics::DrawSpanT(int y, CGraphics::SpanT *span, CImage *texture, unsigned int palette, CFMath *math)
{
	const int dx = span->xMax - span->xMin;
	if(dx <= 0)
		return;
	F12 uStart = span->uMin;
	F12 dux = math->FastDiv((span->uMax - span->uMin), dx);
	//if dux < 0, using span->uMin as the start of u may result in sampling out of texture
	//bound since u is exclusive on right edge. For example, if span->uMin = u_right, the
	//texture coordinate of first pixel will be u_right, which is not acceptable.
	//To correct this problem, decrease uStart by one when dux < 0.
	if(dux < 0)
		--uStart;

	F12 vStart = span->vMin;
	F12 dvx = math->FastDiv((span->vMax - span->vMin), dx);
	//same as uStart
	if(dvx < 0)
		--vStart;

	int xStart = span->xMin;
	int xSkip = 0;
	if(xStart < m_clipRect[0])
	{
		xSkip = m_clipRect[0] - xStart;
		xStart = m_clipRect[0];
	}
	if(xSkip)
	{
		uStart += dux * xSkip;
		vStart += dvx * xSkip;
	}
	int xEnd = span->xMax;
	if(xEnd > m_clipRect[2])
		xEnd = m_clipRect[2];
	unsigned char* p = m_image->GetPixelData() + m_image->GetPitch() * y;
	FillHLineT(uStart, dux, vStart, dvx, texture, p, xStart, xEnd - xStart, palette);
}

void CGraphics::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	int yMin, yMax;
	yMin = yMax = y1;
	if(y2 < yMin)
		yMin = y2;
	else if(y2 > yMax)
		yMax = y2;
	if(y3 < yMin)
		yMin = y3;
	else if(y3 > yMax)
		yMax = y3;

	yMin = ToRawY(yMin);
	yMax = ToRawY(yMax);

	Vertex2D point[3];
	point[0].screen.x = ToRawX(x1);
	point[0].screen.y = ToRawY(y1);
	point[1].screen.x = ToRawX(x2);
	point[1].screen.y = ToRawY(y2);
	point[2].screen.x = ToRawX(x3);
	point[2].screen.y = ToRawY(y3);
	
	CSpanGenerator sg;
	
	if(!sg.Prepare(yMin, yMax, m_clipRect[1], m_clipRect[3]))
	{
		return;
	}
	sg.EmitEdge(point, point + 1);
	sg.EmitEdge(point + 1, point + 2);
	sg.EmitEdge(point + 2, point);

	int i, n = sg.m_yMax - sg.m_yMin;
	for(i = 0; i < n; i++)
	{
		FillRect(sg.m_span[i].xMin, i + sg.m_yMin, sg.m_span[i].xMax - sg.m_span[i].xMin, 1);
	}

	//the right and bottom edge is not filled by above code, so fills these edges
	//to be consistent with MIDP's FillTriangle
	DrawLine(x1, y1, x2, y2);
	DrawLine(x2, y2, x3, y3);
	DrawLine(x1, y1, x3, y3);
}

#if GRAPHIC_OPENGL > 0

CGraphicsGL::CGraphicsGL()
{
	ResetRegions();
}

CGraphicsGL::~CGraphicsGL()
{

}

void CGraphicsGL::DrawRegion(CImage *pSrc, int x_src, int y_src, int width, int height, int transform,
				int x_dest, int y_dest, unsigned int palette)
{
	//x_dest += m_translate[0];
	//y_dest += m_translate[1];
	
	unsigned int f = TransformToFlag(transform);
	
	//I derived these clipping rules by studying at a bunch of diagrams with
	//all kinds of transforms ;)
	if(!(f & ROTATE_90))
	{
		int delta = m_clipRect[0] - x_dest;
		if(delta > 0)
		{
			if(!(f & FLIP_X))
				x_src += delta;
			width -= delta;
			x_dest = m_clipRect[0];
		}
		delta = x_dest + width - m_clipRect[2];
		if(delta > 0)
		{
			if(f & FLIP_X)
				x_src += delta;
			width -= delta;
		}
		delta = m_clipRect[1] - y_dest;
		if(delta > 0)
		{
			if(!(f & FLIP_Y))
				y_src += delta;
			height -= delta;
			y_dest = m_clipRect[1];
		}
		delta = y_dest + height - m_clipRect[3];
		if(delta > 0)
		{
			if(f & FLIP_Y)
				y_src += delta;
			height -= delta;
		}
	}
	else
	{
		int delta = m_clipRect[0] - x_dest;
		if(delta > 0)
		{
			if((f & FLIP_Y))
				y_src += delta;
			height -= delta;
			x_dest = m_clipRect[0];
		}
		delta = x_dest + height - m_clipRect[2];
		if(delta > 0)
		{
			if(!(f & FLIP_Y))
				y_src += delta;
			height -= delta;
		}
		delta = m_clipRect[1] - y_dest;
		if(delta > 0)
		{
			if(!(f & FLIP_X))
				x_src += delta;
			width -= delta;
			y_dest = m_clipRect[1];
		}
		delta = y_dest + width - m_clipRect[3];
		if(delta > 0)
		{
			if(f & FLIP_X)
				x_src += delta;
			width -= delta;
		}
	}
	
	if(width > 0 && height > 0)
		DrawRegion_NoClip_NoTranslat(pSrc, x_src, y_src, width, height, transform, x_dest, y_dest, palette);
}

void CGraphicsGL::FillTriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	CGraphics::FillTriangle(x1,y1,x2,y2,x3,y3);
	return;
/*
	GLTvec2D point[3];	
	point[0].x = GLT_INT_TO_FIXED(x1);
	point[0].y = GLT_INT_TO_FIXED(y1);
	point[1].x = GLT_INT_TO_FIXED(x2);
	point[1].y = GLT_INT_TO_FIXED(y2);
	point[2].x = GLT_INT_TO_FIXED(x3);
	point[2].y = GLT_INT_TO_FIXED(y3);
	*/
	GLTvec3D vertex[3];	
	vertex[0] = GLTvec3D( GLT_FLOAT_TO_FIXED(x1), GLT_FLOAT_TO_FIXED(y1),	m_depth);
	vertex[1] = GLTvec3D( GLT_FLOAT_TO_FIXED(x2), GLT_FLOAT_TO_FIXED(y2),	m_depth);
	vertex[2] = GLTvec3D( GLT_FLOAT_TO_FIXED(x3), GLT_FLOAT_TO_FIXED(y3),	m_depth);
	u16      idx[3];	
	idx[0] = 0;
	idx[1] = 1;
	idx[2] = 2;
	if(m_batch)
		m_depth += 256;
	glDisable( GL_TEXTURE_2D );	
	
	glPushMatrix();	
	//glTranslatex(0, 0, 0);
	//glScalex(GLT_INT_TO_FIXED(1), GLT_INT_TO_FIXED(1), GLT_INT_TO_FIXED(1));
	glEnableClientState(GL_VERTEX_ARRAY);
//glVertexPointer(2, GL_FIXED, 0, point);
	glVertexPointer(3, GL_FIXED, 0, vertex);

	float ca = (float)((m_color >> 24) & 0xFF);
	float cr = (float)((m_color >> 16) & 0xFF);
	float cg = (float)((m_color >> 8 ) & 0xFF);
	float cb = (float)((m_color      ) & 0xFF);
	glColor4x(GLT_FLOAT_TO_FIXED((cr/255.0f)), GLT_FLOAT_TO_FIXED((cg/255.0f)), GLT_FLOAT_TO_FIXED((cb/255.0f)), GLT_FLOAT_TO_FIXED((ca/255.0f)));
	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, idx);
	glColor4x(GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f));
	glPopMatrix();
}

void CGraphicsGL::DrawRegion_NoClip_NoTranslat(CImage *pSrc, int x_src, int y_src, int width, int height, int transform,
													   int x_dest, int y_dest, unsigned int palette)
{
	ASSERT(pSrc != NULL);
	ASSERT(x_src >= 0);
	ASSERT(x_src + width <= pSrc->GetWidth());
	ASSERT(y_src >= 0);
	ASSERT(y_src + height <= pSrc->GetHeight());

	CGLImage *image = DYNAMIC_CAST(CGLImage*, pSrc);
	float imgwidth = (float)image->GetWidth();
	float imgheight = (float)image->GetHeight();	
	
	GLTvertexUV originuv[4];
	originuv[0].tu = GLT_FLOAT_TO_FIXED(((float(x_src)) / imgwidth));				
	originuv[0].tv = GLT_FLOAT_TO_FIXED(((float(imgheight - y_src)) / imgheight));	
	originuv[1].tu = GLT_FLOAT_TO_FIXED(((float(x_src)) / imgwidth));				
	originuv[1].tv = GLT_FLOAT_TO_FIXED(((float(imgheight - (y_src + height))) / imgheight));			
	originuv[2].tu = GLT_FLOAT_TO_FIXED(((float(x_src + width)) / imgwidth));		
	originuv[2].tv = GLT_FLOAT_TO_FIXED(((float(imgheight - (y_src + height))) / imgheight));			
	originuv[3].tu = GLT_FLOAT_TO_FIXED(((float(x_src + width)) / imgwidth));		
	originuv[3].tv = GLT_FLOAT_TO_FIXED(((float(imgheight - y_src)) / imgheight));	

	/*originuv[0].tu = GLT_FLOAT_TO_FIXED(1);
	 originuv[0].tv = GLT_FLOAT_TO_FIXED(1);
	 originuv[1].tu = GLT_FLOAT_TO_FIXED(1);
	 originuv[1].tv = GLT_FLOAT_TO_FIXED(0);
	 originuv[2].tu = GLT_FLOAT_TO_FIXED(0);
	 originuv[2].tv = GLT_FLOAT_TO_FIXED(0);
	 originuv[3].tu = GLT_FLOAT_TO_FIXED(0);
	 originuv[3].tv = GLT_FLOAT_TO_FIXED(1);
	 */
	GLTvertexUV uv[4];
	switch(transform)   
	{
		case TRANS_NONE:
			uv[0].tu = originuv[0].tu;
			uv[0].tv = originuv[0].tv;
			uv[1].tu = originuv[1].tu;
			uv[1].tv = originuv[1].tv;
			uv[2].tu = originuv[2].tu;
			uv[2].tv = originuv[2].tv;
			uv[3].tu = originuv[3].tu;
			uv[3].tv = originuv[3].tv;
			
			break;
		case TRANS_MIRROR_ROT180:
			uv[0].tu = originuv[1].tu;
			uv[0].tv = originuv[1].tv;
			uv[1].tu = originuv[0].tu;
			uv[1].tv = originuv[0].tv;
			uv[2].tu = originuv[3].tu;
			uv[2].tv = originuv[3].tv;
			uv[3].tu = originuv[2].tu;
			uv[3].tv = originuv[2].tv;
			
			break;
		case TRANS_MIRROR:
			uv[0].tu = originuv[3].tu;
			uv[0].tv = originuv[3].tv;
			uv[1].tu = originuv[2].tu;
			uv[1].tv = originuv[2].tv;
			uv[2].tu = originuv[1].tu;
			uv[2].tv = originuv[1].tv;
			uv[3].tu = originuv[0].tu;
			uv[3].tv = originuv[0].tv;
			
			break;
		case TRANS_ROT180:
			uv[0].tu = originuv[2].tu;
			uv[0].tv = originuv[2].tv;
			uv[1].tu = originuv[3].tu;
			uv[1].tv = originuv[3].tv;
			uv[2].tu = originuv[0].tu;
			uv[2].tv = originuv[0].tv;
			uv[3].tu = originuv[1].tu;
			uv[3].tv = originuv[1].tv;

			break;
		case TRANS_MIRROR_ROT270:
			uv[0].tu = originuv[2].tu;
			uv[0].tv = originuv[2].tv;
			uv[1].tu = originuv[1].tu;
			uv[1].tv = originuv[1].tv;
			uv[2].tu = originuv[0].tu;
			uv[2].tv = originuv[0].tv;
			uv[3].tu = originuv[3].tu;
			uv[3].tv = originuv[3].tv;
			
			break;
		case TRANS_ROT90:
			uv[0].tu = originuv[3].tu;
			uv[0].tv = originuv[3].tv;
			uv[1].tu = originuv[0].tu;
			uv[1].tv = originuv[0].tv;
			uv[2].tu = originuv[1].tu;
			uv[2].tv = originuv[1].tv;
			uv[3].tu = originuv[2].tu;
			uv[3].tv = originuv[2].tv;
			
			break;
		case TRANS_ROT270:
			uv[0].tu = originuv[1].tu;
			uv[0].tv = originuv[1].tv;
			uv[1].tu = originuv[2].tu;
			uv[1].tv = originuv[2].tv;
			uv[2].tu = originuv[3].tu;
			uv[2].tv = originuv[3].tv;
			uv[3].tu = originuv[0].tu;
			uv[3].tv = originuv[0].tv;
			
			break;
		case TRANS_MIRROR_ROT90:
			uv[0].tu = originuv[0].tu;
			uv[0].tv = originuv[0].tv;
			uv[1].tu = originuv[3].tu;
			uv[1].tv = originuv[3].tv;
			uv[2].tu = originuv[2].tu;
			uv[2].tv = originuv[2].tv;
			uv[3].tu = originuv[1].tu;
			uv[3].tv = originuv[1].tv;

			break;
		default:
			ASSERT(false);
	}
	
	GLTvec3D vertex[4];	
	//vertex[0] = GLTvec3D( GLT_FLOAT_TO_FIXED(0),  GLT_FLOAT_TO_FIXED(0), 0);
	//vertex[1] = GLTvec3D( GLT_FLOAT_TO_FIXED(0),  GLT_FLOAT_TO_FIXED(1), 0);
	//vertex[2] = GLTvec3D( GLT_FLOAT_TO_FIXED(1),  GLT_FLOAT_TO_FIXED(1), 0);
	//vertex[3] = GLTvec3D( GLT_FLOAT_TO_FIXED(1),  GLT_FLOAT_TO_FIXED(0), 0);	
	vertex[0] = GLTvec3D( GLT_FLOAT_TO_FIXED(x_dest			 ),  GLT_FLOAT_TO_FIXED(y_dest			 ),		m_depth);
	vertex[1] = GLTvec3D( GLT_FLOAT_TO_FIXED(x_dest			 ),  GLT_FLOAT_TO_FIXED((y_dest + height)),		m_depth);
	vertex[2] = GLTvec3D( GLT_FLOAT_TO_FIXED((x_dest + width)),  GLT_FLOAT_TO_FIXED((y_dest + height)),		m_depth);
	vertex[3] = GLTvec3D( GLT_FLOAT_TO_FIXED((x_dest + width)),  GLT_FLOAT_TO_FIXED(y_dest			 ),		m_depth);

	
	
	if(m_batch)
	{	
		m_depth += 256;
		AddRegion(image->GLName(), m_color, vertex, uv);
	}
	else
	{
		u16 idx[6] = {0, 3, 2, 0, 2, 1};

		glEnable( GL_TEXTURE_2D );	
		glEnable( GL_BLEND );
		glBindTexture(GL_TEXTURE_2D, image->GLName());
		glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FIXED, 0, vertex);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);	
		glTexCoordPointer(2, GL_FIXED, 0, uv);
		float ca = (float)((m_color >> 24) & 0xFF);
		float cr = (float)((m_color >> 16) & 0xFF);
		float cg = (float)((m_color >> 8 ) & 0xFF);
		float cb = (float)((m_color      ) & 0xFF);
		glColor4x(GLT_FLOAT_TO_FIXED((cr/255.0f)), GLT_FLOAT_TO_FIXED((cg/255.0f)), GLT_FLOAT_TO_FIXED((cb/255.0f)), GLT_FLOAT_TO_FIXED((ca/255.0f)));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, idx);
		glColor4x(GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f));

	}
}

void CGraphicsGL::DrawLine_NoClip_NoTranslat(int x0, int y0, int x1, int y1) {
	ASSERT(x0 >= m_clipRect[0] && x0 < m_clipRect[2]);
	ASSERT(x1 >= m_clipRect[0] && x1 < m_clipRect[2]);
	ASSERT(y0 >= m_clipRect[1] && y0 < m_clipRect[3]);
	ASSERT(y1 >= m_clipRect[1] && y1 < m_clipRect[3]);
	
	GLTvec3D line[2];
	line[0] = GLTvec3D(GLT_FLOAT_TO_FIXED(x0) + GLT_FLOAT_TO_FIXED(0.5), GLT_FLOAT_TO_FIXED(y0) + GLT_FLOAT_TO_FIXED(0.5), m_depth);
	line[1] = GLTvec3D(GLT_FLOAT_TO_FIXED(x1) + GLT_FLOAT_TO_FIXED(0.5), GLT_FLOAT_TO_FIXED(y1) + GLT_FLOAT_TO_FIXED(0.5), m_depth);

	if(m_batch)
		m_depth += 256;
	
	glDisable( GL_TEXTURE_2D );	
	
	glPushMatrix();
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FIXED, 0, line);

	float ca = (float)((m_color >> 24) & 0xFF);
	float cr = (float)((m_color >> 16) & 0xFF);
	float cg = (float)((m_color >> 8 ) & 0xFF);
	float cb = (float)((m_color      ) & 0xFF);
	glColor4x(GLT_FLOAT_TO_FIXED((cr/255.0f)), GLT_FLOAT_TO_FIXED((cg/255.0f)), GLT_FLOAT_TO_FIXED((cb/255.0f)), GLT_FLOAT_TO_FIXED((ca/255.0f)));
	glDrawArrays(GL_LINES, 0, 2);
	glColor4x(GLT_INT_TO_FIXED(1), GLT_INT_TO_FIXED(1), GLT_INT_TO_FIXED(1), GLT_INT_TO_FIXED(1));

	glPopMatrix();	
}

unsigned int CGraphicsGL::ToNativeColor(unsigned int rgb)
{
	return 0;
}

unsigned int CGraphicsGL::ToRGB24Color(unsigned int color) 
{
	return 0;
}

void CGraphicsGL::SetPixel(int x, int y, unsigned int nativeColor)
{
	return;
}

void CGraphicsGL::FillRect(int x, int y, int width, int height)
{
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
	
	GLTvec3D vertex[4];	
	vertex[0] = GLTvec3D( GLT_FLOAT_TO_FIXED(x),		  GLT_FLOAT_TO_FIXED(y),			m_depth);
	vertex[1] = GLTvec3D( GLT_FLOAT_TO_FIXED(x),		  GLT_FLOAT_TO_FIXED(y + height),	m_depth);
	vertex[2] = GLTvec3D( GLT_FLOAT_TO_FIXED(x + width),  GLT_FLOAT_TO_FIXED(y + height),	m_depth);
	vertex[3] = GLTvec3D( GLT_FLOAT_TO_FIXED(x + width),  GLT_FLOAT_TO_FIXED(y),			m_depth);
	
	if(m_batch)
		m_depth += 256;

	u16 idx[6];	
	idx[0] = 0;
	idx[1] = 3;
	idx[2] = 2;
	idx[3] = 0;
	idx[4] = 2;
	idx[5] = 1;
	
	glPushMatrix();
	//glTranslatex(GLT_INT_TO_FIXED(x), GLT_INT_TO_FIXED(y), 0);
	//glScalex(GLT_INT_TO_FIXED(width), GLT_INT_TO_FIXED(height), GLT_INT_TO_FIXED(1));
	glTranslatex(0, 0, 0);
	glScalex(GLT_INT_TO_FIXED(1), GLT_INT_TO_FIXED(1), GLT_INT_TO_FIXED(1));

	
	float ca = (float)((m_color >> 24) & 0xFF);
	float cr = (float)((m_color >> 16) & 0xFF);
	float cg = (float)((m_color >> 8 ) & 0xFF);
	float cb = (float)((m_color      ) & 0xFF);

	glDisable( GL_TEXTURE_2D );	

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FIXED, 0, vertex);
	glColor4x(GLT_FLOAT_TO_FIXED((cr/255.0f)), GLT_FLOAT_TO_FIXED((cg/255.0f)), GLT_FLOAT_TO_FIXED((cb/255.0f)), GLT_FLOAT_TO_FIXED((ca/255.0f)));
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, idx);

	glColor4x(GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f));
	
	glPopMatrix();
}

void CGraphicsGL::DrawArc(int x, int y, int width, int height)
{
	return;
}

void CGraphicsGL::DrawCircle(int x, int y, int r)
{
	return;
}

void CGraphicsGL::FillCircle(int x, int y, int r)
{
	return;
}

void CGraphicsGL::FillHLineT(F12 uStart, F12 dux, F12 vStart, F12 dvx, CImage *texture, void *pStart, int x, int length, unsigned int palette)
{
	return;
}

void CGraphicsGL::ResetRegions()
{
	m_texCount = 0;

	for(int i = 0; i < REGION_SIZE; i++)
	{
		m_index[i * 6 + 0] = i* 4 + 0;
		m_index[i * 6 + 1] = i* 4 + 3;
		m_index[i * 6 + 2] = i* 4 + 2;
		m_index[i * 6 + 3] = i* 4 + 0;
		m_index[i * 6 + 4] = i* 4 + 2;
		m_index[i * 6 + 5] = i* 4 + 1;
	}
}

void CGraphicsGL::AddRegion(int tex, int col, GLTvec3D* pVertex, GLTvertexUV* pUV)
{
	ASSERT(m_texCount < TEX_SIZE);

	int i = 0;

	for(i = 0; i < m_texCount; i++)
	{
		if(m_texRegions[i]._tex == tex && m_texRegions[i]._col == col)
			break;
	}

	if(m_texCount <= i)
	{
		m_texRegions[i]._tex = tex;
		m_texRegions[i]._col = col;
		m_texRegions[i]._region_count = 0;
		m_texCount++;
	}
	
	int index = m_texRegions[i]._region_count;

	ASSERT(index < REGION_SIZE);

	m_texRegions[i]._vertex[index * 4 + 0] = pVertex[0];
	m_texRegions[i]._vertex[index * 4 + 1] = pVertex[1];
	m_texRegions[i]._vertex[index * 4 + 2] = pVertex[2];
	m_texRegions[i]._vertex[index * 4 + 3] = pVertex[3];

	m_texRegions[i]._uv[index * 4 + 0] = pUV[0];
	m_texRegions[i]._uv[index * 4 + 1] = pUV[1];
	m_texRegions[i]._uv[index * 4 + 2] = pUV[2];
	m_texRegions[i]._uv[index * 4 + 3] = pUV[3];

	++m_texRegions[i]._region_count;
}

void CGraphicsGL::RenderRegions()
{
	for(int i = 0; i < m_texCount; i++)
	{
		glEnable( GL_TEXTURE_2D );	
		glBindTexture(GL_TEXTURE_2D, m_texRegions[i]._tex);
		glTexEnvx(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FIXED, 0, m_texRegions[i]._vertex);
	
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);	
		glTexCoordPointer(2, GL_FIXED, 0, m_texRegions[i]._uv);
		
		float ca = (float)((m_texRegions[i]._col >> 24) & 0xFF);
		float cr = (float)((m_texRegions[i]._col >> 16) & 0xFF);
		float cg = (float)((m_texRegions[i]._col >> 8 ) & 0xFF);
		float cb = (float)((m_texRegions[i]._col      ) & 0xFF);
		
		glColor4x(GLT_FLOAT_TO_FIXED((cr/255.0f)), GLT_FLOAT_TO_FIXED((cg/255.0f)), GLT_FLOAT_TO_FIXED((cb/255.0f)), GLT_FLOAT_TO_FIXED((ca/255.0f)));	
	
		glDrawElements(GL_TRIANGLES, 6 * m_texRegions[i]._region_count, GL_UNSIGNED_SHORT, m_index);
	
		glColor4x(GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f), GLT_FLOAT_TO_FIXED(1.0f));
	}
}

extern int g_screenAngle;

void CGraphicsGL::BeginDraw2D(int width, int height)
{	
	m_depth	   = 0;
	m_texCount = 0;
	
	if(m_batch)
	{
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		glDisable(GL_DEPTH_TEST);
	}

	glClear(GL_DEPTH_BUFFER_BIT);

	SetARGBColor(0xFFFFFFFF);
	
	glDisable(GL_CULL_FACE);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_ALPHA);
	glEnable( GL_TEXTURE_2D );	
	glEnable(GL_BLEND);
	glClear(GL_DEPTH_BUFFER_BIT);

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();	

	glViewport(0, 0, width, height);
	
	glOrthox(GLT_INT_TO_FIXED(0),		GLT_INT_TO_FIXED(width),
			 GLT_INT_TO_FIXED(height),	GLT_INT_TO_FIXED(0), 
			 GLT_INT_TO_FIXED(-100),		GLT_INT_TO_FIXED(100));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	glColor4x(GLT_INT_TO_FIXED(1), GLT_INT_TO_FIXED(1), GLT_INT_TO_FIXED(1), GLT_INT_TO_FIXED(1));
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLoadIdentity();	

	glTranslatex(GLT_INT_TO_FIXED(width), GLT_INT_TO_FIXED(0), GLT_INT_TO_FIXED(0));
	glRotatex(GLT_FLOAT_TO_FIXED(90.0f), GLT_FLOAT_TO_FIXED(0.0f), GLT_FLOAT_TO_FIXED(0.0f), GLT_FLOAT_TO_FIXED(1.0f));

#if PLATFORM == PLATFORM_WIN32
	glTranslatex(GLT_INT_TO_FIXED(height / 2), GLT_INT_TO_FIXED(width / 2), GLT_INT_TO_FIXED(0));
	glRotatex(GLT_FLOAT_TO_FIXED(g_screenAngle), GLT_FLOAT_TO_FIXED(0.0f), GLT_FLOAT_TO_FIXED(0.0f), GLT_FLOAT_TO_FIXED(1.0f));
	glTranslatex(GLT_INT_TO_FIXED(-height / 2), GLT_INT_TO_FIXED(-width / 2), GLT_INT_TO_FIXED(0));
#endif
}

void CGraphicsGL::EndDraw2D()
{
	//if(m_cache)
	RenderRegions();

	//glFlush();	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
}

void CGraphicsGL::SetClip(int x, int y, int width, int height)
{
	m_clipRect[0] = x + m_translate[0];
	m_clipRect[1] = y + m_translate[1];
	m_clipRect[2] = m_clipRect[0] + width;
	m_clipRect[3] = m_clipRect[1] + height;
	
	//if(m_clipRect[0] < 0)
	//	m_clipRect[0] = 0;
	//if(m_clipRect[1] < 0)
	//	m_clipRect[1] = 0;
}

void CGraphicsGL::ResetClip()
{
	m_clipRect[0] = m_clipRect[1] = 0;
	m_clipRect[2] = 480;
	m_clipRect[3] = 320;
}

#endif