/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#include "StdAfx.h"
#include "FCDocument/FCDEffectPassStateParameter.h"
//#include "FCDocument/FCDObject.h"

ImplementObjectType(FCDEffectPassStateParameter);

FCDEffectPassStateParameter::FCDEffectPassStateParameter( Type type, FCDocument *document )
	: FCDObject( document )
	//, InitializeParameterAnimatable( m_value )
	, InitializeParameter( m_pExportBehavior, NULL )
	, InitializeParameter( m_type, type )
{}

FCDEffectPassStateParameter *FCDEffectPassStateParameter::Clone( FCDEffectPassStateParameter *clone ) const
{
	FCDEffectPassStateParameter *pClone = clone ? clone : FCDEffectPassStateParameter::Create( m_type, const_cast<FCDocument *>( GetDocument() ));
	//CLONE_PARAMETER( pClone, m_value );
	CLONE_PARAMETER( pClone, m_pExportBehavior );
	CLONE_PARAMETER( pClone, m_type );
	return pClone;
}

FCDEffectPassStateParameter *FCDEffectPassStateParameter::Create( Type type, FCDocument *pDocument )
{
#define CASE( __case ) \
case __case: \
{ \
	pParameter = new FCDEffectPassStateParameter__##__case( pDocument ); \
	break; \
}

	FCDEffectPassStateParameter *pParameter = NULL;

	switch ( type )
	{
		CASE( FLOAT )
		CASE( FLOAT4 )
	}

	return pParameter;

#undef CASE
}

// ----------------------------------------------------------------------
// FLOAT
// ----------------------------------------------------------------------
ImplementObjectType(FCDEffectPassStateParameter__FLOAT);

FCDEffectPassStateParameter__FLOAT::FCDEffectPassStateParameter__FLOAT( FCDocument *document )
	: FCDEffectPassStateParameter( FLOAT, document )
	, InitializeParameterAnimatable( m_value )
{}

FCDEffectPassStateParameter__FLOAT *FCDEffectPassStateParameter__FLOAT::Clone( FCDEffectPassStateParameter__FLOAT *clone ) const
{
	FCDEffectPassStateParameter__FLOAT *pClone = clone ? clone : new FCDEffectPassStateParameter__FLOAT( const_cast<FCDocument *>( GetDocument() ));
	pClone = ( FCDEffectPassStateParameter__FLOAT * )FCDEffectPassStateParameter::Clone( pClone );
	CLONE_PARAMETER( pClone, m_value );
	return pClone;
}

// ----------------------------------------------------------------------
// FLOAT4
// ----------------------------------------------------------------------
ImplementObjectType(FCDEffectPassStateParameter__FLOAT4);

FCDEffectPassStateParameter__FLOAT4::FCDEffectPassStateParameter__FLOAT4( FCDocument *document )
	: FCDEffectPassStateParameter( FLOAT4, document )
	, InitializeParameterAnimatable( m_value )
{}

FCDEffectPassStateParameter__FLOAT4 *FCDEffectPassStateParameter__FLOAT4::Clone( FCDEffectPassStateParameter__FLOAT4 *clone ) const
{
	FCDEffectPassStateParameter__FLOAT4 *pClone = clone ? clone : new FCDEffectPassStateParameter__FLOAT4( const_cast<FCDocument *>( GetDocument() ));
	pClone = ( FCDEffectPassStateParameter__FLOAT4 * )FCDEffectPassStateParameter::Clone( pClone );
	CLONE_PARAMETER( pClone, m_value );
	return pClone;
}

//ImplementObjectType(FCDEffectPassState__ALPHA_FUNC);
//#include "FUtils/FUDaeSyntax.h"

