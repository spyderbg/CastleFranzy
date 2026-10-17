//-*-c++-*-
#ifndef __C_WII_MATERIAL_RENDERER_H_INCLUDED__
#define __C_WII_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_USE_WII_DEVICE_

#include "CWiiDriver.h"
#include "IMaterialRenderer.h"

#include <revolution.h>

namespace irr
{
namespace video
{

class CWiiMaterialRenderer : public IMaterialRenderer
{
public:

	//! Constructor
	CWiiMaterialRenderer(video::CWiiDriver* driver)
		: Driver(driver)
	{
	}

protected:

	void baseTevSetup(const SMaterial& material,
					  const SMaterial& lastMaterial,
					  bool resetAllRenderstates);

	video::CWiiDriver* Driver;
};

//! Solid material renderer
class CWiiMaterialRenderer_SOLID : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_SOLID(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	virtual u32 getVertexAttributeMask() const;
};

//! Generic Texture Blend
class CWiiMaterialRenderer_ONETEXTURE_BLEND : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_ONETEXTURE_BLEND(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	virtual u32 getVertexAttributeMask() const;
};

//! Solid 2 layer material renderer
class CWiiMaterialRenderer_SOLID_2_LAYER : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_SOLID_2_LAYER(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	virtual u32 getVertexAttributeMask() const;
};

//! Solid 2 layer material (with decal) renderer
class CWiiMaterialRenderer_SOLID_2_LAYER_DECAL : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_SOLID_2_LAYER_DECAL(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	virtual u32 getVertexAttributeMask() const;
};

//! Transparent add color material renderer
class CWiiMaterialRenderer_TRANSPARENT_ADD_COLOR : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_TRANSPARENT_ADD_COLOR(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

	virtual u32 getVertexAttributeMask() const;
};

//! Transparent vertex alpha material renderer
class CWiiMaterialRenderer_TRANSPARENT_VERTEX_ALPHA : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_TRANSPARENT_VERTEX_ALPHA(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

	virtual u32 getVertexAttributeMask() const;
};


//! Transparent alpha channel material renderer
class CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

	virtual u32 getVertexAttributeMask() const;
};

//! Transparent alpha channel material renderer
class CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

	virtual u32 getVertexAttributeMask() const;
};

//! material renderer for all kinds of lightmaps
class CWiiMaterialRenderer_LIGHTMAP : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_LIGHTMAP(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	virtual u32 getVertexAttributeMask() const;
};

//! detail map  material renderer
class CWiiMaterialRenderer_DETAIL_MAP : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_DETAIL_MAP(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	void onSetMaterial(const SMaterial& material,
					   const SMaterial& lastMaterial,
					   bool resetAllRenderstates,
					   IMaterialRendererServices* services);

	virtual u32 getVertexAttributeMask() const;
};

//! sphere map material renderer
class CWiiMaterialRenderer_SPHERE_MAP : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_SPHERE_MAP(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	virtual u32 getVertexAttributeMask() const;
};

//! reflection 2 layer material renderer
class CWiiMaterialRenderer_REFLECTION_2_LAYER : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_REFLECTION_2_LAYER(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	virtual u32 getVertexAttributeMask() const;
};

//! reflection 2 layer material renderer
class CWiiMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

	virtual u32 getVertexAttributeMask() const;
};

//! Transparent alpha channel material renderer
class CWiiMaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_TRANSPARENT_TEXTURE_VERTEX_ALPHA(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

	virtual u32 getVertexAttributeMask() const;
};

//! Transparent alpha channel material renderer
class CWiiMaterialRenderer_2D_ALPHA : public CWiiMaterialRenderer
{
public:

	CWiiMaterialRenderer_2D_ALPHA(video::CWiiDriver* d)
		: CWiiMaterialRenderer(d) {}

	virtual void onSetMaterial(const SMaterial& material,
							   const SMaterial& lastMaterial,
							   bool resetAllRenderstates,
							   IMaterialRendererServices* services);

	//virtual void onUnsetMaterial();

	//! Returns if the material is transparent.
	virtual bool isTransparent() const;

	virtual u32 getVertexAttributeMask() const;
};

} // end namespace video
} // end namespace irr

#endif
#endif

