#ifndef __SDATABASECOLLADA_H__
#define __SDATABASECOLLADA_H__

#include "resVector.h"
#include "resFile.h"
#include "aabbox3d.h"
#include "matrix4.h"
#include "quaternion.h"
#include "SDatabaseCollada_Version.h"

namespace irr
{
namespace video
{
	class ITexture;
}
}

namespace irr
{
namespace collada
{

struct SInstance;
struct SInput;
struct SSource;
struct SEffect;
struct SInstanceGeometry;
struct SInstanceController;
struct SInstanceModularSkin;
struct SInstanceCamera;
struct SInstanceVisualScene;
struct SController;
struct SAnimation;
struct SImage;
struct SInstanceLight;
struct SInstanceEmitter;
struct SInstanceForce;
struct SLight;
struct SMaterial;
struct SGeometry;
struct SController;

struct SInstance
{
	enum InstanceType
	{
		ITAnimation,
		ITCamera,
		ITController,
		ITGeometry,
		ITLight,
		ITNode,
		ITVisualScene,
		ITPhysicsScene,
		ITMaterial,
		ITEmitter,
		ITForce,
		ITModularSkin,
	};
	InstanceType			type;

	union
	{
		void					*pData;
		SInstanceGeometry		*pGeometry;
		SInstanceController		*pController;
		SInstanceCamera			*pCamera;
		SInstanceLight			*pLight;
		SInstanceVisualScene	*pVisualScene;
		SInstanceEmitter		*pEmitter;
		SInstanceForce			*pForce;
		SInstanceModularSkin	*pModularSkin;
	};
};

struct SInstanceMaterial
{
	res::String				filename;
	res::String				url;
	union
	{
		int					iMaterial;
		SMaterial *			pMaterial;
	};
	int						iEffectComponent;
	int						iGeometryComponent;
};

struct SBindMaterial
{
	res::vector<SInstanceMaterial> instanceMaterials;
};

struct SInstanceController
{
	res::String					filename;
	res::String					url;
	SController					*pController;
	SBindMaterial				bindMaterial;
	int							drawLayer;
	res::vector<res::String>	skeletons;
};

struct SCategory
{
	res::String							name;
	res::String							selection;
	res::vector<SInstance>	modules;
};

struct SInstanceModularSkin
{
	res::vector<SCategory>				categories;
	res::vector<SInstanceController>	modules;
};

struct SInstanceGeometry
{
	res::String				filename;
	res::String				url;
	SGeometry				*pGeometry;
	SBindMaterial			bindMaterial;
	int						drawLayer;
};

struct SSetParam
{
	res::String				ref;
	void *					value;
};

struct SInstanceEffect
{
	res::String				filename;
	res::String				url;
	res::vector<SSetParam>		setParams;
	union
	{
		int effectId;
		SEffect *pEffect;
	};
};

struct SInstanceVisualScene
{
	res::String				filename;
	res::String				url;
};

struct SInstanceLight
{
	res::String				filename;
	res::String				url;
	union
	{
		int lightId;
		SLight *pLight;
	};
};

struct SInstanceCamera
{
	res::String				filename;
	res::String				url;
	union
	{
		int cameraId;
		SEffect *pCamera;
	};
};

struct SCamera
{
	res::String				id;
	int						orthographic;
	union
	{
		float					xfov;
		float					xmag;
	};
	float					aspectRatio;
	float					znear;
	float					zfar;
	res::String				target;
};

struct SColor
{
	unsigned char r, g, b, a;
};


struct SPointLight
{
	float			constantAttenuation;
	float			linearAttenuation;
	float			quadraticAttenuation;
};

struct SSpotLight
{
	float			constantAttenuation;
	float			linearAttenuation;
	float			quadraticAttenuation;
	float			falloffAngle;
	float			falloffExponent;
};


struct SLight
{
	enum Type
	{
		LT_AMBIENT,
		LT_POINT,
		LT_SPOT,
		LT_DIRECTIONNAL,
	};

