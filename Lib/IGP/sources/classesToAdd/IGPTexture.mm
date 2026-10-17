/*
 *  texture.mm
 *  IGP
 *
 *  Created by Fernando Mazzon on 08/05/08.
 *  Copyright 2008 Gameloft. All rights reserved.
 *
 */

#import <UIKit/UIImage.h>
#import "IGPTexture.h"
#import "IGPUtil.h"
#import "IGPDefines.h"
#import "IGP.h"

int IGPTexture::totalUsedMemory = 0;

IGPTexture::IGPTexture(char* buff, int len, bool landscape, int desiredFormat)
{
	int internalFormat;
	int format;
	int type;

	_landscape = landscape;
	p1 = new float[4];
	p2 = new float[4];
	p3 = new float[4];
	p4 = new float[4];
	
	NSData* data = [[NSData alloc]initWithBytes:buff length:len];
	UIImage* img = [UIImage imageWithData:data];
	[data release];
	int canvasSize = K_CANVAS_SIZE;
	
	//SAFE_DELETE(spriteImage);
	spriteImage = img.CGImage;
	
	width  = img.size.width;
	height = img.size.height;
	while (canvasSize < width or canvasSize < height)
	{
		canvasSize = canvasSize * 2;
	}
	
	if(spriteImage) {
		spriteData = (GLubyte *) malloc(canvasSize * canvasSize * 4);
		spriteContext = CGBitmapContextCreate(spriteData, canvasSize, canvasSize, 8, canvasSize * 4, CGImageGetColorSpace(spriteImage), kCGImageAlphaPremultipliedLast);
		CGContextDrawImage(spriteContext, CGRectMake(0, 0, canvasSize, canvasSize), spriteImage);
		CGContextRelease(spriteContext);
		glGenTextures(1, &spriteTexture);

		glBindTexture(GL_TEXTURE_2D, spriteTexture);
		
		usedMemory = ConvertToFormat(spriteData, canvasSize, desiredFormat, &internalFormat, &format, &type);
		totalUsedMemory += usedMemory;
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, canvasSize, canvasSize, 0, format, type, spriteData);

		
		SAFE_DELETE_ARRAY(spriteData);
	}
	trace(DEBUG_TEXTURE, "Loaded tex:%d\n", spriteTexture);
	trace(DEBUG_TEXTURE, "Total tex memory:%d\n", totalUsedMemory/1024);

}

IGPTexture::~IGPTexture()
{

	glDeleteTextures(1, &spriteTexture);
	SAFE_DELETE_ARRAY(p1);
	SAFE_DELETE_ARRAY(p2);
	SAFE_DELETE_ARRAY(p3);
	SAFE_DELETE_ARRAY(p4);
	//SAFE_DELETE(spriteImage);
	
	totalUsedMemory -= usedMemory;
	trace(DEBUG_TEXTURE, "Released tex:%d\n", spriteTexture);
	trace(DEBUG_TEXTURE, "Total tex memory:%d\n", totalUsedMemory/1024);

}

void IGPTexture::paint(float x, float y, float z, float scale, float angle, float alpha, float light, bool useShadow)
{
	paint(x, y, z, scale, angle, alpha, light, useShadow, 0);
}

