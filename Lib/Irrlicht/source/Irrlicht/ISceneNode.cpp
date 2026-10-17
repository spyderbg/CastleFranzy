#include "stdafx.h"
#include "IrrCompileConfig.h"
#include "ISceneNode.h"
#include "CSceneManager.h"

#include "IrrPlatform.h"	//for irr::stricmp

namespace irr {
namespace scene {

#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
#    ifdef DEBUG_ISCENE_NODE
int ISceneNode::NodeCount = 0;
int ISceneNode::AnimatedCount = 0;
int ISceneNode::AbsoluteUpdate = 0;
int ISceneNode::RelativeUpdate = 0;
int ISceneNode::RelativeRotationUpdate = 0;
int ISceneNode::RelativeTranslationUpdate = 0;

void ISceneNode::ResetXFormDebugCounters()
{
	NodeCount = AnimatedCount = AbsoluteUpdate = RelativeUpdate = RelativeRotationUpdate = RelativeTranslationUpdate = 0;
}
#    endif
#endif


ISceneNode::ISceneNode(s32 id,
		const core::vector3df& position,
		const core::quaternion& rotation,
		const core::vector3df& scale)
	: RelativeTranslation(position), RelativeRotation(rotation), RelativeScale(scale),
		Parent(0), ID(id), SceneManager(0), TriangleSelector(0),
		AutomaticCullingState(EAC_BOX), OptionFlags(0),
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
		NodeFlags(ESNF_TRANSFORMATION_CHANGED), // ensure proper matrices initialization
#else
		NodeFlags(ESNF_TRANSFORMATION_CHANGED),
#endif
#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
		IsLocalyVisible(true), IsGlobalyVisible(true),
#endif
		IsVisible(true),
		DebugDataVisible(EDS_OFF),
		GameData(0), CameraOffset(0.0f), RenderingLayer(0)
{
	updateAbsolutePosition();
}

//! Constructor
ISceneNode::ISceneNode(ISceneNode* parent, ISceneManager* mgr, s32 id,
		const core::vector3df& position,
		const core::quaternion& rotation,
		const core::vector3df& scale)
	: RelativeTranslation(position), RelativeRotation(rotation), RelativeScale(scale),
		Parent(0), ID(id), SceneManager(mgr), TriangleSelector(0),
		AutomaticCullingState(EAC_BOX), OptionFlags(0),
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
		NodeFlags(ESNF_TRANSFORMATION_CHANGED), // ensure proper matrices initialization
#else
		NodeFlags(ESNF_TRANSFORMATION_CHANGED),
#endif
#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
		IsLocalyVisible(true), IsGlobalyVisible(true),
#endif
		IsVisible(true),
		DebugDataVisible(EDS_OFF),
		GameData(0), CameraOffset(0.0f), RenderingLayer(0)
{
	DEPRECATED("scene manager set inside setParent", DEPRACATED_REMOVE_DATE_0);
	DEPRECATED("parent set with addChild", DEPRACATED_REMOVE_DATE_0);

	if (parent)
		parent->addChild(this);

	updateAbsolutePosition();
}

ISceneNode::~ISceneNode()
{
	// delete all children
	removeAll();

	// clearing animators moved to onDelete

	if (TriangleSelector)
		TriangleSelector->drop();
}

void ISceneNode::onDelete()
{
	// delete all animators
	removeBindedAnimators();
	removeAnimators();
// 	core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
// 	for (; ait != Animators.end(); ++ait)
// 		(*ait)->drop();
}

void ISceneNode::SetOption(E_OPTION_FLAG opt, bool value) 
{
	if(value)
		OptionFlags |= opt;
	else
		OptionFlags &= ~opt;
}

bool ISceneNode::GetOption(E_OPTION_FLAG opt) const
{
	if(OptionFlags & opt)
		return true;
	else
		return false;
}

void ISceneNode::LogTree(int tabs )
{
	char buf[80];
	memset(buf, ' ', sizeof(buf));
	snprintf(buf + tabs, 80 - tabs, "%s %s %s", getUID(), Name.c_str(), getScopeID());
	os::Printer::print(buf);

	core::list<ISceneNode*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->LogTree(tabs + 1);
};


void ISceneNode::OnRegisterSceneNode()
{
#if !defined(_IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL)
	if (IsVisible)
	{
		core::list<ISceneNode*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->OnRegisterSceneNode();
	}
#endif
}

void ISceneNode::OnUpdateTime(u32 timeMs)
{
	if (IsVisible && !GetOption(EOF_DISABLE_ONANIMATE))
	{
		// update all animators time without animating them
		core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
			for (; ait != Animators.end(); ++ait)
				(*ait)->updateTime(timeMs);

#if !defined(_IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL)
		core::list<ISceneNode*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->OnUpdateTime(timeMs);
#endif
	}
}

void ISceneNode::OnAnimate(u32 timeMs)
{
	if (IsVisible && !GetOption(EOF_DISABLE_ONANIMATE))
	{
		// animate this node with all animators

#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
#    ifdef DEBUG_ISCENE_NODE
		if (NodeFlags & ESNF_TRANSFORMATION_ANIMATED)
		{
			++AnimatedCount;
		}
		++NodeCount;
#    endif
#endif

		core::list<ISceneNodeAnimator*>::Iterator ait = Animators.begin();
		for (; ait != Animators.end(); ++ait)
			(*ait)->animateNode(this, timeMs);

		// update absolute position
		updateAbsolutePosition();

		// perform the post render process on all children

		core::list<ISceneNode*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->OnAnimate(timeMs);

#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
		NodeFlags &= ~ESNF_WAS_TRANSFORMATION_UPDATED;
#endif
	}
}

const c8* ISceneNode::getName() const
{
	return Name.c_str();
}


//! Sets the name of the node.
/** \param name New name of the scene node. */
void ISceneNode::setName(const c8* name)
{
	Name = name;
}


void ISceneNode::setName(const core::stringc& name)
{
	Name = name;
}


#ifdef _IRR_CACHE_SCENE_NODE_TRANSFORMED_BBOX_
const core::aabbox3d<f32>& ISceneNode::getTransformedBoundingBox() const
{
	if (NodeFlags & ESNF_TRANSFORMED_BBOX_DIRTY)
	{
		TransformedBBox = getBoundingBox();
		AbsoluteTransformation.transformBoxEx(TransformedBBox);
		NodeFlags &= ~ESNF_TRANSFORMED_BBOX_DIRTY;
	}
	return TransformedBBox;
}
#else
const core::aabbox3d<f32> ISceneNode::getTransformedBoundingBox() const
{
	core::aabbox3d<f32> box = getBoundingBox();
	AbsoluteTransformation.transformBoxEx(box);
	return box;
}
#endif

const core::matrix4& ISceneNode::getAbsoluteTransformation() const
{
	return AbsoluteTransformation;
}

void ISceneNode::setAbsoluteTransformation(core::matrix4& mat)
{
	AbsoluteTransformation = mat;
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
	// Setting absolute transformation invalidates children's absolute
	// transformation => needs to be flagged
	NodeFlags |= ESNF_WAS_TRANSFORMATION_UPDATED;
#endif
	// TODO: update relative transformation such that on next call to
	// updateAbsolutePosition() will produce the same matrix
}


const core::matrix4& ISceneNode::getRelativeTransformation() const
{
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
	if (NodeFlags & ESNF_IS_TRANSFORMATION_DIRTY)
	{
		if (NodeFlags & (ESNF_IS_SCALE_DIRTY | ESNF_IS_ROTATION_DIRTY))
		{
#endif
			RelativeTransformation = RelativeRotation.getMatrix();
			// Note: seems like we could avoid comparing three floats and use
			// more flags here (like ESNF_SCALE_ANIMATED plus something like
			// ESNF_HAS_SCALE that would be set when calling setScale)
			if (RelativeScale != core::vector3df(1.f, 1.f, 1.f))
			{
				RelativeTransformation.postScale(RelativeScale);
			}
			RelativeTransformation.setTranslation(RelativeTranslation);
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
#    ifdef DEBUG_ISCENE_NODE
			++RelativeRotationUpdate;
#    endif
		}
		else // ESNF_TRANSFORMATION_DIRTY is necessarily true
		{
			RelativeTransformation.setTranslation(RelativeTranslation);
#    ifdef DEBUG_ISCENE_NODE
			++RelativeTranslationUpdate;
#    endif
		}
		NodeFlags &= ~ESNF_TRANSFORMATION_CHANGED;
#    ifdef DEBUG_ISCENE_NODE
		++RelativeUpdate;
#    endif
	}
#endif
	return RelativeTransformation;
}

void ISceneNode::setRelativeTransformation(const core::matrix4 &mat)
{
	setPosition(mat.getTranslation());
	setRotation(mat);
	setScale(mat.getScale());
	RelativeTransformation = mat;
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
	NodeFlags &= ~ESNF_TRANSFORMATION_CHANGED;
#endif
}

int &ISceneNode::getVisibility()
{
	return IsVisible;
}

bool ISceneNode::isVisible() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return (IsVisible !=0);
}

void ISceneNode::notifyVisibilityChanged(bool visibility)
{
#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
	bool oldVisibility = IsVisible != 0;

	IsGlobalyVisible = visibility;
	IsVisible = IsLocalyVisible && IsGlobalyVisible;
	
	if(oldVisibility != (IsVisible != 0))
	{
		core::list<ISceneNode*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->notifyVisibilityChanged(IsVisible != 0);
	}
#endif
}

void ISceneNode::setVisible(bool isVisible)
{
#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
	if(IsLocalyVisible == (isVisible != 0))
		return;

	bool bOldVisibility = IsVisible != 0;
	IsLocalyVisible = isVisible != 0;
	IsVisible = IsLocalyVisible && IsGlobalyVisible;

	if(bOldVisibility == (IsVisible != 0))
		return;

	core::list<ISceneNode*>::Iterator it = Children.begin();
	for (; it != Children.end(); ++it)
		(*it)->notifyVisibilityChanged(IsVisible != 0);
#else
	IsVisible = isVisible;
#endif
}


#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
bool ISceneNode::isAnimated(ESCENE_NODE_ANIMATION_FLAG what) const
{
	return (NodeFlags & what) != 0;
}


void ISceneNode::setIsAnimated(ESCENE_NODE_ANIMATION_FLAG what, bool value)
{
	if (value)
	{
		NodeFlags |= what;
	}
	else
	{
		NodeFlags &= ~what;
	}
}
#endif


s32 ISceneNode::getID() const
{
	return ID;
}


void ISceneNode::setID(s32 id)
{
	ID = id;
}


ISceneNode* ISceneNode::getSceneNodeFromUID(const char* uid)
{
	if(irr::stricmp(getUID(), uid) == 0)
		return this;

	ISceneNode* node = 0;

	core::list<ISceneNode*>::Iterator it = Children.begin();
	for (; it!=Children.end(); ++it)
	{
		node = (*it)->getSceneNodeFromUID(uid);
		if (node)
			return node;
	}

	return 0;
}

const ISceneNode* ISceneNode::getSceneNodeFromUID(const char* uid) const
{
	if(irr::stricmp(getUID(), uid) == 0)
		return this;

	ISceneNode* node = 0;

	const core::list<ISceneNode*>& list = getChildren();
	core::list<ISceneNode*>::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = (*it)->getSceneNodeFromUID(uid);
		if (node)
			return node;
	}

