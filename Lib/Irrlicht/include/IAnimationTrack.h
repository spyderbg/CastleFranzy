#ifndef __IANIMATIONTRACK_H_INCLUDED__
#define __IANIMATIONTRACK_H_INCLUDED__

#include "irrString.h"
#include "IReferenceCounted.h"

namespace irr
{
namespace scene
{
	class IAnimationTrack
		: virtual public IReferenceCounted
	{
	public:
		enum Type
		{
			FLOAT,
			MATRIX,
			FLOAT4x4,
			FLOATSRT,
		};

		//! Retrieve the animation value
		virtual void getValue(s32 timeMs, void *pOutputPtr) const = 0;

		//! Retrieve the animation value and use an hint to accelerate the lookup in the track
		virtual void getValue(s32 timeMs, void *pOutputPtr, s32 &hintKeyLookUp) const = 0;

		//! Retrieve the animation Length in Milliseconds
		inline s32 getLength() const { return m_Length; }

		

		//! Retrieve object lookup URI
		// \return: Return the URI of an acceptable target
		const core::stringc& getBindObjURI() const { return m_BindObjURI; }

		//! Set the lookup URI
		void setBindObjURI( const core::stringc &uri) { m_BindObjURI = uri; }

	#ifdef _DEBUG
		//! Retrieve value type
		virtual IAnimationTrack::Type getValueType() const = 0;
	#endif

	protected:
		//! Target ID
		core::stringc	m_BindObjURI;
		s32				m_Length;
	};
};
};

#endif