void IGPTexture::paint(float x, float y, float z, float scale, float angle, float alpha, float light, bool useShadow, float t)
{
	
	GLfloat _width2;  
	GLfloat _height2; 

	if (!_landscape)
	{
		_width2  = width * (K_IMAGE_HEIGHT / K_IMAGE_WIDTH);
		_height2 = height * (K_IMAGE_WIDTH / K_IMAGE_HEIGHT);
	}
	else
	{
		_width2  = width * (K_IMAGE_WIDTH / K_IMAGE_HEIGHT);
		_height2 = height * (K_IMAGE_HEIGHT / K_IMAGE_WIDTH);
	}
	GLfloat	_width  = lineal(t, 1, width, _width2);
	GLfloat _height = lineal(t, 1, height, _height2);
	GLfloat _dy		= (height - _height) / 2;

	GLfloat spriteVertices[] = {
		-_width/2, -_height/2 + _dy, 0, 1,
		_width/2,  -_height/2 + _dy, 0, 1, 
		-_width/2, _height/2 + _dy,  0, 1,
		_width/2,  _height/2 + _dy, 0, 1,
	};
	
	GLfloat	mirror[] = {
		-_width / 2,		(_height) + _dy, 0, 1,
		_width / 2 ,		(_height) + _dy, 0, 1,
		-_width / 2,		(_height / 2) - 2 + _dy, 0, 1,
		_width / 2 ,		(_height / 2) - 2 + _dy, 0, 1

	};
	
	const GLshort spriteTexcoords[] = {
		0, 0,
		1, 0,
		0, 1,
		1, 1,
	};
	GLfloat		color[] = {
		light, light, light, alpha,
		light, light, light, alpha,
		light, light, light, alpha,
		light, light, light, alpha
	};
	
	GLfloat		colorShadow[] = {
		0.0, 0.0, 0.0, 0.0,
		0.0, 0.0, 0.0, 0.0,
		.5, .5, .5, 1,
		.5, .5, .5, 1
	};
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(x, y, -K_FRUSTRUM_Z_NEAR -K_DISTANCE_TO_CAMARA + z);
	glScalef(scale, scale, scale);
	glRotatef(angle, 0, 1, 0);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, spriteTexture);
	glVertexPointer(4, GL_FLOAT, 0, spriteVertices);
	glTexCoordPointer(2, GL_SHORT, 0, spriteTexcoords);
	glColorPointer(4, GL_FLOAT, 0, color);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	if (useShadow)
	{
		glColorPointer(4, GL_FLOAT, 0, colorShadow);
		glVertexPointer(4, GL_FLOAT, 0, mirror);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	
	calcCollisionBox();
	glPopMatrix();


}
void IGPTexture::calcCollisionBox()
{
	int w, h, dy;
	if (_landscape)
	{
		w = K_IMAGE_HEIGHT;
		h = K_IMAGE_WIDTH;
	}
	else
	{
		w = K_IMAGE_WIDTH;
		h = K_IMAGE_HEIGHT;
	}
	dy = (width - h)/2;
	
	GLfloat spriteVertices2[]={
		-w/2, -h/2 + dy, 0, 1,
		w/2,  -h/2 + dy, 0, 1, 
		-w/2, h/2 + dy, 0, 1,
		w/2,  h/2 + dy, 0, 1,
	};
	float model[16];
	float proj[16];
	int    view[4];
	float zz;
	
	glGetFloatv(GL_MODELVIEW_MATRIX, model) ;
	glGetFloatv(GL_PROJECTION_MATRIX, proj) ;
	glGetIntegerv(GL_VIEWPORT, view) ;
	gluProject(spriteVertices2[0], spriteVertices2[1], spriteVertices2[2], model, proj, view, &p1[0], &p1[1], &zz);
	gluProject(spriteVertices2[4], spriteVertices2[5], spriteVertices2[6], model, proj, view, &p2[0], &p2[1], &zz);
	gluProject(spriteVertices2[8], spriteVertices2[9], spriteVertices2[10], model, proj, view, &p3[0], &p3[1], &zz);
	gluProject(spriteVertices2[12], spriteVertices2[13], spriteVertices2[14], model, proj, view, &p4[0], &p4[1], &zz);
	
	p1[1]= 480 - p1[1];
	p2[1]= 480 - p2[1];
	p3[1]= 480 - p3[1];
	p4[1]= 480 - p4[1];
}
/* 
 * Transform a point (column vector) by a 4x4 matrix.  I.e.  out = m * in 
 * Input:  m - the 4x4 matrix 
 *         in - the 4x1 vector 
 * Output: out - the resulting 4x1 vector. 
 */ 

void IGPTexture::transform_point(float out[4], const float m[16], const float in[4]) 
{ 
#define M(row,col) m[col*4+row] 
	out[0] = M(0,0) * in[0] + M(0,1) * in[1] + M(0,2) * in[2] + M(0,3) * in[3]; 
	out[1] = M(1,0) * in[0] + M(1,1) * in[1] + M(1,2) * in[2] + M(1,3) * in[3]; 
	out[2] = M(2,0) * in[0] + M(2,1) * in[1] + M(2,2) * in[2] + M(2,3) * in[3]; 
	out[3] = M(3,0) * in[0] + M(3,1) * in[1] + M(3,2) * in[2] + M(3,3) * in[3]; 
#undef M 
}