	return 0;
}

ISceneNode* ISceneNode::getSceneNodeFromType(scene::ESCENE_NODE_TYPE type)
{
	if (getType() == type || ESNT_ANY == type)
		return this;

	ISceneNode* node = 0;

	core::list<ISceneNode*>::Iterator it = Children.begin();
	for (; it!=Children.end(); ++it)
	{
		node = (*it)->getSceneNodeFromType(type);
		if (node)
			return node;
	}

	return 0;
}

void ISceneNode::getSceneNodesFromType(scene::ESCENE_NODE_TYPE type, core::array<scene::ISceneNode*>& outNodes)
{
	if (getType() == type || ESNT_ANY == type)
		outNodes.push_back(this);

	core::list<ISceneNode*>::Iterator it = Children.begin();
	for (; it != Children.end(); ++it) {
		(*it)->getSceneNodesFromType(type, outNodes);
	}
}

const char * ISceneNode::getUID() const
{
	return "";
}

const char * ISceneNode::getScopeID() const
{
	return "";
}

void ISceneNode::addChild(ISceneNode* child)
{
	if (child && (child != this))
	{
		child->grab();
		child->remove(); // remove from old parent
		Children.push_back(child);
		child->setParent(this);

#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
		if(SceneManager)
			SceneManager->notifyHierarchyChanged();
		child->notifyVisibilityChanged(IsVisible != 0);
#endif
	}
}


