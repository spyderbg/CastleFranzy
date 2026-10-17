/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _DRAW_LAYER_MANAGER_H_
#define _DRAW_LAYER_MANAGER_H_

class DrawLayerManager
{
public:
	enum LayerId
	{
		DL_DEFAULT = 0
		, DL_INDETERMINATE = INT_MIN
	};

private:
	static DrawLayerManager *s_pDrawLayerManager;

	DrawLayerManager(){}
	~DrawLayerManager(){}
public:
	static DrawLayerManager *GetInst();
	static void Release();
};

inline
DrawLayerManager *DrawLayerManager::GetInst()
{
	return s_pDrawLayerManager ? s_pDrawLayerManager : ( s_pDrawLayerManager = new DrawLayerManager() );
}

inline
void DrawLayerManager::Release()
{
	SAFE_DELETE( s_pDrawLayerManager );
}

#endif // _DRAW_LAYER_MANAGER_H_


