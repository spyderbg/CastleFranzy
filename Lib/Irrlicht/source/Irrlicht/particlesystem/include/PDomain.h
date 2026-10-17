/// original pDomain class
/// Copyright 1997-2007 by David K. McAllister
/// http://www.ParticleSystems.org
/// modified and adapted by Francis Hurteau

#ifndef __PDomain_H__
#define __PDomain_H__

#include "vector3d.h"
#include "matrix4.h"

namespace irr
{
namespace ps
{

class PSRandom;

/// A representation of a region of space.
///
class PDomain
{
public:
	enum PDomainType
	{
		DT_BOX = 0,
		DT_SPHERE,
		DT_CYLINDER,
		DT_POINT,
		DT_LINE,
		DT_CONE,
		DT_BLOB
	};

    virtual bool			within(const core::vector3df &) const = 0;	///< Returns true if the given point is within the domain.
    virtual core::vector3df generate(PSRandom& rand) const = 0;			///< Returns a random point in the domain.
	virtual void			transform(const core::matrix4& mat) = 0;	///< Translate and rotate the domain by the transform

	virtual float			size() const = 0;							///< Returns the size of the domain (length, area, or volume).
	virtual PDomainType		getType() const = 0;

	//NOTE-FH: Not sure about this...
    virtual PDomain *copy() const = 0;									///< Returns a pointer to a heap-allocated copy of the derived class

    virtual ~PDomain() {}
};

/// A single point.
///
/// generate always returns this point. within returns true if the point is exactly equal.
class PDPoint : public PDomain
{
public:
    core::vector3df p;

public:
	PDPoint(const core::vector3df &p0 = core::vector3df());
	~PDPoint() {}

	bool			within(const core::vector3df &pos) const;
    core::vector3df generate(PSRandom& rand) const;
	void			transform(const core::matrix4& mat);
    float			size() const;
	PDomainType		getType() const {
		return DT_POINT;
	}

    PDomain *copy() const;
};

/// A line segment.
///
/// e0 and e1 are the endpoints of the segment.
///
/// generate returns a random point on this segment. within returns true for points within epsilon of the line segment.
class PDLine : public PDomain
{
public:
    core::vector3df p0, vec, vecNrm;
    float len;
public:
    PDLine(const core::vector3df &e0, const core::vector3df &e1);
	~PDLine() {}

    bool			within(const core::vector3df &pos) const;	/// Returns true for points within epsilon of the line segment.
    core::vector3df generate(PSRandom& rand) const;				/// Returns a random point on this segment.
	void			transform(const core::matrix4& mat);

    float			size() const;
	PDomainType		getType() const {
		return DT_LINE;
	}

    PDomain *copy() const;
};


/// Axis-aligned box
///
/// e0 and e1 are opposite corners of an axis-aligned box. It doesn't matter which of each coordinate is min and which is max.
///
/// generate returns a random point in this box. within returns true if the point is in the box.
class PDBox : public PDomain
{
public:
    // p0 is the min corner. p1 is the max corner.
    core::vector3df p0, p1, dif;
    float vol, widthX, heightY, lenZ;

public:
    PDBox(const core::vector3df &e0, const core::vector3df &e1);
	PDBox(float _width, float _height, float _length);

	~PDBox() {}

    bool			within(const core::vector3df &pos) const;	/// Returns true if the point is in the box.
    core::vector3df generate(PSRandom& rand) const;				/// Returns a random point in this box.
	void			transform(const core::matrix4& mat);

    float			size() const;
	PDomainType		getType() const {
		return DT_BOX;
	}


    PDomain *copy() const;
};

/// Cylinder
///
/// e0 and e1 are the endpoints of the axis of the right cylinder. OuterRadius is the outer radius, and InnerRadius is the inner
/// radius for a cylindrical shell. InnerRadius = 0 for a solid cylinder with no empty space in the middle.
///
/// generate returns a random point in the cylindrical shell. within returns true if the point is within the cylindrical shell.
class PDCylinder : public PDomain
{
public:
    core::vector3df apex, axis, u, v; // Apex is one end. Axis is vector from one end to the other.
    float radOut, radIn, radOutSqr, radInSqr, radDif, axisLenInvSqr, vol, height;
    bool ThinShell;

public:
    PDCylinder(const core::vector3df &e0, const core::vector3df &e1, const float OuterRadius, const float InnerRadius = 0.0f);
	PDCylinder(float height, float radius);
	~PDCylinder() {}

