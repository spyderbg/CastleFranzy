/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDEffectPassState.h
	This file contains the FCDEffectPassState class.
*/

#ifndef _FCD_EFFECT_PASS_STATE_PARAMETER_H_
#define _FCD_EFFECT_PASS_STATE_PARAMETER_H_

#ifndef _FCD_PARAMETER_ANIMATABLE_H_
#include "FCDocument/FCDParameterAnimatable.h"
#endif // _FCD_PARAMETER_ANIMATABLE_H_

#include "FCDocument/FCDObject.h"
//#include "FCDEffectPassState.h"
#include "FCDocument/FCDExportBehavior.h"

class FCOLLADA_EXPORT FCDEffectPassStateParameter : public FCDObject
{
public:
	enum Type
	{
		FLOAT
		, FLOAT4
		, COUNT
	};

private:
	DeclareObjectType(FCDObject);
	//DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_value, FC("Value"));
	DeclareParameterPtr( FCDExportBehavior, m_pExportBehavior, FC( "Export Behavior" ) ); // do not own pointer
	DeclareParameter( Type, FUParameterQualifiers::SIMPLE, m_type, FC( "Type" ) ); // do not own pointer

protected:
	/** Constructor.
		Once built, the render state associated with this object should never change
		and the data pointer should be allocated to the correct size and never
		re-allocated.
		@param document The COLLADA document that owns this render state.
		@param renderState The render state type. */
	FCDEffectPassStateParameter(Type type, FCDocument* document);

public:
	/** Destructor. */
	virtual ~FCDEffectPassStateParameter(){}

	//DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Value, m_value );
	
	/** Clones the effect pass and shaders.
		@param clone The cloned pass.
			If this pointer is NULL, a new pass is created and
			you will need to release this new pass.
		@return The cloned pass. */
	virtual FCDEffectPassStateParameter *Clone(FCDEffectPassStateParameter* clone = NULL) const;

	inline void SetExportBehavior( FCDExportBehavior *pExportBehavior )
	{
		m_pExportBehavior = pExportBehavior;
	}

	inline FCDExportBehavior *GetExportBehavior()
	{
		return m_pExportBehavior;
	}

	inline Type GetType() const
	{
		return m_type;
	}

	static FCDEffectPassStateParameter *Create( Type type, FCDocument *pDocument );
};

//template <class Type, class Qualifiers>
//class FCOLLADA_EXPORT FCDEffectPassStateParameterT : public FCDEffectPassStateParameter
//{
//private:
//	DeclareObjectType(FCDEffectPassStateParameter);
//	DeclareParameterAnimatable(Type, Qualifiers, m_value, FC("Value"));
//
//public:
//	/** Constructor.
//		Once built, the render state associated with this object should never change
//		and the data pointer should be allocated to the correct size and never
//		re-allocated.
//		@param document The COLLADA document that owns this render state.
//		@param renderState The render state type. */
//	FCDEffectPassStateParameterT(FCDocument* document);
//
//	/** Destructor. */
//	virtual ~FCDEffectPassStateParameterT(){}
//
//	DECL_IMPL_ACCESSORS( FCDParameterAnimatableT<FMVector4,Qualifiers>, FMVector4, Value, m_value );
//	
//	/** Clones the effect pass and shaders.
//		@param clone The cloned pass.
//			If this pointer is NULL, a new pass is created and
//			you will need to release this new pass.
//		@return The cloned pass. */
//	virtual FCDEffectPassStateParameter *Clone(FCDEffectPassStateParameter* clone = NULL) const;
//};

class FCOLLADA_EXPORT FCDEffectPassStateParameter__FLOAT : public FCDEffectPassStateParameter
{
private:
	DeclareObjectType(FCDEffectPassStateParameter);
	DeclareParameterAnimatable(float, FUParameterQualifiers::SIMPLE, m_value, FC("Value"));

public:
	/** Constructor.
		Once built, the render state associated with this object should never change
		and the data pointer should be allocated to the correct size and never
		re-allocated.
		@param document The COLLADA document that owns this render state.
		@param renderState The render state type. */
	FCDEffectPassStateParameter__FLOAT(FCDocument* document);

