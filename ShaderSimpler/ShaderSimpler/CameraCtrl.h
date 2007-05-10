#pragma once
#include "inc.h"

class CameraCtrl
{
public:
	CameraCtrl(void);
	~CameraCtrl(void);

	void setPerspective(float fov, int clientW, int clientH, float znear, float zFar);

	void setDefaultView();
	
	const D3DXMATRIX& getProjectMat()	{	return m_matPrj;}
	const D3DXMATRIX& getViewMat()		{	return m_matView;}
	const D3DXVECTOR3& getEyePos()		{	return m_eyePos;}

	void onKeyDown(DWORD vk);
	void onLButtonDown(int x,int y);
	void onLbuttonUp();
	void onMouseMove(int x,int y);
	void onMouseWheel(int delta);
private:
	void updateViewMat();
private:
	D3DXMATRIX m_matPrj;
	D3DXMATRIX m_matView;

	D3DXVECTOR3		m_eyePos;
	D3DXVECTOR3		m_lookAt;
	float			m_dist;
	D3DXQUATERNION	m_rotate;
	D3DXQUATERNION	m_lastRotate;

	//--
	bool	m_bDrag;
	POINT	m_lastDragPt;
};

extern CameraCtrl g_camera;