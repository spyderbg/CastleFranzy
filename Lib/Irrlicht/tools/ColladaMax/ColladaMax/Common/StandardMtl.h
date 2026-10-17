/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _STANDARD_MTL_H_
#define _STANDARD_MTL_H_

// This information is taken from the 'StdMat2' sample.
// Note : Some of these enums are used to export StdMat2 and ICMaterial
// If they need to be modified, make sure export works fine for these two materials.
namespace FSStandardMaterial
{
	// reference numbers
	const uint32 OLD_PBLOCK_REF = 0;
	const uint32 TEXMAPS_REF = 1;
	const uint32 SHADER_REF = 2;
	const uint32 SHADER_PB_REF = 3;
	const uint32 EXTENDED_PB_REF = 4;
	const uint32 SAMPLING_PB_REF = 5;
	const uint32 MAPS_PB_REF = 6;
	const uint32 DYNAMICS_PB_REF = 7;		// [glaforte] typo corrected, so you won't find this #define in the MaxSDK!
	const uint32 SAMPLER_REF = 8;
	const uint32 RENDER_STATES_PB_REF = 9;
	const uint32 NUM_REFS = 10;


	// shdr_params param IDs
	enum 
	{ 
		shdr_ambient, shdr_diffuse, shdr_specular,
		shdr_ad_texlock, shdr_ad_lock, shdr_ds_lock, 
		shdr_use_self_illum_color, shdr_self_illum_amnt, shdr_self_illum_color, 
		shdr_spec_lvl, shdr_glossiness, shdr_soften
	};

	// std2_shader param IDs
	enum 
	{ 
		std2_shader_type, std2_wire, std2_two_sided, std2_face_map, std2_faceted,
		std2_shader_by_name,  // virtual param for accessing shader type by name
	};

	// std2_extended param IDs
	enum 
	{ 
		std2_opacity_type, std2_opacity, std2_filter_color, std2_ep_filter_map,
		std2_falloff_type, std2_falloff_amnt, 
		std2_ior,
		std2_wire_size, std2_wire_units,
		std2_apply_refl_dimming, std2_dim_lvl, std2_refl_lvl,
		std2_translucent_blur, std2_ep_translucent_blur_map,
		std2_translucent_color, std2_ep_translucent_color_map,
	};

	// std2_sampling param IDs
	enum 
	{ 
		std2_ssampler, std2_ssampler_qual, std2_ssampler_enable, 
		std2_ssampler_adapt_on, std2_ssampler_adapt_threshold, std2_ssampler_advanced,
		std2_ssampler_subsample_tex_on, std2_ssampler_by_name, 
		std2_ssampler_param0, std2_ssampler_param1, std2_ssampler_useglobal
	};

	// std2_maps param IDs
	enum 
	{
		std2_map_enables, std2_maps, std2_map_amnts, std2_mp_ad_texlock, 
	};

	// std2_dynamics param IDs
	enum 
	{
		std2_bounce, std2_static_friction, std2_sliding_friction,
	};

	enum RenderStateParamBlockVersion
	{
		RSPBV_000
		, RSPBV_001
		, RSPBV_CURRENT = RSPBV_001
	};

	enum PID
	{
		P_ALPHA_FUNC_FUNCTION
		, P_ALPHA_FUNC_VALUE

		, P_BLEND_FUNC_SRC
		, P_BLEND_FUNC_DST

		, P_BLEND_FUNC_SEPARATE_SRC_COLOR
		, P_BLEND_FUNC_SEPARATE_DST_COLOR
		, P_BLEND_FUNC_SEPARATE_SRC_ALPHA
		, P_BLEND_FUNC_SEPARATE_DST_ALPHA

		, P_BLEND_EQUATION

		, P_BLEND_EQUATION_SEPARATE_COLOR
		, P_BLEND_EQUATION_SEPARATE_ALPHA

		, P_COLOR_MATERIAL_FACE
		, P_COLOR_MATERIAL_TYPE

		, P_CULL_FACE
		, P_DEPTH_FUNC
		, P_FOG_MODE
		, P_FOG_COORD_SRC

		, P_FRONT_FACE
		, P_LIGHT_MODEL_COLOR_CONTROL
		, P_LOGIC_OP

		, P_POLYGON_MODE_FACE
		, P_POLYGON_MODE_MODE

		, P_SHADE_MODEL

		, P_STENCIL_FUNC_FUNCTION
		, P_STENCIL_FUNC_REFERENCE_VALUE
		, P_STENCIL_FUNC_MASK

		, P_STENCIL_OP_FAIL_OPERATION
		, P_STENCIL_OP_DEPTH_FAIL_OPERATION
		, P_STENCIL_OP_DEPTH_PASS_OPERATION
		
		, P_STENCIL_FUNC_SEPARATE_FRONT_FACES_FUNCTION
		, P_STENCIL_FUNC_SEPARATE_BACK_FACES_FUNCTION
		, P_STENCIL_FUNC_SEPARATE_REFERENCE_VALUE
		, P_STENCIL_FUNC_SEPARATE_MASK

