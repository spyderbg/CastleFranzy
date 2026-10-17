/*
 *  IGPUtil.h
 *  IGP
 *
 *  Created by Fernando Mazzon on 09/05/08.
 *  Copyright 2008 Gameloft. All rights reserved.
 *
 */

#ifndef __IGPUTIL_H__
#define __IGPUTIL_H__

#import "IGPTexture.h"
#import <CommonCrypto/CommonDigest.h>

//namespace Util;
float lineal(float t ,float endT, float startY ,float endY);
float parabolic(float t, float endT, float startY, float endY);
bool pointInTriangle(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3);
float Triangle_Area(float x1, float y1, float x2, float y2, float x3, float y3);
float blendDif(float now, float target);
//binary file access
void GetFileName (const char* filename, bool bWrite, char*p);
void bufferFromFile(FILE* f, char* buff, int* len);
FILE* openFile(char* name);
void skipTexture(FILE* f);
UIImage* loadImage(FILE* f);
UIImage* imageFromBuffer(char* buff, int len);
IGPTexture* loadTexture(FILE* f, char* buff, int landscape, int desiredFormat);
bool MD5Matches(char* file, char* comp);
void getSaveFileName(const char* name, char* p);
bool fileExists(char* name);
void trace(int type, const char* format, ... );

#endif
