#ifndef __GL3DCAMERA_H_INCLUDED__
#define __GL3DCAMERA_H_INCLUDED__

#if(PLATFORM==PLATFORM_MAC_OS_X)&&(!defined(WIN32))
#include <OpenGLES/ES1/gl.h>
#elif IPHONE > 0
#include "../../tools/rasteroid3.1/include/gles/gl.h"
#else
#include "gl.h"	// Added by ClassView
#endif
#include "glt_math_alg.h"


fx32 FX_MUL(fx32 v1, fx32 v2);

//---------------------------------------------------------------------
//--
//--	class GL3DCamera
//--
//---------------------------------------------------------------------
class CGame;
class GL3DCamera
{
	enum{	DEFAULT_ZNEAR = GLT_FLOAT_TO_FIXED(1.5),
			DEFAULT_ZFAR = GLT_INT_TO_FIXED(129)
	};

public:
	enum {DEFAULT_FOV = GLT_INT_TO_FIXED(23),
		DEFAULT_LAND_SCAPE_FOV = GLT_INT_TO_FIXED(18)};

	enum EVisibilityFlags
	{
		kUp	= 0x01,
		kDown	= 0x02,
		kLeft	= 0x04,
		kRight	= 0x08,
		kNear	= 0x10,
		kFar	= 0x20,
	};

	GL3DCamera(bool isLandScape = false);
	virtual ~GL3DCamera();
	
	
	void			SetFov(GLfixed fov_in);// The camera matrix is still synchronize
	void			SetLookAt( const GLTvec3D& eyepos,const GLTvec3D& lookat );

	void			ApplyTransform() const;

	const GLTmatrix&	GetInvMatrixNoTrans() const;
	//const GLTmatrix&	GetInvMatrix() const;
//	GLTcoord			GetViewCoord(void) const;
	
//	void				SetViewportRatio(int viewportSizeX,int viewportSizeY);

	virtual void		Update(GLfixed deltaT);	// in secondc
	void			Init(CGame* pGame, bool isLandScape);

	unsigned int		TestVisibility(const GLTvec3D&) const;	

	const GLTvec3D&		GetPos()const	{return m_pos;}
	const GLTvec3D&		GetRot() const	{return m_rot;}
	GLfixed			GetFov()const	{return m_fov;}	
	GLTmatrix		GetMatrix(void) const;
	GLfixed			GetZNear() const {return m_zNear;}
	GLfixed			GetZFar() const { return m_zFar;}

	static int		GetAlignmentRotation(const GLTvec3D&,const GLTvec3D&);
	static GLfixed		ASin(GLfixed x);

	int			GetRotationYToAllignToCamera(const GLTvec3D&) const;

	GLfixed			AspectRatio() const {return m_viewportRatio;}

	// if the returned value Z ==0, the screen position is not computable
	GLTvec3D		GetScreenPosition(const GLTvec3D& worldSpace/*,int screenWidth,int screenHeight*/) const;
	void			Set(const GLTvec3D& pos,const GLTvec3D& rot);

	void			InvertAspect(CGame* pGame, bool isLandScape);

private:	
	void			ComputeMatrix() const;

	inline int		GetFoVAtScale() const {return m_fov /  ( 360 <<  (GLT_FIXED_SHIFT - GLT_ANGLE_SHIFT));}


	
private:
	GLTvec3D	m_pos;
	GLTvec3D	m_rot;
	
	GLfixed		m_fov;

	GLfixed		m_viewportRatio;
	GLfixed		m_zNear;
	GLfixed		m_zFar;
	
	GLfixed		m_viewportCenterX;
	GLfixed		m_viewportCenterY;
	GLfixed		m_viewportW;
	GLfixed		m_viewportH;
//	GLfixed		m_viewportX;
//	GLfixed		m_viewportY;
	// Thoses members are set By Compiutematrix() at updateTime
	mutable bool	m_dirty;
	mutable GLTmatrix	m_matrix;
//	mutable GLTmatrix	m_mtx_inv;
	mutable GLTmatrix	m_mtx_inv_notrans;



private:
	//  [11/4/2006 baibo.zhang]
	// added m_frustrumBTLR to test sphere visibility
#define GLT_FRUSTRUM_BOTTOM		0
#define GLT_FRUSTRUM_TOP		1
#define GLT_FRUSTRUM_LEFT		2
#define GLT_FRUSTRUM_RIGHT		3
#define GLT_FRUSTRUM_MAX		4
	mutable GLfixed	m_frustrumBTLR[GLT_FRUSTRUM_MAX][3];
	void ComputeFrustrumBTLR(GLfixed w, GLfixed h, GLfixed n, GLfixed f) const;
	
