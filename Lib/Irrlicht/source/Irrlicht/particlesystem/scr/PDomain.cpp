#include "stdafx.h"

/// original pDomain class
/// Copyright 1997-2007 by David K. McAllister
/// http://www.ParticleSystems.org
/// modified and adapted by Francis Hurteau

#include "PDomain.h"
#include "PSUtils.h"

namespace irr
{
namespace ps
{

/// A single point.
///
/// generate always returns this point. within returns true if the point is exactly equal.
PDPoint::PDPoint(const core::vector3df &p0)
{
	p = p0;
}
	
bool PDPoint::within(const core::vector3df &pos) const
{
	return p == pos;
}

core::vector3df PDPoint::generate(PSRandom& rand) const
{
	return p;
}

void PDPoint::transform(const core::matrix4& mat)
{
	p += mat.getTranslation();
}

float PDPoint::size() const
{
	return 1.0f;
}

PDomain *PDPoint::copy() const
{
	PDPoint *P = irrnew PDPoint(*this);
	return P;
}

/// A line segment.
///
/// e0 and e1 are the endpoints of the segment.
///
/// generate returns a random point on this segment. within returns true for points within epsilon of the line segment.
PDLine::PDLine(const core::vector3df &e0, const core::vector3df &e1)
{
	p0 = e0;
	vec = e1 - e0;
	vecNrm = vec;
	vecNrm.normalize();
	len = vec.getLength();
}

/// Returns true for points within epsilon of the line segment.
bool PDLine::within(const core::vector3df &pos) const
{
	core::vector3df to = pos - p0;
	float d = vecNrm.dotProduct(to);
	float dif = fabs(d - to.getLength()) / len; // Has a sqrt(). Kind of slow.
	return dif < 1e-7f; // It's inaccurate, so we need this epsilon.
}

core::vector3df PDLine::generate(PSRandom& rand) const /// Returns a random point on this segment.
{
	return p0 + vec * rand.Randf();
}

void PDLine::transform(const core::matrix4& mat)
{
	mat.transformVect(p0);
	mat.transformVect(vec);
	mat.transformVect(vecNrm);
}

float PDLine::size() const
{
	return len;
}

PDomain *PDLine::copy() const
{
	PDLine *P = irrnew PDLine(*this);
	return P;
}

/// Axis-aligned box
///
/// e0 and e1 are opposite corners of an axis-aligned box. It doesn't matter which of each coordinate is min and which is max.
///
/// generate returns a random point in this box. within returns true if the point is in the box.
PDBox::PDBox(const core::vector3df &e0, const core::vector3df &e1)
{
    p0 = e0;
    p1 = e1;
    if(e1.X < e0.X) { p0.X = e1.X; p1.X = e0.X; }
    if(e1.Y < e0.Y) { p0.Y = e1.Y; p1.Y = e0.Y; }
    if(e1.Z < e0.Z) { p0.Z = e1.Z; p1.Z = e0.Z; }

    dif = p1 - p0;
    vol = dif.dotProduct(core::vector3df(1,1,1));
}

PDBox::PDBox(float _width, float _height, float _length)
: widthX(_width)
, heightY(_height)
, lenZ(_length)
{
	p0 = core::vector3df(-widthX / 2.0f, -heightY / 2.0f, -lenZ / 2.0f);
	p1 = core::vector3df(widthX / 2.0f, heightY / 2.0f, lenZ / 2.0f);
	dif = core::vector3df(widthX, heightY, lenZ);
	vol = dif.dotProduct(core::vector3df(1,1,1));
}

bool PDBox::within(const core::vector3df &pos) const /// Returns true if the point is in the box.
{
    return !((pos.X < p0.X) || (pos.X > p1.X) ||
        (pos.Y < p0.Y) || (pos.Y > p1.Y) ||
        (pos.Z < p0.Z) || (pos.Z > p1.Z));
}

core::vector3df PDBox::generate(PSRandom& rand) const /// Returns a random point in this box.
{
    // Scale and translate [0,1] random to fit box
    return p0 + (rand.RandVec() * dif);
}

void PDBox::transform(const core::matrix4& mat)
{
	p0 = core::vector3df(-widthX / 2.0f, -heightY / 2.0f, -lenZ / 2.0f);
	p1 = core::vector3df(widthX / 2.0f, heightY / 2.0f, lenZ / 2.0f);
	mat.transformVect(p0);
	mat.transformVect(p1);
	dif = p1 - p0;
}

float PDBox::size() const
{
    return vol;
}

PDomain *PDBox::copy() const
{
    PDBox *P = irrnew PDBox(*this);
    return P;
}

/// Cylinder
///
/// e0 and e1 are the endpoints of the axis of the right cylinder. OuterRadius is the outer radius, and InnerRadius is the inner
/// radius for a cylindrical shell. InnerRadius = 0 for a solid cylinder with no empty space in the middle.
///
/// Generate returns a random point in the cylindrical shell. Within returns true if the point is within the cylindrical shell.
PDCylinder::PDCylinder(const core::vector3df &e0, const core::vector3df &e1, const float OuterRadius, const float InnerRadius)
{
    apex = e0;
    axis = e1 - e0;

	//TODO-FH: ASSERT
    //if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
    //if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

    if(OuterRadius < InnerRadius) {
        radOut = InnerRadius; radIn = OuterRadius;
    } else {
        radOut = OuterRadius; radIn = InnerRadius;
    }

    radOutSqr = fsqr(radOut);
    radInSqr = fsqr(radIn);

    ThinShell = (radIn == radOut);
    radDif = radOut - radIn;

    // Given an arbitrary nonzero vector n, make two orthonormal
    // vectors u and v forming a frame [u,v,n.normalize()].
    core::vector3df n = axis;
    float axisLenSqr = axis.getLengthSQ();
    float len = sqrtf(axisLenSqr);
    axisLenInvSqr = axisLenSqr ? (1.0f / axisLenSqr) : 0.0f;
    n *= sqrtf(axisLenInvSqr);

    // Find a vector orthogonal to n.
    core::vector3df basis(1.0f, 0.0f, 0.0f);
    if (fabsf(basis.dotProduct(n)) > 0.999f)
        basis = core::vector3df(0.0f, 1.0f, 0.0f);

    // Project away N component, normalize and cross to get
    // second orthonormal vector.
    u = basis - n * basis.dotProduct(n);
    u.normalize();
    v = n.crossProduct(u);

    float EndCapArea = M_PI * radOutSqr - M_PI * radInSqr;
    if(ThinShell)
        vol = len * 2.0f * M_PI * radOut; // Surface area of cylinder
    else
        vol = EndCapArea * len; // Volume of cylindrical shell
}

PDCylinder::PDCylinder(float _height, float radius)
: height(_height)
{
	apex = core::vector3df(.0f, -height / 2.0f, .0f);
	axis = core::vector3df(.0f, height, .0f);

	// Given an arbitrary nonzero vector n, make two orthonormal
    // vectors u and v forming a frame [u,v,n.normalize()].
    core::vector3df n = axis;
    float axisLenSqr = axis.getLengthSQ();
    float len = sqrtf(axisLenSqr);
    axisLenInvSqr = axisLenSqr ? (1.0f / axisLenSqr) : 0.0f;
    n *= sqrtf(axisLenInvSqr);

    // Find a vector orthogonal to n.
    core::vector3df basis(1.0f, 0.0f, 0.0f);
    if (fabsf(basis.dotProduct(n)) > 0.999f)
        basis = core::vector3df(0.0f, 1.0f, 0.0f);

    // Project away N component, normalize and cross to get
    // second orthonormal vector.
    u = basis - n * basis.dotProduct(n);
    u.normalize();
    v = n.crossProduct(u);

	radIn = 0.0f;
	radOut = radDif = radius;
    radOutSqr = fsqr(radOut);
    radInSqr = fsqr(radIn);
	ThinShell = false;
	float EndCapArea = M_PI * radOutSqr - M_PI * radInSqr;
    vol = EndCapArea * len; // Volume of cylindrical shell
}

bool PDCylinder::within(const core::vector3df &pos) const /// Returns true if the point is within the cylindrical shell.
{
    // This is painful and slow. Might be better to do quick accept/reject tests.
    // Axis is vector from base to tip of the cylinder.
    // x is vector from base to pos.
    //         x . axis
    // dist = ---------- = projected distance of x along the axis
    //        axis. axis   ranging from 0 (base) to 1 (tip)
    //
    // rad = x - dist * axis = projected vector of x along the base

    core::vector3df x = pos - apex;

    // Check axial distance
    float dist = axis.dotProduct(x) * axisLenInvSqr;
    if(dist < 0.0f || dist > 1.0f)
        return false;

    // Check radial distance
    core::vector3df xrad = x - axis * dist; // Radial component of x
    float rSqr = xrad.getLengthSQ();

    return (rSqr >= radInSqr && rSqr <= radOutSqr);
}

core::vector3df PDCylinder::generate(PSRandom& rand) const /// Returns a random point in the cylindrical shell.
{
    float dist = rand.Randf(); // Distance between base and tip
    float theta = rand.Randf() * 2.0f * float(M_PI); // Angle around axis
    // Distance from axis
    float r = radIn + rand.Randf() * radDif;

    // Another way to do this is to choose a random point in a square and keep it if it's in the circle.
    float x = r * cosf(theta);
    float y = r * sinf(theta);

    core::vector3df pos = apex + axis * dist + u * x + v * y;
    return pos;
}

void PDCylinder::transform(const core::matrix4& mat)
{
	apex = core::vector3df(.0f, -height / 2.0f, .0f);
	axis = core::vector3df(.0f, height, .0f);

	// Given an arbitrary nonzero vector n, make two orthonormal
    // vectors u and v forming a frame [u,v,n.normalize()].
    core::vector3df n = axis;
    float axisLenSqr = axis.getLengthSQ();
    float len = sqrtf(axisLenSqr);
    axisLenInvSqr = axisLenSqr ? (1.0f / axisLenSqr) : 0.0f;
    n *= sqrtf(axisLenInvSqr);

    // Find a vector orthogonal to n.
    core::vector3df basis(1.0f, 0.0f, 0.0f);
    if (fabsf(basis.dotProduct(n)) > 0.999f)
        basis = core::vector3df(0.0f, 1.0f, 0.0f);

    // Project away N component, normalize and cross to get
    // second orthonormal vector.
    u = basis - n * basis.dotProduct(n);
    u.normalize();
    v = n.crossProduct(u);

	mat.transformVect(apex);
	mat.transformVect(axis);
	mat.transformVect(u);
	mat.transformVect(v);
}

float PDCylinder::size() const/// Returns the thick cylindrical shell volume or the thin cylindrical shell area if OuterRadius==InnerRadius.
{
    return vol;
}

PDomain *PDCylinder::copy() const
{
    PDCylinder *P = irrnew PDCylinder(*this);
    return P;
}

///  Cone
///
/// e0 is the apex of the cone and e1 is the endpoint of the axis at the cone's base. OuterRadius is the radius of the base of the cone.
/// InnerRadius is the radius of the base of a cone to subtract from the first cone to create a conical shell. This is similar to the 
/// cylindrical shell, which can be thought of as a large cylinder with a smaller cylinder subtracted from the middle. Both cones share the
/// same apex and axis, which implies that the thickness of the conical shell tapers to 0 at the apex. InnerRadius = 0 for a solid cone with
/// no empty space in the middle.
///
/// Generate returns a random point in the conical shell. Within returns true if the point is within the conical shell.
PDCone::PDCone(const core::vector3df &e0, const core::vector3df &e1, const float OuterRadius, const float InnerRadius)
{
    apex = e0;
    axis = e1 - e0;

    //if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
    //if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

    if(OuterRadius < InnerRadius) {
        radOut = InnerRadius; radIn = OuterRadius;
    } else {
        radOut = OuterRadius; radIn = InnerRadius;
    }

    radOutSqr = fsqr(radOut);
    radInSqr = fsqr(radIn);

    ThinShell = (radIn == radOut);
    radDif = radOut - radIn;

    // Given an arbitrary nonzero vector n, make two orthonormal
    // vectors u and v forming a frame [u,v,n.normalize()].
    core::vector3df n = axis;
    float axisLenSqr = axis.getLengthSQ();
    float len = sqrtf(axisLenSqr);
    axisLenInvSqr = axisLenSqr ? 1.0f / axisLenSqr : 0.0f;
    n *= sqrtf(axisLenInvSqr);

    // Find a vector orthogonal to n.
    core::vector3df basis(1.0f, 0.0f, 0.0f);
    if (fabsf(basis.dotProduct(n)) > 0.999f)
        basis = core::vector3df(0.0f, 1.0f, 0.0f);

    // Project away N component, normalize and cross to get
    // second orthonormal vector.
    u = basis - n * basis.dotProduct(n);
    u.normalize();
    v = n.crossProduct(u);

    if(ThinShell)
        vol = sqrtf(axisLenSqr + radOutSqr) * M_PI * radOut; // Surface area of cone (not counting endcap)
    else {
        float OuterVol = 0.33333333f * M_PI * radOutSqr * len; 
        float InnerVol = 0.33333333f * M_PI * radInSqr * len;
        vol = OuterVol - InnerVol; // Volume of conical shell
    }
}

bool PDCone::within(const core::vector3df &pos) const /// Returns true if the point is within the conical shell.
{
    // This is painful and slow. Might be better to do quick
    // accept/reject tests.
    // Let axis = vector from base to tip of the cone
    // x = vector from base to test point
    //         x . axis
    // dist = ---------- = projected distance of x along the axis
    //        axis. axis   ranging from 0 (base) to 1 (tip)
    //
    // rad = x - dist * axis = projected vector of x along the base

    core::vector3df x = pos - apex;

    // Check axial distance
    // axisLenInvSqr stores 1 / dot(axis, axis)
    float dist = axis.dotProduct(x) * axisLenInvSqr;
    if(dist < 0.0f || dist > 1.0f)
        return false;

    // Check radial distance; scale radius along axis for cones
    core::vector3df xrad = x - axis * dist; // Radial component of x
    float rSqr = xrad.getLengthSQ();

    return (rSqr >= fsqr(dist * radIn) && rSqr <= fsqr(dist * radOut));
}

core::vector3df PDCone::generate(PSRandom& rand) const /// Returns a random point in the conical shell.
{
    float dist = rand.Randf(); // Distance between base and tip
    float theta = rand.Randf() * 2.0f * float(M_PI); // Angle around axis
    // Distance from axis
    float r = radIn + rand.Randf() * radDif;

    // Another way to do this is to choose a random point in a square and keep it if it's in the circle.
    float x = r * cosf(theta);
    float y = r * sinf(theta);

    // Scale radius along axis for cones
    x *= dist;
    y *= dist;

    core::vector3df pos = apex + axis * dist + u * x + v * y;
    return pos;
}

void PDCone::transform(const core::matrix4& mat)
{
}

float PDCone::size() const /// Returns the thick conical shell volume or the thin conical shell area if OuterRadius==InnerRadius.
{
    return vol;
}

PDomain *PDCone::copy() const
{
    PDCone *P = irrnew PDCone(*this);
    return P;
}

/// Sphere
///
/// The point Center is the center of the sphere. OuterRadius is the outer radius of the spherical shell and InnerRadius is the inner radius.
///
/// Generate returns a random point in the thick shell at a distance between OuterRadius and InnerRadius from point Center. If InnerRadius
/// is 0, then it is the whole sphere. Within returns true if the point lies within the thick shell at a distance between InnerRadius to OuterRadius from point Center.
PDSphere::PDSphere(const core::vector3df &Center, const float OuterRadius, const float InnerRadius)
{
    ctr = Center;

    //if(InnerRadius < 0) throw PErrInvalidValue("Can't have negative radius.");
    //if(OuterRadius < 0) throw PErrInvalidValue("Can't have negative radius.");

    if(OuterRadius < InnerRadius) {
        radOut = InnerRadius; radIn = OuterRadius;
    } else {
        radOut = OuterRadius; radIn = InnerRadius;
    }

    radOutSqr = fsqr(radOut);
    radInSqr = fsqr(radIn);

    ThinShell = (radIn == radOut);
    radDif = radOut - radIn;

    if(ThinShell)
        vol = 4.0f * M_PI * radOutSqr; // Surface area of sphere
    else {
        float OuterVol = 1.33333333f * M_PI * radOutSqr * radOut; 
        float InnerVol = 1.33333333f * M_PI * radInSqr * radIn;
        vol = OuterVol - InnerVol; // Volume of spherical shell
    }
}

bool PDSphere::within(const core::vector3df &pos) const /// Returns true if the point lies within the thick shell.
{
    core::vector3df rvec(pos - ctr);
    float rSqr = rvec.getLengthSQ();
    return rSqr <= radOutSqr && rSqr >= radInSqr;
}

core::vector3df PDSphere::generate(PSRandom& rand) const /// Returns a random point in the thick spherical shell.
{
    core::vector3df pos;

    do {
        pos = rand.RandVec() - vHalf; // Point on [-0.5,0.5] box
    } while (pos.getLengthSQ() > fsqr(0.5)); // Make sure it's also on r=0.5 sphere.
    pos.normalize(); // Now it's on r=1 spherical shell

    // Scale unit sphere pos by [0..r] and translate
    if(ThinShell) // I can't remember why I make this distinction. Is it for precision or speed? Speed doesn't seem likely.
        pos = ctr + pos * radOut;
    else
        pos = ctr + pos * (radIn + rand.Randf() * radDif);

    return pos;
}

void PDSphere::transform(const core::matrix4& mat)
{
	ctr = mat.getTranslation();
}

float PDSphere::size() const /// Returns the thick spherical shell volume or the thin spherical shell area if OuterRadius==InnerRadius.
{
    return vol;
}

PDomain *PDSphere::copy() const
{
    PDSphere *P = irrnew PDSphere(*this);
    return P;
}

/// Gaussian blob
///
/// The point Center is the center of a normal probability density of standard deviation StandardDev. The density is radially symmetrical.
/// The blob domain allows for some very natural-looking effects because there is no sharp, artificial-looking boundary at the edge of the domain.
///
/// Generate returns a point with normal probability density. Within has a probability of returning true equal to the probability density at the specified point.
PDBlob::PDBlob(const core::vector3df &Center, const float StandardDev)
{
    ctr = Center;
    stdev = StandardDev;
    float oneOverSigma = 1.0f/(stdev+0.000000000001f);
    Scale1 = -0.5f*fsqr(oneOverSigma);
    Scale2 = P_ONEOVERSQRT2PI * oneOverSigma;
}

bool PDBlob::within(const core::vector3df &pos) const /// Has a probability of returning true equal to the probability density at the specified point.
{
    core::vector3df x = pos - ctr;
    float Gx = expf(x.getLengthSQ() * Scale1) * Scale2;
    return (pRandf() < Gx);
}

core::vector3df PDBlob::generate(PSRandom& rand) const /// Returns a point with normal probability density.
{
    return ctr + rand.NRandVec(stdev);
}

void PDBlob::transform(const core::matrix4& mat)
{
	ctr = mat.getTranslation();
}

float PDBlob::size() const /// Returns the probability density integral, which is 1.0.
{
    return 1.0f;
}

PDomain *PDBlob::copy() const
{
    PDBlob *P = irrnew PDBlob(*this);
    return P;
}

}; //end namespace ps
}; //end namespace irr