    bool			within(const core::vector3df &pos) const;	/// Returns true if the point is within the cylindrical shell.
    core::vector3df generate(PSRandom& rand) const;				/// Returns a random point in the cylindrical shell.
	void			transform(const core::matrix4& mat);
    float			size() const;								/// Returns the thick cylindrical shell volume or the thin cylindrical shell area if OuterRadius==InnerRadius.
	PDomainType		getType() const {
		return DT_CYLINDER;
	}


    PDomain *copy() const;
};

///  Cone
///
/// e0 is the apex of the cone and e1 is the endpoint of the axis at the cone's base. OuterRadius is the radius of the base of the cone.
/// InnerRadius is the radius of the base of a cone to subtract from the first cone to create a conical shell. This is similar to the 
/// cylindrical shell, which can be thought of as a large cylinder with a smaller cylinder subtracted from the middle. Both cones share the
/// same apex and axis, which implies that the thickness of the conical shell tapers to 0 at the apex. InnerRadius = 0 for a solid cone with
/// no empty space in the middle.
///
/// generate returns a random point in the conical shell. within returns true if the point is within the conical shell.
class PDCone : public PDomain
{
public:
    core::vector3df apex, axis, u, v; // Apex is one end. Axis is vector from one end to the other.
    float radOut, radIn, radOutSqr, radInSqr, radDif, axisLenInvSqr, vol;
    bool ThinShell;

public:
    PDCone(const core::vector3df &e0, const core::vector3df &e1, const float OuterRadius, const float InnerRadius = 0.0f);
	~PDCone() {}

    bool			within(const core::vector3df &pos) const;	/// Returns true if the point is within the conical shell.
    core::vector3df generate(PSRandom& rand) const;				/// Returns a random point in the conical shell.
	void			transform(const core::matrix4& mat);
    float			size() const;								/// Returns the thick conical shell volume or the thin conical shell area if OuterRadius==InnerRadius.
	PDomainType		getType() const {
		return DT_CONE;
	}

    PDomain *copy() const;
};

/// Sphere
///
/// The point Center is the center of the sphere. OuterRadius is the outer radius of the spherical shell and InnerRadius is the inner radius.
///
/// generate returns a random point in the thick shell at a distance between OuterRadius and InnerRadius from point Center. If InnerRadius
/// is 0, then it is the whole sphere. within returns true if the point lies within the thick shell at a distance between InnerRadius to OuterRadius from point Center.
class PDSphere : public PDomain
{
public:
    core::vector3df ctr;
    float radOut, radIn, radOutSqr, radInSqr, radDif, vol;
    bool ThinShell;

public:
	PDSphere(const core::vector3df &Center, const float OuterRadius, const float InnerRadius = 0.0f);
	~PDSphere() {}

    bool			within(const core::vector3df &pos) const;	/// Returns true if the point lies within the thick shell.
    core::vector3df generate(PSRandom& rand) const;				/// Returns a random point in the thick spherical shell.
	void			transform(const core::matrix4& mat);
    float			size() const;								/// Returns the thick spherical shell volume or the thin spherical shell area if OuterRadius==InnerRadius.
	PDomainType		getType() const {
		return DT_SPHERE;
	}

    PDomain *copy() const;
};

/// Gaussian blob
///
/// The point Center is the center of a normal probability density of standard deviation StandardDev. The density is radially symmetrical.
/// The blob domain allows for some very natural-looking effects because there is no sharp, artificial-looking boundary at the edge of the domain.
///
/// generate returns a point with normal probability density. within has a probability of returning true equal to the probability density at the specified point.
class PDBlob : public PDomain
{
public:
    core::vector3df ctr;
    float stdev, Scale1, Scale2;

public:
    PDBlob(const core::vector3df &Center, const float StandardDev);
	~PDBlob() {}

    bool			within(const core::vector3df &pos) const;	/// Has a probability of returning true equal to the probability density at the specified point.
    core::vector3df generate(PSRandom& rand) const;				/// Returns a point with normal probability density.
	void			transform(const core::matrix4& mat);
    float			size() const;								/// Returns the probability density integral, which is 1.0.
	PDomainType		getType() const {
		return DT_BLOB;
	}

    PDomain *copy() const;
};


}; //end namespace ps
}; //end namespace irr

#endif // __PDomain_H__
