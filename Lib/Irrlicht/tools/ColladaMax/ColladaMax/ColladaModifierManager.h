/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _COLLADA_MODIFIER_MANAGER_H_
#define _COLLADA_MODIFIER_MANAGER_H_

class ColladaModifier;

typedef fm::pvector<ColladaModifier> ColladaModifierStack;

class ColladaModifierManager
{
protected:
	ColladaModifierStack modifiers;

public:
	~ColladaModifierManager();

	virtual void ResolveModifiers( INode *pNode, Object* obj );
	virtual void ResolveModifiers( INode *pNode, Object* obj, ColladaModifierStack &modifiers ) = 0;	
	
	static void ResolveModifiers( INode *pNode, Object *obj, const ColladaModifierStack &targetModifiers, ColladaModifierStack &foundModifiers );
};

#endif