		, P_STENCIL_OP_SEPARATE_FACE
		, P_STENCIL_OP_SEPARATE_FAIL_OPERATION
		, P_STENCIL_OP_SEPARATE_DEPTH_FAIL_OPERATION
		, P_STENCIL_OP_SEPARATE_DEPTH_PASS_OPERATION
		
		, P_STENCIL_MASK_SEPARATE_FACE
		, P_STENCIL_MASK_SEPARATE_MASK

		// LIGHT_ENABLE
		// LIGHT_AMBIENT
		// LIGHT_DIFFUSE
		// LIGHT_SPECULAR
		// LIGHT_POSITION
		// LIGHT_CONSTANT_ATTENUATION
		// LIGHT_LINEAR_ATTENUATION
		// LIGHT_QUADRATIC_ATTENUATION
		// LIGHT_SPOT_CUTOFF
		// LIGHT_SPOT_DIRECTION
		// LIGHT_SPOT_EXPONENT

		// TEXTURE1D
		// TEXTURE2D
		// TEXTURE3D
		// TEXTURECUBE
		// TEXTURERECT
		// TEXTUREDEPTH
		// TEXTURE1D_ENABLE
		// TEXTURE2D_ENABLE
		// TEXTURE3D_ENABLE
		// TEXTURECUBE_ENABLE
		// TEXTURERECT_ENABLE
		// TEXTUREDEPTH_ENABLE
		// TEXTURE_ENV_COLOR
		// TEXTURE_ENV_MODE

		// CLIP_PLANE
		// CLIP_PLANE_ENABLE
		// BLEND_COLOR
		// CLEAR_COLOR
		// CLEAR_STENCIL
		// CLEAR_DEPTH
		, P_COLOR_MASK
		// DEPTH_BOUNDS
		, P_DEPTH_MASK
		// DEPTH_RANGE

		// FOG_DENSITY
		// FOG_START
		// FOG_END
		// FOG_COLOR
		// LIGHT_MODEL_AMBIENT
		, P_LIGHTING_ENABLE
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
		, P_ALPHA_TEST_ENABLE
		, P_AUTO_NORMAL_ENABLE
		, P_BLEND_ENABLE
		, P_COLOR_LOGIC_OP_ENABLE
		, P_COLOR_MATERIAL_ENABLE
		, P_CULL_FACE_ENABLE

		, P_DEPTH_BOUNDS_ENABLE
		, P_DEPTH_CLAMP_ENABLE
		, P_DEPTH_TEST_ENABLE
		, P_DITHER_ENABLE
		, P_FOG_ENABLE
		, P_LIGHT_MODEL_LOCAL_VIEWER_ENABLE
		, P_LIGHT_MODEL_TWO_SIDE_ENABLE
		, P_LINE_SMOOTH_ENABLE
		, P_LINE_STIPPLE_ENABLE
		, P_LOGIC_OP_ENABLE
		, P_MULTISAMPLE_ENABLE

		, P_NORMALIZE_ENABLE
		, P_POINT_SMOOTH_ENABLE
		, P_POLYGON_OFFSET_FILL_ENABLE
		, P_POLYGON_OFFSET_LINE_ENABLE
		, P_POLYGON_OFFSET_POINT_ENABLE
		, P_POLYGON_SMOOTH_ENABLE
		, P_POLYGON_STIPPLE_ENABLE
		, P_RESCALE_NORMAL_ENABLE

		, P_SAMPLE_ALPHA_TO_COVERAGE_ENABLE
		, P_SAMPLE_ALPHA_TO_ONE_ENABLE
		, P_SAMPLE_COVERAGE_ENABLE
		, P_SCISSOR_TEST_ENABLE
		, P_STENCIL_TEST_ENABLE

		, P_TEXTURE_PIPELINE_ENABLE
		// TEXTURE_PIPELINE
	};

	enum ShaderClassID
	{
		STD2_PHONG_CLASS_ID					= 0x00000037,
		STD2_BLINN_SHADER_CLASS_ID			= 0x00000038,
		STD2_METAL_SHADER_CLASS_ID			= 0x00000039,
		STD2_ANISOTROPIC_SHADER_CLASS_ID	= 0x2857f460,
		STD2_MULTI_LAYER_SHADER_CLASS_ID	= 0x2857f470,
		STD2_OREN_NAYER_BLINN_CLASS_ID		= 0x2857f421
	};
}

namespace FSStdUVGen
{
	enum { pblock }; 

	// paramblock subanims
	enum 
	{
		u_offset,
		v_offset,
		u_tiling,
		v_tiling,
		u_angle,
		v_angle,
		w_angle,
		blur,
		noise_amt,
		noise_size,
		noise_level,
		phase,
		blur_offset,
		num_subs
	};
}

#endif // _STANDARD_MTL_H_