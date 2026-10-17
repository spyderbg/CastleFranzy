#ifndef __IRR_PS_MANAGER_H__
#define __IRR_PS_MANAGER_H__

#include "ParticleSystem.h"
#include "PBehaviorModel.h"
#include "Particle.h"
#include "PForceImpl.h"

namespace irr
{
namespace ps
{

class PSManager
{
private:
	PSManager() {}
	PSManager(const PSManager&);
	const PSManager& operator=(const PSManager&);

	void reallocateBuffers(size_t size);
public:
	~PSManager() {}

	static PSManager& getInstance()
	{
		static PSManager s_Instance;
		return s_Instance;
	}

	//NOTE-FH: Might modify template parameters
	template<class PType, class GenM, class SizeM, class ColorM, class EmitterM, class MotionM, class ForceM, class SpinM, class LifeM, class RenderM>
	IParticleSystem<PType>* createParticleSystem()
	{
		typedef struct
			: public IParticleSystem<PType>
			, public GenM
			, public SizeM
			, public ColorM
			, public EmitterM
			, public MotionM
			, public ForceM
			, public SpinM
			, public LifeM
			, public RenderM
		{
		} Mixin;
		return irrnew Mixin();
	}

	IParticleSystem<SParticle>* createPCloudSystem()
	{
		return createParticleSystem<SParticle,
									PGenerationModel<SParticle>,
									PSizeModel<SParticle>,
									PColorModel<SParticle>,
									PEmitterModel<SParticle>,
									PMotionModel<SParticle>,
									PForcesModel<SParticle>,
									PSpinModel<SParticle>,
									PLifeModel<SParticle>,
									PRenderDataBillboardModel<SParticle> >();
	}
};


} // end namespace ps
} // end namespace irr

#endif //__IRR_PS_MANAGER_H__