	res::String		id;
	res::String		name;
	int				type;
	SColor			color;
	float			intensity;
	union
	{
		SSpotLight	*pSpot;
		SPointLight *pPoint;
	};
};

struct SLibraryLights
{
	res::vector<SLight> lights;
};



struct SSurface
{
	union
	{
		int				image;
		SImage			*pImage;
	};
};

struct SSampler2D
{
	union
	{
		int				surface;
		SSurface		*pSurface;
	};
};

struct STexture
{
	union
	{
		int				texture; //samplerId
		SSampler2D*		sampler;
	};
	
	res::String		texcoord;
	
	float offsetU;
	float offsetV;
	float rotateUV;
	float repeatU;
	float repeatV;
};

struct SCommonTexture
{
	res::vector<STexture> data;
};

struct SCommonColorOrTexture
{
	enum Type
	{
		ECOLOR = 0,
		ETEXTURE = 1,
	};

	u8	 eType;
	u8	 reserved[3];
	union
	{
		res::vector<STexture> *pTexture;
		SColor	 *pColor;
	};
};
/*
struct SSamplerState
{
	enum E_WrapMode
	{
		EWM_REPEAT,
	};

	enum E_FileterMode
	{
		EFM_NEAREST,
		EFM_LINEAR,
		EFM_LINEAR_MIPMAP_LINEAR,
	};

	res::String sid;
	E_WrapMode wrapS;
	E_WrapMode wrapT;
	u8 magFilter;
	u8 minFilter;
	u8 mipmapBias;
	u8 mipmapMaxlevel;
};

struct STextureUnit
{
	
};

struct STexCombiner
{
	enum E_OPERATOR
	{
		EO_REPLACE,
		EO_MODULATE,
		EO_ADD,
		EO_ADD_SIGNED,
		EO_INTERPOLATE,
		EO_SUBSTRACT,
		EO_DOT3_RGB,
		EO_DOT3_RGBA,
	};
	
	enum E_SOURCE
	{
		ES_TEXTURE,
		ES_PRIMARY,
		ES_PREVIOUS,
	};

	enum E_OPERAND
	{
		EO_SRC_RGB,
		EO_SRC_ALPHA,
	};

	struct SArg
	{
		E_SOURCE source;
		E_OPERAND operand;
		int	unit;
	}

	int					rgbOperator;
	res::vector<SArg>	rgbArguments;
	int					alphaOperator;
	res::vector<SArg>	alphaArguments;
};

struct STexEnv
{
	enum E_OPERATOR
	{
		EO_REPLACE,
		EO_MODULATE,
		EO_DECAL,
		EO_BLEND,
		EO_ADD,
	};
	
	int iOperator;
	int	unit;
};

struct STexturePipeline
{
	enum E_TexMode
	{
		ETM_COMBINER,
		ETM_ENV,
	};

	struct STexStage
	{
		int iType;
		union
		{
			STexCombiner *pCombiner;
			STexEnv *pTexEnv;
		};
	};

	res::vector<STexStage> texStages;
};


struct SPass
{
	void *colorTarget;
	void *depthTarget;
	void *stencilTarget;
	void *colorClear;
	void *depthClear;
	void *stencilClear;
	void *draw;
	void *renderStates;
};

struct SProfileGLES
{
	float							depthClear;
	res::vector<STextureUnit>		textureUnits;
	STexturePipeline				texturePipeline;		
};
*/
struct SEffect
{
	res::String id;
	res::String name;
	SCommonColorOrTexture ambient;
	SCommonColorOrTexture diffuse;
	SCommonColorOrTexture specular;
	SCommonColorOrTexture emission;
	float shininess;
	SCommonColorOrTexture reflective;
	float reflectivity;
	SCommonColorOrTexture transparent;
	float transparency;
	float specLevel;
	int		doubleSided;
	res::vector<SSampler2D>	  samplers;
	res::vector<SSurface>	  surfaces;
};

struct SLibraryEffects
{
	res::vector<SEffect> effects;
};



struct SMaterial
{
	res::String id;
	res::String name;
	SInstanceEffect instanceEffect;
	int isAdditive;
	SColor diffuseColor;
	union
	{
		int				iDiffuseSurface;
		SImage			*pDiffuseSurface;
	};
	union
	{
		int				iEnvmapSurface;
		SImage			*pEnvmapSurface;
	};
	float		envmapIntensity;
	int			renderSet;			// 0 - Solid, 1 - Translucent, 2 - Solid with Alpha Test
	int			renderPriority;		// 0 - close, 255 - far
	int			backfaceCulling;
	int			frontfaceCulling;
};

struct SLibraryMaterials
{
	res::vector<SMaterial> materials;
};

struct SAnimationClip
{
	res::String				id;
	int						start;
	int						end;
};

struct SLibraryAnimationClips
{
	int getClipIndex(const char *pClipName) const
	{
		for(int i = 0; i < getClipCount(); i++)
		{
			if(animationClips[i].id == pClipName)
			{
				return i;
			}
		}

		return -1;
	}

