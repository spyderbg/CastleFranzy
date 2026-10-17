#ifndef __CSCENENODEANIMATORCHANNELLIBRARY_H_INCLUDED__
#define __CSCENENODEANIMATORCHANNELLIBRARY_H_INCLUDED__

#include "irrArray.h"
#include "ISceneNodeAnimatorChannelLibrary.h"
#include "CChannelAnimator.h"
#include "ISceneNode.h"

namespace irr
{
namespace scene
{
	class CSceneNodeAnimatorChannelLibrary 
		: public ISceneNodeAnimatorChannelLibrary
	{
	public:
		CSceneNodeAnimatorChannelLibrary() 
			: m_BindTo(0)
			, m_Length(0)

		{
		}

		virtual ~CSceneNodeAnimatorChannelLibrary()
		{
			for(u32 i = 0; i < m_ChannelAnimators.size(); i++)
			{
				m_ChannelAnimators[i]->drop();
			}
		}

		virtual const core::stringc& getChannelBindURI(int iChannelID)
		{
			return m_ChannelAnimators[iChannelID]->getBindURI();
		}

		virtual void BindChannel(int iChannelID, void *pPtr)
		{
			m_ChannelAnimators[iChannelID]->bind(pPtr);
		}

		virtual s32 getChannelAnimatorsCount() const 
		{ 
			return m_ChannelAnimators.size();
		}

		//! animates a scene node
		virtual void animateNode(ISceneNode* node, u32 timeMs)
		{
			_IRR_DEBUG_BREAK_IF(m_ChannelAnimators.size() == 0);

			ITimelineController *timeCtrl = getTimelineCtrl();
			s32 relativeTime = timeMs;

			// Remap the time base on the controller
			if(timeCtrl)
			{
				timeCtrl->update(relativeTime);
				relativeTime = timeCtrl->getCtrlTime();
			}
			else
			{
				relativeTime = timeMs % m_Length;
			}

			for(u32 i = 0; i < m_ChannelAnimators.size(); i++)
			{
				m_ChannelAnimators[i]->OnAnimate(relativeTime);
			}
		}

		//! Writes attributes of the scene node animator.
		virtual void serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options=0) const
		{
			_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
		}

		//! Reads attributes of the scene node animator.
		virtual void deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options=0)
		{
			_IRR_DEBUG_BREAK_IF("NOT IMPLEMENTED");
		}

		//! Returns type of the scene node animator
		//virtual ESCENE_NODE_ANIMATOR_TYPE getType() const { return ESNAT_TEXTURE; }
		
		//! Creates a clone of this animator.
		/** Please note that you will have to drop
		(IReferenceCounted::drop()) the returned pointer after calling
		this. */
		virtual ISceneNodeAnimator* createClone();

		//! Add a new channel animator to the current set.
		void addAnimator(CChannelAnimator* pAnimator)
		{
			pAnimator->grab();
			m_ChannelAnimators.push_back(pAnimator);

			m_Length = (m_Length > pAnimator->getLength() ? m_Length : pAnimator->getLength());
		}


		//! Allow Animator to link with the node
		virtual void OnBind(ISceneNode* node)
		{
			//node->grab();
			m_BindTo = node;
			BindChannels();
		}

		//! Allow Animator to unlink with the node
		virtual void onUnbind(ISceneNode* node)
		{
			_IRR_DEBUG_BREAK_IF(node != m_BindTo);
			//m_BindTo->drop();
			m_BindTo = 0;
		}

		virtual int GetLength() { return m_Length; }

	protected:

		void BindChannels();
		void BindChannel(CChannelAnimator*);

	private:
		s32 m_Length;
		ISceneNode* m_BindTo;
		core::array<CChannelAnimator*> m_ChannelAnimators;
	};


} // end namespace scene
} // end namespace irr

#endif