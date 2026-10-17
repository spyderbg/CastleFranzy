/*
 *  IGPUtil.mm
 *  IGP
 *
 *  Created by Fernando Mazzon on 09/05/08.
 *  Copyright 2008 Gameloft. All rights reserved.
 *
 */

#import "IGPUtil.h"
#import "IGPTexture.h"
#import "IGP.h"
#include <stdarg.h>


void trace(int type, const char* format, ... ) 
{
	int t = 1;
	char buf[400] = {0};
	if ((DEBUG_FLAGS & type) != 0)
	{
		while (type != 1)
		{
			t++;
			type >>=1;
		}
		va_list args;
		fprintf( stdout, "[Debug %d]", t);
		va_start( args, format );
		vfprintf( stdout, format, args );
		vsprintf(buf, format, args);
		va_end( args );
	
	//	CGPoint p = [IGP::_debugBox contentOffset];
	//	IGP::_debugBox.text = [IGP::_debugBox.text stringByAppendingString:[[NSString alloc] initWithUTF8String:buf]];
	//	[IGP::_debugBox setContentOffset:p animated:NO];
	//	[IGP::_debugBox scrollRangeToVisible:NSMakeRange([IGP::_debugBox.text length], 0)];

	}
}
			
float lineal(float t ,float endT, float startY ,float endY)
{
	if (t < 0) t = 0;
	if (t > endT) t = endT;
	float dif = endY - startY;
	return startY + (dif * t) / endT;
}


float parabolic(float t, float endT, float startY, float endY)
{
	if (t < 0) t = 0;
	float dif = endY - startY;
	if (t >= endT)
	{
		return endY;
	}
	return startY + dif * t * (t - 2 * endT) / (-endT * endT);
}
float blendDif(float now, float target)
{
	return MIN(MAX((now-target)/3, -100), 100) ;
}
bool pointInTriangle(float x, float y, float x1, float y1, float x2, float y2, float x3, float y3)
{
	float total, t1, t2, t3;
	total = Triangle_Area(x1,y1,x2,y2,x3,y3);
	t1 = Triangle_Area(x, y, x2, y2, x3, y3);
	t2 = Triangle_Area(x, y, x1, y1, x2, y2);
	t3 = Triangle_Area(x, y, x1, y1, x3, y3);	
	
	return ((t1 + t2 + t3) * 10 - total * 10) < 1; 
}

/**
 * Returns the area of a triangle
 */
float Triangle_Area(float x1, float y1, float x2, float y2, float x3, float y3)
{
	float a = (x1-x3);
	float b = (y1-y3);
	float c = (x2-x3);
	float d = (y2-y3);
	return (ABS((a*d)-(b*c)))/2;
}

/* Get path to a file in resources or documents folder*/
void GetFileName (const char* filename, bool bWrite, char*p)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	sprintf(p, "%s", filename);
	NSString *path = [NSString stringWithUTF8String:p];
	
	if(bWrite)
	{
		NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
		NSString *documentsDirectory = [paths objectAtIndex:0];
		path = [documentsDirectory stringByAppendingPathComponent:path];
	}
	else
	{
		if(![path isAbsolutePath])
		{
			path = [[NSBundle mainBundle] pathForResource:path ofType:nil];
		}
	}
	if (path != nil) sprintf(p, "%s", [path UTF8String]);
	
	[pool release];
}

UIImage* imageFromBuffer(char* buff, int len)
{
	NSData* data = [[NSData alloc] initWithBytes:buff length:len];
	UIImage* tex = [[UIImage alloc] initWithData: data];
	[data release];
	return tex;
}

void bufferFromFile(FILE* f, char* buff, int* len)
{
	fread(len, sizeof(int), 1, f);
	fread(buff, sizeof(char), *len, f);
}

void skipTexture(FILE* f)
{
	int len;
	char type;
	fread(&type, sizeof(char), 1, f);
	if (!type)
	{
		fread(&len, sizeof(int), 1, f);
		fseek(f, len, SEEK_CUR);	
	}
	else
	{
		fseek(f, 4, SEEK_CUR);
	}	
}

UIImage* loadImage(FILE* f)
{
	int len;
	char type;
	UIImage* tex;
	NSData*  data;
	fread(&type, sizeof(char), 1, f);
	if (!type)
	{
		bufferFromFile(f, IGP::s_buff, &len);
		data = [[NSData alloc] initWithBytes:IGP::s_buff length:len];
		tex = [[UIImage alloc] initWithData:data];
		[data release];
	}
	else
	{
		int currentPositionInFile;
		int positionInFile;
		
		fread(&positionInFile, sizeof(int), 1, f);
		currentPositionInFile = ftell(f);
		fseek(f, positionInFile + 1, SEEK_SET);
		
		bufferFromFile(f, IGP::s_buff, &len);
		data = [[NSData alloc] initWithBytes:IGP::s_buff length:len];
		tex = [[UIImage alloc] initWithData:data];
		[data release];
		
		fseek(f, currentPositionInFile, SEEK_SET);
	}
	[tex autorelease];
	return tex;
}

IGPTexture* loadTexture(FILE* f, char* buff, int landscape, int desiredFormat)
{
	int len;
	char type;
	IGPTexture* tex;
	fread(&type, sizeof(char), 1, f);
	if (!type)
	{
		bufferFromFile(f, buff, &len);
		tex = new IGPTexture(buff, len, landscape == 1, desiredFormat);
	}
	else
	{
		int currentPositionInFile;
		int positionInFile;
		
		fread(&positionInFile, sizeof(int), 1, f);
		currentPositionInFile = ftell(f);
		fseek(f, positionInFile + 1, SEEK_SET);
		
		bufferFromFile(f, buff, &len);
		tex = new IGPTexture(buff, len, landscape == 1, desiredFormat);
		
		fseek(f, currentPositionInFile, SEEK_SET);
	}
	return tex;
}

FILE* openFile(char* name)
{
	FILE* f;
	char path[256];
	GetFileName(name, false, path);
	f = fopen(path, "rb");
	if (f) return f;
	GetFileName(name, true, path);
	f = fopen(path, "rb");
	return f;

}

void getSaveFileName(const char* name, char* p)
{
	char savePath[255];
	GetFileName(name, true, savePath);
	
	sprintf(p, "%s", savePath);
}

bool MD5Matches(char* file, char* comp)
{
	unsigned char MD5[16];
	char	md5[32];
	FILE*	f;
	char*	buff;
	long	len;
	int		i;
	
	f = openFile(file);
	if (!f) return false;
	len = ftell(f);
	fseek( f, 0L, SEEK_END);
	len = ftell(f) - len;
	fseek( f, 0L, SEEK_SET);
	buff = IGP::s_igpInstance->s_buff;
	fread(buff, sizeof(char), len, f);
	
	
	fclose(f);
	
	CC_MD5(buff, len, MD5);
	sprintf(md5, "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X", MD5[0], MD5[1], MD5[2], MD5[3], MD5[4], MD5[5], MD5[6], 
		MD5[7], MD5[8], MD5[9], MD5[10], MD5[11], MD5[12], MD5[13], MD5[14], MD5[15]);

	i = 0;
	//SAFE_DELETE_ARRAY(buff);
	while(i < 32)
	{
		if (md5[i] != comp[i])
		{
			return false;
		}
		i++;
	}
	return true;
}

bool fileExists(char* name)
{
	
	FILE* f = openFile(name);
	if (f)
	{
		fclose(f);
		return true;
	}
	else
	{
		return false;
	}
	
}