	inline bool	CircleLineTest( GLfixed planeNormalx, GLfixed planeNormaly, GLfixed centrex, GLfixed centrey, GLfixed radius)
	{
		GLfixed	centre2x = centrex - GLTMul(planeNormalx, radius);
		GLfixed	centre2y = centrey - GLTMul(planeNormaly, radius);


		GLfixed64 dot = planeNormalx * (GLfixed64)centre2x + planeNormaly * (GLfixed64)centre2y;

		if (dot >= 0)	// > ?
			return false;
		else
			return true;
	}
public:


	bool	TestSphereInFrustum(const GLTvec3D& centerPos, GLfixed radius)
	{
		GLTvec3D	pos;
		m_matrix.Mul(centerPos, pos);
		
		//pos.z = -pos.z;

		//if ((-pos.z - radius) > m_zFar)
		//	return false;

		//if ((-pos.z + radius) < m_zNear)
		//	return false;

		//if (!CircleLineTest(m_frustrumBTLR[2][0], m_frustrumBTLR[2][2], pos.x, pos.z, radius))
		//	return false;

		//if (!CircleLineTest(m_frustrumBTLR[3][0], m_frustrumBTLR[3][2], pos.x, pos.z, radius))
		//	return false;

		//if (!CircleLineTest(m_frustrumBTLR[0][1], m_frustrumBTLR[0][2], pos.y, pos.z, radius))
		//	return false;

		//if (!CircleLineTest(m_frustrumBTLR[1][1], m_frustrumBTLR[1][2], pos.y, pos.z, radius))
		//	return false;


		//pos.z = -pos.z;

		if ((-pos.z - radius) > m_zFar)
			return false;

		if ((-pos.z + radius) < m_zNear)
			return false;

		int dot1 = FX_MUL(m_frustrumBTLR[3][0], pos.x);
		int dot2 = FX_MUL(m_frustrumBTLR[3][2], pos.z);
	
		if(dot2 + dot1 >= radius)	//LEFT
			return false;
		if(dot2 - dot1 >= radius)	//RIGHT
			return false;

		dot1 = FX_MUL(m_frustrumBTLR[1][1], pos.y);
		dot2 = FX_MUL(m_frustrumBTLR[1][2], pos.z);
		
		if(dot2 + dot1 >= radius)	//TOP
			return false;
		if(dot2 - dot1 >= radius)	//BOT
			return false;


		return true;
	}
	void TestSphereInFrustum(const GLTvec3D* centerPos, bool* visibility)
	{
		GLTvec3D	pos;
		bool visible[6][2];
		int sign[6] = {0};
		for (int i = 0; i < 6; i++)
		{
			m_matrix.Mul(centerPos[i], pos);

			int dot1 = FX_MUL(m_frustrumBTLR[GLT_FRUSTRUM_RIGHT][0], pos.x);
			int dot2 = FX_MUL(m_frustrumBTLR[GLT_FRUSTRUM_RIGHT][2], pos.z);
			visible[i][0] = ((dot2 + dot1 < 0)&&(dot2 - dot1 < 0));

			dot1 = FX_MUL(m_frustrumBTLR[GLT_FRUSTRUM_TOP][1], pos.y);
			dot2 = FX_MUL(m_frustrumBTLR[GLT_FRUSTRUM_TOP][2], pos.z);
//			visible[i][1] = ((dot2 + dot1 < 0)&&(dot2 - dot1 < 0));
			if (dot2 + dot1 >= 0)
			{
				sign[i] = 1;
				visible[i][1] = false;
			} 
			else if (dot2 - dot1 >= 0)
			{
				sign[i] = 2;
				visible[i][1] = false;
			} 
			else
			{
				visible[i][1] = true;
			}
		}
		visibility[0] = (((visible[0][0] || visible[1][0] || visible[2][0] || visible[3][0])&&
			(visible[0][1] || visible[1][1] || visible[2][1] || visible[3][1]))||
			((sign[0]|sign[1]|sign[2]|sign[3]|sign[4]|sign[5])==3));
		visibility[1] = (((visible[2][0] || visible[3][0] || visible[4][0] || visible[5][0])&&
			(visible[2][1] || visible[3][1] || visible[4][1] || visible[5][1]))||
			((sign[0]|sign[1]|sign[2]|sign[3]|sign[4]|sign[5])==3));
	}

public:

	void SetNearFarPlane(GLfixed zNear, GLfixed zFar);
	void GetScreenPosition(int vectorNum, const GLTvec3D* worldSpace/*,int screenWidth,int sceenHeight */,GLTvec3D *outCamSpace) const;

	bool ScreenPositionToWorld(const GLTvec3D& screenPos, GLTvec3D &worldPos/*, int screenWidth, int screenHeight*/);
	void GetDirectionFromRot(GLTvec3D &dV);
	
//WJ	DebugFixedVec _pos;
//WJ	DebugFixedVec _rot;

protected:
	GLfixed GetFocalLength(/*int screenWidth*/) const;
};



#endif 
