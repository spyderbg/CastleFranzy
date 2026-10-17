#include "stdafx.h"
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_COLLADA_BINARY_LOADER_
#include "CColladaBillboardSceneNode.h"

#include "ICameraSceneNode.h"
#include "ISceneManager.h"

namespace irr
{
namespace collada
{

void
CBillboardSceneNode::updateAbsolutePosition(bool updateChildren)
{
	if(SceneManager)
	{
		const core::matrix4& pivotRelativeTransformation
			= Parent->getRelativeTransformation();
		const core::matrix4& pivotAbsoluteTransformation
			= Parent->getAbsoluteTransformation();
		core::matrix4 pivotAbsoluteTransformationNoTrans
			= pivotAbsoluteTransformation;
		pivotAbsoluteTransformationNoTrans.setTranslation(core::vector3df(0.f, 0.f, 0.f));
		const core::vector3df pivotAbsolutePosition = Parent->getAbsolutePosition();
		
		scene::ICameraSceneNode* camera = SceneManager->getActiveCamera();
		const core::vector3df cameraAbsolutePosition = camera->getAbsolutePosition();
		core::matrix4 viewMatrix = camera->getViewMatrix();
		core::vector3df cameraUp(viewMatrix( 0, 1 ), viewMatrix( 1, 1 ), viewMatrix( 2, 1 ));
		core::vector3df cameraFront(viewMatrix( 0, 2 ), viewMatrix( 1, 2 ), viewMatrix( 2, 2 ));
		core::vector3df cameraRight(viewMatrix( 0, 0 ), viewMatrix( 1, 0 ), viewMatrix( 2, 0 ));

		core::vector3df billboardFront = Node->pBillboard->front;
		pivotAbsoluteTransformationNoTrans.transformVect(billboardFront);
		billboardFront.normalize();
		
		core::vector3df billboardUp = Node->pBillboard->up;
		pivotAbsoluteTransformationNoTrans.transformVect(billboardUp);
		billboardUp.normalize();

		core::vector3df billboardRight
			= billboardFront.crossProduct(billboardUp).normalize();
		core::vector3df billboardUpPrime
			= billboardRight.crossProduct(billboardFront).normalize();

		core::vector3df billboardOrientedFront;
		core::vector3df billboardOrientedRight;
		core::vector3df billboardOrientedUp;

		core::matrix4 transform;
		if (Node->pBillboard->type == SBillboard::EBT_UP_ONLY)
		{
			if (Node->pBillboard->orientation == SBillboard::EBO_PARALLEL_TO_CAMERA_AXIS)
			{
				billboardOrientedFront = -cameraFront;
			}
			else /*if (Node->pBillboard->orientation == FUDaeBillboard::EBO_TOWARD_CAMERA)*/
			{
				billboardOrientedFront
					= (cameraAbsolutePosition - pivotAbsolutePosition).normalize();
			}

			core::vector3df billboardFrontVOnPlane
				= billboardUp.crossProduct(billboardOrientedFront.crossProduct(billboardUp)).normalize();
			core::vector3df origFrontVectorOnPlane
				= billboardUp.crossProduct(billboardRight).normalize();

			float radians
				= acos(billboardFrontVOnPlane.dotProduct(origFrontVectorOnPlane));
			radians
				= billboardUp.dotProduct(billboardFrontVOnPlane.crossProduct(origFrontVectorOnPlane)) < 0 ? radians : -radians;

			core::quaternion quat;
			quat.fromAngleAxis(radians, billboardUp);
			transform = quat.getMatrix();
		}
		else // Node->pBillboard->type == EBT_FREE
		{
			if (Node->pBillboard->orientation == SBillboard::EBO_PARALLEL_TO_CAMERA_AXIS)
			{
				billboardOrientedFront = -cameraFront;
				billboardOrientedUp = cameraUp;
				billboardOrientedRight = cameraRight;
			}
			else /*if (Node->pBillboard->orientation == FUDaeBillboard::EBO_TOWARD_CAMERA )*/
			{
				billboardOrientedFront
					= (cameraAbsolutePosition - pivotAbsolutePosition).normalize();
				billboardOrientedRight
					= -cameraUp.crossProduct(billboardOrientedFront).normalize();
				billboardOrientedUp
					= billboardOrientedRight.crossProduct(billboardOrientedFront).normalize();
			}

			core::matrix4 billboardTM;
			{
				billboardTM(0, 0) = billboardOrientedRight.X;
				billboardTM(0, 1) = billboardOrientedRight.Y;
				billboardTM(0, 2) = billboardOrientedRight.Z;
				billboardTM(0, 3) = 0;

				billboardTM(1, 0) = billboardOrientedUp.X;
				billboardTM(1, 1) = billboardOrientedUp.Y;
				billboardTM(1, 2) = billboardOrientedUp.Z;
				billboardTM(1, 3) = 0;

				billboardTM(2, 0) = billboardOrientedFront.X;
				billboardTM(2, 1) = billboardOrientedFront.Y;
				billboardTM(2, 2) = billboardOrientedFront.Z;
				billboardTM(2, 3) = 0;
			}

			{
				transform(0, 0) = billboardRight.X;
				transform(0, 1) = billboardRight.Y;
				transform(0, 2) = billboardRight.Z;
				transform(0, 3) = 0;

				transform(1, 0) = billboardUpPrime.X;
				transform(1, 1) = billboardUpPrime.Y;
				transform(1, 2) = billboardUpPrime.Z;
				transform(1, 3) = 0;

				transform(2, 0) = billboardFront.X;
				transform(2, 1) = billboardFront.Y;
				transform(2, 2) = billboardFront.Z;
				transform(2, 3) = 0;

				bool inverseSuccess = transform.makeInverse();
				_IRR_DEBUG_BREAK_IF(!inverseSuccess);
				
				transform = billboardTM.mult34(transform);
			}
		}

		// apply pivot translation
		core::matrix4 pivotTranslationMatrix;
		pivotTranslationMatrix.setTranslation(pivotAbsolutePosition);

		// apply billboard rotation
		core::matrix4 tmp;
		pivotTranslationMatrix.mult34(transform, tmp);

		// apply pivot rotation + scale
		core::matrix4 tmp2;
		tmp.mult34( pivotAbsoluteTransformationNoTrans, tmp2);

		// apply relative transformation
		NodeFlags |= scene::ESNF_WAS_TRANSFORMATION_UPDATED;
		tmp2.mult34(getRelativeTransformation(), AbsoluteTransformation);
	}

	if (updateChildren)
	{
		core::list<ISceneNode*>::Iterator it = Children.begin();
		for (; it != Children.end(); ++it)
		{
			(*it)->updateAbsolutePosition(true);
		}
	}
}

} // namespace collada
} // namespace irr
#endif