bool ISceneNode::removeChild(ISceneNode* child)
{
	core::list<ISceneNode*>::Iterator it = Children.begin();
	for (; it != Children.end(); ++it)
		if ((*it) == child)
		{
			(*it)->Parent = 0;
			(*it)->SceneManager = 0;
			(*it)->drop();
			Children.erase(it);

#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
			if(SceneManager)
				SceneManager->notifyHierarchyChanged();
#endif
			return true;
		}

	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return false;
}


void ISceneNode::removeAll()
{
	core::list<ISceneNode*>::Iterator it = Children.begin();
	for (; it != Children.end(); ++it)
	{
		(*it)->Parent = 0;
		(*it)->SceneManager = 0;
		(*it)->drop();
	}

	Children.clear();

#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
	if(SceneManager)
		SceneManager->notifyHierarchyChanged();
#endif
}


void ISceneNode::remove()
{
	if (Parent)
		Parent->removeChild(this);
}


void ISceneNode::addAnimator(ISceneNodeAnimator* animator)
{
	if (animator)
	{
		Animators.push_back(animator);
		//Animators.push_front(animator);
		animator->grab();
		animator->onBind(this);

#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
		if(SceneManager)
			SceneManager->notifyHierarchyChanged();
#endif
	}
}

const core::list<ISceneNodeAnimator*>& ISceneNode::getAnimators() const
{
	return Animators;
}