	inline const char * getClipName(int idx) const
	{
		return animationClips[idx].id;
	}

	inline int getClipStart(int idx) const
	{
		return animationClips[idx].start;
	}

	inline int getClipEnd(int idx) const
	{
		return animationClips[idx].end;
	}

	inline int getClipLength(int idx) const
	{
		return getClipEnd(idx) - getClipStart(idx);
	}

	inline int getClipCount() const
	{
		return animationClips.size();
	}

	res::vector<SAnimationClip>	animationClips;
};

struct SChannel
{
	enum Type
	{
		CT_TRANSFORM,
		CT_POSITION,
		CT_POSITION_X,
		CT_POSITION_Y,
		CT_POSITION_Z,
		CT_ROTATION,
		CT_ROTATION_X,
		CT_ROTATION_Y,
		CT_ROTATION_Z,
		CT_ROTATION_W,
		CT_SCALE,
		CT_SCALE_X,
		CT_SCALE_Y,
		CT_SCALE_Z,
		CT_MORPHING_WEIGHT,
		CT_IFL,
		CT_LIGHT_COLOR,
		CT_LIGHT_CONSTANT_ATTENUATION,
		CT_LIGHT_LINEAR_ATTENUATION,
		CT_LIGHT_QUADRATIC_ATTENUATION,
		CT_VISIBILITY,
		CT_EFFECT_DIFFUSE,
		CT_EFFECT_DIFFUSE_R,
		CT_EFFECT_DIFFUSE_G,
		CT_EFFECT_DIFFUSE_B,
		CT_EFFECT_DIFFUSE_A,
		CT_EFFECT_TRANSPARENCY,
		CT_EVENTS,
		CT_EMITTER_BIRTH_RATE,
		CT_EMITTER_PARTICLE_SIZE,
		CT_EMITTER_PARTICLE_SIZE_VARIANCE,
		CT_EMITTER_PARTICLE_SPEED,
		CT_EMITTER_PARTICLE_SPEED_VARIANCE,
		CT_EMITTER_PARTICLE_DIRECTION,
		CT_EMITTER_PARTICLE_DIRECTION_VARIANCE,
		CT_EMITTER_PARTICLE_SPIN_TIME,
		CT_EMITTER_PARTICLE_SPIN_TIME_VARIANCE,
		CT_EMITTER_PARTICLE_SPIN_PHASE,
		CT_EMITTER_PARTICLE_SPIN_PHASE_VARIANCE,
		CT_EMITTER_PARTICLE_SPIN_AXIS,
		CT_EMITTER_PARTICLE_SPIN_AXIS_VARIANCE,
		CT_EMITTER_PARTICLE_ANIMATION_OFFSET,
		CT_EMITTER_PARTICLE_ANIMATION_OFFSET_VARIANCE,
		CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_LENGTH,
		CT_EMITTER_PARTICLE_ANIMATION_SCALING_SRC_VARIANCE,
		CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_MULTIPLIER,
		CT_EMITTER_PARTICLE_ANIMATION_SCALING_DST_VARIANCE,
		CT_EFFECT_AMBIENT,
		CT_EFFECT_AMBIENT_R,
		CT_EFFECT_AMBIENT_G,
		CT_EFFECT_AMBIENT_B,
		CT_EFFECT_AMBIENT_A,
		CT_EFFECT_SPECULAR,
		CT_EFFECT_SPECULAR_R,
		CT_EFFECT_SPECULAR_G,
		CT_EFFECT_SPECULAR_B,
		CT_EFFECT_SPECULAR_A,
		CT_EFFECT_EMISSION,
		CT_EFFECT_EMISSION_R,
		CT_EFFECT_EMISSION_G,
		CT_EFFECT_EMISSION_B,
		CT_EFFECT_EMISSION_A,
		CT_EFFECT_TRANSFORM = 0x100,
		CT_EFFECT_TRANSFORM_MASK = 0xFF,
		CT_EFFECT_TRANSFORM_OFFSET_U = CT_EFFECT_TRANSFORM + 1,
		CT_EFFECT_TRANSFORM_OFFSET_V = CT_EFFECT_TRANSFORM + 2,
		CT_EFFECT_TRANSFORM_ROTATE = CT_EFFECT_TRANSFORM + 3,
		CT_EFFECT_TRANSFORM_SCALE_U = CT_EFFECT_TRANSFORM + 4,
		CT_EFFECT_TRANSFORM_SCALE_V = CT_EFFECT_TRANSFORM + 5,
	};

