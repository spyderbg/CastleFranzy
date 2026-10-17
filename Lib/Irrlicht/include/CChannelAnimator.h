#ifndef __CCHANNELANIMATOR_H_INCLUDED__
#define __CCHANNELANIMATOR_H_INCLUDED__

#include "IChannelAnimator.h"
#include "IAnimationTrack.h"
#include "CAnimationTrack.h"
#include "matrix4.h"
#include "ISceneNode.h"
namespace irr
{
namespace scene
{
	class CChannelAnimator 
		: public IChannelAnimator
		, public virtual IReferenceCounted 
	{
	public:
		enum Type
		{
			SingleFieldF32,
			Transform4x4
		};

		CChannelAnimator(IAnimationTrack *pSource) : m_pSource(pSource)
		{
			m_pSource->grab();
		}
		virtual ~CChannelAnimator()
		{
			if (m_pSource)
				m_pSource->drop();
		}

		//! Update the animated field(s)
		virtual void OnAnimate(s32 timeMs)
		{
			if(getTarget())
			{
				switch(eType)
				{
				case Transform4x4:
					{
						SPositionDirection mat;
						m_pSource->getValue(timeMs, &mat);
						ISceneNode *node = (ISceneNode *)getTarget();
						node->setPosition(mat.pos);
						node->setRotation(mat.dir);
						node->setScale(mat.scale);
					}
					break;
				case SingleFieldF32:
					{
						f32 value;
						m_pSource->getValue(timeMs, &value);
						*(f32 *)getTarget() = value;
					}
					break;
				}
			}
		}

		virtual const core::stringc& getBindURI() const
		{
			_IRR_DEBUG_BREAK_IF(m_pSource == 0);
			return m_pSource->getBindObjURI();
		};

		virtual s32 getLength() const
		{
			_IRR_DEBUG_BREAK_IF(m_pSource == 0);
			return m_pSource->getLength();
		};

		void setType(Type t) { eType = t; }

		virtual CChannelAnimator* createClone() 
		{			
			CChannelAnimator* newAnimator = irrnew CChannelAnimator(m_pSource);
			newAnimator->setType(eType);
			return newAnimator;
		};

	protected:
		//! Source track
		IAnimationTrack *m_pSource;
		//!
		
		Type eType;
	};
};
};

#endif