void ISceneNode::removeAnimator(ISceneNodeAnimator* animator)
{
	core::list<ISceneNodeAnimator*>::Iterator it = Animators.begin();
	for (; it != Animators.end(); ++it)
		if ((*it) == animator)
		{
			(*it)->onUnbind(this);
			(*it)->drop();
			Animators.erase(it);

#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
			if(SceneManager)
				SceneManager->notifyHierarchyChanged();
#endif
			return;
		}
}


void ISceneNode::removeAnimators()
{
	core::list<ISceneNodeAnimator*>::Iterator it = Animators.begin();
	for (; it != Animators.end(); ++it)
	{
		(*it)->onUnbind(this);
		(*it)->drop();
	}

	Animators.clear();
#ifdef _IRR_ENABLE_SCENE_OPTIMIZED_TRAVERSAL
	if(SceneManager)
		SceneManager->notifyHierarchyChanged();
#endif
}

void ISceneNode::addBindedAnimator(ISceneNodeAnimator* animator)
{
	if (animator)
	{
		//animator->grab();
		// [NOTE] removeBindedAnimator is never called and implementing OnBind would consume RAM or CPU
		// For now, we avoid each single target to addref its animator, this fixes memory leaks.
		// addAnimator is still addrefing
		core::list<ISceneNodeAnimator*>::Iterator it = BindedAnimators.begin();
		for(; it != BindedAnimators.end(); it++)
		{	
			if(*it == animator)
				return;
		}
		animator->grab();
		BindedAnimators.push_back(animator);
	}
}

const core::list<ISceneNodeAnimator*>& ISceneNode::getBindedAnimators() const
{
	return BindedAnimators;
}

void ISceneNode::removeBindedAnimator(ISceneNodeAnimator* animator)
{
	core::list<ISceneNodeAnimator*>::Iterator it = BindedAnimators.begin();
	for (; it != BindedAnimators.end(); ++it)
		if ((*it) == animator)
		{
			(*it)->onUnbind(this);
			(*it)->drop();
			// [NOTE] see other note in addBindedAnimator
			BindedAnimators.erase(it);
			return;
		}
}


void ISceneNode::removeBindedAnimators()
{
	core::list<ISceneNodeAnimator*>::Iterator it = BindedAnimators.begin();
	for (; it != BindedAnimators.end(); ++it)
	{
		(*it)->onUnbind(this);
		(*it)->drop();
	}

	BindedAnimators.clear();
}

video::SMaterial& ISceneNode::getMaterial(u32 num)
{
	return *((video::SMaterial*)0);
}

