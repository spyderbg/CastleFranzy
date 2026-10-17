/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef __IC_RENDER_STATE_MANAGER__H
#define __IC_RENDER_STATE_MANAGER__H

#include "FUtils/FUDaeEnum.h"
#include "Common/StandardMtl.h"

class ICWidget;
class DlgTemplateBuilder;
class ICMaterial;
class ICRenderState;
class ICUIParameter;
typedef fm::map<uint32,ICRenderState *> RenderStateMap;

class ICRenderStateManager
{
public:
	class RenderStateAccessor : public PBAccessor
	{
		void Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t );
	};

	class ICRenderStateManagerPLCB : public PostLoadCallback
	{
	private:
		ICMaterial *m_pOwner;

	public:
		ICRenderStateManagerPLCB( ICMaterial *pOwner )
			: m_pOwner( pOwner )
		{}

		void proc( ILoad *pLoadingInterface );
	};

private:
	ICMaterial *m_pParent;
	RenderStateMap m_renderStates;
	ParamBlockDesc2 *m_pRenderStatesParamBlockDesc;
	DLGTEMPLATE *m_pDialogTemplate;

	static RenderStateAccessor s_renderStatesAccessor;

private:
	// helpers
	ICUIParameter *InitRenderState_EnumParameter( ICRenderState *pRenderState, FSStandardMaterial::PID pid
		, int nameIds, int defaultValue
		, int UNUSED( labelIdc ), int UNUSED( comboBoxIdc )
		, const char *( pConversionFunction )( int )
		, const FUDaeCommon::EnumMapping enumMap[], size_t mappingCount );

	ICUIParameter *InitRenderState_FloatParameter( ICRenderState *pRenderState, FSStandardMaterial::PID pid
		, int nameIds, float defaultValue );

	ICUIParameter *InitRenderState_IntParameter( ICRenderState *pRenderState, FSStandardMaterial::PID pid
		, int nameIds, int defaultValue
		, int _min, int _max, int scale );

	ICUIParameter *ICRenderStateManager::InitRenderState_ColorParameter( ICRenderState *pRenderState, FSStandardMaterial::PID pid
		, int nameIds, const Point4 &defaultValue );

	void InitRenderState_Single( FUDaePassState::State state, FSStandardMaterial::PID pid
		, int nameIds, int defaultValue
		, int labelIdc, int comboBoxIdc
		, const char *( pConversionFunction )( int )
		, const FUDaeCommon::EnumMapping enumMap[], size_t mappingCount );
	
	void BuildDialogTemplateItem( DlgTemplateBuilder &tb, WORD& dialogCount, WORD& lineCount, ICWidget *pWidget );
	void ConstructDialogTemplate();
	void ConstructParamBlockDesc( ClassDesc2 &classDesc );

public:
	ICRenderStateManager( ICMaterial *pParent )
		: m_pParent( pParent )
		, m_pRenderStatesParamBlockDesc( NULL )
		, m_pDialogTemplate( NULL )
	{}

	~ICRenderStateManager();

	void InitUI( TimeValue t, HWND hWnd );
	void BuildDialogTemplateItems( DlgTemplateBuilder &tb, WORD& dialogCount, WORD& lineCount, ICRenderState *&pRenderState );
	void Init( ClassDesc2 &classDesc );
	void Reset( IParamBlock2 *pParamBlock );
	void UpdateUI( HWND hWnd, TimeValue t );
	void GetValidity(  IParamBlock2 *pParamBlock, TimeValue t, Interval &valid );
	BOOL DlgProc( TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	inline size_t GetRenderStateCount()
	{
		return m_renderStates.size();
	}
	
	inline RenderStateMap::iterator GetRenderStatesBegin()
	{
		return m_renderStates.begin();
	}

	inline RenderStateMap::iterator GetRenderStatesEnd()
	{
		return m_renderStates.end();
	}

	inline ICMaterial *GetParent()
	{
		return m_pParent;
	}

	//inline void SetParent( ICMaterial *pParent )
	//{
	//	m_pParent = pParent;
	//}

	inline ParamBlockDesc2 *GetParamBlockDesc()
	{
		return m_pRenderStatesParamBlockDesc;
	}

	//inline ICRenderState *GetRenderState( size_t index )
	//{
	//	FUAssert( index < m_renderStates.size(), return NULL );
	//	return m_renderStates[index];
	//}

	//ICRenderState *AddRenderState( FUDaePassState::State renderStateType );
	IParamBlock2 *GetParentParamBlock();
	//virtual HWND GetRenderStatesHWnd() = 0;

	inline DLGTEMPLATE *GetDialogTemplate()
	{
		return m_pDialogTemplate;
	}
};

#endif