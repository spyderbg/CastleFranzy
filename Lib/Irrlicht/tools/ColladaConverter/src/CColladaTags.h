#ifndef __CCOLLADATAGS_H__
#define __CCOLLADATAGS_H__

#include "irrString.h"

namespace irr
{
namespace collada
{
namespace tags
{
static const core::stringc collada							= "COLLADA";
static const core::stringc library							= "library";
static const core::stringc libraryNodes						= "library_nodes";
static const core::stringc libraryAnimations				= "library_animations";
static const core::stringc libraryAnimationClips			= "library_animation_clips";
static const core::stringc libraryGeometries				= "library_geometries";
static const core::stringc libraryControllers				= "library_controllers";
static const core::stringc libraryMaterials					= "library_materials";
static const core::stringc libraryImages					= "library_images";
static const core::stringc libraryVisualScenes				= "library_visual_scenes";
static const core::stringc libraryCameras					= "library_cameras";
static const core::stringc libraryLights					= "library_lights";
static const core::stringc libraryEffects					= "library_effects";
static const core::stringc asset							= "asset";
static const core::stringc comments							= "comments";
static const core::stringc upAxis							= "up_axis";
static const core::stringc authoringTool					= "authoring_tool";
static const core::stringc author							= "author";
static const core::stringc sourceData						= "source_data";
static const core::stringc scene							= "scene";
static const core::stringc visualScene						= "visual_scene";

static const core::stringc light							= "light";
static const core::stringc animation						= "animation";
static const core::stringc animationClip					= "animation_clip";
static const core::stringc camera							= "camera";
static const core::stringc material							= "material";
static const core::stringc geometry							= "geometry";
static const core::stringc controller						= "controller";
static const core::stringc image							= "image";
static const core::stringc texture							= "texture";
static const core::stringc effect							= "effect";
static const core::stringc mirrorU							= "mirrorU";
static const core::stringc mirrorV							= "mirrorV";
static const core::stringc repeatU							= "repeatU";
static const core::stringc repeatV							= "repeatV";
static const core::stringc offsetU							= "offsetU";
static const core::stringc offsetV							= "offsetV";
static const core::stringc rotateUV							= "rotateUV";
static const core::stringc wrapU							= "wrapU";
static const core::stringc wrapV							= "wrapV";

static const core::stringc ambient							= "ambient";
static const core::stringc diffuse							= "diffuse";
static const core::stringc specular							= "specular";
static const core::stringc shininess						= "shininess";
static const core::stringc reflective						= "reflective";
static const core::stringc reflectivity						= "reflectivity";
static const core::stringc transparent						= "transparent";
static const core::stringc transparency						= "transparency";
static const core::stringc specLevel						= "spec_level";
static const core::stringc emissionLevel					= "emission_level";
static const core::stringc point							= "point";
static const core::stringc spot								= "spot";
static const core::stringc directional						= "directional";
static const core::stringc input							= "input";
static const core::stringc targets							= "targets";
static const core::stringc idrefArray						= "IDREF_array";
static const core::stringc nameArray						= "Name_array";
static const core::stringc morph							= "morph";
static const core::stringc skin								= "skin";
static const core::stringc jointsBoundingBoxes				= "joints_bounding_boxes";
static const core::stringc mesh								= "mesh";
static const core::stringc spline							= "spline";
static const core::stringc bindShapeMatrix					= "bind_shape_matrix";
static const core::stringc joints							= "joints";
static const core::stringc vertexWeights					= "vertex_weights";
static const core::stringc vcount							= "vcount";
static const core::stringc v								= "v";
static const core::stringc source							= "source";
static const core::stringc wrapS							= "wrap_s";
static const core::stringc wrapT							= "wrap_t";
static const core::stringc minfilter						= "minfilter";
static const core::stringc magfilter						= "magfilter";
static const core::stringc mipfilter						= "mipfilter";
static const core::stringc sampler							= "sampler";
static const core::stringc channel							= "channel";
static const core::stringc array							= "array";
static const core::stringc floatArray						= "float_array";
static const core::stringc intArray							= "int_array";
static const core::stringc techniqueCommon					= "technique_common";
static const core::stringc perspective						= "perspective";
static const core::stringc optics							= "optics";
static const core::stringc target							= "target";
static const core::stringc accessor							= "accessor";
static const core::stringc vertices							= "vertices";
static const core::stringc polylist							= "polylist";
static const core::stringc triangles						= "triangles";
static const core::stringc linestrips						= "linestrips";
static const core::stringc polygons							= "polygons";
static const core::stringc p								= "p";

static const core::stringc node								= "node";
static const core::stringc lookat							= "lookat";
static const core::stringc matrix							= "matrix";
static const core::stringc rotate							= "rotate";
static const core::stringc scale							= "scale";
static const core::stringc visibility						= "visibility";
static const core::stringc translate						= "translate";
static const core::stringc skew								= "skew";
static const core::stringc boundingbox						= "boundingbox";
static const core::stringc min								= "min";
static const core::stringc max								= "max";
static const core::stringc instance							= "instance";
static const core::stringc instanceAnimation				= "instance_animation";
static const core::stringc instanceCamera					= "instance_camera";
static const core::stringc instanceController				= "instance_controller";
static const core::stringc instanceModularSkinMaster		= "instance_modular_skin_master";
static const core::stringc instanceGeometry					= "instance_geometry";
static const core::stringc instanceVisualScene				= "instance_visual_scene";
static const core::stringc instanceEffect					= "instance_effect";
static const core::stringc instanceMaterial					= "instance_material";
static const core::stringc instanceLight					= "instance_light";
static const core::stringc instanceNode						= "instance_node";
static const core::stringc skeleton							= "skeleton";
static const core::stringc bindMaterial						= "bind_material";
static const core::stringc extra							= "extra";
static const core::stringc technique						= "technique";
static const core::stringc constantAttenuation				= "constant_attenuation";
static const core::stringc linearAttenuation				= "linear_attenuation";
static const core::stringc quadraticAttenuation				= "quadratic_attenuation";
static const core::stringc falloffAngle						= "falloffAngle";
static const core::stringc falloffExponent					= "falloffExponent";
static const core::stringc aspectRatio						= "aspect_ratio";
static const core::stringc orthographic						= "orthographic";
static const core::stringc xfov								= "xfov";
static const core::stringc xmag								= "xmag";
static const core::stringc znear							= "znear";
static const core::stringc zfar								= "zfar";
static const core::stringc color							= "color";
static const core::stringc intensity						= "intensity";
static const core::stringc _float							= "float";
static const core::stringc _float2							= "float2";
static const core::stringc _float3							= "float3";

static const core::stringc surface							= "surface";
static const core::stringc sampler2D						= "sampler2D";
static const core::stringc setparam							= "setparam";
static const core::stringc newparam							= "newparam";
static const core::stringc param							= "param";
static const core::stringc initFrom							= "init_from";
static const core::stringc dat								= "data";

static const core::stringc doubleSided						= "double_sided";
static const core::stringc userProperties					= "user_properties";

static const core::stringc profileCOMMON					= "profile_COMMON";

static const core::stringc drawLayer						= "draw_layer";

static const core::stringc length							= "length";
static const core::stringc width							= "width";
static const core::stringc height							= "height";
static const core::stringc box								= "box";
static const core::stringc sphere							= "sphere";
static const core::stringc cylinder							= "cylinder";
static const core::stringc radius							= "radius";
static const core::stringc shapeElement						= "shape_element";
static const core::stringc shape							= "shape";
static const core::stringc rate								= "rate";
static const core::stringc total							= "total";
static const core::stringc birthRate						= "birth_rate";
static const core::stringc emitStart						= "emit_start";
static const core::stringc emitStop							= "emit_stop";
static const core::stringc maxParticles						= "max_particles";
static const core::stringc displayUntil						= "display_until";
static const core::stringc emission							= "emission";
static const core::stringc seed								= "seed";
static const core::stringc randomization					= "randomization";
static const core::stringc life								= "life";
static const core::stringc lifeVariance						= "life_variance";
static const core::stringc particleLife						= "particle_life";
static const core::stringc size								= "size";
static const core::stringc sizeVariance						= "size_variance";
static const core::stringc sizeGrowFor						= "size_grow_for";
static const core::stringc sizeFadeFor						= "size_fade_for";
static const core::stringc particleSize						= "particle_size";
static const core::stringc vector							= "vector";
static const core::stringc variance							= "variance";
static const core::stringc variableVector					= "variable_vector";
static const core::stringc directionTarget					= "direction_target";
static const core::stringc randomVector						= "random_vector";
static const core::stringc direction						= "direction";
static const core::stringc speed							= "speed";
static const core::stringc speedVariance					= "speed_variance";
static const core::stringc particleMotion					= "particle_motion";
static const core::stringc stdParticleType					= "std_particle_type";
static const core::stringc standardParticle					= "standard_particle";
static const core::stringc metaParticle						= "meta_particle";
static const core::stringc instancedGeometryParticle		= "instanced_geometry_particle";
static const core::stringc particleType						= "particle_type";
static const core::stringc stretch							= "stretch";
static const core::stringc travelOrMBlurDir					= "travel_or_mblur_dir";
static const core::stringc axis								= "axis";
static const core::stringc time								= "time";
static const core::stringc timeVariance						= "time_variance";
static const core::stringc phase							= "phase";
static const core::stringc phaseVariance					= "phase_variance";
static const core::stringc particleSpin						= "particle_spin";
static const core::stringc particleAnimationOffset			= "particle_animation_offset";
static const core::stringc particleAnimationScaling			= "particle_animation_scaling";
static const core::stringc id								= "id";
static const core::stringc name								= "name";
static const core::stringc instanceEmitter					= "instance_emitter";
static const core::stringc emitter							= "emitter";
static const core::stringc libraryEmitters					= "library_emitters";
static const core::stringc method							= "method";
static const core::stringc offset							= "offset";
static const core::stringc offsetVariance					= "offset_variance";
static const core::stringc srcLength						= "src_length";
static const core::stringc srcVariance						= "src_variance";
static const core::stringc dstMultiplier					= "dst_multiplier";
static const core::stringc dstVariance						= "dst_variance";

static const core::stringc instanceForce					= "instance_force";
static const core::stringc force							= "force";
static const core::stringc forceNode						= "force_node";
static const core::stringc libraryForces					= "library_forces";
static const core::stringc wind								= "wind";
static const core::stringc gravity							= "gravity";
static const core::stringc forceStrength					= "force_strength";
static const core::stringc forceDecay						= "force_decay";
static const core::stringc forceType						= "force_type";
static const core::stringc windTurbulence					= "wind_turbulence";
static const core::stringc windFrequency					= "wind_frequency";
static const core::stringc windScale						= "wind_scale";

static const core::stringc deflector						= "deflector";
static const core::stringc bounce							= "bounce";
static const core::stringc variation						= "variation";
static const core::stringc chaos							= "chaos";
static const core::stringc friction							= "friction";
static const core::stringc inheritVelocity					= "inherit_velocity";

static const core::stringc billboard						= "billboard";
static const core::stringc type								= "type";
static const core::stringc orientation						= "orientation";
static const core::stringc front							= "front";
static const core::stringc up								= "up";
static const core::stringc upOnly							= "UP_ONLY";
static const core::stringc free								= "FREE";
static const core::stringc towardCamera						= "TOWARD_CAMERA";
static const core::stringc parallelCamAxis					= "PARALLEL_CAM_AXIS";

static const core::stringc skinnedMeshCategories			= "skinned_mesh_categories";
static const core::stringc category							= "instance_modular_skin_category";
static const core::stringc skinnedMeshes					= "skinned_meshes";

static const core::stringc defaultValues					= "default_values";
}; // namespace tags
}; // namespace collada
}; // namespace irr

#endif