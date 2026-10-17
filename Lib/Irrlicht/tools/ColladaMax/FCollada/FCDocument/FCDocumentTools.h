/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

/**
	@file FCDocumentTools.h
	This file contains the FCDocumentTools namespace.
*/

#ifndef _FC_DOCUMENT_TOOLS_H_
#define _FC_DOCUMENT_TOOLS_H_

#include "FCDocument.h"
#include "FCDocument/FCDSceneNodeIterator.h"

class FCDTRotation;
class FCDAsset;

/**
	Contains pre-processing tools or post-processing tools used to
	modify whole COLLADA documents.
	@ingroup FCDocument
*/
namespace FCDocumentTools
{
	/** Standardizes the up-axis and the length units accross the whole document.
		Regardless of whether your application can handle different up-axis values or
		different length units, you should call this function at least once
		to ensure that the whole document has only one up-axis and one length unit.
		@param document The COLLADA document to process.
		@param upAxis The wanted up-axis for the whole document. If the
			zero-vector is given, the up-axis of the document-level asset information
			structure is used.
		@param unitInMeters The wanted length unit. If this value is zero, the length
			unit of the document-level asset information structure is used.
		@param handleTargets This flag is very specific to ColladaMax which handles the
			specificities of targeted cameras and lights: the pivots cannot be modified
			simply, since the re-targeting will happen before the pivot transform is done. */
	void FCOLLADA_EXPORT StandardizeUpAxisAndLength(FCDocument* document, const FMVector3& upAxis = FMVector3::Origin, float unitInMeters = 0.0f, bool handleTargets=false, bool bIsImporting = true );

	FMVector3 FCOLLADA_EXPORT GetUpAxisFromAsset( FCDAsset *pAsset );

	class FCDConversionSwapFunctor
	{
	private:
		enum Axis { X, Y, Z, UNKNOWN } current, target;
		Axis ConvertVector(const FMVector3& v);
		typedef void (*ConversionFn) (FMVector3& data, int32 sign);
		ConversionFn functor;

	public:
		FCDConversionSwapFunctor(const FMVector3& targetAxis)
			:	current(UNKNOWN), target(UNKNOWN)
		{
			target = ConvertVector(targetAxis);
		};

		inline void SetCurrent(const FMVector3& axis)
		{
			current = ConvertVector(axis);
			PrepareFunctor();
		}

		inline bool HasConversion() { return functor != Identity; }

		static FCDTRotation* GetLastTransformForPivot(FCDSceneNode* node);
		static void SmartAddRotationPivot(FCDSceneNode* node, const FMVector3& axis, float angle);

		void SetPivotTransform(FCDSceneNode* node, bool rollOnly);
		void operator() (FMVector3& data, bool isScale = false) { (*functor)(data, (isScale ? 1 : -1)); }

		// hack
		void operator() ( int &data, bool isScale = false){}
		void operator() ( float &data, bool isScale = false){}

	private:
		void PrepareFunctor();

		static void Identity(FMVector3& UNUSED(data), int32 UNUSED(sign)) {}
		static void XtoY(FMVector3& data, int32 sign) { float t = sign * data.x; data.x = data.y; data.y = t; }
		static void XtoZ(FMVector3& data, int32 UNUSED(sign)) { float t = data.x; data.x = data.y; data.y = data.z; data.z = t; }
		static void YtoX(FMVector3& data, int32 sign) { float t = data.x; data.x = sign * data.y; data.y = t; }
		static void YtoZ(FMVector3& data, int32 sign) { float t = sign * data.y; data.y = data.z; data.z = t; }
		static void ZtoX(FMVector3& data, int32 UNUSED(sign)) { float t = data.z; data.z = data.y; data.y = data.x; data.x = t; }
		static void ZtoY(FMVector3& data, int32 sign) { float t = data.y; data.y = sign * data.z; data.z = t; }	
	};

	class FCDConversionUnitFunctor
	{
	private:
		float factor, target;

	public:
		FCDConversionUnitFunctor(float _target)
			:	factor(1.0f), target(_target) {}

		bool HasConversion() { return !IsEquivalent(factor, 1.0f); }
		void SetCurrent(float current) { factor = ((current <= 0.0f) ? 1.0f : (current / target)); }
		float GetConversionFactor() { return factor; }
		void operator() (float& data) { data *= factor; }
		void operator() (FMVector3& data) { data *= factor; }
	};

	typedef fm::pvector<FCDSceneNodeIterator> FCDSceneNodeIteratorList;
	class VisualSceneNodeIterator
	{
	private:
		FCDSceneNodeIteratorList queue;

	public:
		VisualSceneNodeIterator(FCDVisualSceneNodeLibrary* visualSceneLibrary);
		virtual ~VisualSceneNodeIterator();
		
		FCDSceneNode* Next();
		bool IsDone();

	private:
		void CleanQueue(); // makes sure we can get the next node easily
	};
};

#endif // _FC_DOCUMENT_TOOLS_H_