//#define SET_VALUE( offset, valueType, actualValue ) *((valueType*)(data + offset)) = actualValue;
//#define SET_ENUM( offset, nameSpace, actualValue ) *((uint32*)(data + offset)) = nameSpace::actualValue;
//#define GET_VALUE( offset, valueType )( *( valueType * )( data + offset ) )
//
////void FCDEffectPassState::SetDefaultValue()
////{
////	switch ((uint32) type)
////	{
////	case FUDaePassState::ALPHA_FUNC:
////		//SetAlphaFunc( FUDaePassStateFunction::ALWAYS, 0 );
////		break;
////
////	case FUDaePassState::BLEND_FUNC:
////		//SetBlendFuncs( FUDaePassStateBlendType::ONE, FUDaePassStateBlendType::ZERO );
////		break;
////
////	case FUDaePassState::BLEND_FUNC_SEPARATE:
////		//SetBlendFuncsSeparate( FUDaePassStateBlendType::ONE, FUDaePassStateBlendType::ZERO, FUDaePassStateBlendType::ONE, FUDaePassStateBlendType::ZERO );
////		break;
////
////	case FUDaePassState::BLEND_EQUATION:
////		//SetBlendEquation( FUDaePassStateBlendEquation::ADD );
////		break;
////
////	case FUDaePassState::BLEND_EQUATION_SEPARATE:
////		//SetBlendEquationSeparate( FUDaePassStateBlendEquation::ADD, FUDaePassStateBlendEquation::ADD );
////		SET_ENUM(0, FUDaePassStateBlendEquation, ADD);
////		SET_ENUM(4, FUDaePassStateBlendEquation, ADD);
////		break;
////
////	case FUDaePassState::COLOR_MATERIAL:
////		//SetColorMaterial( FUDaePassStateFaceType::FRONT_AND_BACK, FUDaePassStateMaterialType::AMBIENT_AND_DIFFUSE );
////		SET_ENUM(0, FUDaePassStateFaceType, FRONT_AND_BACK);
////		SET_ENUM(4, FUDaePassStateMaterialType, AMBIENT_AND_DIFFUSE);
////		break;
////
////	case FUDaePassState::CULL_FACE:
////		//SetCullFace( FUDaePassStateFaceType::BACK );
////		SET_ENUM(0, FUDaePassStateFaceType, BACK);
////		break;
////
////	case FUDaePassState::DEPTH_FUNC:
////		//SetDepthFunc( FUDaePassStateFunction::ALWAYS );
////		SET_ENUM(0, FUDaePassStateFunction, ALWAYS);
////		break;
////
////	case FUDaePassState::FOG_MODE:
////		//SetFogMode( FUDaePassStateFogType::EXP );
////		SET_ENUM(0, FUDaePassStateFogType, EXP);
////		break;
////
////	case FUDaePassState::FOG_COORD_SRC:
////		//SetFogCoordSrc( FUDaePassStateFogCoordinateType::FOG_COORDINATE );
////		SET_ENUM(0, FUDaePassStateFogCoordinateType, FOG_COORDINATE);
////		break;
////
////	case FUDaePassState::FRONT_FACE:
////		//SetFrontFace( FUDaePassStateFrontFaceType::COUNTER_CLOCKWISE );
////		SET_ENUM(0, FUDaePassStateFrontFaceType, COUNTER_CLOCKWISE);
////		break;
////
////	case FUDaePassState::LIGHT_MODEL_COLOR_CONTROL:
////		//SetLightModelColorControl( FUDaePassStateLightModelColorControlType::SINGLE_COLOR );
////		SET_ENUM(0, FUDaePassStateLightModelColorControlType, SINGLE_COLOR);
////		break;
////
////	case FUDaePassState::LOGIC_OP:
////		//SetLogicOperation( FUDaePassStateLogicOperation::COPY );
////		SET_ENUM(0, FUDaePassStateLogicOperation, COPY);
////		break;
////
////	case FUDaePassState::POLYGON_MODE:
////		//SetPolygonMode( FUDaePassStateFaceType::FRONT_AND_BACK, FUDaePassStatePolygonMode::FILL );
////		SET_ENUM(0, FUDaePassStateFaceType, FRONT_AND_BACK);
////		SET_ENUM(4, FUDaePassStatePolygonMode, FILL);
////		break;
////
////	case FUDaePassState::SHADE_MODEL:
////		//SetShadeModel( FUDaePassStateLogicOperation::COPY );
////		break;
////
////	case FUDaePassState::STENCIL_FUNC:
////		//SetStencilFunc( FUDaePassStateLogicOperation::COPY );
//
////		break;
////
////	case FUDaePassState::STENCIL_OP:
////		//SetStencilOperation( FUDaePassStateLogicOperation::COPY );
//
////		break;
////
////	case FUDaePassState::STENCIL_FUNC_SEPARATE:
////		//SetStencilFuncSeparate( FUDaePassStateLogicOperation::COPY );
////		break;
////
////	case FUDaePassState::STENCIL_OP_SEPARATE:
////		//SetStencilOperationSeparate( FUDaePassStateLogicOperation::COPY );
//
////		break;
////
////	case FUDaePassState::STENCIL_MASK_SEPARATE:
////		//SetLogicOperation( FUDaePassStateLogicOperation::COPY );
////		break;
////
////	case FUDaePassState::LIGHT_ENABLE:
////		//SetLightEnable( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, bool, false);
////		break;
////
////	case FUDaePassState::LIGHT_AMBIENT:
////		//SetLightAmbient( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, FMVector4, FMVector4(0,0,0,1));
////		break;
////
////	case FUDaePassState::LIGHT_DIFFUSE:
////	case FUDaePassState::LIGHT_SPECULAR:
////	case FUDaePassState::TEXTURE_ENV_COLOR:
////	case FUDaePassState::CLIP_PLANE:
////		//SetLightDiffuse( FUDaePassStateLogicOperation::COPY );
////		//SetLightSpecular( FUDaePassStateLogicOperation::COPY );
////		//SetTextureEnvColor( FUDaePassStateLogicOperation::COPY );
////		//SetClipPlane( FUDaePassStateLogicOperation::COPY );
////		break;
////
////	case FUDaePassState::LIGHT_POSITION:
////		//SetLightPosition( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, FMVector4, FMVector4(0,0,1,0));
////		break;
////
////	case FUDaePassState::LIGHT_CONSTANT_ATTENUATION:
////		//SetLightConstantAttenuation( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, float, 1.0f);
////		break;
////
////	case FUDaePassState::LIGHT_LINEAR_ATTENUATION:
////		//SetLightLinearAttenuation( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, float, 0.0f);
////		break;
////
////	case FUDaePassState::LIGHT_QUADRATIC_ATTENUATION:
////		//SetLightQuadraticAttenuation( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, float, 0.0f);
////		break;
////
////	case FUDaePassState::LIGHT_SPOT_CUTOFF:
////		//SetLightSpotCutoff( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, float, 180.0f);
////		break;
////
////	case FUDaePassState::LIGHT_SPOT_DIRECTION:
////		//SetLightSpotDirection( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, FMVector3, FMVector3(0,0,-1));
////		break;
////
////	case FUDaePassState::LIGHT_SPOT_EXPONENT:
////		//SetLightSpotExponent( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, float, 0.0f);
////		break;
////
////	case FUDaePassState::TEXTURE1D:
////	case FUDaePassState::TEXTURE2D:
////	case FUDaePassState::TEXTURE3D:
////	case FUDaePassState::TEXTURECUBE:
////	case FUDaePassState::TEXTURERECT:
////	case FUDaePassState::TEXTUREDEPTH:
////		//SetTexture1D( FUDaePassStateLogicOperation::COPY );
////		//SetTexture2D( FUDaePassStateLogicOperation::COPY );
////		//SetTexture3D( FUDaePassStateLogicOperation::COPY );
////		//SetTextureCube( FUDaePassStateLogicOperation::COPY );
////		//SetTextureRect( FUDaePassStateLogicOperation::COPY );
////		//SetTextureDepth( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, uint32, 0);
////		break;
////
////	case FUDaePassState::TEXTURE1D_ENABLE:
////	case FUDaePassState::TEXTURE2D_ENABLE:
////	case FUDaePassState::TEXTURE3D_ENABLE:
////	case FUDaePassState::TEXTURECUBE_ENABLE:
////	case FUDaePassState::TEXTURERECT_ENABLE:
////	case FUDaePassState::TEXTUREDEPTH_ENABLE:
////	case FUDaePassState::CLIP_PLANE_ENABLE:
////		//SetTexture1DEnable( FUDaePassStateLogicOperation::COPY );
////		//SetTexture2DEnable( FUDaePassStateLogicOperation::COPY );
////		//SetTexture3DEnable( FUDaePassStateLogicOperation::COPY );
////		//SetTextureCubeEnable( FUDaePassStateLogicOperation::COPY );
////		//SetTextureRectEnable( FUDaePassStateLogicOperation::COPY );
////		//SetTextureDepthEnable( FUDaePassStateLogicOperation::COPY );
////		//SetClipPlaneEnable( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, bool, false);
////		break;
////
////	case FUDaePassState::TEXTURE_ENV_MODE:
////		//SetTextureEnvMode( FUDaePassStateLogicOperation::COPY );
////		memset(data, 0, dataSize);
////		break;
////
////	case FUDaePassState::BLEND_COLOR:
////	case FUDaePassState::CLEAR_COLOR:
////	case FUDaePassState::FOG_COLOR:
////	case FUDaePassState::SCISSOR:
////		//SetBlendColor( FUDaePassStateLogicOperation::COPY );
////		//SetClearColor( FUDaePassStateLogicOperation::COPY );
////		//SetFogColor( FUDaePassStateLogicOperation::COPY );
////		//SetScissor( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, FMVector4, FMVector4::Zero);
////		break;
////
////	case FUDaePassState::LIGHT_MODEL_AMBIENT:
////	case FUDaePassState::MATERIAL_AMBIENT:
////		//SetLightModelAmbient( FUDaePassStateLogicOperation::COPY );
////		//SetMaterialAmbient( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, FMVector4, FMVector4(0.2f,0.2f,0.2f,1.0f));
////		break;
////
////	case FUDaePassState::MATERIAL_DIFFUSE:
////		//SetMaterialDiffuse( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, FMVector4, FMVector4(0.8f,0.8f,0.8f,1.0f));
////		break;
////
////	case FUDaePassState::MATERIAL_EMISSION:
////	case FUDaePassState::MATERIAL_SPECULAR:
////		//SetMaterialEmission( FUDaePassStateLogicOperation::COPY );
////		//SetMaterialSpecular( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, FMVector4, FMVector4(0,0,0,1));
////		break;
////
////	case FUDaePassState::POINT_DISTANCE_ATTENUATION:
////		//SetPointDistanceAttenuation( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, FMVector3, FMVector3(1,0,0));
////		break;
////
////	case FUDaePassState::DEPTH_BOUNDS:
////	case FUDaePassState::DEPTH_RANGE:
////		//SetDepthBounds( FUDaePassStateLogicOperation::COPY );
////		//SetDepthRange( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, FMVector2, FMVector2(0,1));
////		break;
////
////	case FUDaePassState::POLYGON_OFFSET:
////		//SetPolygonOffset( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, FMVector2, FMVector2(0,0));
////		break;
////
////	case FUDaePassState::DEPTH_MASK:
////		//SetDepthMask( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, bool, true);
////		break;
////
////	case FUDaePassState::CLEAR_STENCIL:
////		//SetClearStencil( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint32, 0);
////		break;
////
////	case FUDaePassState::STENCIL_MASK:
////		//SetStencilMask( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint32, 0xFFFFFFFF);
////		break;
////
////	case FUDaePassState::CLEAR_DEPTH:
////	case FUDaePassState::FOG_DENSITY:
////	case FUDaePassState::FOG_END:
////	case FUDaePassState::LINE_WIDTH:
////	case FUDaePassState::POINT_FADE_THRESHOLD_SIZE:
////	case FUDaePassState::POINT_SIZE:
////	case FUDaePassState::POINT_SIZE_MAX:
////		//SetClearDepth( FUDaePassStateLogicOperation::COPY );
////		//SetFogDensity( FUDaePassStateLogicOperation::COPY );
////		//SetFogEnd( FUDaePassStateLogicOperation::COPY );
////		//SetLineWidth( FUDaePassStateLogicOperation::COPY );
////		//SetPointFadeThresholdSize( FUDaePassStateLogicOperation::COPY );
////		//SetPointSize( FUDaePassStateLogicOperation::COPY );
////		//SetPointSizeMax( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, float, 1.0f);
////		break;
////
////	case FUDaePassState::FOG_START:
////	case FUDaePassState::MATERIAL_SHININESS:
////	case FUDaePassState::POINT_SIZE_MIN:
////		//SetFogStart( FUDaePassStateLogicOperation::COPY );
////		//SetMaterialShininess( FUDaePassStateLogicOperation::COPY );
////		//SetPointSizeMin( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, float, 0.0f);
////		break;
////
////	case FUDaePassState::COLOR_MASK:
////		//SetColorMask( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, bool, true);
////		SET_VALUE(1, bool, true);
////		SET_VALUE(2, bool, true);
////		SET_VALUE(3, bool, true);
////		break;
////
////	case FUDaePassState::LINE_STIPPLE:
////		//SetLineStipple( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, uint16, 1);
////		SET_VALUE(2, uint16, 0xFF);
////		break;
////
////	case FUDaePassState::MODEL_VIEW_MATRIX:
////	case FUDaePassState::PROJECTION_MATRIX:
////		//SetModelViewMatrix( FUDaePassStateLogicOperation::COPY );
////		//SetProjectionMatrix( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, FMMatrix44, FMMatrix44::Identity);
////		break;
////
////	case FUDaePassState::LIGHTING_ENABLE:
////	case FUDaePassState::ALPHA_TEST_ENABLE:
////	case FUDaePassState::AUTO_NORMAL_ENABLE:
////	case FUDaePassState::BLEND_ENABLE:
////	case FUDaePassState::COLOR_LOGIC_OP_ENABLE:
////	case FUDaePassState::CULL_FACE_ENABLE:
////	case FUDaePassState::DEPTH_BOUNDS_ENABLE:
////	case FUDaePassState::DEPTH_CLAMP_ENABLE:
////	case FUDaePassState::DEPTH_TEST_ENABLE:
////	case FUDaePassState::DITHER_ENABLE:
////	case FUDaePassState::FOG_ENABLE:
////	case FUDaePassState::LIGHT_MODEL_LOCAL_VIEWER_ENABLE:
////	case FUDaePassState::LIGHT_MODEL_TWO_SIDE_ENABLE:
////	case FUDaePassState::LINE_SMOOTH_ENABLE:
////	case FUDaePassState::LINE_STIPPLE_ENABLE:
////	case FUDaePassState::LOGIC_OP_ENABLE:
////	case FUDaePassState::MULTISAMPLE_ENABLE:
////	case FUDaePassState::NORMALIZE_ENABLE:
////	case FUDaePassState::POINT_SMOOTH_ENABLE:
////	case FUDaePassState::POLYGON_OFFSET_FILL_ENABLE:
////	case FUDaePassState::POLYGON_OFFSET_LINE_ENABLE:
////	case FUDaePassState::POLYGON_OFFSET_POINT_ENABLE:
////	case FUDaePassState::POLYGON_SMOOTH_ENABLE:
////	case FUDaePassState::POLYGON_STIPPLE_ENABLE:
////	case FUDaePassState::RESCALE_NORMAL_ENABLE:
////	case FUDaePassState::SAMPLE_ALPHA_TO_COVERAGE_ENABLE:
////	case FUDaePassState::SAMPLE_ALPHA_TO_ONE_ENABLE:
////	case FUDaePassState::SAMPLE_COVERAGE_ENABLE:
////	case FUDaePassState::SCISSOR_TEST_ENABLE:
////	case FUDaePassState::STENCIL_TEST_ENABLE:
////		//SetLightingEnable( FUDaePassStateLogicOperation::COPY );
////		//SetAlphaTestEnable( FUDaePassStateLogicOperation::COPY );
////		//SetAutoNormalEnable( FUDaePassStateLogicOperation::COPY );
////		//SetBlendEnable( FUDaePassStateLogicOperation::COPY );
////		//SetColorLogicOpEnable( FUDaePassStateLogicOperation::COPY );
////		//SetCullFaceEnable( FUDaePassStateLogicOperation::COPY );
////		//SetDepthBoundsEnable( FUDaePassStateLogicOperation::COPY );
////		//SetDepthClampEnable( FUDaePassStateLogicOperation::COPY );
////		//SetDepthTestEnable( FUDaePassStateLogicOperation::COPY );
////		//SetDitherEnable( FUDaePassStateLogicOperation::COPY );
////		//SetFogEnable( FUDaePassStateLogicOperation::COPY );
////		//SetLightModelLocalViewerEnable( FUDaePassStateLogicOperation::COPY );
////		//SetLightModelTwoSizeEnable( FUDaePassStateLogicOperation::COPY );
////		//SetLineSmoothEnable( FUDaePassStateLogicOperation::COPY );
////		//SetLineStippleEnable( FUDaePassStateLogicOperation::COPY );
////		//SetLogicOperationEnable( FUDaePassStateLogicOperation::COPY );
////		//SetMultisampleEnable( FUDaePassStateLogicOperation::COPY );
////		//SetNormalizeEnable( FUDaePassStateLogicOperation::COPY );
////		//SetPointSmoothEnable( FUDaePassStateLogicOperation::COPY );
////		//SetPolygonOffsetFillEnable( FUDaePassStateLogicOperation::COPY );
////		//SetPolygonOffsetLineEnable( FUDaePassStateLogicOperation::COPY );
////		//SetPolygonOffsetPointEnable( FUDaePassStateLogicOperation::COPY );
////		//SetPolygonSmoothEnable( FUDaePassStateLogicOperation::COPY );
////		//SetPolygonStippleEnable( FUDaePassStateLogicOperation::COPY );
////		//SetRescaleNormalEnable( FUDaePassStateLogicOperation::COPY );
////		//SetSampleAlphaToCoverageEnable( FUDaePassStateLogicOperation::COPY );
////		//SetSampleAlphaToOneEnable( FUDaePassStateLogicOperation::COPY );
////		//SetSampleCoverageEnable( FUDaePassStateLogicOperation::COPY );
////		//SetScissorTestEnable( FUDaePassStateLogicOperation::COPY );
////		//SetStencilTestEnable( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, bool, false);
////		break;
////
////	case FUDaePassState::COLOR_MATERIAL_ENABLE:
////		//SetColorMaterialEnable( FUDaePassStateLogicOperation::COPY );
////		SET_VALUE(0, bool, true);
////		break;
////
////	case FUDaePassState::COUNT:
////	case FUDaePassState::INVALID:
////	default:
////		FUFail(break);
////	}
////}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__ALPHA_FUNC
//// ----------------------------------------------------------------------
//void FCDEffectPassState__ALPHA_FUNC::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_VALUE( 0, FUDaePassStateFunction::Function, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__ALPHA_FUNC::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__ALPHA_FUNC::SetDefaultValue()
//{
//	Set( FUDaePassStateFunction::ALWAYS, 0 );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_FUNC
//// ----------------------------------------------------------------------
//void FCDEffectPassState__BLEND_FUNC::Set( FUDaePassStateBlendType::Type srcBlendFunc, FUDaePassStateBlendType::Type dstBlendFunc )
//{
//	SET_VALUE( 0, FUDaePassStateBlendType::Type, srcBlendFunc );
//	SET_VALUE( 4, FUDaePassStateBlendType::Type, dstBlendFunc );
//}
//
//void FCDEffectPassState__BLEND_FUNC::Get( FUDaePassStateBlendType::Type &srcBlendFunc, FUDaePassStateBlendType::Type &dstBlendFunc )
//{
//	srcBlendFunc = GET_VALUE( 0, FUDaePassStateBlendType::Type );
//	dstBlendFunc = GET_VALUE( 4, FUDaePassStateBlendType::Type );
//}
//
//void FCDEffectPassState__BLEND_FUNC::SetDefaultValue()
//{
//	Set( FUDaePassStateBlendType::ONE, FUDaePassStateBlendType::ZERO );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_FUNC_SEPARATE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__BLEND_FUNC_SEPARATE::Set( FUDaePassStateBlendType::Type srcColorBlendFunc, FUDaePassStateBlendType::Type dstColorBlendFunc, FUDaePassStateBlendType::Type srcAlphaBlendFunc, FUDaePassStateBlendType::Type dstAlphaBlendFunc )
//{
//	SET_VALUE( 0, FUDaePassStateBlendType::Type, srcColorBlendFunc );
//	SET_VALUE( 4, FUDaePassStateBlendType::Type, dstColorBlendFunc );
//	SET_VALUE( 8, FUDaePassStateBlendType::Type, srcAlphaBlendFunc );
//	SET_VALUE( 12, FUDaePassStateBlendType::Type, dstAlphaBlendFunc );
//}
//
//void FCDEffectPassState__BLEND_FUNC_SEPARATE::Get( FUDaePassStateBlendType::Type &srcColorBlendFunc, FUDaePassStateBlendType::Type &dstColorBlendFunc, FUDaePassStateBlendType::Type &srcAlphaBlendFunc, FUDaePassStateBlendType::Type &dstAlphaBlendFunc )
//{
//	srcColorBlendFunc = GET_VALUE( 0, FUDaePassStateBlendType::Type );
//	dstColorBlendFunc = GET_VALUE( 4, FUDaePassStateBlendType::Type );
//	srcAlphaBlendFunc = GET_VALUE( 8, FUDaePassStateBlendType::Type );
//	dstAlphaBlendFunc = GET_VALUE( 12, FUDaePassStateBlendType::Type );
//}
//
//void FCDEffectPassState__BLEND_FUNC_SEPARATE::SetDefaultValue()
//{
//	Set( FUDaePassStateBlendType::ONE, FUDaePassStateBlendType::ZERO, FUDaePassStateBlendType::ONE, FUDaePassStateBlendType::ZERO );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_EQUATION
//// ----------------------------------------------------------------------
//void FCDEffectPassState__BLEND_EQUATION::Set( FUDaePassStateBlendEquation::Equation blendEquation )
//{
//	SET_VALUE( 0, FUDaePassStateBlendEquation::Equation, blendEquation );
//}
//
//void FCDEffectPassState__BLEND_EQUATION::Get( FUDaePassStateBlendEquation::Equation &blendEquation )
//{
//	blendEquation = GET_VALUE( 0, FUDaePassStateBlendEquation::Equation );
//}
//
//void FCDEffectPassState__BLEND_EQUATION::SetDefaultValue()
//{
//	Set( FUDaePassStateBlendEquation::ADD );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_EQUATION_SEPARATE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__BLEND_EQUATION_SEPARATE::Set( FUDaePassStateBlendEquation::Equation colorBlendEquation, FUDaePassStateBlendEquation::Equation alphaBlendEquation )
//{
//	SET_VALUE( 0, FUDaePassStateBlendEquation::Equation, colorBlendEquation );
//	SET_VALUE( 4, FUDaePassStateBlendEquation::Equation, alphaBlendEquation );
//}
//
//void FCDEffectPassState__BLEND_EQUATION_SEPARATE::Get( FUDaePassStateBlendEquation::Equation &colorBlendEquation, FUDaePassStateBlendEquation::Equation &alphaBlendEquation )
//{
//	colorBlendEquation = GET_VALUE( 0, FUDaePassStateBlendEquation::Equation );
//	alphaBlendEquation = GET_VALUE( 4, FUDaePassStateBlendEquation::Equation );
//}
//
//void FCDEffectPassState__BLEND_EQUATION_SEPARATE::SetDefaultValue()
//{
//	Set( FUDaePassStateBlendEquation::ADD, FUDaePassStateBlendEquation::ADD );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__COLOR_MATERIAL
//// ----------------------------------------------------------------------
//void FCDEffectPassState__COLOR_MATERIAL::Set( FUDaePassStateFaceType::Type faces, FUDaePassStateMaterialType::Type materialType )
//{
//	SET_VALUE( 0, FUDaePassStateFaceType::Type, faces );
//	SET_VALUE( 4, FUDaePassStateMaterialType::Type, materialType );
//}
//
//void FCDEffectPassState__COLOR_MATERIAL::Get( FUDaePassStateFaceType::Type &faces, FUDaePassStateMaterialType::Type &materialType )
//{
//	faces = GET_VALUE( 0, FUDaePassStateFaceType::Type );
//	materialType = GET_VALUE( 4, FUDaePassStateMaterialType::Type );
//}
//
//void FCDEffectPassState__COLOR_MATERIAL::SetDefaultValue()
//{
//	Set( FUDaePassStateFaceType::FRONT_AND_BACK, FUDaePassStateMaterialType::AMBIENT_AND_DIFFUSE );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CULL_FACE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__CULL_FACE::Set( FUDaePassStateFaceType::Type faceType )
//{
//	SET_VALUE( 0, FUDaePassStateFaceType::Type, faceType );
//}
//
//void FCDEffectPassState__CULL_FACE::Get( FUDaePassStateFaceType::Type &faceType )
//{
//	faceType = GET_VALUE( 0, FUDaePassStateFaceType::Type );
//}
//
//void FCDEffectPassState__CULL_FACE::SetDefaultValue()
//{
//	Set( FUDaePassStateFaceType::BACK );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_FUNC
//// ----------------------------------------------------------------------
//void FCDEffectPassState__DEPTH_FUNC::Set( FUDaePassStateFunction::Function depthFunc )
//{
//	SET_VALUE( 0, FUDaePassStateFunction::Function, depthFunc );
//}
//
//void FCDEffectPassState__DEPTH_FUNC::Get( FUDaePassStateFunction::Function &depthFunc )
//{
//	depthFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//}
//
//void FCDEffectPassState__DEPTH_FUNC::SetDefaultValue()
//{
//	Set( FUDaePassStateFunction::ALWAYS );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_MODE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__FOG_MODE::Set( FUDaePassStateFogType::Type fogMode )
//{
//	SET_VALUE( 0, FUDaePassStateFogType::Type, fogMode );
//}
//
//void FCDEffectPassState__FOG_MODE::Get( FUDaePassStateFogType::Type &fogMode )
//{
//	fogMode = GET_VALUE( 0, FUDaePassStateFogType::Type );
//}
//
//void FCDEffectPassState__FOG_MODE::SetDefaultValue()
//{
//	Set( FUDaePassStateFogType::EXP );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_COORD_SRC
//// ----------------------------------------------------------------------
//void FCDEffectPassState__FOG_COORD_SRC::Set( FUDaePassStateFogCoordinateType::Type fogCoordinateType )
//{
//	SET_VALUE( 0, FUDaePassStateFogCoordinateType::Type, fogCoordinateType );
//}
//
//void FCDEffectPassState__FOG_COORD_SRC::Get( FUDaePassStateFogCoordinateType::Type &fogCoordinateType )
//{
//	fogCoordinateType = GET_VALUE( 0, FUDaePassStateFogCoordinateType::Type );
//}
//
//void FCDEffectPassState__FOG_COORD_SRC::SetDefaultValue()
//{
//	Set( FUDaePassStateFogCoordinateType::FOG_COORDINATE );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FRONT_FACE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__FRONT_FACE::Set( FUDaePassStateFrontFaceType::Type frontFaceType )
//{
//	SET_VALUE( 0, FUDaePassStateFrontFaceType::Type, frontFaceType );
//}
//
//void FCDEffectPassState__FRONT_FACE::Get( FUDaePassStateFrontFaceType::Type &frontFaceType )
//{
//	frontFaceType = GET_VALUE( 0, FUDaePassStateFrontFaceType::Type );
//}
//
//void FCDEffectPassState__FRONT_FACE::SetDefaultValue()
//{
//	Set( FUDaePassStateFrontFaceType::COUNTER_CLOCKWISE );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_MODEL_COLOR_CONTROL
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_MODEL_COLOR_CONTROL::Set( FUDaePassStateLightModelColorControlType::Type colorControlType )
//{
//	SET_VALUE( 0, FUDaePassStateLightModelColorControlType::Type, colorControlType );
//}
//
//void FCDEffectPassState__LIGHT_MODEL_COLOR_CONTROL::Get( FUDaePassStateLightModelColorControlType::Type &colorControlType )
//{
//	colorControlType = GET_VALUE( 0, FUDaePassStateLightModelColorControlType::Type );
//}
//
//void FCDEffectPassState__LIGHT_MODEL_COLOR_CONTROL::SetDefaultValue()
//{
//	Set( FUDaePassStateLightModelColorControlType::SINGLE_COLOR );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LOGIC_OP
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LOGIC_OP::Set( FUDaePassStateLogicOperation::Operation operation )
//{
//	SET_VALUE( 0, FUDaePassStateLogicOperation::Operation, operation );
//}
//
//void FCDEffectPassState__LOGIC_OP::Get( FUDaePassStateLogicOperation::Operation &operation )
//{
//	operation = GET_VALUE( 0, FUDaePassStateLogicOperation::Operation );
//}
//
//void FCDEffectPassState__LOGIC_OP::SetDefaultValue()
//{
//	Set( FUDaePassStateLogicOperation::COPY );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_MODE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POLYGON_MODE::Set( FUDaePassStateFaceType::Type faceType, FUDaePassStatePolygonMode::Mode polygonMode )
//{
//	SET_VALUE( 0, FUDaePassStateFaceType::Type, faceType );
//	SET_VALUE( 4, FUDaePassStatePolygonMode::Mode, polygonMode );
//}
//
//void FCDEffectPassState__POLYGON_MODE::Get( FUDaePassStateFaceType::Type &faceType, FUDaePassStatePolygonMode::Mode &polygonMode )
//{
//	faceType = GET_VALUE( 0, FUDaePassStateFaceType::Type );
//	polygonMode = GET_VALUE( 4, FUDaePassStatePolygonMode::Mode );
//}
//
//void FCDEffectPassState__POLYGON_MODE::SetDefaultValue()
//{
//	Set( FUDaePassStateFaceType::FRONT_AND_BACK, FUDaePassStatePolygonMode::FILL );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SHADE_MODEL
//// ----------------------------------------------------------------------
//void FCDEffectPassState__SHADE_MODEL::Set( FUDaePassStateShadeModel::Model shadeModel )
//{
//	SET_VALUE( 0, FUDaePassStateShadeModel::Model, shadeModel );
//}
//
//void FCDEffectPassState__SHADE_MODEL::Get( FUDaePassStateShadeModel::Model &shadeModel )
//{
//	shadeModel = GET_VALUE( 0, FUDaePassStateShadeModel::Model );
//}
//
//void FCDEffectPassState__SHADE_MODEL::SetDefaultValue()
//{
//	Set( FUDaePassStateShadeModel::SMOOTH );
//}
//
//// TODO
///*
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_FUNC
//// ----------------------------------------------------------------------
//void FCDEffectPassState__STENCIL_FUNC::Set( FUDaePassStateFunction::Function stencilFunc, uint8 referenceValue, uint8 mask )
//{
//	SET_VALUE( 0, FUDaePassStateFunction::Function, stencilFunc );
//	SET_VALUE( 4, uint8, referenceValue );
//	SET_VALUE( 5, uint8, mask );
//}
//
//void FCDEffectPassState__STENCIL_FUNC::Get( FUDaePassStateFunction::Function &stencilFunc, uint8 &referenceValue, uint8 &mask )
//{
//	stencilFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	referenceValue = GET_VALUE( 4, uint8 );
//	mask = GET_VALUE( 5, uint8 );
//}
//
//void FCDEffectPassState__STENCIL_FUNC::SetDefaultValue()
//{
//	Set( FUDaePassStateFunction::ALWAYS, 0, 0xFF );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_OP
//// ----------------------------------------------------------------------
//void FCDEffectPassState__STENCIL_OP::Set( FUDaePassStateStencilOperation::Operation failOperation, FUDaePassStateStencilOperation::Operation depthFailOperation, FUDaePassStateStencilOperation::Operation depthPassOperation )
//{
//	SET_VALUE( 0, FUDaePassStateStencilOperation::Operation, failOperation );
//	SET_VALUE( 4, FUDaePassStateStencilOperation::Operation, depthFailOperation );
//	SET_VALUE( 8, FUDaePassStateStencilOperation::Operation, depthPassOperation );
//}
//
//void FCDEffectPassState__STENCIL_OP::Get( FUDaePassStateStencilOperation::Operation &failOperation, FUDaePassStateStencilOperation::Operation &depthFailOperation, FUDaePassStateStencilOperation::Operation &depthPassOperation )
//{
//	failOperation = GET_VALUE( 0, FUDaePassStateStencilOperation::Operation );
//	depthFailOperation = GET_VALUE( 4, FUDaePassStateStencilOperation::Operation );
//	depthPassOperation = GET_VALUE( 8, FUDaePassStateStencilOperation::Operation );
//}
//
//void FCDEffectPassState__STENCIL_OP::SetDefaultValue()
//{
//	Set( FUDaePassStateStencilOperation::KEEP, FUDaePassStateStencilOperation::KEEP, FUDaePassStateStencilOperation::KEEP );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_FUNC_SEPARATE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__STENCIL_FUNC_SEPARATE::Set( FUDaePassStateFunction::Function frontFacesFunction, FUDaePassStateFunction::Function backFacesFunction, uint8 referenceValue, uint8 mask )
//{
//	SET_VALUE( 0, FUDaePassStateFunction::Function, frontFacesFunction );
//	SET_VALUE( 4, FUDaePassStateFunction::Function, backFacesFunction );
//	SET_VALUE( 8, uint8, referenceValue );
//	SET_VALUE( 9, uint8, mask );
//}
//
//void FCDEffectPassState__STENCIL_FUNC_SEPARATE::Get( FUDaePassStateFunction::Function &frontFacesFunction, FUDaePassStateFunction::Function &backFacesFunction, uint8 &referenceValue, uint8 &mask )
//{
//	frontFacesFunction = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	backFacesFunction = GET_VALUE( 4, FUDaePassStateFunction::Function );
//	referenceValue = GET_VALUE( 8, uint8 );
//	mask = GET_VALUE( 9, uint8 );
//}
//
//void FCDEffectPassState__STENCIL_FUNC_SEPARATE::SetDefaultValue()
//{
//	Set( FUDaePassStateFunction::ALWAYS, FUDaePassStateFunction::ALWAYS, 0, 0xFF );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_OP_SEPARATE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__STENCIL_OP_SEPARATE::Set( FUDaePassStateFaceType::Type faceType, FUDaePassStateStencilOperation::Operation failOperation, FUDaePassStateStencilOperation::Operation depthFailOperation, FUDaePassStateStencilOperation::Operation depthPassOperation )
//{
//	SET_VALUE( 0, FUDaePassStateFaceType::Type, faceType );
//	SET_VALUE( 4, FUDaePassStateStencilOperation::Operation, failOperation );
//	SET_VALUE( 8, FUDaePassStateStencilOperation::Operation, depthFailOperation );
//	SET_VALUE( 12, FUDaePassStateStencilOperation::Operation, depthPassOperation );
//}
//
//void FCDEffectPassState__STENCIL_OP_SEPARATE::Get( FUDaePassStateFaceType::Type &faceType, FUDaePassStateStencilOperation::Operation &failOperation, FUDaePassStateStencilOperation::Operation &depthFailOperation, FUDaePassStateStencilOperation::Operation &depthPassOperation )
//{
//	faceType = GET_VALUE( 0, FUDaePassStateFaceType::Type );
//	failOperation = GET_VALUE( 4, FUDaePassStateStencilOperation::Operation );
//	depthFailOperation = GET_VALUE( 8, FUDaePassStateStencilOperation::Operation );
//	depthPassOperation = GET_VALUE( 12, FUDaePassStateStencilOperation::Operation );
//}
//
//void FCDEffectPassState__STENCIL_OP_SEPARATE::SetDefaultValue()
//{
//	Set( FUDaePassStateFaceType::FRONT_AND_BACK, FUDaePassStateStencilOperation::KEEP, FUDaePassStateStencilOperation::KEEP, FUDaePassStateStencilOperation::KEEP );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_MASK_SEPARATE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__STENCIL_MASK_SEPARATE::Set( FUDaePassStateFaceType::Type faceType, uint8 mask )
//{
//	SET_VALUE( 0, FUDaePassStateFaceType::Type, faceType );
//	SET_VALUE( 4, uint8, mask );
//}
//
//void FCDEffectPassState__STENCIL_MASK_SEPARATE::Get( FUDaePassStateFaceType::Type &faceType, uint8 &mask )
//{
//	faceType = GET_VALUE( 0, FUDaePassStateFaceType::Type );
//	mask = GET_VALUE( 4, uint8 );
//}
//
//void FCDEffectPassState__STENCIL_MASK_SEPARATE::SetDefaultValue()
//{
//	Set( FUDaePassStateFaceType::FRONT_AND_BACK, 0xFF );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_ENABLE::Set( uint8 lightIndex, bool enabled )
//{
//	SET_VALUE( 0, uint8, lightIndex );
//	SET_VALUE( 1, bool, enabled );
//}
//
//void FCDEffectPassState__LIGHT_ENABLE::Get( uint8 &lightIndex, bool &enabled )
//{
//	lightIndex = GET_VALUE( 0, uint8 );
//	enabled = GET_VALUE( 1, bool );
//}
//
//void FCDEffectPassState__LIGHT_ENABLE::SetDefaultValue()
//{
//	Set( 0, false );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_AMBIENT
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_AMBIENT::Set( uint8 lightIndex, const FMVector4 &color )
//{
//	SET_VALUE( 0, uint8, lightIndex );
//	SET_VALUE( 1, FMVector4, color  );
//}
//
//void FCDEffectPassState__LIGHT_AMBIENT::Get( uint8 &lightIndex, FMVector4 &color )
//{
//	lightIndex = GET_VALUE( 0, uint8 );
//	color = GET_VALUE( 1, FMVector4 );
//}
//
//void FCDEffectPassState__LIGHT_AMBIENT::SetDefaultValue()
//{
//	Set( 0, FMVector4( 0, 0, 0, 1 ) );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_DIFFUSE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_DIFFUSE::Set( uint8 lightIndex, const FMVector4 &color )
//{
//	SET_VALUE( 0, uint8, lightIndex );
//	SET_VALUE( 1, FMVector4, color  );
//}
//
//void FCDEffectPassState__LIGHT_DIFFUSE::Get( uint8 &lightIndex, FMVector4 &color )
//{
//	lightIndex = GET_VALUE( 0, uint8 );
//	color = GET_VALUE( 1, FMVector4 );
//}
//
//void FCDEffectPassState__LIGHT_DIFFUSE::SetDefaultValue()
//{
//	Set( 0, FMVector4::Zero );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_SPECULAR
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_SPECULAR::Set( uint8 lightIndex, const FMVector4 &color )
//{
//	SET_VALUE( 0, uint8, lightIndex );
//	SET_VALUE( 1, FMVector4, color  );
//}
//
//void FCDEffectPassState__LIGHT_SPECULAR::Get( uint8 &lightIndex, FMVector4 &color )
//{
//	lightIndex = GET_VALUE( 0, uint8 );
//	color = GET_VALUE( 1, FMVector4 );
//}
//
//void FCDEffectPassState__LIGHT_SPECULAR::SetDefaultValue()
//{
//	Set( 0, FMVector4::Zero );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_POSITION
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_POSITION::Set( uint8 lightIndex, const FMVector4 &pos )
//{
//	SET_VALUE( 0, uint8, lightIndex );
//	SET_VALUE( 1, FMVector4, pos  );
//}
//
//void FCDEffectPassState__LIGHT_POSITION::Get( uint8 &lightIndex, FMVector4 &pos )
//{
//	lightIndex = GET_VALUE( 0, uint8 );
//	pos = GET_VALUE( 1, FMVector4 );
//}
//
//void FCDEffectPassState__LIGHT_POSITION::SetDefaultValue()
//{
//	Set( 0, FMVector4(0,0,1,0) );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_CONSTANT_ATTENUATION
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_CONSTANT_ATTENUATION::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LIGHT_CONSTANT_ATTENUATION::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LIGHT_CONSTANT_ATTENUATION::SetDefaultValue()
//{
//	Set( 0, 1.0f );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, float, 1.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_LINEAR_ATTENUATION
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_LINEAR_ATTENUATION::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LIGHT_LINEAR_ATTENUATION::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LIGHT_LINEAR_ATTENUATION::SetDefaultValue()
//{
//	Set( 0, 0.0f );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, float, 0.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_QUADRATIC_ATTENUATION
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_QUADRATIC_ATTENUATION::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LIGHT_QUADRATIC_ATTENUATION::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LIGHT_QUADRATIC_ATTENUATION::SetDefaultValue()
//{
//	Set( 0, 0.0f );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, float, 0.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_SPOT_CUTOFF
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_SPOT_CUTOFF::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LIGHT_SPOT_CUTOFF::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LIGHT_SPOT_CUTOFF::SetDefaultValue()
//{
//	Set( 0, 180.0f );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, float, 180.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_SPOT_DIRECTION
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_SPOT_DIRECTION::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LIGHT_SPOT_DIRECTION::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LIGHT_SPOT_DIRECTION::SetDefaultValue()
//{
//	Set( 0, FMVector3(0,0,-1) );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, FMVector3, FMVector3(0,0,-1));
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_SPOT_EXPONENT
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_SPOT_EXPONENT::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LIGHT_SPOT_EXPONENT::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LIGHT_SPOT_EXPONENT::SetDefaultValue()
//{
//	Set( 0, 0.0f );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE1D
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURE1D::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURE1D::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURE1D::SetDefaultValue()
//{
//	Set( 0, 0 );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, uint32, 0);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE2D
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURE2D::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURE2D::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURE2D::SetDefaultValue()
//{
//	Set( 0, 0 );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, uint32, 0);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE3D
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURE3D::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURE3D::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURE3D::SetDefaultValue()
//{
//	Set( 0, 0 );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, uint32, 0);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURECUBE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURECUBE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURECUBE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURECUBE::SetDefaultValue()
//{
//	Set( 0, 0 );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, uint32, 0);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURERECT
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURERECT::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURERECT::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURERECT::SetDefaultValue()
//{
//	Set( 0, 0 );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, uint32, 0);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTUREDEPTH
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTUREDEPTH::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTUREDEPTH::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTUREDEPTH::SetDefaultValue()
//{
//	Set( 0, 0 );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, uint32, 0);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE1D_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURE1D_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURE1D_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURE1D_ENABLE::SetDefaultValue()
//{
//	Set( 0, false );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE2D_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURE2D_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURE2D_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURE2D_ENABLE::SetDefaultValue()
//{
//	Set( 0, false );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE3D_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURE3D_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURE3D_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURE3D_ENABLE::SetDefaultValue()
//{
//	Set( 0, false );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURECUBE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURECUBE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURECUBE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURECUBE_ENABLE::SetDefaultValue()
//{
//	Set( 0, false );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURERECT_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURERECT_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURERECT_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURERECT_ENABLE::SetDefaultValue()
//{
//	Set( 0, false );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTUREDEPTH_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTUREDEPTH_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTUREDEPTH_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTUREDEPTH_ENABLE::SetDefaultValue()
//{
//	Set( 0, false );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE_ENV_COLOR
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURE_ENV_COLOR::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURE_ENV_COLOR::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURE_ENV_COLOR::SetDefaultValue()
//{
//	Set( 0, FMVector4::Zero );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, FMVector4, FMVector4::Zero);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE_ENV_MODE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__TEXTURE_ENV_MODE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__TEXTURE_ENV_MODE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__TEXTURE_ENV_MODE::SetDefaultValue()
//{
//	//Set( FUDaePassStateFunction::ALWAYS, 0 );
//	memset(data, 0, dataSize);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CLIP_PLANE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__CLIP_PLANE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__CLIP_PLANE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__CLIP_PLANE::SetDefaultValue()
//{
//	Set( 0, FMVector4::Zero );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, FMVector4, FMVector4::Zero);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CLIP_PLANE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__CLIP_PLANE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__CLIP_PLANE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__CLIP_PLANE_ENABLE::SetDefaultValue()
//{
//	Set( 0, false );
////		SET_VALUE(0, uint8, 0);
////		SET_VALUE(1, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_COLOR
//// ----------------------------------------------------------------------
//void FCDEffectPassState__BLEND_COLOR::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__BLEND_COLOR::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__BLEND_COLOR::SetDefaultValue()
//{
//	Set( FMVector4::Zero );
////		SET_VALUE(0, FMVector4, FMVector4::Zero);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CLEAR_COLOR
//// ----------------------------------------------------------------------
//void FCDEffectPassState__CLEAR_COLOR::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__CLEAR_COLOR::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__CLEAR_COLOR::SetDefaultValue()
//{
//	Set( FMVector4::Zero );
////		SET_VALUE(0, FMVector4, FMVector4::Zero);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CLEAR_STENCIL
//// ----------------------------------------------------------------------
//void FCDEffectPassState__CLEAR_STENCIL::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__CLEAR_STENCIL::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__CLEAR_STENCIL::SetDefaultValue()
//{
//	Set( 0 );
////		SET_VALUE(0, uint32, 0);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CLEAR_DEPTH
//// ----------------------------------------------------------------------
//void FCDEffectPassState__CLEAR_DEPTH::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__CLEAR_DEPTH::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__CLEAR_DEPTH::SetDefaultValue()
//{
//	Set( 1.0f );
////		SET_VALUE(0, float, 1.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__COLOR_MASK
//// ----------------------------------------------------------------------
//void FCDEffectPassState__COLOR_MASK::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__COLOR_MASK::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__COLOR_MASK::SetDefaultValue()
//{
//	Set( true, true, true, true );
////		SET_VALUE(0, bool, true);
////		SET_VALUE(1, bool, true);
////		SET_VALUE(2, bool, true);
////		SET_VALUE(3, bool, true);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_BOUNDS
//// ----------------------------------------------------------------------
//void FCDEffectPassState__DEPTH_BOUNDS::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__DEPTH_BOUNDS::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__DEPTH_BOUNDS::SetDefaultValue()
//{
//	Set( FMVector2(0,1) );
////		SET_VALUE(0, FMVector2, FMVector2(0,1));
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_MASK
//// ----------------------------------------------------------------------
//void FCDEffectPassState__DEPTH_MASK::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__DEPTH_MASK::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__DEPTH_MASK::SetDefaultValue()
//{
//	Set( true );
//	// SET_VALUE(0, bool, true);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_RANGE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__DEPTH_RANGE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__DEPTH_RANGE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__DEPTH_RANGE::SetDefaultValue()
//{
//	Set( FMVector2(0,1) );
////		SET_VALUE(0, FMVector2, FMVector2(0,1));
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_DENSITY
//// ----------------------------------------------------------------------
//void FCDEffectPassState__FOG_DENSITY::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__FOG_DENSITY::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__FOG_DENSITY::SetDefaultValue()
//{
////		SET_VALUE(0, float, 1.0f);
//	Set( 1.0f );
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_START
//// ----------------------------------------------------------------------
//void FCDEffectPassState__FOG_START::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__FOG_START::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__FOG_START::SetDefaultValue()
//{
//	Set( 0.0f );
////		SET_VALUE(0, float, 0.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_END
//// ----------------------------------------------------------------------
//void FCDEffectPassState__FOG_END::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__FOG_END::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__FOG_END::SetDefaultValue()
//{
//	Set( 1.0f );
////		SET_VALUE(0, float, 1.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_COLOR
//// ----------------------------------------------------------------------
//void FCDEffectPassState__FOG_COLOR::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__FOG_COLOR::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__FOG_COLOR::SetDefaultValue()
//{
//	Set( FMVector4::Zero );
////		SET_VALUE(0, FMVector4, FMVector4::Zero);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_MODEL_AMBIENT
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_MODEL_AMBIENT::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LIGHT_MODEL_AMBIENT::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LIGHT_MODEL_AMBIENT::SetDefaultValue()
//{
//	Set( FMVector4(0.2f,0.2f,0.2f,1.0f) );
//	//SET_VALUE(0, FMVector4, FMVector4(0.2f,0.2f,0.2f,1.0f));
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHTING_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHTING_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LIGHTING_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LIGHTING_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LINE_STIPPLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LINE_STIPPLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LINE_STIPPLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LINE_STIPPLE::SetDefaultValue()
//{
//	Set( 1, 0xFF );
////		SET_VALUE(0, uint16, 1);
////		SET_VALUE(2, uint16, 0xFF);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LINE_WIDTH
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LINE_WIDTH::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LINE_WIDTH::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LINE_WIDTH::SetDefaultValue()
//{
//	Set( 1.0f );
////		SET_VALUE(0, float, 1.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MATERIAL_AMBIENT
//// ----------------------------------------------------------------------
//void FCDEffectPassState__MATERIAL_AMBIENT::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__MATERIAL_AMBIENT::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__MATERIAL_AMBIENT::SetDefaultValue()
//{
//	Set( FMVector4(0.2f,0.2f,0.2f,1.0f) );
//	//SET_VALUE(0, FMVector4, FMVector4(0.2f,0.2f,0.2f,1.0f));
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MATERIAL_DIFFUSE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__MATERIAL_DIFFUSE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__MATERIAL_DIFFUSE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__MATERIAL_DIFFUSE::SetDefaultValue()
//{
//	Set( FMVector4(0.8f,0.8f,0.8f,1.0f) );
//	//SET_VALUE(0, FMVector4, FMVector4(0.8f,0.8f,0.8f,1.0f));
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MATERIAL_EMISSION
//// ----------------------------------------------------------------------
//void FCDEffectPassState__MATERIAL_EMISSION::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__MATERIAL_EMISSION::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__MATERIAL_EMISSION::SetDefaultValue()
//{
//	Set( FMVector4(0,0,0,1) );
////		SET_VALUE(0, FMVector4, FMVector4(0,0,0,1));
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MATERIAL_SHININESS
//// ----------------------------------------------------------------------
//void FCDEffectPassState__MATERIAL_SHININESS::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__MATERIAL_SHININESS::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__MATERIAL_SHININESS::SetDefaultValue()
//{
//	Set( 0.0f );
////		SET_VALUE(0, float, 0.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MATERIAL_SPECULAR
//// ----------------------------------------------------------------------
//void FCDEffectPassState__MATERIAL_SPECULAR::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__MATERIAL_SPECULAR::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__MATERIAL_SPECULAR::SetDefaultValue()
//{
//	Set( FMVector4(0,0,0,1) );
////		SET_VALUE(0, FMVector4, FMVector4(0,0,0,1));
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MODEL_VIEW_MATRIX
//// ----------------------------------------------------------------------
//void FCDEffectPassState__MODEL_VIEW_MATRIX::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__MODEL_VIEW_MATRIX::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__MODEL_VIEW_MATRIX::SetDefaultValue()
//{
//	Set( FMMatrix44::Identity );
////		SET_VALUE(0, FMMatrix44, FMMatrix44::Identity);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_DISTANCE_ATTENUATION
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POINT_DISTANCE_ATTENUATION::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POINT_DISTANCE_ATTENUATION::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POINT_DISTANCE_ATTENUATION::SetDefaultValue()
//{
//	Set( FMVector3(1,0,0) );
////		SET_VALUE(0, FMVector3, FMVector3(1,0,0));
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_FADE_THRESHOLD_SIZE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POINT_FADE_THRESHOLD_SIZE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POINT_FADE_THRESHOLD_SIZE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POINT_FADE_THRESHOLD_SIZE::SetDefaultValue()
//{
//	Set( 1.0f );
////		SET_VALUE(0, float, 1.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_SIZE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POINT_SIZE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POINT_SIZE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POINT_SIZE::SetDefaultValue()
//{
//	Set( 1.0f );
////		SET_VALUE(0, float, 1.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_SIZE_MIN
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POINT_SIZE_MIN::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POINT_SIZE_MIN::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POINT_SIZE_MIN::SetDefaultValue()
//{
//	Set( 0.0f );
////		SET_VALUE(0, float, 0.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_SIZE_MAX
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POINT_SIZE_MAX::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POINT_SIZE_MAX::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POINT_SIZE_MAX::SetDefaultValue()
//{
//	Set( 1.0f );
////		SET_VALUE(0, float, 1.0f);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_OFFSET
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POLYGON_OFFSET::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POLYGON_OFFSET::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POLYGON_OFFSET::SetDefaultValue()
//{
//	Set( FMVector2(0,0) );
////		SET_VALUE(0, FMVector2, FMVector2(0,0));
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__PROJECTION_MATRIX
//// ----------------------------------------------------------------------
//void FCDEffectPassState__PROJECTION_MATRIX::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__PROJECTION_MATRIX::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__PROJECTION_MATRIX::SetDefaultValue()
//{
//	Set( FMMatrix44::Identity );
////		SET_VALUE(0, FMMatrix44, FMMatrix44::Identity);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SCISSOR
//// ----------------------------------------------------------------------
//void FCDEffectPassState__SCISSOR::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__SCISSOR::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__SCISSOR::SetDefaultValue()
//{
//	Set( FMVector4::Zero );
////		SET_VALUE(0, FMVector4, FMVector4::Zero);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_MASK
//// ----------------------------------------------------------------------
//void FCDEffectPassState__STENCIL_MASK::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__STENCIL_MASK::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__STENCIL_MASK::SetDefaultValue()
//{
//	Set( 0xFFFFFFFF );
////		SET_VALUE(0, uint32, 0xFFFFFFFF);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__ALPHA_TEST_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__ALPHA_TEST_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__ALPHA_TEST_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__ALPHA_TEST_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__AUTO_NORMAL_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__AUTO_NORMAL_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__AUTO_NORMAL_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__AUTO_NORMAL_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__BLEND_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__BLEND_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__BLEND_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__COLOR_LOGIC_OP_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__COLOR_LOGIC_OP_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__COLOR_LOGIC_OP_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__COLOR_LOGIC_OP_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__COLOR_MATERIAL_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__COLOR_MATERIAL_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__COLOR_MATERIAL_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__COLOR_MATERIAL_ENABLE::SetDefaultValue()
//{
//	Set( true );
////		SET_VALUE(0, bool, true);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CULL_FACE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__CULL_FACE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__CULL_FACE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__CULL_FACE_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_BOUNDS_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__DEPTH_BOUNDS_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__DEPTH_BOUNDS_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__DEPTH_BOUNDS_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_CLAMP_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__DEPTH_CLAMP_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__DEPTH_CLAMP_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__DEPTH_CLAMP_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_TEST_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__DEPTH_TEST_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__DEPTH_TEST_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__DEPTH_TEST_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DITHER_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__DITHER_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__DITHER_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__DITHER_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__FOG_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__FOG_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__FOG_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_MODEL_LOCAL_VIEWER_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_MODEL_LOCAL_VIEWER_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LIGHT_MODEL_LOCAL_VIEWER_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LIGHT_MODEL_LOCAL_VIEWER_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_MODEL_TWO_SIDE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LIGHT_MODEL_TWO_SIDE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LIGHT_MODEL_TWO_SIDE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LIGHT_MODEL_TWO_SIDE_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LINE_SMOOTH_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LINE_SMOOTH_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LINE_SMOOTH_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LINE_SMOOTH_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LINE_STIPPLE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LINE_STIPPLE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LINE_STIPPLE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LINE_STIPPLE_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LOGIC_OP_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__LOGIC_OP_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__LOGIC_OP_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__LOGIC_OP_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MULTISAMPLE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__MULTISAMPLE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__MULTISAMPLE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__MULTISAMPLE_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__NORMALIZE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__NORMALIZE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__NORMALIZE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__NORMALIZE_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_SMOOTH_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POINT_SMOOTH_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POINT_SMOOTH_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POINT_SMOOTH_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_OFFSET_FILL_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POLYGON_OFFSET_FILL_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POLYGON_OFFSET_FILL_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POLYGON_OFFSET_FILL_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_OFFSET_LINE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POLYGON_OFFSET_LINE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POLYGON_OFFSET_LINE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POLYGON_OFFSET_LINE_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_OFFSET_POINT_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POLYGON_OFFSET_POINT_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POLYGON_OFFSET_POINT_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POLYGON_OFFSET_POINT_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_SMOOTH_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POLYGON_SMOOTH_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POLYGON_SMOOTH_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POLYGON_SMOOTH_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_STIPPLE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__POLYGON_STIPPLE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__POLYGON_STIPPLE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__POLYGON_STIPPLE_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__RESCALE_NORMAL_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__RESCALE_NORMAL_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__RESCALE_NORMAL_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__RESCALE_NORMAL_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SAMPLE_ALPHA_TO_COVERAGE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__SAMPLE_ALPHA_TO_COVERAGE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__SAMPLE_ALPHA_TO_COVERAGE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__SAMPLE_ALPHA_TO_COVERAGE_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SAMPLE_ALPHA_TO_ONE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__SAMPLE_ALPHA_TO_ONE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__SAMPLE_ALPHA_TO_ONE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__SAMPLE_ALPHA_TO_ONE_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SAMPLE_COVERAGE_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__SAMPLE_COVERAGE_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__SAMPLE_COVERAGE_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__SAMPLE_COVERAGE_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SCISSOR_TEST_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__SCISSOR_TEST_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__SCISSOR_TEST_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__SCISSOR_TEST_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_TEST_ENABLE
//// ----------------------------------------------------------------------
//void FCDEffectPassState__STENCIL_TEST_ENABLE::Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue )
//{
//	SET_ENUM_VALUE( 0, alphaFunc );
//	SET_VALUE( 4, float, alphaValue );
//}
//
//void FCDEffectPassState__STENCIL_TEST_ENABLE::Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue )
//{
//	alphaFunc = GET_VALUE( 0, FUDaePassStateFunction::Function );
//	alphaValue = GET_VALUE( 4, float );
//}
//
//void FCDEffectPassState__STENCIL_TEST_ENABLE::SetDefaultValue()
//{
//	Set( false );
////		SET_VALUE(0, bool, false);
//}
//*/
//
//#undef SET_ENUM
//#undef SET_VALUE
//#undef GET_VALUE