u32 ISceneNode::getMaterialCount() const
{
	return 0;
}



void ISceneNode::setMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
	for (u32 i=0; i<getMaterialCount(); ++i)
		getMaterial(i).setFlag(flag, newvalue);
}



void ISceneNode::setMaterialTexture(u32 textureLayer, video::ITexture* texture)
{
	if (textureLayer >= video::MATERIAL_MAX_TEXTURES)
		return;

	for (u32 i=0; i<getMaterialCount(); ++i)
		getMaterial(i).setTexture(textureLayer, texture);
}


void ISceneNode::setMaterialType(video::E_MATERIAL_TYPE newType)
{
	for (u32 i=0; i<getMaterialCount(); ++i)
		getMaterial(i).setMaterialType(newType);
}


const core::vector3df& ISceneNode::getScale() const
{
	return RelativeScale;
}


void ISceneNode::setScale(const core::vector3df& scale)
{
	RelativeScale = scale;
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
	NodeFlags |= ESNF_SCALE_CHANGED;
#endif
}


const core::quaternion& ISceneNode::getRotation() const
{
	return RelativeRotation;
}



void ISceneNode::setRotation(const core::quaternion& rotation)
{
	RelativeRotation = rotation;
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
	NodeFlags |= ESNF_ROTATION_CHANGED;
#endif
}


const core::vector3df& ISceneNode::getPosition() const
{
	return RelativeTranslation;
}


void ISceneNode::setPosition(const core::vector3df& newpos)
{
	RelativeTranslation = newpos;
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
	NodeFlags |= ESNF_TRANSLATION_CHANGED;
#endif
}

void ISceneNode::setPosition(f32 x, f32 y, f32 z)
{
	core::vector3df pos(x, y, z);
	setPosition(pos);
}

core::vector3df ISceneNode::getAbsolutePosition() const
{
	return AbsoluteTransformation.getTranslation();
}


void ISceneNode::setAutomaticCulling( E_CULLING_TYPE state)
{
	AutomaticCullingState = state;
}

E_CULLING_TYPE ISceneNode::getAutomaticCulling() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return AutomaticCullingState;
}



void ISceneNode::setDebugDataVisible(s32 state)
{
	DebugDataVisible = state;
}

s32 ISceneNode::isDebugDataVisible() const
{
	return DebugDataVisible;
}



void ISceneNode::setIsDebugObject(bool debugObject)
{
	if (debugObject)
	{
		NodeFlags |= ESNF_IS_DEBUG_OBJECT;
	}
	else
	{
		NodeFlags &= ~ESNF_IS_DEBUG_OBJECT;
	}
}



bool ISceneNode::isDebugObject() const
{
	_IRR_IMPLEMENT_MANAGED_MARSHALLING_BUGFIX;
	return (NodeFlags & ESNF_IS_DEBUG_OBJECT) != 0;
}



const core::list<ISceneNode*>& ISceneNode::getChildren() const
{
	return Children;
}


ISceneNode* ISceneNode::getSceneNodeFromName(const char* name)
{
	if(irr::stricmp(getName(), name) == 0)
		return this;

	ISceneNode* node = 0;

	const core::list<ISceneNode*>& list = getChildren();
	core::list<ISceneNode*>::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = (*it)->getSceneNodeFromName(name);
		if (node)
			return node;
	}

	return 0;
}


const ISceneNode* ISceneNode::getSceneNodeFromName(const char* name) const
{
	if(irr::stricmp(getName(), name) == 0)
		return this;

	ISceneNode* node = 0;

	const core::list<ISceneNode*>& list = getChildren();
	core::list<ISceneNode*>::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = (*it)->getSceneNodeFromName(name);
		if (node)
			return node;
	}

	return 0;
}


ISceneNode* ISceneNode::getSceneNodeFromScopeID(const char* scopeID)
{
	if(irr::stricmp(getScopeID(), scopeID) == 0)
		return this;

	ISceneNode* node = 0;

	const core::list<ISceneNode*>& list = getChildren();
	core::list<ISceneNode*>::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = (*it)->getSceneNodeFromScopeID(scopeID);
		if (node)
			return node;
	}

	return 0;
}


