/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/
/*
	Based on the 3dsMax COLLADA Tools:
	Copyright (C) 2005-2006 Autodesk Media Entertainment
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _ANIMATION_EXPORTER_H_
#define _ANIMATION_EXPORTER_H_

#ifndef _BASE_EXPORTER_H_
#include "BaseExporter.h"
#endif // _BASE_EXPORTER_H_
#ifndef _FCD_PARAMETER_ANIMATABLE_H_
#include "FCDocument/FCDParameterAnimatable.h"
#endif // _FCD_PARAMETER_ANIMATABLE_H_

#include "FCDocument/FCDAnimated.h"
#include "FUtils/FUDaeEnum.h"

#include "AnimationClipManager.h"

#define MAX_CURVE_KEYS_IDX 65536

class INode;
class Control;
class FCDAnimatedCustom;
class FCDAnimation;
class FCDAnimationClip;
class FCDAnimationCurve;
class FCDAnimationChannel;
class FCDConversionFunctor;
class FCDENode;
class FCDAnimationKey;
class FCDTRotation;
class FCDExportBehavior__T_CONVERT_ENUM_TO_STRING;

typedef fm::pvector<FCDAnimationKey> FCDAnimationKeyList;
typedef fm::map<size_t,FCDAnimationKey *> FCDAnimationKeyMap;
typedef fm::map<FUDaeInterpolation::Interpolation,bool> InterpolationMap;

class AnimationExporter : public BaseExporter
{
public:
	enum AddParamA
	{
		APA_ANIM_TARGET_INVALID = -1
		, APA_ANIM_TARGET_MORPH
		, APA_ANIM_TARGET_ROTATION
		, APA_FORCE_SECONDARY_SAMPLE_RATE
	};

	enum GetAnimatableTimeMappingListFlags
	{
		GATMLF_NONE                        = 0x00000000
		, GATMLF_USE_SECONDARY_SAMPLE_RATE = 0x00000001
		, GATMLF_DEFAULT                   = GATMLF_NONE
	};

public:
	class TimeMapping
	{
	public:
		TimeValue m_ticks;
		float m_secs;

	public:
		TimeMapping( TimeValue ticks, float secs )
			: m_ticks( ticks )
			, m_secs( secs )
		{}

		inline bool operator< ( const TimeMapping& timeMapping ) const
		{
			return m_ticks < timeMapping.m_ticks;
		}

		inline bool operator== ( const TimeMapping& timeMapping ) const
		{
			return m_ticks == timeMapping.m_ticks;
		}
	};

	typedef fm::vector<TimeMapping> TimeMappingList; // maps tick to it's corresponding sec value

private:
	struct Clip
	{
		FCDAnimationClip* colladaClip;
		FCDAnimation* colladaContainer;
		Control* controller;

		Clip();
	};
	typedef fm::map<uint32, Clip> ClipMap;
	typedef fm::pvector<Clip> ClipList;

private:
	TicksToSecsMap m_primaryLinearSamplingTimeMap;
	TicksToSecsMap m_secondaryLinearSamplingTimeMap;
	
public:
	AnimationExporter(DocumentExporter* document);
	virtual ~AnimationExporter();

	void FinalizeExport();

	void ExportAnimatedFloat(const fm::string& id, Control* controller, FCDAnimated* colladaAnimated, FCDConversionFunctor* conversion=NULL, bool forceSampling=false, int addParamA = APA_ANIM_TARGET_INVALID, int addParamB = 0 );
	void ExportAnimatedPoint3(const fm::string& id, Control* controller, FCDAnimated* colladaAnimated, FCDConversionFunctor* conversion=NULL, bool forceSampling=false, int baseType = -1 );
	void ExportAnimatedPoint4(const fm::string& id, Control* controller, FCDAnimated* colladaAnimated, FCDConversionFunctor* conversion=NULL, bool forceSampling=false);

	FCDAnimated* ExportAnimatedAngle(const fm::string& id, Control* controller, ColladaMaxAnimatable::Type type, FCDParameterAnimatableAngleAxis& colladaValue, FCDConversionFunctor* conversion=NULL, bool forceSampling=false);
	FCDAnimated* ExportAnimatedFloat(const fm::string& id, Control* controller, FCDParameterAnimatableFloat& colladaValue, int arrayIndex=-1, FCDConversionFunctor* conversion=NULL, bool forceSampling=false, int addParamA = APA_ANIM_TARGET_INVALID, int addParamB = 0 );
	FCDAnimated* ExportAnimatedPoint3(const fm::string& id, Control* controller, FCDParameterAnimatableVector3& colladaValue, int arrayIndex=-1, FCDConversionFunctor* conversion=NULL, bool forceSampling = false, int baseType = -1 );
	void ExportAnimatedPoint3(const fm::string& id, Control* controllerX, Control* controllerY, Control* controllerZ, FCDAnimated* animated, FCDConversionFunctor* conversion = NULL, bool forceSampling = false, int baseType = -1 );
	FCDAnimated* ExportAnimatedPoint4(const fm::string& id, Control* controller, FCDParameterAnimatableVector4& colladaValue, int arrayIndex=-1, FCDConversionFunctor* conversion=NULL, bool forceSampling=false);
	FCDAnimated* ExportAnimatedAngAxis(const fm::string& id, Control* controller, ColladaMaxAnimatable::Type type, FCDParameterAnimatableAngleAxis& colladaAngleAxis, FCDConversionFunctor* conversion=NULL, bool forceSampling=false);
	void ExportAnimatedEulerAngles(const fm::string& id, Control* controller, fm::pvector<FCDTRotation> rotations, FCDConversionFunctor* conversion=NULL, bool forceSampling=false);
	FCDAnimated* ExportAnimatedTransform(const fm::string& id, INode* node, Control* c, FCDParameterAnimatableMatrix44& colladaValue, bool bIsWSMPivot );
	void ExportDecomposedAnimatedTransform( const fm::string& id, INode* node, Control* c, FCDParameterAnimatableVector3& tColladaValue, FCDParameterAnimatableVector3 &kColladaValue, fm::pvector<FCDTRotation> rotations, bool bIsWSMPivot );
	void FitCubicCurve( const fm::string& id, INode* node, Control* c, FCDParameterAnimatableVector3& tColladaValue, FCDParameterAnimatableVector3 &kColladaValue, fm::pvector<FCDTRotation> rotations, bool bIsWSMPivot );
	
	void ExportProperty(const fm::string& id, IParamBlock2* parameters, TSTR name, int tabIndex, FCDParameterAnimatableFloat& colladaValue, FCDConversionFunctor* conversion);
	void ExportProperty(const fm::string& id, IParamBlock2* parameters, TSTR name, int tabIndex, FCDParameterAnimatableVector3& colladaValue, FCDConversionFunctor* conversion);
	void ExportProperty(const fm::string& id, IParamBlock2* parameters, TSTR name, int tabIndex, FCDParameterAnimatableVector4& colladaValue, FCDConversionFunctor* conversion);
	inline void ExportProperty(const fm::string& id, IParamBlock2* parameters, TSTR name, int tabIndex, FCDParameterAnimatableColor4& colladaValue, FCDConversionFunctor* conversion)
	{ ExportProperty(id, parameters, name, tabIndex, (FCDParameterAnimatableVector4&) colladaValue, conversion); }

	void ExportProperty(const fm::string& id, IParamBlock* parameters, int parameterId, FCDParameterAnimatableFloat& colladaValue, FCDConversionFunctor* conversion = NULL);
	void ExportProperty(const fm::string& id, IParamBlock* parameters, int parameterIdA, int parameterIdB, int parameterIdC, FCDParameterAnimatableVector3& colladaValue, FCDConversionFunctor* conversion = NULL);
	void ExportProperty(const fm::string& id, IParamBlock* parameters, int parameterId, FCDParameterAnimatableVector3& colladaValue, FCDConversionFunctor* conversion = NULL);
	void ExportProperty(const fm::string& id, IParamBlock* parameters, int parameterId, FCDParameterAnimatableVector4& colladaValue, FCDConversionFunctor* conversion = NULL);
	void ExportProperty(const fm::string& id, IParamBlock* parameters, int parameterId, FCDAnimated* colladaAnimated, FCDConversionFunctor* conversion = NULL);
	void ExportProperty(const fm::string& id, IParamBlock2* parameters, int parameterId, int tabIndex, FCDParameterAnimatableFloat& colladaValue, FCDConversionFunctor* conversion = NULL);
	void ExportProperty(const fm::string& id, IParamBlock2* parameters, int parameterId, int tabIndex, FCDParameterAnimatableVector3& colladaValue, FCDConversionFunctor* conversion = NULL);
	void ExportProperty(const fm::string& id, IParamBlock2* parameters, int parameterId, int tabIndex, FCDParameterAnimatableVector4& colladaValue, FCDConversionFunctor* conversion = NULL);
	inline void ExportProperty(const fm::string& id, IParamBlock2* parameters, int parameterId, int tabIndex, FCDParameterAnimatableColor4& colladaValue, FCDConversionFunctor* conversion = NULL)
	{ ExportProperty(id, parameters, parameterId, tabIndex, (FCDParameterAnimatableVector4&) colladaValue, conversion); }

	void ExportEvents();

	void SetRelativeAnimationFlag(bool isRelative) { isRelativeAnimation = isRelative; }
	void SetControlType(ColladaMaxAnimatable::Type _controlType) { controlType = _controlType; }

	void GenerateLinearSamplingTimesMaps();
	void GetAnimatableTimeMappingList( Animatable *pAnimatable, TimeMappingList &keyTimeMappingList, int flags = GATMLF_DEFAULT );

private:
	bool ExportAnimation(Clip* controller, ColladaMaxAnimatable::Type type, FCDAnimated* animated, FCDAnimationChannel** channels, FCDAnimationCurveTrackList &addedCurves, FCDConversionFunctor* conversion=NULL, bool forceSampling=false);
	void ExportCurve(Control* controller, FCDAnimationCurve*& curve, FCDAnimated *pAnimated, FCDConversionFunctor* conversion=NULL, bool forceSampling=false, int addParamA = APA_ANIM_TARGET_INVALID, int addParamB = 0 );

	FCDAnimationCurve* CreateCurve(FCDAnimationChannel* colladaChannel, Control* controller = NULL);
	FCDAnimation* CreateAnimation(Clip* clip, const fchar* id);
	ClipList GetAnimationClips(Control* controller);

	void AddCurvesToClips( FCDAnimationCurveTrackList &curves, Clip **dstStartIter, Clip **dstEndIter );
	void RemoveCurveFromClips( FCDAnimationCurve *curve, Clip **dstStartIter, Clip **dstEndIter );
	bool ValidateAnimKeyTime( FCDAnimationCurve *curve, TimeValue t );
	void AddAnimationKeysToCurve( FCDAnimationKeyList &keys, FCDAnimationCurve *pCurve, bool bOneChannelPerCurve, bool bIsSampled );
	bool ValidateAnimationCurve( FCDAnimationCurve *&pCurve, bool bIsSampled );
	void AddControllerKeyTimesToMap( Control *pControl, fm::map<TimeValue,bool> &map, FUDaeInterpolation::Interpolation interpolation );
	void GetDecomposedTransformControllerArray( Control *pControl, int numCurves[], fm::vector<fm::pvector<Control>> &controllers );
	void DecomposedAnimatedTransformPostExport( INode *pNode, Control *pControl, FCDAnimationCurveList curves[], int numCurves[], FUDaeInterpolation::Interpolation interpolation, fm::pvector<FCDTRotation> rotations, FCDAnimationKeyList *keyBuffers[], fm::vector<fm::pvector<Control>> &controllers );
	
	float GetParameterAsFloat( IParamBlock* parameters, int parameterId, FCDConversionFunctor* conversion );

	void GetSamplingTimeMap( float sampleRate, TicksToSecsMap &samplingMap );

private:
	// Animation layer/clip information
	ClipMap clips;

	// Relative animation support - only for per-vertex positions
	bool isRelativeAnimation;
	static const size_t maxInitialValues = 16;
	float initialValue[maxInitialValues];

	// INTERNAL ONLY. Used in ExportCurve when exporting animated angle, to know which angle (X, Y or Z)
	// to retrieve from the Quaternion. Also used when exporting partial positions..
	ColladaMaxAnimatable::Type controlType;

	// INTERNAL ONLY
	short m_curveKeyIdx[MAX_CURVE_KEYS_IDX];
	bool m_curveKeyValidation[MAX_CURVE_KEYS_IDX];

public:
	template <class ObjectType, int Qualifiers>
	void ExportAnimatedEnum(const fm::string& id, IParamBlock2* parameters, int parameterId, int tabIndex, FCDParameterAnimatableT<ObjectType,Qualifiers>& colladaValue, FCDExportBehavior__T_CONVERT_ENUM_TO_STRING *pExportBehavior, FCDConversionFunctor* conversion = NULL )
	{
		ExportProperty( id, parameters, parameterId, tabIndex, colladaValue, conversion );

		if ( !colladaValue.IsAnimated() )
		{
			return;
		}
		
		// convert all keys to string keys
		FCDAnimated *pAnimated = colladaValue.GetAnimated();
		size_t valueCount = pAnimated->GetValueCount();
		for ( size_t valueId = 0; valueId < valueCount; ++valueId )
		{
			size_t curveCount = pAnimated->GetCurveCount( valueId );
			for ( size_t curveId = 0; curveId < curveCount; ++curveId )
			{
				FCDAnimationCurve *pCurve = pAnimated->GetCurve( valueId, curveId );
				pCurve->ConvertValuesToStrings( pCurve, pExportBehavior );
			}
		}
	}
};

class CurveEvalData
{
public:
	float m_t; 
	TimeValue m_time;
	float m_secs;
	AffineParts m_ap;
	Point4 m_rotation, m_translation, m_scale;
};

#endif // _ANIMATION_EXPORTER_H_