	res::String				source;
	res::String				target;
	Type					type;
	unsigned char			reserved0;
	unsigned char			reserved1;
	unsigned char			reserved2;
	unsigned char			reserved3;
};



struct SSampler
{	
	enum EInterpolationType
	{
		IT_STEP = 0,
		IT_LINEAR = 1,
		IT_HERMITE = 2,
		IT_UNKNOWN = -1,
	};

	int iInterpolationType;

	union
	{
		int iInput;
		SSource *pInput;
	};

	union
	{
		int iOutput;
		SSource *pOutput;
	};

	union
	{
		int iInTangent;
		SSource *pInTangent;
	};

	union
	{
		int iOutTangent;
		SSource *pOutTangent;
	};
};

struct SMatrix
{
	float M[16];

	inline void transformVect( core::vector3df& out, const core::vector3df& in ) const
	{
		out.X = in.X*M[0] + in.Y*M[4] + in.Z*M[8] + M[12];
		out.Y = in.X*M[1] + in.Y*M[5] + in.Z*M[9] + M[13];
		out.Z = in.X*M[2] + in.Y*M[6] + in.Z*M[10] + M[14];
	}
};

struct SVectorMatrix
	: private res::vector<float>
{
	inline SMatrix& operator[](int idx)
	{
		return *(SMatrix*)&res::vector<float>::operator[](idx * 16);
	}
};

struct STransformSRT
{
	core::vector3df translation;
	core::quaternion quaternion;
	core::vector3df scale;
};

struct SVectorSRT
	: private res::vector<float>
{
	inline STransformSRT& operator[](int idx)
	{
		return *(STransformSRT*)&res::vector<float>::operator[](idx * 10);
	}
};


template <typename T>
struct SVectorTemplate
	: private res::vector<float>
{
	inline T& operator[](int idx)
	{
		return *(T*)&res::vector<float>::operator[](idx * sizeof(T) / sizeof(float));
	}
};

struct SSource
{
	enum Type
	{
		ST_FLOAT,			// 1 float
		ST_FLOAT4x4,		// 16 floats
		ST_FLOAT_SRT,		// 10 floats 
		ST_STRING,
		ST_FLOAT_ANGLE,		// 1 float
		ST_FLOAT_ANGLE_AXE, // 4 floats
		ST_FLOAT_COLOR3,	// 3 bytes - converted
		ST_FLOAT_COLOR4,	// 4 bytes - converted	
	};
	int type;

	union
	{	
		res::vector<res::String>		sData;
		res::vector<void *>				pData;
		res::vector<float>				fData;
		res::vector<int>				iData;
		res::vector<u8>					bData;
		SVectorSRT						srt;
		SVectorMatrix					matrix;
	};
};

struct SAnimation
{
	res::String					id;
	res::vector<SSource>		sources;
	res::vector<SSampler>		samplers;
	res::vector<SChannel>		channels;
	void						*pAnimator;
	void						*pAnimationExtra;

