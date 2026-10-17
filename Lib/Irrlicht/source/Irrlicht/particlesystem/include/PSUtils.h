#ifndef __IRR_PS_UTILS_H__
#define __IRR_PS_UTILS_H__

#include "vector3d.h"

//Compiler like MetroWerks CodeWarrior seems not to notice the templated parent typedefs
#define DECLARE_PARTICLE_CONTEXT_TYPEDEF(PType) \
public: \
	typedef typename IParticleContext<PType>	PSuper; \
	typedef typename PSuper::time_t				time_t;
	//typedef typename PSuper::ParticleList		ParticleList; \
	//typedef typename PSuper::ParticleItt		ParticleItt; \
	//typedef typename PSuper::ParticleConstItt	ParticleConstItt; \
	//typedef typename PSuper::ParamMap			ParamMap; \
	//typedef typename PSuper::ParamItt			ParamItt; \
	//typedef typename PSuper::PRenderData_t	PRenderData_t;


namespace irr
{
namespace ps
{

extern void* PS_NEW(size_t size);
extern void PS_FREE(void* p);


///Some math defines and utils
///////////////////////////////////////////
#ifndef M_PI
#define M_PI 3.1415926535897932384626433f
#endif
const float P_PLANAR_EPSILON = 1e-3f;	///< How small the dot product must be to declare that a point is in a plane for Within().
const float P_SQRT2PI = 2.506628274631000502415765284811045253006f;
const float P_ONEOVERSQRT2PI = (1.f / P_SQRT2PI);

inline float fsqr(float f) { return f * f; }
inline bool pSameSign(const float &a, const float &b) { return a * b >= 0.0f; }

// Compute the inverse matrix of the plane basis.
static inline void NewBasis(const core::vector3df &u, const core::vector3df &v, core::vector3df &s1, core::vector3df &s2)
{
    core::vector3df w = u.crossProduct(v);

    float det = 1.0f / (w.Z*u.X*v.Y - w.Z*u.Y*v.X - u.Z*w.X*v.Y - u.X*v.Z*w.Y + v.Z*w.X*u.Y + u.Z*v.X*w.Y);

    s1 = core::vector3df((v.Y*w.Z - v.Z*w.Y), (v.Z*w.X - v.X*w.Z), (v.X*w.Y - v.Y*w.X));
    s1 *= det;
    s2 = core::vector3df((u.Y*w.Z - u.Z*w.Y), (u.Z*w.X - u.X*w.Z), (u.X*w.Y - u.Y*w.X));
    s2 *= -det;
}

///Some random number generation utils
///////////////////////////////////////////

// To offset [0 .. 1] vectors to [-.5 .. .5]
static core::vector3df vHalf(0.5, 0.5, 0.5);

//TODO-FH: Seed generator
#define DEFAULT_SEED 123456789

class PSRandom
{
private:
	static const long mod = 2147483647;
	static const long mul = 48271;
	long m_seed;
	long m_start;
public:
	PSRandom(long seed = DEFAULT_SEED)
		: m_seed(seed)
		, m_start(seed)
	{}
	~PSRandom() {}

	void Reset() {
		m_seed = m_start;
	}

	void Seed(long x) {
		if (x) {
			m_start = x;
			m_seed = x;
		}
		else {
			//TODO-FH: Plant seed
		}
	}

	double Rand() {
		const long q = mod / mul;
		const long r = mod % mul;
		m_seed = mul * (m_seed % q) - r * (m_seed / q);
		if (m_seed < 0)
			m_seed += mod;
		return (static_cast<double>(m_seed) / mod);
	}

	float Randf() {
		return static_cast<float>(Rand());
	}

	float NRandf(float sigma = 1.0f)
	{
		float x, y, r2;
		do {
			x = Randf()*2.0f-1.0f;
			y = Randf()*2.0f-1.0f;
			r2 = x*x+y*y;
		}
		while(r2 > 1.0f || r2 == 0.0f);
		float m = sqrtf(-2.0f * logf(r2)/r2);
		float px = x*m*sigma;
		return px;
	}

	core::vector3df RandVec() {
		return core::vector3df(Randf(), Randf(), Randf());
	}

	core::vector3df NRandVec(float sigma = 1.0f) {
		float x, y, r2;
		do {
			x = Randf()*2.0f-1.0f;
			y = Randf()*2.0f-1.0f;
			r2 = x*x+y*y;
		}
		while(r2 > 1.0f || r2 == 0.0f);
		float m = sqrtf(-2.0f * logf(r2)/r2);
		float px = x*m*sigma;
		float py = y*m*sigma;
		return core::vector3df(px, py, NRandf(sigma));
	}
};

const float P_ONEOVER_RAND_MAX = (1.0f/((float) RAND_MAX));
inline float pRandf() { return ((float) rand())*P_ONEOVER_RAND_MAX; }
inline void pSRandf(int x) { srand(x); }


/// Some useful functors to use in conjunction with standard algorithm
/////////////////////////////////////////////////////////////////////////

template<class PType>
class AgeNKillCheck
{
public:
	 typedef float delta_t;
private:
	delta_t dt;
public:
	AgeNKillCheck(delta_t _dt) : dt(_dt) {}

	bool operator()(PType& p)
	{
		p.age += dt;

		if (p.age >= p.life || p.age < 0)
			return true;
		return false;
	}

	bool operator()(const PType& p)
	{
		return operator() (const_cast<PType>(p));
	}
};

template<class T>
struct DeleteFunctor
{
	void operator()(T* ptr)
	{
		delete ptr;
	}
};

class IndexOffsetFunctor
{
private:
	short m_offset;
public:
	IndexOffsetFunctor(short offset) : m_offset(offset) {}

	short operator()(const short& index)
	{
		return index + m_offset;
	}
};

} // end namespace ps
} // end namespace irr

#endif //__IRR_PS_UTILS_H__