	/** Destructor. */
	virtual ~FCDEffectPassStateParameter__FLOAT(){}

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Value, m_value );
	
	/** Clones the effect pass and shaders.
		@param clone The cloned pass.
			If this pointer is NULL, a new pass is created and
			you will need to release this new pass.
		@return The cloned pass. */
	virtual FCDEffectPassStateParameter__FLOAT *Clone(FCDEffectPassStateParameter__FLOAT* clone = NULL) const;
};

class FCOLLADA_EXPORT FCDEffectPassStateParameter__FLOAT4 : public FCDEffectPassStateParameter
{
private:
	DeclareObjectType(FCDEffectPassStateParameter);
	DeclareParameterAnimatable(FMVector4, FUParameterQualifiers::COLOR, m_value, FC("Value"));

public:
	/** Constructor.
		Once built, the render state associated with this object should never change
		and the data pointer should be allocated to the correct size and never
		re-allocated.
		@param document The COLLADA document that owns this render state.
		@param renderState The render state type. */
	FCDEffectPassStateParameter__FLOAT4(FCDocument* document);

	/** Destructor. */
	virtual ~FCDEffectPassStateParameter__FLOAT4(){}

	DECL_IMPL_ACCESSORS( FCDParameterAnimatableColor4, FMVector4, Value, m_value );
	
	/** Clones the effect pass and shaders.
		@param clone The cloned pass.
			If this pointer is NULL, a new pass is created and
			you will need to release this new pass.
		@return The cloned pass. */
	virtual FCDEffectPassStateParameter__FLOAT4 *Clone(FCDEffectPassStateParameter__FLOAT4* clone = NULL) const;
};

//// ----------------------------------------------------------------------
//// FCDEffectPassState__ALPHA_FUNC
//// ----------------------------------------------------------------------
//class FCDEffectPassState__ALPHA_FUNC
//	: public FCDEffectPassState
//{
////private:
////	DeclareObjectType( FCDEffectPassState );
//
//private:
//	DeclareParameterAnimatable( float, FUParameterQualifiers::SIMPLE, m_function, FC("Alpha Func Function"));
//	DeclareParameterAnimatable( float, FUParameterQualifiers::SIMPLE, m_value, FC("Alpha Value Function"));
//
//public:
//	FCDEffectPassState__ALPHA_FUNC( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::ALPHA_FUNC )
//		, InitializeParameterAnimatable( m_function, ( float )FUDaePassStateFunction::ALWAYS )
//		, InitializeParameterAnimatable( m_value, 0.0f )
//	{}
//
//	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Function, m_function );
//	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, Value, m_value );
//
//	inline FCDEffectPassState *Clone( FCDEffectPassState *clone = NULL )
//	{
//		FCDEffectPassState__ALPHA_FUNC *pClone = ( FCDEffectPassState__ALPHA_FUNC * )( clone ? clone : new FCDEffectPassState__ALPHA_FUNC( GetDocument() ) );
//		FCDEffectPassState::Clone( pClone );
//		CLONE_PARAMETER( pClone, m_function );
//		CLONE_PARAMETER( pClone, m_value );
//	}
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_FUNC
//// ----------------------------------------------------------------------
//class FCDEffectPassState__BLEND_FUNC
//	: public FCDEffectPassState
//{
////private:
////	DeclareObjectType( FCDEffectPassState );
//
//private:
//	DeclareParameterAnimatable( float, FUParameterQualifiers::SIMPLE, m_blendFuncSrc, FC("Blend Func Src Function"));
//	DeclareParameterAnimatable( float, FUParameterQualifiers::SIMPLE, m_blendFuncDst, FC("Blend Func Dst Function"));
//
//public:
//	FCDEffectPassState__BLEND_FUNC( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::BLEND_FUNC )
//		, InitializeParameterAnimatable( m_blendFuncSrc, ( float )FUDaePassStateBlendType::ONE )
//		, InitializeParameterAnimatable( m_blendFuncDst, ( float )FUDaePassStateBlendType::ZERO )
//	{}
//
//	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, BlendFuncSrc, m_blendFuncSrc );
//	DECL_IMPL_ACCESSORS( FCDParameterAnimatableFloat, float, BlendFuncDst, m_blendFuncDst );
//
//	inline FCDEffectPassState *Clone( FCDEffectPassState *clone = NULL )
//	{
//		FCDEffectPassState__BLEND_FUNC *pClone = ( FCDEffectPassState__BLEND_FUNC * )( clone ? clone : new FCDEffectPassState__BLEND_FUNC( GetDocument() ) );
//		FCDEffectPassState::Clone( pClone );
//		CLONE_PARAMETER( pClone, m_blendFuncSrc );
//		CLONE_PARAMETER( pClone, m_blendFuncDst );
//	}
//};