const ISceneNode* ISceneNode::getSceneNodeFromScopeID(const char* scopeID) const
{
	if(irr::stricmp(getScopeID(), scopeID) == 0)
		return this;

	ISceneNode* node = 0;

	const core::list<ISceneNode*>& list = getChildren();
	core::list<ISceneNode*>::ConstIterator it = list.begin();
	for (; it!=list.end(); ++it)
	{
		node = (*it)->getSceneNodeFromScopeID(scopeID);
		if (node)
			return node;
	}

	return 0;
}


void ISceneNode::setParent(ISceneNode* newParent)
{
	grab();
	remove();

	Parent = newParent;
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
	NodeFlags |= ESNF_PARENT_CHANGED;
#endif

	if (Parent)
	{
		//Parent->addChild(this);
		if(SceneManager != Parent->SceneManager)
		{
			// Propagate Scene Manager
			setSceneManager(Parent->SceneManager);
		}
	}


	drop();
}


ITriangleSelector* ISceneNode::getTriangleSelector() const
{
	return TriangleSelector;
}

void ISceneNode::setTriangleSelector(ITriangleSelector* selector)
{
	if (TriangleSelector)
		TriangleSelector->drop();

	TriangleSelector = selector;
	if (TriangleSelector)
		TriangleSelector->grab();
}

void ISceneNode::updateAbsolutePosition(bool updateChildren)
{
	if (Parent
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
		&& ((Parent->NodeFlags & ESNF_WAS_TRANSFORMATION_UPDATED)
			|| NodeFlags & (ESNF_IS_TRANSFORMATION_DIRTY
							| ESNF_PARENT_CHANGED))
#endif
	)
	{
		Parent->getAbsoluteTransformation().mult34(getRelativeTransformation(),
												   AbsoluteTransformation);
// 		AbsoluteTransformation =
// 			Parent->getAbsoluteTransformation() * getRelativeTransformation();
#ifdef _IRR_CACHE_SCENE_NODE_TRANSFORMED_BBOX_
		NodeFlags |= ESNF_WAS_TRANSFORMATION_UPDATED | ESNF_TRANSFORMED_BBOX_DIRTY;
#else
		NodeFlags |= ESNF_WAS_TRANSFORMATION_UPDATED;
#endif
		NodeFlags &= ~ESNF_PARENT_CHANGED;
#ifdef DEBUG_ISCENE_NODE
		++AbsoluteUpdate;
#endif
	}
	else
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
		if (NodeFlags & (ESNF_IS_TRANSFORMATION_DIRTY | ESNF_PARENT_CHANGED))
#endif
	{
		AbsoluteTransformation = getRelativeTransformation();
#ifdef _IRR_ENABLE_SCENE_NODE_FLAGS_
#    ifdef _IRR_CACHE_SCENE_NODE_TRANSFORMED_BBOX_
		NodeFlags |= ESNF_WAS_TRANSFORMATION_UPDATED | ESNF_TRANSFORMED_BBOX_DIRTY;
#    else
		NodeFlags |= ESNF_WAS_TRANSFORMATION_UPDATED;
#    endif
		NodeFlags &= ~ESNF_PARENT_CHANGED;
#    ifdef DEBUG_ISCENE_NODE
		++AbsoluteUpdate;
#    endif
#endif
	}

	if(updateChildren)
	{
		core::list<ISceneNode*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
			(*it)->updateAbsolutePosition(true);
	}
}


scene::ISceneNode* ISceneNode::getParent() const
{
	return Parent;
}


ESCENE_NODE_TYPE ISceneNode::getType() const
{
	return ESNT_UNKNOWN;
}


void ISceneNode::serializeAttributes(io::IAttributes* out, io::SAttributeReadWriteOptions* options) const
{
	if (!out)
		return; 
	out->addString	("Name", Name.c_str());
	out->addInt	("Id", ID );

	out->addVector3d("Position", getPosition() );
	out->addQuaternion("Rotation", getRotation() );
	out->addVector3d("Scale", getScale() );

	out->addBool	("Visible", isVisible() );
	out->addEnum	("AutomaticCulling", AutomaticCullingState, AutomaticCullingNames);
	out->addInt	("DebugDataVisible", DebugDataVisible );
	out->addBool	("IsDebugObject", isDebugObject() );

	if (options && options->Flags == io::EARWF_FOR_EDITOR)
	{
		out->addFloat	("Camera Offset", getCameraOffset());
		out->addInt		("Rendering Layer", getRenderingLayer());
	}
}



