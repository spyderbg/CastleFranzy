/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "ICRenderStateManager.h"
#include "ICRenderState.h"
#include "ICUIParameter.h"
#include "resource.h"
#include "FCDocument/FCDExportBehavior.h"
#include "FUtils/FUDaeEnumSyntax.h"
#include "UIHelpers.h"
#include "ICMaterial.h"
#include "DlgTemplateBuilder.h"

ICRenderStateManager::RenderStateAccessor ICRenderStateManager::s_renderStatesAccessor;

static int s_currentIdc = 0;
static int GetNextIdc()
{
	return s_currentIdc++;
}

ICUIParameter *ICRenderStateManager::InitRenderState_EnumParameter( ICRenderState *pRenderState, FSStandardMaterial::PID pid
		, int nameIds, int defaultValue
		, int UNUSED( labelIdc ), int UNUSED( comboBoxIdc )
		, const char *( pConversionFunction )( int )
		, const FUDaeCommon::EnumMapping enumMap[], size_t mappingCount )
{
	fm::string name( GetString( nameIds ) );
	ICUIParameter *pParameter = pRenderState->SetParameter( pid, ICWidget::ComboBox );

	// param desc
	{
		m_pRenderStatesParamBlockDesc->AddParam( pid
			, name.c_str()
			, TYPE_INT
			, P_ANIMATABLE + P_RESET_DEFAULT
			, nameIds
			, p_default, defaultValue
			, p_accessor, &s_renderStatesAccessor
			, end );
	}

	// export behavior
	{
		FCDExportBehavior__T_CONVERT_ENUM_TO_STRING *pExportBehavior = ( FCDExportBehavior__T_CONVERT_ENUM_TO_STRING * )pParameter->SetExportBehavior( FCDExportBehavior::T_CONVERT_ENUM_TO_STRING );
		pExportBehavior->SetEnumConversionFunction( pConversionFunction );
	}

	// ui
	{
		ICComboBox &widget = * ( ICComboBox * )pParameter->GetWidget();
		widget.SetName( name.c_str() );
		widget.Construct( GetNextIdc(), GetNextIdc() );
		UIHelpers::FillComboBox( widget, enumMap, mappingCount );
	}

	return pParameter;
}

ICUIParameter *ICRenderStateManager::InitRenderState_FloatParameter( ICRenderState *pRenderState, FSStandardMaterial::PID pid
		, int nameIds, float defaultValue )
{
	fm::string name( GetString( nameIds ) );

	ICUIParameter *pParameter = pRenderState->SetParameter( pid, ICWidget::SpinnerFloat );

	// param desc
	{
		m_pRenderStatesParamBlockDesc->AddParam( pid
			, name.c_str()
			, TYPE_FLOAT
			, P_ANIMATABLE + P_RESET_DEFAULT
			, nameIds
			, p_default, defaultValue
			, p_accessor, &s_renderStatesAccessor
			, end );
	}

	// export behavior
	
	// ui
	ICSpinner &widget = * ( ICSpinner * )pParameter->GetWidget();
	widget.SetName( name.c_str() );
	widget.Construct( GetNextIdc(), GetNextIdc(), GetNextIdc(), 0.f, 1.f, 0.01f, defaultValue );

	return pParameter;
}

ICUIParameter *ICRenderStateManager::InitRenderState_IntParameter( ICRenderState *pRenderState, FSStandardMaterial::PID pid
	, int nameIds, int defaultValue
	, int _min, int _max, int scale )
{
	fm::string name( GetString( nameIds ) );

	ICUIParameter *pParameter = pRenderState->SetParameter( pid, ICWidget::SpinnerInt );

	// param desc
	{
		m_pRenderStatesParamBlockDesc->AddParam( pid
			, name.c_str()
			, TYPE_INT
			, P_ANIMATABLE + P_RESET_DEFAULT
			, nameIds
			, p_default, defaultValue
			, p_accessor, &s_renderStatesAccessor
			, end );
	}

	// export behavior
	
	// ui
	ICSpinner &widget = * ( ICSpinner * )pParameter->GetWidget();
	widget.SetName( name.c_str() );
	widget.Construct( GetNextIdc(), GetNextIdc(), GetNextIdc(), ( float )_min, ( float )_max, ( float )scale, ( float )defaultValue );

	return pParameter;
}

ICUIParameter *ICRenderStateManager::InitRenderState_ColorParameter( ICRenderState *pRenderState, FSStandardMaterial::PID pid
	, int nameIds, const Point4 &defaultValue )
{
	fm::string name( GetString( nameIds ) );

	ICUIParameter *pParameter = pRenderState->SetParameter( pid, ICWidget::ColorChooser );

	// param desc
	{
		m_pRenderStatesParamBlockDesc->AddParam( pid
			, name.c_str()
			, TYPE_FRGBA
			, P_ANIMATABLE + P_RESET_DEFAULT
			, nameIds
			, p_default, defaultValue
			, p_accessor, &s_renderStatesAccessor
			, end );
	}

	// export behavior
	
	// ui
	ICColorChooser &widget = * ( ICColorChooser * )pParameter->GetWidget();
	widget.SetName( name.c_str() );
	widget.Construct( GetNextIdc(), GetNextIdc() );

	return pParameter;
}

// convenience functions
void ICRenderStateManager::InitRenderState_Single( FUDaePassState::State state, FSStandardMaterial::PID pid
		, int nameIds, int defaultValue
		, int labelIdc, int comboBoxIdc
		, const char *( pConversionFunction )( int )
		, const FUDaeCommon::EnumMapping enumMap[], size_t mappingCount )
{
	ICRenderState *pMainRenderState = new ICRenderState( FUDaePassState::ToString( state ), this );
	InitRenderState_EnumParameter( pMainRenderState, pid
		, nameIds, defaultValue
		, labelIdc, comboBoxIdc
		, pConversionFunction
		, enumMap, mappingCount );
	m_renderStates[state] = pMainRenderState;
}

#define ADD_MAIN_RENDER_STATE( __state ) \
	ICRenderState *pMainRenderState = m_renderStates[FUDaePassState::__state] = new ICRenderState( FUDaePassState::ToString( FUDaePassState::__state ), this );

#define INIT_RENDER_STATE_SINGLE_ENUM( __state, __pid, __nameIds, __labelIdc, __comboBoxIdc, __enumNamespace, __default ) \
	InitRenderState_Single( __state, __pid \
		, __nameIds, __enumNamespace::__default \
		, __labelIdc, __comboBoxIdc \
		, &__enumNamespace::ToString \
		, __enumNamespace::enumMap, ARRAY_SIZE( __enumNamespace::enumMap ) \
		)

#define INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( __stateName, __enumNamespace, __default ) \
	INIT_RENDER_STATE_SINGLE_ENUM( FUDaePassState::__stateName, FSStandardMaterial::P_##__stateName \
		, IDS_RS_##__stateName \
		, 0, 0 \
		, __enumNamespace, __default )

