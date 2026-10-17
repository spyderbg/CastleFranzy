/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _EMITTER_EXPORTER_H_
#define _EMITTER_EXPORTER_H_

#ifndef _BASE_EXPORTER_H_
#include "BaseExporter.h"
#endif // _BASE_EXPORTER_H_

#include "ColladaModifierManager.h"

class FCDEntity;
class FCDEntityInstance;
class ExportNode;
class FCDEmitter;
class IParamBlock;
class INode;
class FCDSceneNode;

typedef fm::pvector<INode> NodeList;

class EmitterExporter : public BaseExporter, public ColladaModifierManager
{
public:
	enum
	{
		//EXPORT_AFFECTS_CONTROLLERS                 = 0x00000001
		//, EXPORT_REMOVE_DUPLICATE_SOURCE_DATA      = 0x00000002
		//, EXPORT_REMOVE_UNREFERENCED_SOURCE_DATA   = 0x00000004
		//, EXPORT_DEINDEX_VERTEX_DATA			     = 0x00000008
		//, EXPORT_ENABLE_NAIVE_DEINDEXATION		 = 0x00000010
		//, EXPORT_DEFAULTS		                     = EXPORT_REMOVE_DUPLICATE_SOURCE_DATA | EXPORT_REMOVE_UNREFERENCED_SOURCE_DATA | EXPORT_DEINDEX_VERTEX_DATA
		/*,*/ EXPORT_DEFAULTS		                   = 0
	};

private:
	void ExportEmitterShape( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle );
	void ExportEmitterEmission( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle );
	void ExportEmitterRandomization( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle );
	void ExportParticleLife( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle );
	void ExportParticleSize( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle );
	void ExportParticleMotion( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle );
	void ExportParticleType( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle );
	void ExportParticleAnimationOffset( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle );
	void ExportParticleAnimationScaling( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle );
	void ExportParticleSpin( FCDEmitter *pEmitter, SimpleParticle *pMaxSimpleParticle );
	void ExportVariableVector( SimpleParticle *pMaxSimpleParticle, fstring &prefix, const FMVector3 &vectorPBlockIds, FCDParameterAnimatableVector3 &vector, int variancePBlockId, FCDParameterAnimatableFloat &variance );

	void AddNodeTarget( INode *pNode, FCDEmitter* pEmitter, void ( FCDEmitter::*pNodeTargetSetterFunc )( FCDSceneNode * ) );
	void AddToMaxForceNodes( INode *pNode, Modifier *pModifier, NodeList& maxForceNodes );
	void GetEmitterForceMaxNodes( INode* node, NodeList& maxForceNodes );

public:
	EmitterExporter(DocumentExporter* document);
	virtual ~EmitterExporter();

	FCDEntity* ExportEmitter(INode* node, Object* object, int flags = EXPORT_DEFAULTS);
	void CalculateForcedNodes(INode* node, Object* object, NodeList& forcedNodes);
	void LinkEmitters();

	void ResolveModifiers( INode *pNode, Object* obj, ColladaModifierStack &modifiers );
};

#endif // _EMITTER_EXPORTER_H_