	const res::vector<float> &getOutput(int index = 0) const { return samplers[index].pOutput->fData; }
	const res::vector<int>	&getKeyTime(int index = 0) const { return samplers[index].pInput->iData; }
	int getStart(int index = 0) const						{ return getKeyTime(index)[0]; }
	int getEnd(int index = 0) const							{ return getKeyTime(index)[getKeyTime(index).size() - 1]; }
	int getLength(int index = 0) const						{ return getEnd(index) - getStart(index);  }
	int getInterpolationType(int index = 0) const			{ return samplers[index].iInterpolationType; }
	//! Do a binary lookup
	/** \return : Return true if the value is in between the 2 frame
				  or false if the value is exactly the prev frame
	*/
	bool findKeyFrameNo(int track, s32 timeMs, s32 &outFrame) const
	{
		const res::vector<int> &m_input = getKeyTime(track);

		s32 m;
		/** Left intentionnaly fixed to 1 instead of 0.
			Right value at the end of the search is always at Left - 1.
			When the timeMs is before the first key frame time, the right value is = to left(1) - 1.
		*/
		s32 left = 1; 
		s32 right = m_input.size() - 1;
		
		while(left<=right)
		{
			m = (left+right)>>1;

			if (timeMs < m_input[m])
				right = m - 1;
			else
				left = m + 1;

		}

		outFrame = right;
		return !((timeMs == m_input[outFrame]) || (outFrame == m_input.size() - 1));
	}

	//! Do a linear lookup based on the hint
	bool findKeyFrameNo(int track, s32 timeMs, s32 &outFrame, s32 hint) const
	{
		const res::vector<int> &m_input = getKeyTime(track);

		s32 max = m_input.size() - 1;
		s32 m = core::clamp<s32>(hint, 0, max);
		
		if(timeMs < m_input[m] && m > 0)
		{
			m--;
		}
		else if(m < max && m_input[m+1] < timeMs)
		{
			m++;

			if(m < max && m_input[m+1] < timeMs)
			{
				m++;
			}
		}

		if((m < max) && (timeMs < m_input[m] || m_input[m+1] < timeMs))
		{
			return findKeyFrameNo(track, timeMs, outFrame);
		}
		else
		{
			outFrame = m;
			return !((timeMs == m_input[outFrame]) || (outFrame == m_input.size() - 1));
		}	
	}
};

struct SEventsTrack
{
	SSource	times;
	res::vector<res::vector<res::String> > events;
	//SSource events;
};

struct SLibraryAnimations
{
	res::vector<SAnimation>		animations;
	SEventsTrack				*eventsTrack;
};

struct SMeshBuffer
{
	enum E_BufferType
	{
		EBT_Triangles = 0,
		EBT_TriangleStrips = 1,
		EBT_Lines = 2,
		EBT_LineStrips = 3,
		EBT_LineLoop = 4,
	};

	E_BufferType		type;				//Primitive type
	res::String			material;			
	int					count;				//Primitive count

	char				positionSource;		//-1 if not used

	char				normalSource;		//-1 if not used
	char				binormalSource;		//-1 if not used
	char				tangentSource;		//-1 if not used

	char				texcoord0Source;	//-1 if not used
	char				texbinormal0Source;	//-1 if not used
	char				textangent0Source;	//-1 if not used

	char				texcoord1Source;	//-1 if not used
	char				texbinormal1Source;	//-1 if not used
	char				textangent1Source;	//-1 if not used

	char				color0Source;		//-1 if not used
	char				color1Source;		//-1 if not used

	res::vector<short>	indices;			//indices array
	short				minIndex;
	short				maxIndex;

	irr::core::aabbox3df		boundingBox;		//boundingBox				
};

struct SSpline
{
	int			closed;
	SSource		positions;
};

struct SMesh
{
	
	struct InterleavedData
	{
		struct ScaleAndOffset
		{
			core::vector3df scale;
			core::vector3df offset;
		};

		int					stride;
		res::vector<int>	offsets;
		res::vector<int>	types;
		res::vector<ScaleAndOffset *>	pScaleOffsets;
		void				*data;
	};

	enum VertexComponents
	{
		VT_POSITION = 1,
		VT_NORMAL = 2,
		VT_COLOR = 4,
		VT_TEXCOOR = 8
	};

	int								bInterleaved;
	union
	{
		struct
		{
			int							iInterleaved;
			InterleavedData				*pInterleaved;
		};		
		res::vector<SSource>		pSources;
	};

	res::vector<SMeshBuffer>		meshBuffers;	  

	core::aabbox3df					boundingBox;