//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_FUNC
//// ----------------------------------------------------------------------
//class FCDEffectPassState__BLEND_FUNC
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__BLEND_FUNC( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::BLEND_FUNC )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateBlendType::Type srcBlendFunc, FUDaePassStateBlendType::Type dstBlendFunc );
//	void Get( FUDaePassStateBlendType::Type &srcBlendFunc, FUDaePassStateBlendType::Type &dstBlendFunc );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_FUNC_SEPARATE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__BLEND_FUNC_SEPARATE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__BLEND_FUNC_SEPARATE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::BLEND_FUNC_SEPARATE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateBlendType::Type srcColorBlendFunc, FUDaePassStateBlendType::Type dstColorBlendFunc, FUDaePassStateBlendType::Type srcAlphaBlendFunc, FUDaePassStateBlendType::Type dstAlphaBlendFunc );
//	void Get( FUDaePassStateBlendType::Type &srcColorBlendFunc, FUDaePassStateBlendType::Type &dstColorBlendFunc, FUDaePassStateBlendType::Type &srcAlphaBlendFunc, FUDaePassStateBlendType::Type &dstAlphaBlendFunc );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_EQUATION
//// ----------------------------------------------------------------------
//class FCDEffectPassState__BLEND_EQUATION
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__BLEND_EQUATION( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::BLEND_EQUATION )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateBlendEquation::Equation blendEquation );
//	void Get( FUDaePassStateBlendEquation::Equation &blendEquation );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_EQUATION_SEPARATE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__BLEND_EQUATION_SEPARATE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__BLEND_EQUATION_SEPARATE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::BLEND_EQUATION_SEPARATE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateBlendEquation::Equation colorBlendEquation, FUDaePassStateBlendEquation::Equation alphaBlendEquation );
//	void Get( FUDaePassStateBlendEquation::Equation &colorBlendEquation, FUDaePassStateBlendEquation::Equation &alphaBlendEquation );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__COLOR_MATERIAL
//// ----------------------------------------------------------------------
//class FCDEffectPassState__COLOR_MATERIAL
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__COLOR_MATERIAL( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::COLOR_MATERIAL )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFaceType::Type faces, FUDaePassStateMaterialType::Type materialType );
//	void Get( FUDaePassStateFaceType::Type &faces, FUDaePassStateMaterialType::Type &materialType );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CULL_FACE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__CULL_FACE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__CULL_FACE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::CULL_FACE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFaceType::Type faceType );
//	void Get( FUDaePassStateFaceType::Type &faceType );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_FUNC
//// ----------------------------------------------------------------------
//class FCDEffectPassState__DEPTH_FUNC
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__DEPTH_FUNC( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::DEPTH_FUNC )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function depthFunc );
//	void Get( FUDaePassStateFunction::Function &depthFunc );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_MODE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__FOG_MODE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__FOG_MODE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::FOG_MODE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFogType::Type fogMode );
//	void Get( FUDaePassStateFogType::Type &fogMode );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_COORD_SRC
//// ----------------------------------------------------------------------
//class FCDEffectPassState__FOG_COORD_SRC
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__FOG_COORD_SRC( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::FOG_COORD_SRC )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFogCoordinateType::Type fogCoordinateType );
//	void Get( FUDaePassStateFogCoordinateType::Type &fogCoordinateType );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FRONT_FACE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__FRONT_FACE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__FRONT_FACE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::FRONT_FACE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFrontFaceType::Type frontFaceType );
//	void Get( FUDaePassStateFrontFaceType::Type &frontFaceType );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_MODEL_COLOR_CONTROL
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_MODEL_COLOR_CONTROL
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_MODEL_COLOR_CONTROL( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_MODEL_COLOR_CONTROL )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateLightModelColorControlType::Type colorControlType );
//	void Get( FUDaePassStateLightModelColorControlType::Type &colorControlType );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LOGIC_OP
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LOGIC_OP
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LOGIC_OP( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LOGIC_OP )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateLogicOperation::Operation operation );
//	void Get( FUDaePassStateLogicOperation::Operation &operation );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_MODE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POLYGON_MODE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POLYGON_MODE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POLYGON_MODE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFaceType::Type faceType, FUDaePassStatePolygonMode::Mode polygonMode );
//	void Get( FUDaePassStateFaceType::Type &faceType, FUDaePassStatePolygonMode::Mode &polygonMode );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SHADE_MODEL
//// ----------------------------------------------------------------------
//class FCDEffectPassState__SHADE_MODEL
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__SHADE_MODEL( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::SHADE_MODEL )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateShadeModel::Model shadeModel );
//	void Get( FUDaePassStateShadeModel::Model &shadeModel );
//};
//
//// TODO
///*
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_FUNC
//// ----------------------------------------------------------------------
//class FCDEffectPassState__STENCIL_FUNC
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__STENCIL_FUNC( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::STENCIL_FUNC )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function stencilFunc, uint8 referenceValue, uint8 mask );
//	void Get( FUDaePassStateFunction::Function &stencilFunc, uint8 &referenceValue, uint8 &mask );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_OP
//// ----------------------------------------------------------------------
//class FCDEffectPassState__STENCIL_OP
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__STENCIL_OP( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::STENCIL_OP )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateStencilOperation::Operation failOperation, FUDaePassStateStencilOperation::Operation depthFailOperation, FUDaePassStateStencilOperation::Operation depthPassOperation );
//	void Get( FUDaePassStateStencilOperation::Operation &failOperation, FUDaePassStateStencilOperation::Operation &depthFailOperation, FUDaePassStateStencilOperation::Operation &depthPassOperation );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_FUNC_SEPARATE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__STENCIL_FUNC_SEPARATE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__STENCIL_FUNC_SEPARATE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::STENCIL_FUNC_SEPARATE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function frontFacesFunction, FUDaePassStateFunction::Function backFacesFunction, uint8 referenceValue, uint8 mask );
//	void Get( FUDaePassStateFunction::Function &frontFacesFunction, FUDaePassStateFunction::Function &backFacesFunction, uint8 &referenceValue, uint8 &mask );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_OP_SEPARATE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__STENCIL_OP_SEPARATE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__STENCIL_OP_SEPARATE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::STENCIL_OP_SEPARATE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFaceType::Type faceType, FUDaePassStateStencilOperation::Operation failOperation, FUDaePassStateStencilOperation::Operation depthFailOperation, FUDaePassStateStencilOperation::Operation depthPassOperation );
//	void Get( FUDaePassStateFaceType::Type &faceType, FUDaePassStateStencilOperation::Operation &failOperation, FUDaePassStateStencilOperation::Operation &depthFailOperation, FUDaePassStateStencilOperation::Operation &depthPassOperation );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_MASK_SEPARATE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__STENCIL_MASK_SEPARATE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__STENCIL_MASK_SEPARATE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::STENCIL_MASK_SEPARATE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFaceType::Type faceType, uint8 mask );
//	void Get( FUDaePassStateFaceType::Type &faceType, uint8 &mask );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( uint8 lightIndex, bool enabled );
//	void Get( uint8 &lightIndex, bool &enabled );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_AMBIENT
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_AMBIENT
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_AMBIENT( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_AMBIENT )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( uint8 lightIndex, const FMVector4 &color );
//	void Get( uint8 &lightIndex, FMVector4 &color );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_DIFFUSE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_DIFFUSE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_DIFFUSE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_DIFFUSE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( uint8 lightIndex, const FMVector4 &color );
//	void Get( uint8 &lightIndex, FMVector4 &color );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_SPECULAR
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_SPECULAR
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_SPECULAR( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_SPECULAR )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( uint8 lightIndex, const FMVector4 &color );
//	void Get( uint8 &lightIndex, FMVector4 &color );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_POSITION
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_POSITION
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_POSITION( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_POSITION )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( uint8 lightIndex, const FMVector4 &pos );
//	void Get( uint8 &lightIndex, FMVector4 &pos );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_CONSTANT_ATTENUATION
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_CONSTANT_ATTENUATION
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_CONSTANT_ATTENUATION( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_CONSTANT_ATTENUATION )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_LINEAR_ATTENUATION
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_LINEAR_ATTENUATION
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_LINEAR_ATTENUATION( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_LINEAR_ATTENUATION )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_QUADRATIC_ATTENUATION
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_QUADRATIC_ATTENUATION
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_QUADRATIC_ATTENUATION( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_QUADRATIC_ATTENUATION )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_SPOT_CUTOFF
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_SPOT_CUTOFF
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_SPOT_CUTOFF( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_SPOT_CUTOFF )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_SPOT_DIRECTION
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_SPOT_DIRECTION
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_SPOT_DIRECTION( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_SPOT_DIRECTION )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_SPOT_EXPONENT
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_SPOT_EXPONENT
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_SPOT_EXPONENT( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_SPOT_EXPONENT )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE1D
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURE1D
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURE1D( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURE1D )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE2D
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURE2D
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURE2D( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURE2D )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE3D
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURE3D
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURE3D( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURE3D )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURECUBE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURECUBE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURECUBE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURECUBE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURERECT
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURERECT
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURERECT( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURERECT )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTUREDEPTH
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTUREDEPTH
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTUREDEPTH( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTUREDEPTH )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE1D_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURE1D_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURE1D_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURE1D_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE2D_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURE2D_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURE2D_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURE2D_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE3D_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURE3D_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURE3D_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURE3D_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURECUBE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURECUBE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURECUBE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURECUBE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURERECT_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURERECT_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURERECT_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURERECT_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTUREDEPTH_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTUREDEPTH_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTUREDEPTH_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTUREDEPTH_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE_ENV_COLOR
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURE_ENV_COLOR
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURE_ENV_COLOR( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURE_ENV_COLOR )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__TEXTURE_ENV_MODE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__TEXTURE_ENV_MODE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__TEXTURE_ENV_MODE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::TEXTURE_ENV_MODE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CLIP_PLANE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__CLIP_PLANE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__CLIP_PLANE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::CLIP_PLANE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CLIP_PLANE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__CLIP_PLANE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__CLIP_PLANE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::CLIP_PLANE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_COLOR
//// ----------------------------------------------------------------------
//class FCDEffectPassState__BLEND_COLOR
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__BLEND_COLOR( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::BLEND_COLOR )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CLEAR_COLOR
//// ----------------------------------------------------------------------
//class FCDEffectPassState__CLEAR_COLOR
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__CLEAR_COLOR( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::CLEAR_COLOR )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CLEAR_STENCIL
//// ----------------------------------------------------------------------
//class FCDEffectPassState__CLEAR_STENCIL
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__CLEAR_STENCIL( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::CLEAR_STENCIL )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CLEAR_DEPTH
//// ----------------------------------------------------------------------
//class FCDEffectPassState__CLEAR_DEPTH
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__CLEAR_DEPTH( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::CLEAR_DEPTH )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__COLOR_MASK
//// ----------------------------------------------------------------------
//class FCDEffectPassState__COLOR_MASK
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__COLOR_MASK( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::COLOR_MASK )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_BOUNDS
//// ----------------------------------------------------------------------
//class FCDEffectPassState__DEPTH_BOUNDS
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__DEPTH_BOUNDS( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::DEPTH_BOUNDS )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_MASK
//// ----------------------------------------------------------------------
//class FCDEffectPassState__DEPTH_MASK
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__DEPTH_MASK( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::DEPTH_MASK )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_RANGE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__DEPTH_RANGE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__DEPTH_RANGE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::DEPTH_RANGE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_DENSITY
//// ----------------------------------------------------------------------
//class FCDEffectPassState__FOG_DENSITY
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__FOG_DENSITY( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::FOG_DENSITY )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_START
//// ----------------------------------------------------------------------
//class FCDEffectPassState__FOG_START
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__FOG_START( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::FOG_START )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_END
//// ----------------------------------------------------------------------
//class FCDEffectPassState__FOG_END
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__FOG_END( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::FOG_END )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_COLOR
//// ----------------------------------------------------------------------
//class FCDEffectPassState__FOG_COLOR
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__FOG_COLOR( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::FOG_COLOR )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_MODEL_AMBIENT
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_MODEL_AMBIENT
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_MODEL_AMBIENT( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_MODEL_AMBIENT )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHTING_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHTING_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHTING_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHTING_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LINE_STIPPLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LINE_STIPPLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LINE_STIPPLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LINE_STIPPLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LINE_WIDTH
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LINE_WIDTH
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LINE_WIDTH( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LINE_WIDTH )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MATERIAL_AMBIENT
//// ----------------------------------------------------------------------
//class FCDEffectPassState__MATERIAL_AMBIENT
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__MATERIAL_AMBIENT( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::MATERIAL_AMBIENT )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MATERIAL_DIFFUSE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__MATERIAL_DIFFUSE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__MATERIAL_DIFFUSE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::MATERIAL_DIFFUSE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MATERIAL_EMISSION
//// ----------------------------------------------------------------------
//class FCDEffectPassState__MATERIAL_EMISSION
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__MATERIAL_EMISSION( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::MATERIAL_EMISSION )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MATERIAL_SHININESS
//// ----------------------------------------------------------------------
//class FCDEffectPassState__MATERIAL_SHININESS
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__MATERIAL_SHININESS( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::MATERIAL_SHININESS )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MATERIAL_SPECULAR
//// ----------------------------------------------------------------------
//class FCDEffectPassState__MATERIAL_SPECULAR
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__MATERIAL_SPECULAR( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::MATERIAL_SPECULAR )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MODEL_VIEW_MATRIX
//// ----------------------------------------------------------------------
//class FCDEffectPassState__MODEL_VIEW_MATRIX
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__MODEL_VIEW_MATRIX( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::MODEL_VIEW_MATRIX )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_DISTANCE_ATTENUATION
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POINT_DISTANCE_ATTENUATION
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POINT_DISTANCE_ATTENUATION( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POINT_DISTANCE_ATTENUATION )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_FADE_THRESHOLD_SIZE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POINT_FADE_THRESHOLD_SIZE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POINT_FADE_THRESHOLD_SIZE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POINT_FADE_THRESHOLD_SIZE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_SIZE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POINT_SIZE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POINT_SIZE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POINT_SIZE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_SIZE_MIN
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POINT_SIZE_MIN
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POINT_SIZE_MIN( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POINT_SIZE_MIN )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_SIZE_MAX
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POINT_SIZE_MAX
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POINT_SIZE_MAX( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POINT_SIZE_MAX )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_OFFSET
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POLYGON_OFFSET
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POLYGON_OFFSET( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POLYGON_OFFSET )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__PROJECTION_MATRIX
//// ----------------------------------------------------------------------
//class FCDEffectPassState__PROJECTION_MATRIX
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__PROJECTION_MATRIX( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::PROJECTION_MATRIX )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SCISSOR
//// ----------------------------------------------------------------------
//class FCDEffectPassState__SCISSOR
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__SCISSOR( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::SCISSOR )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_MASK
//// ----------------------------------------------------------------------
//class FCDEffectPassState__STENCIL_MASK
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__STENCIL_MASK( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::STENCIL_MASK )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__ALPHA_TEST_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__ALPHA_TEST_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__ALPHA_TEST_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::ALPHA_TEST_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__AUTO_NORMAL_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__AUTO_NORMAL_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__AUTO_NORMAL_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::AUTO_NORMAL_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__BLEND_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__BLEND_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__BLEND_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::BLEND_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__COLOR_LOGIC_OP_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__COLOR_LOGIC_OP_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__COLOR_LOGIC_OP_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::COLOR_LOGIC_OP_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__COLOR_MATERIAL_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__COLOR_MATERIAL_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__COLOR_MATERIAL_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::COLOR_MATERIAL_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__CULL_FACE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__CULL_FACE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__CULL_FACE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::CULL_FACE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_BOUNDS_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__DEPTH_BOUNDS_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__DEPTH_BOUNDS_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::DEPTH_BOUNDS_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_CLAMP_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__DEPTH_CLAMP_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__DEPTH_CLAMP_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::DEPTH_CLAMP_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DEPTH_TEST_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__DEPTH_TEST_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__DEPTH_TEST_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::DEPTH_TEST_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__DITHER_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__DITHER_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__DITHER_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::DITHER_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__FOG_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__FOG_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__FOG_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::FOG_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_MODEL_LOCAL_VIEWER_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_MODEL_LOCAL_VIEWER_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_MODEL_LOCAL_VIEWER_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_MODEL_LOCAL_VIEWER_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LIGHT_MODEL_TWO_SIDE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LIGHT_MODEL_TWO_SIDE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LIGHT_MODEL_TWO_SIDE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LIGHT_MODEL_TWO_SIDE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LINE_SMOOTH_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LINE_SMOOTH_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LINE_SMOOTH_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LINE_SMOOTH_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LINE_STIPPLE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LINE_STIPPLE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LINE_STIPPLE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LINE_STIPPLE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__LOGIC_OP_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__LOGIC_OP_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__LOGIC_OP_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::LOGIC_OP_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__MULTISAMPLE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__MULTISAMPLE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__MULTISAMPLE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::MULTISAMPLE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__NORMALIZE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__NORMALIZE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__NORMALIZE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::NORMALIZE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POINT_SMOOTH_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POINT_SMOOTH_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POINT_SMOOTH_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POINT_SMOOTH_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_OFFSET_FILL_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POLYGON_OFFSET_FILL_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POLYGON_OFFSET_FILL_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POLYGON_OFFSET_FILL_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_OFFSET_LINE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POLYGON_OFFSET_LINE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POLYGON_OFFSET_LINE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POLYGON_OFFSET_LINE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_OFFSET_POINT_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POLYGON_OFFSET_POINT_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POLYGON_OFFSET_POINT_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POLYGON_OFFSET_POINT_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_SMOOTH_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POLYGON_SMOOTH_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POLYGON_SMOOTH_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POLYGON_SMOOTH_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__POLYGON_STIPPLE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__POLYGON_STIPPLE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__POLYGON_STIPPLE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::POLYGON_STIPPLE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__RESCALE_NORMAL_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__RESCALE_NORMAL_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__RESCALE_NORMAL_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::RESCALE_NORMAL_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SAMPLE_ALPHA_TO_COVERAGE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__SAMPLE_ALPHA_TO_COVERAGE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__SAMPLE_ALPHA_TO_COVERAGE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::SAMPLE_ALPHA_TO_COVERAGE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SAMPLE_ALPHA_TO_ONE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__SAMPLE_ALPHA_TO_ONE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__SAMPLE_ALPHA_TO_ONE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::SAMPLE_ALPHA_TO_ONE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SAMPLE_COVERAGE_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__SAMPLE_COVERAGE_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__SAMPLE_COVERAGE_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::SAMPLE_COVERAGE_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__SCISSOR_TEST_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__SCISSOR_TEST_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__SCISSOR_TEST_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::SCISSOR_TEST_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//
//// ----------------------------------------------------------------------
//// FCDEffectPassState__STENCIL_TEST_ENABLE
//// ----------------------------------------------------------------------
//class FCDEffectPassState__STENCIL_TEST_ENABLE
//	: public FCDEffectPassState
//{
//public:
//	FCDEffectPassState__STENCIL_TEST_ENABLE( FCDocument *pDocument )
//		: FCDEffectPassState( pDocument, FUDaePassState::STENCIL_TEST_ENABLE )
//	{
//		// Set the render state to its default value(s)
//		SetDefaultValue();
//	}
//
//	void SetDefaultValue();
//	void Set( FUDaePassStateFunction::Function alphaFunc, float alphaValue );
//	void Get( FUDaePassStateFunction::Function &alphaFunc, float &alphaValue );
//};
//*/

#endif // _FCD_EFFECT_PASS_STATE_PARAMETER_H_
