/*
 *  texture.h
 *  IGP
 *
 *  Created by Fernando Mazzon on 08/05/08.
 *  Copyright 2008 Gameloft. All rights reserved.
 *
 */
#ifndef __IGPTEXTURE_H__
#define __IGPTEXTURE_H__

#import <OpenGLES/ES1/gl.h>
#import <QuartzCore/QuartzCore.h>
#import "IGPTexture.h"

class IGPTexture
{
	public:
	IGPTexture(char* buff, int len, bool landscape, int desiredFormat);
	~IGPTexture();
	bool isLandScape();
	void paint(float x, float y, float z, float scale, float angle, float alpha, float light, bool useShadow, float t);
	void paint(float x, float y, float z, float scale, float angle, float alpha, float light, bool useShadow);
	bool pointInRect(float x, float y);
	private:
	
	int  ConvertToFormat(GLubyte* buff, int canvasSize, int desiredFormat, int* internalFormat, int* format, int* type);
	void RGBA8888toRGB888(GLubyte* buff, int canvasSize);
	void RGBA8888toRGBA4444(GLubyte * buff, int canvasSize);
	void RGBA8888toRGBA5551(GLubyte * buff, int canvasSize);
	
	void calcCollisionBox();
	void transform_point(float out[4], const float m[16], const float in[4]);
	bool gluProject(float objx, float objy, float objz, const float model[16], const float proj[16], const int viewport[4], float * winx, float * winy, float * winz);
	float width, height;
	bool _landscape;
	CGImageRef spriteImage;
	CGContextRef spriteContext;
	GLuint      spriteTexture;
	GLubyte* spriteData;
	float* p1;
	float* p2;
	float* p3;
	float* p4;
	int usedMemory;
	static int totalUsedMemory;
};

#endif