	int getVertexCount() const
	{
		if(bInterleaved)
		{
			return iInterleaved;
		}
		else
		{
			return 0;
		}
	}

	const void *getSourceBuffer(int i) const
	{
		if(bInterleaved)
		{
			return ((char *)pInterleaved->data) + pInterleaved->offsets[i];
		}
		else
		{
			return pSources[i].pData.ptr();
		}
	}

	const core::vector3df &getSourceScale(int i) const
	{
		_IRR_DEBUG_BREAK_IF(!bInterleaved && !pInterleaved->pScaleOffsets[i]);
		return pInterleaved->pScaleOffsets[i]->scale;
	}

	const core::vector3df &getSourceOffset(int i) const
	{
		_IRR_DEBUG_BREAK_IF(!bInterleaved && !pInterleaved->pScaleOffsets[i]);
		return pInterleaved->pScaleOffsets[i]->offset;
	}

	const int getSourceType(int i) const
	{
		if(bInterleaved)
		{
			return pInterleaved->types[i];
		}
		else
		{
			return pSources[i].type;
		}
	}
};

struct SGeometry
{
	enum E_GEOMETRY_TYPE
	{
		EGT_MESH = 0,
		EGT_SPLINE = 1, 
	};

	res::String				id;
	res::String				name;
	int						type;

	union
	{
		SMesh				*pMesh;
		SSpline				*pSpline;
	};

	//core::aabbox3df			boundingBox;
};

struct SLibraryGeometries
{
	res::vector<SGeometry>		geometries;

};

struct SMorph
{
	res::String				source;
	u32						method;
	/*union
	{
		int					iSource;
		SGeometry			*pGeometry;
		SController			*pController;
	};*/

	res::vector<res::String>		targets;
	union
	{
		res::vector<int>		iTargets;
		res::vector<SGeometry*>	pTargets;
	};
	res::vector<float>			weights;
};

struct SSkin
{
	res::String					source;
	SMatrix						bindShapeMatrix;
	res::vector<res::String>	nodes;
	SVectorMatrix				bindPoses;
	res::vector<float>			weights;	// weight table
	res::vector<unsigned char>	vcount;		// number of influence per vertex
	res::vector<unsigned short>	v;			// {[jointId, weight]}
	res::vector<core::aabbox3df>	jointsBoundingBoxes;
};

struct SModularSkin
{
	res::vector<SInstanceController> sources;
};

struct SController
{
	enum Type
	{
		ESKIN = 0,
		EMORPH = 1,
		EMODULARSKIN = 2
	};

	u32						type;
	res::String				id;

	union
	{
		void				*pData;
		SMorph				*pMorph;
		SSkin				*pSkin;
		SModularSkin		*pModularSkin;
	};
};

struct SLibraryCameras
{
	res::vector<SCamera>		cameras;
};

struct SLibraryControllers
{
	res::vector<SController>	controllers;
};

struct SBillboard
{
	// Billboard data
	enum E_Billboard_Type
	{
		EBT_NONE,
		EBT_FREE,
		EBT_UP_ONLY,
	};

	enum E_Billboard_Orientation
	{
		EBO_NONE,
		EBO_TOWARD_CAMERA,
		EBO_PARALLEL_TO_CAMERA_AXIS,
	};

	int							type;
	int							orientation;
	core::vector3df				front;
	core::vector3df				up;
};

struct SNode
{
	res::String					id;
	res::String					name;
	res::String					sid;
	
	//SMatrix					transform;
	core::vector3df				position;
	core::quaternion			rotation;
	core::vector3df				scale;
	int							visibility;

	res::vector<SNode>			children;
	res::vector<SInstance>		instances;
	union
	{
		void					*pUserProperty;
		res::String				*pUserPropertyStr;
	};

	union
	{
		void					*pExtraTypeData;
		SBillboard				*pBillboard;
	};
};

struct SVisualScene
{
	res::String					id;
	res::String					name;
	res::vector<SNode>			nodes;
};

struct SLibraryVisualScenes
{
	res::vector<SVisualScene>	visualScenes;
};

struct SScene
{
	res::vector<SInstance>		instances;
};


struct SImageIFL
{
	int								iTotalTime;
	SAnimation						animation;