bool IGPTexture::pointInRect(float x, float y)
{
	if (IGP::s_igpInstance->m_state == IGP_STATE_END || this == NULL)
		return false;
	return (pointInTriangle(x, y, p1[0],  p1[1], p2[0],  p2[1], p3[0], p3[1]) || pointInTriangle(x, y, p2[0],  p2[1], p3[0],  p3[1], p4[0], p4[1]));
}

bool IGPTexture::gluProject(float objx, float objy, float objz,
			const float model[16], const float proj[16],
			const int viewport[4],
			float * winx, float * winy, float * winz)
{
	float in[4], out[4];
 
	in[0] = objx;
	in[1] = objy;
	in[2] = objz;
	in[3] = 1.0;
	transform_point(out, model, in);
	transform_point(in, proj, out);
    if (in[3] == 0.0)
		return false;
	in[0] /= in[3];
	in[1] /= in[3];
	in[2] /= in[3];
	*winx = viewport[0] + (1 + in[0]) * viewport[2] / 2;
	*winy = viewport[1] + (1 + in[1]) * viewport[3] / 2;
	*winz = (1 + in[2]) / 2;
	return true;
}

int IGPTexture::ConvertToFormat(GLubyte * buff, int canvasSize, int desiredFormat, int* internalFormat, int* format, int* type)
{
	switch(desiredFormat)
	{
		case K_FORMAT_RGBA8888:
		{
			//do nothing;
			*type = GL_UNSIGNED_BYTE;
			*internalFormat = GL_RGBA;
			*format = GL_RGBA;
			return 4 * canvasSize * canvasSize;
			break;
		}
		case K_FORMAT_RGB888:
		{
			RGBA8888toRGB888(buff, canvasSize);
			*type = GL_UNSIGNED_BYTE;
			*internalFormat = GL_RGB;
			*format = GL_RGB;
			return 3 * canvasSize * canvasSize;
			break;
		}
		case K_FORMAT_RGBA5551:
		{
			RGBA8888toRGBA5551(buff, canvasSize);
			*type = GL_UNSIGNED_SHORT_5_5_5_1;
			*internalFormat = GL_RGBA;
			*format = GL_RGBA;
			return 2 * canvasSize * canvasSize;
			break;
		}
		default:
		{
			
			return 0;
			break;
		}
	}
}
void IGPTexture::RGBA8888toRGBA4444(GLubyte * buff, int canvasSize)
{
	
	GLubyte r, g, b, a;
	short* d;
	for (int i = 0; i < canvasSize * canvasSize; i++)
	{
		
		r =  buff[i * 4] >> 4;
		g =  buff[i * 4 + 1] >> 4;
		b =  buff[i * 4 + 2] >> 4;
		a =  buff[i * 4 + 3] >> 4;
		d =  (short*) (&buff[i * 2]);
		*d = (r << 12) | (g << 8) | (b << 4) | (a << 0);
		
	}
}
void IGPTexture::RGBA8888toRGBA5551(GLubyte * buff, int canvasSize)
{
	
	GLubyte r, g, b, a;
	short* d;
	for (int i = 0; i < canvasSize * canvasSize; i++)
	{
		
		r =  buff[i * 4] >> 3;
		g =  buff[i * 4 + 1] >> 3;
		b =  buff[i * 4 + 2] >> 3;
		a =  buff[i * 4 + 3] != 0?1:0;
		d =  (short*) (&buff[i * 2]);
		*d = (r << 11) | (g << 6) | (b << 1) | (a << 0);
		
	}
}
void IGPTexture::RGBA8888toRGB888(GLubyte * buff, int canvasSize)
{
	
	GLubyte r, g, b;
	GLubyte* d;
	for (int i = 0; i < canvasSize * canvasSize; i++)
	{
		r =  buff[i * 4];
		g =  buff[i * 4 + 1];
		b =  buff[i * 4 + 2];
		d =  (GLubyte*) (&buff[i * 3]);
		d[0] = r;
		d[1] = g;
		d[2] = b;
	}
}

bool IGPTexture::isLandScape()
{
	return _landscape;
}