#define INIT_RENDER_STATE_SINGLE_COLOR( __state, __paramIdentifier, __default ) \
{ \
	ADD_MAIN_RENDER_STATE( __state ); \
	InitRenderState_ColorParameter( pMainRenderState, FSStandardMaterial::P_##__paramIdentifier \
		, IDS_RS_##__paramIdentifier, __default ); \
}

#define INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( __paramDaeName, __paramIdentifier, __enumNamespace, __default ) \
{ \
	ICRenderState *pRenderState = pMainRenderState->AddChild( __paramDaeName, this ); \
	InitRenderState_EnumParameter( pRenderState, FSStandardMaterial::P_##__paramIdentifier \
		, IDS_RS_##__paramIdentifier, __enumNamespace::__default \
		, 0, 0 \
		, &__enumNamespace::ToString \
		, __enumNamespace::enumMap, ARRAY_SIZE( __enumNamespace::enumMap ) \
		); \
}

#define INIT_RENDER_STATE_CHILD_FLOAT_PARAMETER( __paramDaeName, __paramIdentifier, __default ) \
{ \
	ICRenderState *pRenderState = pMainRenderState->AddChild( __paramDaeName, this ); \
	InitRenderState_FloatParameter( pRenderState, FSStandardMaterial::P_##__paramIdentifier \
		, IDS_RS_##__paramIdentifier, __default ); \
}

#define INIT_RENDER_STATE_CHILD_INT_PARAMETER( __paramDaeName, __paramIdentifier, __default, __min, __max, __scale ) \
{ \
	ICRenderState *pRenderState = pMainRenderState->AddChild( __paramDaeName, this ); \
	InitRenderState_IntParameter( pRenderState, FSStandardMaterial::P_##__paramIdentifier \
		, IDS_RS_##__paramIdentifier, __default, __min, __max, __scale ); \
}

void ICRenderStateManager::Init( ClassDesc2 &classDesc )
{
	ConstructParamBlockDesc( classDesc );

	s_currentIdc = 1024;

	// ALPHA_FUNC
	{
		ADD_MAIN_RENDER_STATE( ALPHA_FUNC );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_FUNC_ELEMENT, ALPHA_FUNC_FUNCTION, FUDaePassStateFunction, ALWAYS );
		INIT_RENDER_STATE_CHILD_FLOAT_PARAMETER( DAE_FXSTD_STATE_VALUE_ELEMENT, ALPHA_FUNC_VALUE, 0.f );
	}

	// BLEND_FUNC
	{
		ADD_MAIN_RENDER_STATE( BLEND_FUNC );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_SRC_ELEMENT, BLEND_FUNC_SRC, FUDaePassStateBlendType, ONE );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_DEST_ELEMENT, BLEND_FUNC_DST, FUDaePassStateBlendType, ZERO );
	}

	// BLEND_FUNC_SEPARATE
	{
		ADD_MAIN_RENDER_STATE( BLEND_FUNC_SEPARATE );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_SRCRGB_ELEMENT, BLEND_FUNC_SEPARATE_SRC_COLOR, FUDaePassStateBlendType, ONE );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_DESTRGB_ELEMENT, BLEND_FUNC_SEPARATE_DST_COLOR, FUDaePassStateBlendType, ZERO );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_SRCALPHA_ELEMENT, BLEND_FUNC_SEPARATE_SRC_ALPHA, FUDaePassStateBlendType, ONE );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_DESTALPHA_ELEMENT, BLEND_FUNC_SEPARATE_DST_ALPHA, FUDaePassStateBlendType, ZERO );
	}

	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( BLEND_EQUATION, FUDaePassStateBlendEquation, ADD );

	// BLEND_EQUATION_SEPARATE
	{
		ADD_MAIN_RENDER_STATE( BLEND_EQUATION_SEPARATE );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_RGB_ELEMENT, BLEND_EQUATION_SEPARATE_COLOR, FUDaePassStateBlendEquation, ADD );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_ALPHA_ELEMENT, BLEND_EQUATION_SEPARATE_ALPHA, FUDaePassStateBlendEquation, ADD );
	}

	// COLOR_MATERIAL ( MULTIPLE )
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( CULL_FACE, FUDaePassStateFaceType, BACK );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( DEPTH_FUNC, FUDaePassStateFunction, ALWAYS );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( FOG_MODE, FUDaePassStateFogType, EXP );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( FOG_COORD_SRC, FUDaePassStateFogCoordinateType, FOG_COORDINATE );
	
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( FRONT_FACE, FUDaePassStateFrontFaceType, COUNTER_CLOCKWISE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( LIGHT_MODEL_COLOR_CONTROL, FUDaePassStateLightModelColorControlType, SINGLE_COLOR );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( LOGIC_OP, FUDaePassStateLogicOperation, COPY );
	// POLYGON MODE ( MULTIPLE )
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( SHADE_MODEL, FUDaePassStateShadeModel, SMOOTH );
	
	// STENCIL_FUNC
	{
		ADD_MAIN_RENDER_STATE( STENCIL_FUNC );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_FUNC_ELEMENT, STENCIL_FUNC_FUNCTION, FUDaePassStateFunction, ALWAYS );

		//int bitPlaneCount = 8;
		INIT_RENDER_STATE_CHILD_INT_PARAMETER( DAE_FXSTD_STATE_REF_ELEMENT, STENCIL_FUNC_REFERENCE_VALUE, 0, 0, 255, 1 );
		INIT_RENDER_STATE_CHILD_INT_PARAMETER( DAE_FXSTD_STATE_MASK_ELEMENT, STENCIL_FUNC_MASK, 255, 0, 255, 1 );
	}

	// STENCIL_OP
	{
		ADD_MAIN_RENDER_STATE( STENCIL_OP );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_FAIL_ELEMENT, STENCIL_OP_FAIL_OPERATION, FUDaePassStateStencilOperation, KEEP );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_ZFAIL_ELEMENT, STENCIL_OP_DEPTH_FAIL_OPERATION, FUDaePassStateStencilOperation, KEEP );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_ZPASS_ELEMENT, STENCIL_OP_DEPTH_PASS_OPERATION, FUDaePassStateStencilOperation, KEEP );
	}

	// STENCIL_FUNC_SEPARATE
	{
		ADD_MAIN_RENDER_STATE( STENCIL_FUNC_SEPARATE );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_FRONT_ELEMENT, STENCIL_FUNC_SEPARATE_FRONT_FACES_FUNCTION, FUDaePassStateFunction, ALWAYS );
		INIT_RENDER_STATE_CHILD_ENUM_PARAMETER( DAE_FXSTD_STATE_BACK_ELEMENT, STENCIL_FUNC_SEPARATE_BACK_FACES_FUNCTION, FUDaePassStateFunction, ALWAYS );

		//int bitPlaneCount = 8;
		INIT_RENDER_STATE_CHILD_INT_PARAMETER( DAE_FXSTD_STATE_REF_ELEMENT, STENCIL_FUNC_SEPARATE_REFERENCE_VALUE, 0, 0, 255, 1 );
		INIT_RENDER_STATE_CHILD_INT_PARAMETER( DAE_FXSTD_STATE_MASK_ELEMENT, STENCIL_FUNC_SEPARATE_MASK, 255, 0, 255, 1 );
	}

	// STENCIL_OP_SEPARATE ( MULTIPLE )
	// STENCIL_MASK_SEPARATE ( MULTIPLE )

	// LIGHT_ENABLE ( LIGHT_INDEX )
	// LIGHT_AMBIENT ( LIGHT_INDEX )
	// LIGHT_DIFFUSE ( LIGHT_INDEX )
	// LIGHT_SPECULAR ( LIGHT_INDEX )
	// LIGHT_POSITION ( LIGHT_INDEX )
	// LIGHT_CONSTANT_ATTENUATION ( LIGHT_INDEX )
	// LIGHT_LINEAR_ATTENUATION ( LIGHT_INDEX )
	// LIGHT_QUADRATIC_ATTENUATION ( LIGHT_INDEX )
	// LIGHT_SPOT_CUTOFF ( LIGHT_INDEX )
	// LIGHT_SPOT_DIRECTION ( LIGHT_INDEX )
	// LIGHT_SPOT_EXPONENT ( LIGHT_INDEX )

	// TEXTURE1D ( TEXTURE_UNIT_INDEX )
	// TEXTURE2D ( TEXTURE_UNIT_INDEX )
	// TEXTURE3D ( TEXTURE_UNIT_INDEX ) 
	// TEXTURECUBE ( TEXTURE_UNIT_INDEX )
	// TEXTURERECT ( TEXTURE_UNIT_INDEX )
	// TEXTUREDEPTH ( TEXTURE_UNIT_INDEX )
	// TEXTURE1D_ENABLE ( TEXTURE_UNIT_INDEX )
	// TEXTURE2D_ENABLE ( TEXTURE_UNIT_INDEX )
	// TEXTURE3D_ENABLE ( TEXTURE_UNIT_INDEX )
	// TEXTURECUBE_ENABLE ( TEXTURE_UNIT_INDEX )
	// TEXTURERECT_ENABLE ( TEXTURE_UNIT_INDEX )
	// TEXTUREDEPTH_ENABLE ( TEXTURE_UNIT_INDEX )
	// TEXTURE_ENV_COLOR ( TEXTURE_UNIT_INDEX )
	// TEXTURE_ENV_MODE ( TEXTURE_UNIT_INDEX )

	// CLIP_PLANE ( CLIP_PLANE_INDEX )
	// CLIP_PLANE_ENABLE ( CLIP_PLANE_INDEX )
	// BLEND_COLOR
	//INIT_RENDER_STATE_SINGLE_COLOR( BLEND_COLOR );
	//{
	//	
	//}
	// CLEAR_COLOR
	// CLEAR_STENCIL
	// CLEAR_DEPTH
	// COLOR_MASK
	{
		ICRenderState *pMainRenderState = new ICRenderState( FUDaePassState::ToString( FUDaePassState::COLOR_MASK ), this );

		{
			ICRenderState *pRenderState = pMainRenderState;
			ICUIParameter *pParameter = pRenderState->SetParameter( FSStandardMaterial::P_COLOR_MASK, ICWidget::ComboBox4 );

			// param desc
			{
				m_pRenderStatesParamBlockDesc->AddParam( FSStandardMaterial::P_COLOR_MASK
					, GetString( IDS_RS_COLOR_MASK )
					, TYPE_FRGBA
					, P_ANIMATABLE + P_RESET_DEFAULT
					, IDS_RS_COLOR_MASK
					, p_default, Point4( FUDaeBoolAbbreviated::B_TRUE, FUDaeBoolAbbreviated::B_TRUE, FUDaeBoolAbbreviated::B_TRUE, FUDaeBoolAbbreviated::B_TRUE )
					, p_accessor, &s_renderStatesAccessor
					, end );
			}
			
			// export behavior
			//{
			//	FCDExportBehavior__T_CONVERT_ENUM_TO_STRING *pExportBehavior = ( FCDExportBehavior__T_CONVERT_ENUM_TO_STRING * )pParameter->SetExportBehavior( FCDExportBehavior::T_CONVERT_ENUM_TO_STRING );
			//	pExportBehavior->SetEnumConversionFunction( &FUDaeBool::ToStringBool );
			//}

			// ui
			{
				ICComboBox4 &widget = * ( ICComboBox4 * )pParameter->GetWidget();
				widget.SetName( GetString( IDS_RS_COLOR_MASK ) );
				widget.Construct( GetNextIdc() );

				for ( int comboBoxId = 0; comboBoxId < ICComboBox4::k_numChildWidgets; ++comboBoxId )
				{
					ICComboBox &comboBox = widget.GetChildWidgetRef( comboBoxId );
					comboBox.SetName( FUDaeAccessor::RGBA[comboBoxId] );
					comboBox.Construct( GetNextIdc(), GetNextIdc() );
					UIHelpers::FillComboBox( comboBox, FUDaeBoolAbbreviated::enumMap, ARRAY_SIZE( FUDaeBoolAbbreviated::enumMap ) );
				}
			}
		}

		m_renderStates[FUDaePassState::COLOR_MASK] = pMainRenderState;
	}
	// DEPTH_BOUNDS
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( DEPTH_MASK, FUDaeBool, B_TRUE );
	// DEPTH_RANGE

	// FOG_DENSITY
	// FOG_START
	// FOG_END
	// FOG_COLOR
	// LIGHT_MODEL_AMBIENT
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( LIGHTING_ENABLE, FUDaeBool, B_FALSE );
	// LINE_STIPPLE
	// LINE_WIDTH

	// MATERIAL_AMBIENT
	// MATERIAL_DIFFUSE
	// MATERIAL_EMISSION
	// MATERIAL_SHININESS
	// MATERIAL_SPECULAR
	// MODEL_VIEW_MATRIX
	// POINT_DISTANCE_ATTENUATION
	// POINT_FADE_THRESHOLD_SIZE
	// POINT_SIZE
	// POINT_SIZE_MIN
	// POINT_SIZE_MAX

	// POLYGON_OFFSET
	// PROJECTION_MATRIX
	// SCISSOR
	// STENCIL_MASK
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( ALPHA_TEST_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( AUTO_NORMAL_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( BLEND_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( COLOR_LOGIC_OP_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( COLOR_MATERIAL_ENABLE, FUDaeBool, B_TRUE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( CULL_FACE_ENABLE, FUDaeBool, B_FALSE );

	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( DEPTH_BOUNDS_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( DEPTH_CLAMP_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( DEPTH_TEST_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( DITHER_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( FOG_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( LIGHT_MODEL_LOCAL_VIEWER_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( LIGHT_MODEL_TWO_SIDE_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( LINE_SMOOTH_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( LINE_STIPPLE_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( LOGIC_OP_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( MULTISAMPLE_ENABLE, FUDaeBool, B_FALSE );

	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( NORMALIZE_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( POINT_SMOOTH_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( POLYGON_OFFSET_FILL_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( POLYGON_OFFSET_LINE_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( POLYGON_OFFSET_POINT_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( POLYGON_SMOOTH_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( POLYGON_STIPPLE_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( RESCALE_NORMAL_ENABLE, FUDaeBool, B_FALSE );

	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( SAMPLE_ALPHA_TO_COVERAGE_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( SAMPLE_ALPHA_TO_ONE_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( SAMPLE_COVERAGE_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( SCISSOR_TEST_ENABLE, FUDaeBool, B_FALSE );
	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( STENCIL_TEST_ENABLE, FUDaeBool, B_FALSE );

	INIT_RENDER_STATE_SINGLE_ENUM_SIMPLE( TEXTURE_PIPELINE_ENABLE, FUDaeBool, B_FALSE );
	// TEXTURE_PIPELINE

	ConstructDialogTemplate();
}

void ICRenderStateManager::ConstructParamBlockDesc( ClassDesc2 &classDesc )
{
//#define DECL_RENDER_STATE_SINGLE( __stateName, __default ) \
//	, FSStandardMaterial::P_##__stateName, GetString( IDS_RS_##__stateName ), TYPE_INT, P_ANIMATABLE + P_RESET_DEFAULT, IDS_RS_##__stateName \
//		, p_default, ( int )__default \
//		, p_accessor, &s_renderStatesAccessor \
//	, end
//
//#define DECL_RENDER_STATE_SINGLE_OBSOLETE( __stateName, __default ) \
//	, FSStandardMaterial::P_##__stateName, GetString( IDS_RS_##__stateName ), TYPE_INT, P_ANIMATABLE + P_RESET_DEFAULT + P_OBSOLETE, IDS_RS_##__stateName \
//	, end
//
//	FUAssert( !m_pRenderStatesParamBlockDesc, return );
//	SAFE_DELETE( m_pRenderStatesParamBlockDesc );

	m_pRenderStatesParamBlockDesc = new ParamBlockDesc2( 
		std2_render_states
		, _T("RenderStates")
		, FSStandardMaterial::RSPBV_CURRENT
		, &classDesc
		, P_AUTO_CONSTRUCT + /*P_AUTO_UI + */P_VERSION + P_TEMPLATE_UI
		, FSStandardMaterial::RSPBV_CURRENT
		, FSStandardMaterial::RENDER_STATES_PB_REF
		
		//rollout
		//, IDD_DMTL_RENDER_STATES, IDS_KE_RENDER_STATES, 0, 0, pDlgProcWrapper	

		// params
		//, FSStandardMaterial::P_ALPHA_FUNC_FUNCTION, GetString( IDS_RS_ALPHA_FUNC_FUNCTION ), TYPE_INT, P_ANIMATABLE + P_RESET_DEFAULT, IDS_RS_ALPHA_FUNC_FUNCTION
		//	, p_default, ( int )FUDaePassStateFunction::ALWAYS
		//	, p_accessor, &s_renderStatesAccessor
		//, end

		//, FSStandardMaterial::P_ALPHA_FUNC_VALUE, GetString( IDS_RS_ALPHA_FUNC_VALUE ), TYPE_FLOAT, P_ANIMATABLE + P_RESET_DEFAULT, IDS_RS_ALPHA_FUNC_VALUE
		//	, p_default, 0.f
		//	, p_accessor, &s_renderStatesAccessor
		//, end

		//, FSStandardMaterial::P_BLEND_FUNC_SRC, GetString( IDS_RS_BLEND_FUNC_SRC ), TYPE_INT, P_ANIMATABLE + P_RESET_DEFAULT, IDS_RS_BLEND_FUNC_SRC
		//	, p_default, ( int )FUDaePassStateBlendType::ONE
		//	, p_accessor, &s_renderStatesAccessor
		//, end
	
		//, FSStandardMaterial::P_BLEND_FUNC_DST, GetString( IDS_RS_BLEND_FUNC_DST ), TYPE_INT, P_ANIMATABLE + P_RESET_DEFAULT, IDS_RS_BLEND_FUNC_DST
		//	, p_default, ( int )FUDaePassStateBlendType::ZERO
		//	, p_accessor, &s_renderStatesAccessor	
		//, end
	
		//, FSStandardMaterial::P_BLEND_FUNC_SEPARATE_SRC_COLOR, GetString( IDS_RS_BLEND_FUNC_SEPARATE_SRC_COLOR ), TYPE_INT, P_ANIMATABLE, IDS_RS_BLEND_FUNC_SEPARATE_SRC_COLOR, end
		//, FSStandardMaterial::P_BLEND_FUNC_SEPARATE_DST_COLOR, GetString( IDS_RS_BLEND_FUNC_SEPARATE_DST_COLOR ), TYPE_INT, P_ANIMATABLE, IDS_RS_BLEND_FUNC_SEPARATE_DST_COLOR, end
		//, FSStandardMaterial::P_BLEND_FUNC_SEPARATE_SRC_ALPHA, GetString( IDS_RS_BLEND_FUNC_SEPARATE_SRC_ALPHA ), TYPE_INT, P_ANIMATABLE, IDS_RS_BLEND_FUNC_SEPARATE_SRC_ALPHA, end
		//, FSStandardMaterial::P_BLEND_FUNC_SEPARATE_DST_ALPHA, GetString( IDS_RS_BLEND_FUNC_SEPARATE_DST_ALPHA ), TYPE_INT, P_ANIMATABLE, IDS_RS_BLEND_FUNC_SEPARATE_DST_ALPHA, end
	
		//, FSStandardMaterial::P_BLEND_EQUATION, GetString( IDS_RS_BLEND_EQUATION ), TYPE_INT, P_ANIMATABLE, IDS_RS_BLEND_EQUATION, end
	
		//, FSStandardMaterial::P_BLEND_EQUATION_SEPARATE_COLOR, GetString( IDS_RS_BLEND_EQUATION_SEPARATE_COLOR ), TYPE_INT, P_ANIMATABLE, IDS_RS_BLEND_EQUATION_SEPARATE_COLOR, end
		//, FSStandardMaterial::P_BLEND_EQUATION_SEPARATE_ALPHA, GetString( IDS_RS_BLEND_EQUATION_SEPARATE_ALPHA ), TYPE_INT, P_ANIMATABLE, IDS_RS_BLEND_EQUATION_SEPARATE_ALPHA, end
	
		//, FSStandardMaterial::P_COLOR_MATERIAL_FACE, GetString( IDS_RS_COLOR_MATERIAL_FACE ), TYPE_INT, P_ANIMATABLE, IDS_RS_COLOR_MATERIAL_FACE, end
		//, FSStandardMaterial::P_COLOR_MATERIAL_TYPE, GetString( IDS_RS_COLOR_MATERIAL_TYPE ), TYPE_INT, P_ANIMATABLE, IDS_RS_COLOR_MATERIAL_TYPE, end
	
		//DECL_RENDER_STATE_SINGLE( CULL_FACE, FUDaePassStateFaceType::BACK )
		//DECL_RENDER_STATE_SINGLE( DEPTH_FUNC, FUDaePassStateFunction::ALWAYS )
		////, FSStandardMaterial::P_FOG_COORD_SRC, GetString( IDS_RS_FOG_COORD_SRC ), TYPE_INT, P_ANIMATABLE, IDS_RS_FOG_COORD_SRC, end
		//DECL_RENDER_STATE_SINGLE( FOG_MODE, FUDaePassStateFogType::EXP )
		//DECL_RENDER_STATE_SINGLE_OBSOLETE( FRONT_FACE, FUDaePassStateFrontFaceType::COUNTER_CLOCKWISE )
		//DECL_RENDER_STATE_SINGLE( FRONT_FACE, FUDaePassStateFrontFaceType::COUNTER_CLOCKWISE )
		////, FSStandardMaterial::P_LIGHT_MODEL_COLOR_CONTROL, GetString( IDS_RS_LIGHT_MODEL_COLOR_CONTROL ), TYPE_INT, P_ANIMATABLE, IDS_RS_LIGHT_MODEL_COLOR_CONTROL, end
		//DECL_RENDER_STATE_SINGLE( LOGIC_OP, FUDaePassStateLogicOperation::COPY )
	
		//, FSStandardMaterial::P_POLYGON_MODE_FACE, GetString( IDS_RS_POLYGON_MODE_FACE ), TYPE_INT, P_ANIMATABLE, IDS_RS_POLYGON_MODE_FACE, end
		//, FSStandardMaterial::P_POLYGON_MODE_MODE, GetString( IDS_RS_POLYGON_MODE_MODE ), TYPE_INT, P_ANIMATABLE, IDS_RS_POLYGON_MODE_MODE, end
	
		//, FSStandardMaterial::P_SHADE_MODEL, GetString( IDS_RS_SHADE_MODEL ), TYPE_INT, P_ANIMATABLE, IDS_RS_SHADE_MODEL, end
	
		//, FSStandardMaterial::P_STENCIL_FUNC_FUNCTION, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_FUNC_REFERENCE_VALUE, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_FUNC_MASK, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
	
		//, FSStandardMaterial::P_STENCIL_OP_FAIL_OPERATION, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_OP_DEPTH_FAIL_OPERATION, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_OP_DEPTH_PASS_OPERATION, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
	
		//, FSStandardMaterial::P_STENCIL_FUNC_SEPARATE_FRONT_FACES_FUNCTION, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_FUNC_SEPARATE_BACK_FACES_FUNCTION, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_FUNC_SEPARATE_REFERENCE_VALUE, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_FUNC_SEPARATE_MASK, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
	
		//, FSStandardMaterial::P_STENCIL_OP_SEPARATE_FACE, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_OP_SEPARATE_FAIL_OPERATION, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_OP_SEPARATE_DEPTH_FAIL_OPERATION, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_OP_SEPARATE_DEPTH_PASS_OPERATION, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
	
		//, FSStandardMaterial::P_STENCIL_MASK_SEPARATE_FACE, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end
		//, FSStandardMaterial::P_STENCIL_MASK_SEPARATE_MASK, GetString( IDS_RS_ALPHA_FUNC ), TYPE_INT, P_ANIMATABLE, IDS_RS_ALPHA_FUNC, end

		, end
	);

//#undef DECL_RENDER_STATE_SINGLE
}

static const uint16 lineHeight = 16;
static const uint16 lineWidth = 216;
static const uint16 leftSectionLeft = 2;
static const uint16 leftSectionRight = 118;
static const uint16 rightSectionLeft = 124;
static const uint16 rightSectionRight = 213;
static const uint16 rightSectionWidth = rightSectionRight - rightSectionLeft;

void ICRenderStateManager::BuildDialogTemplateItem( DlgTemplateBuilder &tb, WORD& dialogCount, WORD& lineCount, ICWidget *pWidget )
{
	if ( pWidget->GetType() == ICWidget::ComboBox )
	{
		ICComboBox *pDerivedWidget = ( ICComboBox * )pWidget;
		//pDerivedWidget->BuildDialogTemplateItem( tb
		//	, dialogCount, lineCount
		//	, 
		
		// label
		{
			// Create a label for the parameter control
			UIHelpers::BuildDialogTemplateItem_Label( tb
				//, dialogCount, lineCount
				, pDerivedWidget->GetLabelIdc(), pDerivedWidget->GetName()
				, leftSectionLeft, lineCount * lineHeight + 4, leftSectionRight - leftSectionLeft, lineHeight - 2
				, WS_CHILD | WS_VISIBLE, NULL
				);

			++dialogCount;
		}

		// combo box
		{
			uint32 breakX = rightSectionRight - 8;

			UIHelpers::BuildDialogTemplateItem_ComboBox( tb
				//, dialogCount, lineCount
				, pDerivedWidget->GetComboBoxIdc(), pDerivedWidget->GetName()
				, rightSectionLeft, lineCount * lineHeight + 4, breakX - rightSectionLeft, 200
				, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | CBS_OWNERDRAWFIXED | /*CBS_UPPERCASE | */CBS_HASSTRINGS | CBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP, NULL
				);

			++dialogCount;
		}

		++lineCount;
	}
	else if ( pWidget->GetType() == ICWidget::SpinnerFloat
		|| pWidget->GetType() == ICWidget::SpinnerInt )
	{
		ICSpinner *pDerivedWidget = ( ICSpinner * )pWidget;
		
		// label
		{
			// Create a label for the parameter control
			UIHelpers::BuildDialogTemplateItem_Label( tb
				//, dialogCount, lineCount
				, pDerivedWidget->GetLabelIdc(), pDerivedWidget->GetName()
				, leftSectionLeft, lineCount * lineHeight + 4, leftSectionRight - leftSectionLeft, lineHeight - 2
				, WS_CHILD | WS_VISIBLE, NULL
				);

			++dialogCount;
		}

		uint32 breakX = rightSectionRight - 8;

		// edit text
		{
			// Create a label for the parameter control
			UIHelpers::BuildDialogTemplateItem_EditText( tb
				//, dialogCount, lineCount
				, pDerivedWidget->GetEditIdc(), pDerivedWidget->GetName()
				, rightSectionLeft, lineCount * lineHeight + 4, breakX - rightSectionLeft, lineHeight - 1
				, WS_CHILD | WS_VISIBLE, NULL
				);

			++dialogCount;
		}

		// spinner
		{
			// Create a label for the parameter control
			UIHelpers::BuildDialogTemplateItem_Spinner( tb
				//, dialogCount, lineCount
				, pDerivedWidget->GetSpinnerIdc(), pDerivedWidget->GetName()
				, breakX + 1, lineCount * lineHeight + 4, rightSectionRight - breakX - 1, lineHeight - 1
				, WS_CHILD | WS_VISIBLE, NULL
				);

			++dialogCount;
		}

		++lineCount;
	}
	else if ( pWidget->GetType() == ICWidget::ComboBox4 )
	{
		ICComboBox4 *pDerivedWidget = ( ICComboBox4 * )pWidget;
		
		// label
		{
			// Create a label for the parameter control
			UIHelpers::BuildDialogTemplateItem_Label( tb
				//, dialogCount, lineCount
				, pDerivedWidget->GetLabelIdc(), pDerivedWidget->GetName()
				, leftSectionLeft, lineCount * lineHeight + 4, leftSectionRight - leftSectionLeft, lineHeight - 2
				, WS_CHILD | WS_VISIBLE, NULL
				);

			++dialogCount;
			++lineCount;
		}

		for ( size_t childId = 0; childId < ICComboBox4::k_numChildWidgets; ++childId )
		{
			BuildDialogTemplateItem( tb, dialogCount, lineCount, &pDerivedWidget->GetChildWidgetRef( childId ) );
		}

		//uint32 breakX = rightSectionRight - 8;

		//// edit text
		//{
		//	// Create a label for the parameter control
		//	UIHelpers::BuildDialogTemplateItem_EditText( tb
		//		//, dialogCount, lineCount
		//		, pDerivedWidget->GetEditIdc(), pDerivedWidget->GetName()
		//		, rightSectionLeft, lineCount * lineHeight + 4, breakX - rightSectionLeft, lineHeight - 1
		//		, WS_CHILD | WS_VISIBLE, NULL
		//		);

		//	++dialogCount;
		//}

		//// spinner
		//{
		//	// Create a label for the parameter control
		//	UIHelpers::BuildDialogTemplateItem_Spinner( tb
		//		//, dialogCount, lineCount
		//		, pDerivedWidget->GetSpinnerIdc(), pDerivedWidget->GetName()
		//		, breakX + 1, lineCount * lineHeight + 4, rightSectionRight - breakX - 1, lineHeight - 1
		//		, WS_CHILD | WS_VISIBLE, NULL
		//		);

		//	++dialogCount;
		//}

		++lineCount;
	}
	else if ( pWidget->GetType() == ICWidget::ColorChooser )
	{
		ICColorChooser *pDerivedWidget = ( ICColorChooser * )pWidget;
		
		// label
		{
			// Create a label for the parameter control
			UIHelpers::BuildDialogTemplateItem_Label( tb
				//, dialogCount, lineCount
				, pDerivedWidget->GetLabelIdc(), pDerivedWidget->GetName()
				, leftSectionLeft, lineCount * lineHeight + 4, leftSectionRight - leftSectionLeft, lineHeight - 2
				, WS_CHILD | WS_VISIBLE, NULL
				);

			++dialogCount;
			++lineCount;
		}

		//for ( size_t childId = 0; childId < ICComboBox4::k_numChildWidgets; ++childId )
		//{
		//	BuildDialogTemplateItem( tb, dialogCount, lineCount, &pDerivedWidget->GetChildWidgetRef( childId ) );
		//}

		//uint32 breakX = rightSectionRight - 8;

		//// edit text
		//{
		//	// Create a label for the parameter control
		//	UIHelpers::BuildDialogTemplateItem_EditText( tb
		//		//, dialogCount, lineCount
		//		, pDerivedWidget->GetEditIdc(), pDerivedWidget->GetName()
		//		, rightSectionLeft, lineCount * lineHeight + 4, breakX - rightSectionLeft, lineHeight - 1
		//		, WS_CHILD | WS_VISIBLE, NULL
		//		);

		//	++dialogCount;
		//}

		//// spinner
		//{
		//	// Create a label for the parameter control
		//	UIHelpers::BuildDialogTemplateItem_Spinner( tb
		//		//, dialogCount, lineCount
		//		, pDerivedWidget->GetSpinnerIdc(), pDerivedWidget->GetName()
		//		, breakX + 1, lineCount * lineHeight + 4, rightSectionRight - breakX - 1, lineHeight - 1
		//		, WS_CHILD | WS_VISIBLE, NULL
		//		);

		//	++dialogCount;
		//}

		++lineCount;
	}
}

void ICRenderStateManager::BuildDialogTemplateItems( DlgTemplateBuilder &tb, WORD& dialogCount, WORD& lineCount, ICRenderState *&pRenderState )
{
	ICUIParameter *pParameter = pRenderState->GetParameter();
	if ( pParameter )
	{
		ICWidget *pWidget = pParameter->GetWidget();
		if ( pWidget )
		{
			BuildDialogTemplateItem( tb, dialogCount, lineCount, pWidget );
		}
	}

	size_t childCount = pRenderState->GetChildCount();
	for ( size_t childId = 0; childId < childCount; ++childId )
	{
		ICRenderState *pChild = pRenderState->GetChild( childId );
		if ( !pChild )
		{
			continue;
		}

		BuildDialogTemplateItems( tb, dialogCount, lineCount, pChild );
	}
}

void ICRenderStateManager::ConstructDialogTemplate()
{
	SAFE_DELETE_ARRAY( m_pDialogTemplate );
	//m_pParent->GetRenderStateParamBlock

	// create the builder with a large enough buffer
	// TODO : Verify buffer size
	DlgTemplateBuilder tb(16*1024);

	// Generate the dialog template.
	DLGTEMPLATE& dialog = tb.Append<DLGTEMPLATE>();
	dialog.style = WS_CHILD | WS_VISIBLE | DS_SETFONT;
	dialog.cx = lineWidth;

	UNUSED(uint16& menuIdentifier =) tb.Append<uint16>(); // Skip the menu identifier.
	UNUSED(uint16& windowClassName =) tb.Append<uint16>(); // Skip the window class name.
	UNUSED(uint16& windowTitleName =) tb.Append<uint16>(); // Skip the window title name.

	// [DS_SETFONT]
	tb.Append((uint16) 8); // font size
	tb.AppendWString("MS Sans Serif"); // font typename

	// sort the parameter vector using their UI names
	//qsort(mParameters.begin(), mParameters.size(), sizeof(ColladaEffectParameter*), sortParameters);

	// Process the parameters into dialog controls
	WORD lineCount = 0;
	for ( RenderStateMap::iterator renderStateIter = m_renderStates.begin(); renderStateIter != m_renderStates.end(); ++renderStateIter )
	{
		BuildDialogTemplateItems( tb, dialog.cdit, lineCount, renderStateIter->second );
		//m_renderStates[p]->buildTemplateItem(tb,dialog.cdit,lineCount);
	}

	// adjust the height of the dialog based on the resulting UI
	//size_t lineCount = 1;
	dialog.cy = lineCount * lineHeight + 4 + 10;

	// Scale down the buffer in order to save memory.
	m_pDialogTemplate = tb.buildTemplate();
}

void ICRenderStateManager::Reset( IParamBlock2 *pParamBlock )
{
	if ( !pParamBlock )
	{
		return;
	}
	
	pParamBlock->ResetAll();
}

ICRenderStateManager::~ICRenderStateManager()
{
	CLEAR_POINTER_MAP( m_renderStates, RenderStateMap::iterator );
	SAFE_DELETE_ARRAY( m_pDialogTemplate );
}

void ICRenderStateManager::InitUI( TimeValue t, HWND hWnd )
{
	for ( RenderStateMap::iterator renderStateIter = m_renderStates.begin(); renderStateIter != m_renderStates.end(); ++renderStateIter )
	{
		ICRenderState *pRenderState = renderStateIter->second;
		pRenderState->InitUI( t, hWnd );
	}
}

BOOL ICRenderStateManager::DlgProc( TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch ( msg )
	{
		case WM_INITDIALOG:
		{
			InitUI( t, hWnd );
			break;
		}
	}

	for ( RenderStateMap::iterator renderStateIter = m_renderStates.begin(); renderStateIter != m_renderStates.end(); ++renderStateIter )
	{
		ICRenderState *pRenderState = renderStateIter->second;
		if ( pRenderState->DlgProc( t, map, hWnd, msg, wParam, lParam ) )
		{
			return TRUE;
		}
	}

	return FALSE;
}

void ICRenderStateManager::GetValidity( IParamBlock2 *pParamBlock, TimeValue t, Interval &valid )
{
	if ( pParamBlock )
	{
		pParamBlock->GetValidity( t, valid );
	}

	//size_t renderStateCount = GetRenderStateCount();
	//for ( size_t renderStateId = 0; renderStateId < renderStateCount; ++renderStateId )
	//{
	//	ICRenderState *pRenderState = GetRenderState( renderStateId );
	//	pRenderState->GetValidity( t, valid );
	//}
}

void ICRenderStateManager::UpdateUI( HWND hWnd, TimeValue t )
{
	if ( hWnd )
	{
		for ( RenderStateMap::iterator renderStateIter = m_renderStates.begin(); renderStateIter != m_renderStates.end(); ++renderStateIter )
		{
			ICRenderState *pRenderState = renderStateIter->second;
			pRenderState->UpdateUI( t, hWnd );
		}
	}

	m_pRenderStatesParamBlockDesc->InvalidateUI();
}

void ICRenderStateManager::RenderStateAccessor::Set( PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t )
{
	//ICMaterial *pMaterial = ( ICMaterial * )owner;
	//pMaterial->UpdateUI( t );
}

IParamBlock2 *ICRenderStateManager::GetParentParamBlock()
{
	if ( !m_pParent )
	{
		return NULL;
	}

	return m_pParent->GetRenderStateParamBlock();
}

void ICRenderStateManager::ICRenderStateManagerPLCB::proc( ILoad *pLoadingInterface )
{
	FUAssert( m_pOwner, delete this );

	ICRenderStateManager &renderStateManager = m_pOwner->GetRenderStateManager();
	ParamBlockDesc2 *pParamBlockDesc = renderStateManager.GetParamBlockDesc();
	IParamBlock2 *pParamBlock = m_pOwner->GetRenderStateParamBlock();
	
	IParamBlock2PostLoadInfo *pPostLoadInfo = ( IParamBlock2PostLoadInfo * )pParamBlock->GetInterface( IPARAMBLOCK2POSTLOADINFO_ID );

	if ( pPostLoadInfo )
	{
		DWORD previousVersionId = pPostLoadInfo->GetVersion();
		IntTab &paramsLoaded = pPostLoadInfo->GetParamLoaded();
		IntTab &paramsReorder = pPostLoadInfo->GetParamReorder();

		// Remap parameters from old versions to current version here
	}
		
	delete this;
}

//ICRenderState *ICRenderStateManager::AddRenderState( FUDaePassState::State renderStateType )
//{
//	ICRenderState *pRenderState = NULL;
//
//	switch ( renderStateType )
//	{
//	case FUDaePassState::ALPHA_FUNC: pRenderState = new ICRenderState__ALPHA_FUNC( this, FSStandardMaterial::P_ALPHA_FUNC_FUNCTION, FSStandardMaterial::P_ALPHA_FUNC_VALUE ); break;
//	//case FUDaePassState::BLEND_FUNC: pRenderState = new ICRenderState__BLEND_FUNC( pDocument ); break;
//	//case FUDaePassState::BLEND_FUNC_SEPARATE: pRenderState = new ICRenderState__BLEND_FUNC_SEPARATE( pDocument ); break;
//	//case FUDaePassState::BLEND_EQUATION: pRenderState = new ICRenderState__BLEND_EQUATION( pDocument ); break;
//	//case FUDaePassState::BLEND_EQUATION_SEPARATE: pRenderState = new ICRenderState__BLEND_EQUATION_SEPARATE( pDocument ); break;
//	//case FUDaePassState::COLOR_MATERIAL: pRenderState = new ICRenderState__COLOR_MATERIAL( pDocument ); break;
//	//case FUDaePassState::CULL_FACE: pRenderState = new ICRenderState__CULL_FACE( pDocument ); break;
//	//case FUDaePassState::DEPTH_FUNC: pRenderState = new ICRenderState__DEPTH_FUNC( pDocument ); break;
//	//case FUDaePassState::FOG_MODE: pRenderState = new ICRenderState__FOG_MODE( pDocument ); break;
//	//case FUDaePassState::FOG_COORD_SRC: pRenderState = new ICRenderState__FOG_COORD_SRC( pDocument ); break;
//
//	//case FUDaePassState::FRONT_FACE: pRenderState = new ICRenderState__FRONT_FACE( pDocument ); break;
//	//case FUDaePassState::LIGHT_MODEL_COLOR_CONTROL: pRenderState = new ICRenderState__LIGHT_MODEL_COLOR_CONTROL( pDocument ); break;
//	//case FUDaePassState::LOGIC_OP: pRenderState = new ICRenderState__LOGIC_OP( pDocument ); break;
//	//case FUDaePassState::POLYGON_MODE: pRenderState = new ICRenderState__POLYGON_MODE( pDocument ); break;
//	//case FUDaePassState::SHADE_MODEL: pRenderState = new ICRenderState__SHADE_MODEL( pDocument ); break;
//	//case FUDaePassState::STENCIL_FUNC: pRenderState = new ICRenderState__STENCIL_FUNC( pDocument ); break;
//	//case FUDaePassState::STENCIL_OP: pRenderState = new ICRenderState__STENCIL_OP( pDocument ); break;
//	//case FUDaePassState::STENCIL_FUNC_SEPARATE: pRenderState = new ICRenderState__STENCIL_FUNC_SEPARATE( pDocument ); break;
//	//case FUDaePassState::STENCIL_OP_SEPARATE: pRenderState = new ICRenderState__STENCIL_OP_SEPARATE( pDocument ); break;
//	//case FUDaePassState::STENCIL_MASK_SEPARATE: pRenderState = new ICRenderState__STENCIL_MASK_SEPARATE( pDocument ); break;
//
//	//case FUDaePassState::LIGHT_ENABLE: pRenderState = new ICRenderState__LIGHT_ENABLE( pDocument ); break;
//	//case FUDaePassState::LIGHT_AMBIENT: pRenderState = new ICRenderState__LIGHT_AMBIENT( pDocument ); break;
//	//case FUDaePassState::LIGHT_DIFFUSE: pRenderState = new ICRenderState__LIGHT_DIFFUSE( pDocument ); break;
//	//case FUDaePassState::LIGHT_SPECULAR: pRenderState = new ICRenderState__LIGHT_SPECULAR( pDocument ); break;
//	//case FUDaePassState::LIGHT_POSITION: pRenderState = new ICRenderState__LIGHT_POSITION( pDocument ); break;
//	//case FUDaePassState::LIGHT_CONSTANT_ATTENUATION: pRenderState = new ICRenderState__LIGHT_CONSTANT_ATTENUATION( pDocument ); break;
//	//case FUDaePassState::LIGHT_LINEAR_ATTENUATION: pRenderState = new ICRenderState__LIGHT_LINEAR_ATTENUATION( pDocument ); break;
//	//case FUDaePassState::LIGHT_QUADRATIC_ATTENUATION: pRenderState = new ICRenderState__LIGHT_QUADRATIC_ATTENUATION( pDocument ); break;
//	//case FUDaePassState::LIGHT_SPOT_CUTOFF: pRenderState = new ICRenderState__LIGHT_SPOT_CUTOFF( pDocument ); break;
//	//case FUDaePassState::LIGHT_SPOT_DIRECTION: pRenderState = new ICRenderState__LIGHT_SPOT_DIRECTION( pDocument ); break;
//	//case FUDaePassState::LIGHT_SPOT_EXPONENT: pRenderState = new ICRenderState__LIGHT_SPOT_EXPONENT( pDocument ); break;
//
//	//case FUDaePassState::TEXTURE1D: pRenderState = new ICRenderState__TEXTURE1D( pDocument ); break;
//	//case FUDaePassState::TEXTURE2D: pRenderState = new ICRenderState__TEXTURE2D( pDocument ); break;
//	//case FUDaePassState::TEXTURE3D: pRenderState = new ICRenderState__TEXTURE3D( pDocument ); break;
//	//case FUDaePassState::TEXTURECUBE: pRenderState = new ICRenderState__TEXTURECUBE( pDocument ); break;
//	//case FUDaePassState::TEXTURERECT: pRenderState = new ICRenderState__TEXTURERECT( pDocument ); break;
//	//case FUDaePassState::TEXTUREDEPTH: pRenderState = new ICRenderState__TEXTUREDEPTH( pDocument ); break;
//	//case FUDaePassState::TEXTURE1D_ENABLE: pRenderState = new ICRenderState__TEXTURE1D_ENABLE( pDocument ); break;
//	//case FUDaePassState::TEXTURE2D_ENABLE: pRenderState = new ICRenderState__TEXTURE2D_ENABLE( pDocument ); break;
//	//case FUDaePassState::TEXTURE3D_ENABLE: pRenderState = new ICRenderState__TEXTURE3D_ENABLE( pDocument ); break;
//	//case FUDaePassState::TEXTURECUBE_ENABLE: pRenderState = new ICRenderState__TEXTURECUBE_ENABLE( pDocument ); break;
//	//case FUDaePassState::TEXTURERECT_ENABLE: pRenderState = new ICRenderState__TEXTURERECT_ENABLE( pDocument ); break;
//	//case FUDaePassState::TEXTUREDEPTH_ENABLE: pRenderState = new ICRenderState__TEXTUREDEPTH_ENABLE( pDocument ); break;
//	//case FUDaePassState::TEXTURE_ENV_COLOR: pRenderState = new ICRenderState__TEXTURE_ENV_COLOR( pDocument ); break;
//	//case FUDaePassState::TEXTURE_ENV_MODE: pRenderState = new ICRenderState__TEXTURE_ENV_MODE( pDocument ); break;
//	//	
//	//case FUDaePassState::CLIP_PLANE: pRenderState = new ICRenderState__CLIP_PLANE pDocument ); break;
//	//case FUDaePassState::CLIP_PLANE_ENABLE: pRenderState = new ICRenderState__CLIP_PLANE_ENABLE( pDocument ); break;
//	//case FUDaePassState::BLEND_COLOR: pRenderState = new ICRenderState__BLEND_COLOR( pDocument ); break;
//	//case FUDaePassState::CLEAR_COLOR: pRenderState = new ICRenderState__CLEAR_COLOR( pDocument ); break;
//	//case FUDaePassState::CLEAR_STENCIL: pRenderState = new ICRenderState__CLEAR_STENCIL( pDocument ); break;
//	//case FUDaePassState::CLEAR_DEPTH: pRenderState = new ICRenderState__CLEAR_DEPTH( pDocument ); break;
//	//case FUDaePassState::COLOR_MASK: pRenderState = new ICRenderState__COLOR_MASK( pDocument ); break;
//	//case FUDaePassState::DEPTH_BOUNDS: pRenderState = new ICRenderState__DEPTH_BOUNDS( pDocument ); break;
//	//case FUDaePassState::DEPTH_MASK: pRenderState = new ICRenderState__DEPTH_MASK( pDocument ); break;
//	//case FUDaePassState::DEPTH_RANGE: pRenderState = new ICRenderState__DEPTH_RANGE( pDocument ); break;
//	//	
//	//case FUDaePassState::FOG_DENSITY: pRenderState = new ICRenderState__FOG_DENSITY( pDocument ); break;
//	//case FUDaePassState::FOG_START: pRenderState = new ICRenderState__FOG_START( pDocument ); break;
//	//case FUDaePassState::FOG_END: pRenderState = new ICRenderState__FOG_END( pDocument ); break;
//	//case FUDaePassState::FOG_COLOR: pRenderState = new ICRenderState__FOG_COLOR( pDocument ); break;
//	//case FUDaePassState::LIGHT_MODEL_AMBIENT: pRenderState = new ICRenderState__LIGHT_MODEL_AMBIENT( pDocument ); break;
//	//case FUDaePassState::LIGHTING_ENABLE: pRenderState = new ICRenderState__LIGHTING_ENABLE( pDocument ); break;
//	//case FUDaePassState::LINE_STIPPLE: pRenderState = new ICRenderState__LINE_STIPPLE( pDocument ); break;
//	//case FUDaePassState::LINE_WIDTH: pRenderState = new ICRenderState__LINE_WIDTH( pDocument ); break;
//	//	
//	//case FUDaePassState::MATERIAL_AMBIENT: pRenderState = new ICRenderState__MATERIAL_AMBIENT( pDocument ); break;
//	//case FUDaePassState::MATERIAL_DIFFUSE: pRenderState = new ICRenderState__MATERIAL_DIFFUSE( pDocument ); break;
//	//case FUDaePassState::MATERIAL_EMISSION: pRenderState = new ICRenderState__MATERIAL_EMISSION( pDocument ); break;
//	//case FUDaePassState::MATERIAL_SHININESS: pRenderState = new ICRenderState__MATERIAL_SHININESS( pDocument ); break;
//	//case FUDaePassState::MATERIAL_SPECULAR: pRenderState = new ICRenderState__MATERIAL_SPECULAR( pDocument ); break;
//	//case FUDaePassState::MODEL_VIEW_MATRIX: pRenderState = new ICRenderState__MODEL_VIEW_MATRIX( pDocument ); break;
//	//case FUDaePassState::POINT_DISTANCE_ATTENUATION: pRenderState = new ICRenderState__POINT_DISTANCE_ATTENUATION( pDocument ); break;
//	//case FUDaePassState::POINT_FADE_THRESHOLD_SIZE: pRenderState = new ICRenderState__POINT_FADE_THRESHOLD_SIZE( pDocument ); break;
//	//case FUDaePassState::POINT_SIZE: pRenderState = new ICRenderState__POINT_SIZE( pDocument ); break;
//	//case FUDaePassState::POINT_SIZE_MIN: pRenderState = new ICRenderState__POINT_SIZE_MIN( pDocument ); break;
//	//case FUDaePassState::POINT_SIZE_MAX: pRenderState = new ICRenderState__POINT_SIZE_MAX( pDocument ); break;
//	//	
//	//case FUDaePassState::POLYGON_OFFSET: pRenderState = new ICRenderState__POLYGON_OFFSET( pDocument ); break;
//	//case FUDaePassState::PROJECTION_MATRIX: pRenderState = new ICRenderState__PROJECTION_MATRIX( pDocument ); break;
//	//case FUDaePassState::SCISSOR: pRenderState = new ICRenderState__SCISSOR( pDocument ); break;
//	//case FUDaePassState::STENCIL_MASK: pRenderState = new ICRenderState__STENCIL_MASK( pDocument ); break;
//	//case FUDaePassState::ALPHA_TEST_ENABLE: pRenderState = new ICRenderState__ALPHA_TEST_ENABLE( pDocument ); break;
//	//case FUDaePassState::AUTO_NORMAL_ENABLE: pRenderState = new ICRenderState__AUTO_NORMAL_ENABLE( pDocument ); break;
//	//case FUDaePassState::BLEND_ENABLE: pRenderState = new ICRenderState__BLEND_ENABLE( pDocument ); break;
//	//case FUDaePassState::COLOR_LOGIC_OP_ENABLE: pRenderState = new ICRenderState__COLOR_LOGIC_OP_ENABLE( pDocument ); break;
//	//case FUDaePassState::COLOR_MATERIAL_ENABLE: pRenderState = new ICRenderState__COLOR_MATERIAL_ENABLE( pDocument ); break;
//	//case FUDaePassState::CULL_FACE_ENABLE: pRenderState = new ICRenderState__CULL_FACE_ENABLE( pDocument ); break;
//	//	
//	//case FUDaePassState::DEPTH_BOUNDS_ENABLE: pRenderState = new ICRenderState__DEPTH_BOUNDS_ENABLE( pDocument ); break;
//	//case FUDaePassState::DEPTH_CLAMP_ENABLE: pRenderState = new ICRenderState__DEPTH_CLAMP_ENABLE( pDocument ); break;
//	//case FUDaePassState::DEPTH_TEST_ENABLE: pRenderState = new ICRenderState__DEPTH_TEST_ENABLE( pDocument ); break;
//	//case FUDaePassState::DITHER_ENABLE: pRenderState = new ICRenderState__DITHER_ENABLE( pDocument ); break;
//	//case FUDaePassState::FOG_ENABLE: pRenderState = new ICRenderState__FOG_ENABLE( pDocument ); break;
//	//case FUDaePassState::LIGHT_MODEL_LOCAL_VIEWER_ENABLE: pRenderState = new ICRenderState__LIGHT_MODEL_LOCAL_VIEWER_ENABLE( pDocument ); break;
//	//case FUDaePassState::LIGHT_MODEL_TWO_SIDE_ENABLE: pRenderState = new ICRenderState__LIGHT_MODEL_TWO_SIDE_ENABLE( pDocument ); break;
//	//case FUDaePassState::LINE_SMOOTH_ENABLE: pRenderState = new ICRenderState__LINE_SMOOTH_ENABLE( pDocument ); break;
//	//case FUDaePassState::LINE_STIPPLE_ENABLE: pRenderState = new ICRenderState__LINE_STIPPLE_ENABLE( pDocument ); break;
//	//case FUDaePassState::LOGIC_OP_ENABLE: pRenderState = new ICRenderState__LOGIC_OP_ENABLE( pDocument ); break;
//	//case FUDaePassState::MULTISAMPLE_ENABLE: pRenderState = new ICRenderState__MULTISAMPLE_ENABLE( pDocument ); break;
//	//	
//	//case FUDaePassState::NORMALIZE_ENABLE: pRenderState = new ICRenderState__NORMALIZE_ENABLE( pDocument ); break;
//	//case FUDaePassState::POINT_SMOOTH_ENABLE: pRenderState = new ICRenderState__POINT_SMOOTH_ENABLE( pDocument ); break;
//	//case FUDaePassState::POLYGON_OFFSET_FILL_ENABLE: pRenderState = new ICRenderState__POLYGON_OFFSET_FILL_ENABLE( pDocument ); break;
//	//case FUDaePassState::POLYGON_OFFSET_LINE_ENABLE: pRenderState = new ICRenderState__POLYGON_OFFSET_LINE_ENABLE( pDocument ); break;
//	//case FUDaePassState::POLYGON_OFFSET_POINT_ENABLE: pRenderState = new ICRenderState__POLYGON_OFFSET_POINT_ENABLE( pDocument ); break;
//	//case FUDaePassState::POLYGON_SMOOTH_ENABLE: pRenderState = new ICRenderState__POLYGON_SMOOTH_ENABLE( pDocument ); break;
//	//case FUDaePassState::POLYGON_STIPPLE_ENABLE: pRenderState = new ICRenderState__POLYGON_STIPPLE_ENABLE( pDocument ); break;
//	//case FUDaePassState::RESCALE_NORMAL_ENABLE: pRenderState = new ICRenderState__RESCALE_NORMAL_ENABLE( pDocument ); break;
//	//	
//	//case FUDaePassState::SAMPLE_ALPHA_TO_COVERAGE_ENABLE: pRenderState = new ICRenderState__SAMPLE_ALPHA_TO_COVERAGE_ENABLE( pDocument ); break;
//	//case FUDaePassState::SAMPLE_ALPHA_TO_ONE_ENABLE: pRenderState = new ICRenderState__SAMPLE_ALPHA_TO_ONE_ENABLE( pDocument ); break;
//	//case FUDaePassState::SAMPLE_COVERAGE_ENABLE: pRenderState = new ICRenderState__SAMPLE_COVERAGE_ENABLE( pDocument ); break;
//	//case FUDaePassState::SCISSOR_TEST_ENABLE: pRenderState = new ICRenderState__SCISSOR_TEST_ENABLE( pDocument ); break;
//	//case FUDaePassState::STENCIL_TEST_ENABLE: pRenderState = new ICRenderState__STENCIL_TEST_ENABLE( pDocument ); break;
//
//	case FUDaePassState::COUNT:
//	case FUDaePassState::INVALID:
//	default:
//		{
//			FUFail(break);
//		}
//
//	}
//
//	return pRenderState;
//}