	int getCount() { return animation.sources[0].pData.size(); }
	video::ITexture *& getTexture(int i) { return (video::ITexture *&)animation.sources[1].pData[i]; }
	const res::String & getFilename(int i) { return animation.sources[2].sData[i]; }
};

struct SImage
{
	enum Type
	{
		CIT_SINGLE = 0,
		CIT_IFL = 1,
	};

	res::String	id;
	res::String	name;
	res::String	path;
	int			type;
	
	union
	{
		video::ITexture *pTexture;
		SImageIFL		*pIFL;
	};
};

struct SLibraryImages
{
	res::vector<SImage> images;
};

struct SBox
{
	float		length;
	float		width;
	float		height;
};

struct SSphere
{
	float		radius;
};

struct SCylinder
{
	float		radius;
	float		height;
};

struct STarget
{

};

struct SShapeElement
{
	enum Type
	{
		SE_BOX = 0,
		SE_SPHERE = 1,
		SE_CYLINDER = 2,
		SE_TARGET = 3,
	};

	int iType;
	union
	{
		SBox		*pBox;
		SSphere		*pSphere;
		SCylinder	*pCylinder;
		STarget		*pTarget;
	};
};

struct SShape
{
	SShapeElement	shapeElement;
};

struct SBirth
{
	enum Type
	{
		BT_RATE = 0,
		BT_TOTAL = 1,
	};
	int	type;
	union
	{
		int total;
		float rate;
	};	
};

struct SEmission
{
	float	emitStart;
	float	emitStop;
	int		maxParticles;
	float	displayUntil;
	float	birthRate;
};

struct SRandomization
{
	int		seed;
};

struct SParticleLife
{
	float	life;
	float	lifeVariance;
};

struct SParticleSize
{
	float	size;
	float	sizeVariance;
	float	sizeGrowFor;
	float	sizeFadeFor;
};



struct SVariableVector
{
	core::vector3df	vector;
	float			variance;
};

struct SDirectionTarget
{
	res::String		url;
	float			variance;
};

struct SDirection
{
	enum Type
	{
		DT_RANDOM_VECTOR = 0,
		DT_VARIABLE_VECTOR = 1,
		DT_DIRECTION_TARGET = 2,
	};
	int			type;
	union
	{
		SVariableVector		*pVariableVector;
		SDirectionTarget	*pDirectionTarget;
	};
};

struct SParticleMotion
{
	float		speed;
	float		speedVariance;
	SDirection	direction;
};

struct SStandardParticle
{
	enum Type
	{
		SPT_TRIANGLE = 0,
		SPT_CUBE = 1,
		SPT_SPECIAL = 2,
		SPT_FACING = 3,
		SPT_CONSTANT = 4,
		SPT_TETRAHEDRON = 5,
		SPT_SIX_POINTED_STAR = 6,
		SPR_SPHERE = 7,
	};

	int	type;
};

struct SMetaParticle
{
	void *to_be_determined;
};

struct SInstancedGeometryParticle
{
	void *to_be_determined;
};

struct SParticleType
{
	enum Type
	{
		PT_STANDARD_PARTICULE = 0,
		PT_META_PARTICULE = 1,
		PT_INSTANCED_GEOMETRY_PARTICULE = 2,
	};

	int			type;
	union
	{
		SStandardParticle			*pStandardParticle;
		SMetaParticle				*pMetaParticle;
		SInstancedGeometryParticle	*pInstancedGeometryParticle;
	};
};

struct SRandomVector
{
	
};

struct STravelOrMBlurDir
{
	float stretch;
};

struct SAxis
{
	enum Type
	{
		AT_RANDOM_VECTOR = 0,
		AT_VARIABLE_VECTOR = 1,
		AT_TRAVEL_OR_MBLUR_DIR = 2,
	};
	int			type;
	union
	{
		SVariableVector		*pVariableVector;
		STravelOrMBlurDir	*pTravelOrMBlurDir;
	};
};

struct SParticleSpin
{
	float				time;
	float				timeVariance;
	float				phase;
	float				phaseVariance;
	SAxis				axis;
};

struct SWind
{
	float				strength;
	float				decay;
	int					type;
	float				turbulence;
	float				frequency;
	float				scale;
};

struct SGravity
{
	float				strength;
	float				decay;
	int					type;
};

struct SDeflector
{
	float				bounce;
	float				variation;
	float				chaos;
	float				friction;
	float				inheritVelocity;
	float				width;
	float				height;
};

struct SForce
{
	enum Type
	{
		FT_GRAVITY = 0,
		FT_WIND = 1,
		FT_DEFLECTOR = 2,
	};

