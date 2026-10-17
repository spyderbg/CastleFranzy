#ifndef __IC_REFERENCE_MAKER__H
#define __IC_REFERENCE_MAKER__H

typedef fm::vector<RefTargetHandle> ReferenceTargetHandleList;

class SimpleReferenceMaker : public ReferenceMaker
{
protected:
	ReferenceTargetHandleList m_references;

public:

	void RegisterReference( RefTargetHandle refTargetHandle );
	void UnregisterReference( RefTargetHandle refTargetHandle );
	void UnregisterAllReferences();

	int NumRefs();
	RefTargetHandle GetReference( int i );
	void SetReference( int i, RefTargetHandle rtarg );

	virtual RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, PartID& partID, RefMessage message) = 0;
};

//template <class T>
//class SimpleReferenceMakerT : public SimpleReferenceMaker
//{
//protected:
//	T *m_pOwner;
//
//public:
//	SimpleReferenceMakerT( T *pOwner = NULL )
//		: m_pOwner( pOwner )
//	{}
//
//	virtual RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, PartID& partID, RefMessage message) = 0;
//};

//class ModifierReferenceMaker : public SimpleReferenceMaker
//{
//public:
//	RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, PartID& partID, RefMessage message);
//};
//
//class SkinnedMeshReferenceMaker : public SimpleReferenceMaker
//{
//public:
//	RefResult NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, PartID& partID, RefMessage message);
//};

#endif // __IC_REFERENCE_MAKER__H
