#pragma once

#include "inc.h"
#include "ShaderSimpler.h"

enum EShaderSimple
{
	ESS_BasicLighting,
	ESS_PongShading,
	ESS_CelShading,
	ESS_CubeEnvMapping,
	ESS_BumpMapping,
	ESS_ParallaxMapping,
	
	ESS_PP_Bloom,

	ESS_DS_Basic,
	ESS_DS_Basic_MRT,

	ESS_End
};

/**	\class SimpleD3DApp
	\brief 封装win32窗口创建，d3d初始化的操作
*/
class SimpleD3DApp
{
public:
	SimpleD3DApp(void);
	virtual ~SimpleD3DApp(void);

	bool create(HINSTANCE hInstance, int width, int height, const char* szWindowTitle="Simple d3d window");
	void destroy();

	void render();

	void openSimple(EShaderSimple ess);
	void onKeyDown(DWORD vk);

private:
	bool createD3D();
	void destroyD3D();

	void CalFPS();

private:
	HWND			m_hWnd;
	ShaderSimpler	*m_pSimpler;

	IDirect3D9			*m_pD3D;
	IDirect3DDevice9	*m_pD3DDevice;

	float FPS;
};