	res::String			id;
	res::String			name;
	int					type;
	union
	{
		SWind				*pWind;
		SGravity			*pGravity;
		SDeflector			*pDeflector;
	};
};

struct SLibraryForces
{
	res::vector<SForce> forces;
};

struct SInstanceForce 
{
	res::String				filename;
	res::String				url;
	SForce					*pForce;
};

struct SParticleAnimationOffset
{
	enum E_METHOD
	{
		EM_NONE = 0,
		EM_BIRTH = 1,
	};	

	E_METHOD				method;
	float					offset;
	float					offsetVariance;
};

struct SParticleAnimationScaling
{
	enum E_METHOD
	{
		EM_NONE = 0,
		EM_LIFE = 1,
	};

	E_METHOD				method;

	float					srcLength;
	float					srcVariance;

	float					dstMultiplier;
	float					dstVariance;
};

struct SEmitter
{
	res::String					id;
	res::String					name;
	SShape						shape;
	SEmission					emission;
	SRandomization				randomization;
	SParticleLife				particleLife;
	SParticleSize				particleSize;
	SParticleMotion				particleMotion;
	SParticleType				particleType;
	SParticleAnimationOffset	particleAnimationOffset;
	SParticleAnimationScaling	particleAnimationScaling;
	SParticleSpin				particleSpin;
};

struct SInstanceEmitter
{
	res::String					filename;
	res::String					url;
	SEmitter					*pEmitter;
	SBindMaterial				bindMaterial;
	int							drawLayer;
	res::vector<res::String>	forceNodes;
};

struct SLibraryEmitters
{
	res::vector<SEmitter> emitters;
};

struct SConfig
{

	enum E_UpVector
	{
		X_UP = 0,
		Y_UP = 1,
		Z_UP = 2,
	};
	E_UpVector upVector;
	int bPostLoadingDone;
	int samplingRate;
};

struct SCollada
{
	const char *			version;
	SConfig					configurations;
	SLibraryAnimations		libraryAnimations;
	SLibraryAnimationClips	libraryAnimationClips;
	SLibraryCameras			libraryCameras;
	SLibraryLights			libraryLights;
	SLibraryImages			libraryImages;
	SLibraryEffects			libraryEffects;
	SLibraryMaterials		libraryMaterials;
	SLibraryGeometries		libraryGeometries;
	SLibraryControllers		libraryControllers;
	SLibraryEmitters		libraryEmitters;
	SLibraryForces			libraryForces;
	SLibraryVisualScenes	libraryVisualScene;
	SScene					scene;
};


//! Multiply by another matrix.
template <class T>
inline core::CMatrix4<T> operator*(const core::CMatrix4<T>& mat, const collada::SMatrix& other)
{
	core::CMatrix4<T> mat2(core::CMatrix4<T>::EM4CONST_NOTHING);

	if ( mat.getDefinitelyIdentityMatrix() )
	{
		memcpy(mat2.pointer(), other.M, sizeof(collada::SMatrix));
	}
	else
	{
		core::CMatrix4<T>::rowMatrixProduct34(mat2.pointer(), mat.pointer(), other.M);
	}
	
	return mat2;
};

//! Multiply by another matrix.
template <class T>
inline core::CMatrix4<T> operator*(const collada::SMatrix& other, const core::CMatrix4<T>& mat)
{
	core::CMatrix4<T> mat2(core::CMatrix4<T>::EM4CONST_NOTHING);;

	if ( mat.getDefinitelyIdentityMatrix() )
	{
		memcpy(mat2.pointer(), other.M, sizeof(collada::SMatrix));
	}
	else
	{
		core::CMatrix4<T>::rowMatrixProduct34(mat2.pointer(), other.M, mat.pointer());
	}
	
	return mat2;
};

}; // namespace collada

}; // namespace irr




#endif