void ISceneNode::deserializeAttributes(io::IAttributes* in, io::SAttributeReadWriteOptions* options)
{
	if (!in)
		return;
	Name = in->getAttributeAsString("Name");
	ID = in->getAttributeAsInt("Id");

	setPosition(in->getAttributeAsVector3d("Position"));
	const wchar_t* pT = in->getAttributeTypeString("Rotation");
	if(wcscmp(L"vector3d", pT) == 0)
	{
		core::vector3df r = in->getAttributeAsVector3d("Rotation");
		r /= (180.0f/core::PI);//convert to radian
		setRotation(r);
	}
	else
	{//quat
		setRotation(in->getAttributeAsQuaternion("Rotation"));
	}
	setScale(in->getAttributeAsVector3d("Scale"));

	IsVisible = in->getAttributeAsBool("Visible");
	AutomaticCullingState = (scene::E_CULLING_TYPE) in->getAttributeAsEnumeration("AutomaticCulling",
			scene::AutomaticCullingNames);

	DebugDataVisible = in->getAttributeAsInt("DebugDataVisible");
	setIsDebugObject(in->getAttributeAsBool("IsDebugObject"));

	if (options && options->Flags == io::EARWF_FOR_EDITOR)
	{
		setCameraOffset(in->getAttributeAsFloat("Camera Offset"));
		setRenderingLayer(in->getAttributeAsInt("Rendering Layer"));
	}

	updateAbsolutePosition();
}


ISceneNode* ISceneNode::clone()
{
	_IRR_DEBUG_BREAK_IF("MUST implement this function");
	return 0; // to be implemented by derived classes
}

//! Sets the GameData
void ISceneNode::setGameData(void* data)
{
	GameData = data;
}

//! Returns the GameData
void* ISceneNode::getGameData() const
{
	return GameData;
}

void ISceneNode::cloneMembers(ISceneNode* toCopyFrom)
{
	Name = toCopyFrom->Name;
	AbsoluteTransformation = toCopyFrom->AbsoluteTransformation;
	RelativeTransformation = toCopyFrom->RelativeTransformation;
	RelativeTranslation = toCopyFrom->RelativeTranslation;
	RelativeRotation = toCopyFrom->RelativeRotation;
	RelativeScale = toCopyFrom->RelativeScale;
	ID = toCopyFrom->ID;
	setTriangleSelector(toCopyFrom->TriangleSelector);
	AutomaticCullingState = toCopyFrom->AutomaticCullingState;
	DebugDataVisible = toCopyFrom->DebugDataVisible;
	NodeFlags = toCopyFrom->NodeFlags;
	IsVisible = toCopyFrom->IsVisible;
	SceneManager = 0;

	// clone children

	core::list<ISceneNode*>::Iterator it = toCopyFrom->Children.begin();
	for (; it != toCopyFrom->Children.end(); ++it)
	{
		ISceneNode* pNode = (*it)->clone();
		addChild(pNode);
		pNode->drop();
	}

	// clone animators

	core::list<ISceneNodeAnimator*>::Iterator ait = toCopyFrom->Animators.begin();
	for (; ait != toCopyFrom->Animators.end(); ++ait)
	{
		ISceneNodeAnimator* anim = (*ait)->createClone();
		if (anim)
		{
			addAnimator(anim);
			anim->drop();
		}
	}
}


void ISceneNode::SetNodeFlags(s32 flag)
{
	NodeFlags = flag;
}


void ISceneNode::setCameraOffset(f32 cameraOffset)
{
	CameraOffset = cameraOffset;
}

f32 ISceneNode::getCameraOffset() const
{
	return CameraOffset;
}

void ISceneNode::setRenderingLayer(int renderingLayer)
{
	RenderingLayer = renderingLayer;
}

int ISceneNode::getRenderingLayer() const
{
	return RenderingLayer;
}

void ISceneNode::onMaterialChanged()
{
	// Only useful for ColladaMeshSceneNodes
}

//! Reset the local transformation to it initial state
void ISceneNode::resetTransform(bool resetChildren)
{
	
}

} //namespace irr 

} //namespace scene 
