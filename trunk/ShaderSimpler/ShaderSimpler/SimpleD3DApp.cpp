#include ".\simpled3dapp.h"
#include "resource.h"
#include "CameraCtrl.h"
#include "DrawingUtil.h"

#include "01Lighting\BasicLightingSimpler.h"
#include "01Lighting\PhongShading.h"
#include "01Lighting\CelShading.h"
#include "01Lighting\CubeEnvMapping.h"
#include "01Lighting\BumpMapping.h"
#include "01Lighting\ParallaxMapping.h"
#include "01Lighting\DeferredShading.h"

#include "02PostProcess\PostProcessBloom.h"
//-----------------------------------------------------------------------------
SimpleD3DApp simpleApp;

//--win main
int WINAPI WinMain( HINSTANCE hInstance,
				   HINSTANCE hPrevInstance,
				   LPSTR     lpCmdLine,
				   int       nCmdShow )
{
	//--创建
	if(!simpleApp.create(hInstance,clientWidth,clientHeight,"Shader Simpler"))
		return -1;

	//--默认camera参数
	g_camera.setPerspective(45.0f,clientWidth,clientHeight,10,5000);

	simpleApp.openSimple(EShaderSimple(ESS_End-1));

	//--消息循环
	MSG        uMsg;
	memset(&uMsg,0,sizeof(uMsg));

	while( uMsg.message != WM_QUIT )
	{
		if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
		{ 
			TranslateMessage( &uMsg );
			DispatchMessage( &uMsg );
		}
		else
			simpleApp.render();
	}


	//--销毁
	DrawingUtil::getInst()->destroy();
	simpleApp.destroy();
	return 0;
}


//--消息处理函数
LRESULT CALLBACK WindowProc( HWND   hWnd, 
							UINT   msg, 
							WPARAM wParam, 
							LPARAM lParam )
{
	switch( msg )
	{
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDM_Exit:
			PostQuitMessage(0);
			break;
		//--simpler
		// light
		case IDM_BasicLighting:
			simpleApp.openSimple(ESS_BasicLighting);
			break;
		case ID_SIMPLER_PHONGSHADING:
			simpleApp.openSimple(ESS_PongShading);
			break;
		case ID_SIMPLER_CEL:
			simpleApp.openSimple(ESS_CelShading);
			break;
		case ID_SIMPLER_CUBEENVMAPPING:
			simpleApp.openSimple(ESS_CubeEnvMapping);
			break;
		case ID_SIMPLER_BUMPMAPPING:
			simpleApp.openSimple(ESS_BumpMapping);
			break;
		case ID_01LIGHTING_PARALLAXMAPPING:
			simpleApp.openSimple(ESS_ParallaxMapping);
			break;
		case ID_01LIGHTING_DEFERREDSHADING:
			simpleApp.openSimple(ESS_DeferredShading);
			break;
		// post process
		case ID_02POSTPROCESS_BLOOM:
			simpleApp.openSimple(ESS_PP_Bloom);
			break;
		//-- view
		case ID_VIEW_RESETCAMERA:
			g_camera.setDefaultView();
			break;
		}
		break;
	case WM_KEYDOWN:
		{
			switch( wParam )
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			default:
				simpleApp.onKeyDown((DWORD)wParam);
				g_camera.onKeyDown((DWORD)wParam);
				break;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		g_camera.onLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)); 
		break;
	case WM_LBUTTONUP:
		g_camera.onLbuttonUp();
		break;
	case WM_MOUSEMOVE:
		g_camera.onMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;
	case WM_MOUSEWHEEL:
		g_camera.onMouseWheel(GET_WHEEL_DELTA_WPARAM(wParam));
		break;
	case WM_CLOSE:
		{
			PostQuitMessage(0);
		}

	case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;

	default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}

	return 0;
}


//-----------------------------------------------------------------------------
SimpleD3DApp::SimpleD3DApp(void)
{
	m_hWnd = NULL;
	m_pSimpler = NULL;

	m_pD3D = NULL;
	m_pD3DDevice = NULL;

	FPS = 0;
}

SimpleD3DApp::~SimpleD3DApp(void)
{
}

bool SimpleD3DApp::create(HINSTANCE hInstance, int width, int height, const char* szWindowTitle)
{
	//--注册窗口类
	WNDCLASSEX winClass;

	winClass.lpszClassName = "SimpleD3DAppWndClass";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = NULL;
	winClass.hIconSm	   = NULL;
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = MAKEINTRESOURCE(IDR_MENU1);
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if( !RegisterClassEx(&winClass) )
		return false;

	int scrW=::GetSystemMetrics(SM_CXSCREEN);
	int scrH=::GetSystemMetrics(SM_CYSCREEN);

	RECT wndRc;
	wndRc.left = (scrW-width)/2;
	wndRc.top = (scrH-height)/2;
	wndRc.right = wndRc.left + width;
	wndRc.bottom = wndRc.top + height;


	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
	AdjustWindowRect(&wndRc,style,TRUE);

	m_hWnd = CreateWindowEx( NULL, "SimpleD3DAppWndClass", 
		szWindowTitle,
		style,
		wndRc.left, wndRc.top, wndRc.right-wndRc.left, wndRc.bottom-wndRc.top,
		NULL, NULL, hInstance, NULL );

	if( m_hWnd == NULL )
		return false;

	ShowWindow( m_hWnd, SW_SHOW );
	UpdateWindow( m_hWnd );

	if(createD3D())
	{
		DrawingUtil::getInst()->init(m_pD3DDevice);
		return true;
	}
	else
		return false;
}

void SimpleD3DApp::destroy()
{
	SAFE_DELETE(m_pSimpler);

	destroyD3D();
}

bool SimpleD3DApp::createD3D()
{
	HRESULT hr;

	//--创建d3d对象
	m_pD3D = Direct3DCreate9( D3D_SDK_VERSION );
	if(m_pD3D == NULL)
	{
		::MessageBox(m_hWnd,"D3D Create failed.","Error",MB_OK);
		return false;
	}

	//--测试shader版本
	D3DCAPS9 caps;
	hr = m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&caps);
	if(caps.VertexShaderVersion < D3DVS_VERSION(2,0)
		|| caps.PixelShaderVersion < D3DPS_VERSION(2,0))
	{
		::MessageBox(m_hWnd,"显卡支持的Shader版本太低","Error",MB_OK);
		return false;
	}

	//--创建device
	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

	d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed               = TRUE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

	DWORD dwBehaviorFlags = D3DCREATE_HARDWARE_VERTEXPROCESSING;

	D3DDEVTYPE devType = D3DDEVTYPE_HAL;
#ifdef SHADER_DEBUG
	devType = D3DDEVTYPE_REF;
#endif

	hr = m_pD3D->CreateDevice( D3DADAPTER_DEFAULT, 
		devType, 
		m_hWnd,
		dwBehaviorFlags, &d3dpp, &m_pD3DDevice );


	return SUCCEEDED(hr);
}

void SimpleD3DApp::destroyD3D()
{
	SAFE_RELEASE(m_pD3D);
	SAFE_RELEASE(m_pD3DDevice);
}

void SimpleD3DApp::render()
{
	HRESULT hr;

	hr = m_pD3DDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		D3DCOLOR_COLORVALUE(0.8f,0.8f,0.8f,1.0f), 1.0f, 0 );

	hr = m_pD3DDevice->BeginScene();

	if(SUCCEEDED(hr))
	{
		if(m_pSimpler)
			m_pSimpler->render();

		//-- show fps
		CalFPS();
		char szText[512] = {0};
		sprintf(szText,"FPS:%.2f",FPS);
		DrawingUtil::getInst()->drawText(szText,2,2);
		
		//--
		m_pD3DDevice->EndScene();
		m_pD3DDevice->Present( NULL, NULL, NULL, NULL );
	}

	//Sleep(20);
}

void SimpleD3DApp::openSimple(EShaderSimple ess)
{
	//-- close old
	if(m_pSimpler)
	{
		delete m_pSimpler;
		m_pSimpler = NULL;
	}

	//-- create new
	switch(ess)
	{
	case ESS_BasicLighting:
		m_pSimpler = new BasicLightingSimpler;
		::SetWindowText(m_hWnd,"Basic Lighting");
		break;
	case ESS_PongShading:
		m_pSimpler = new PhongShading;
		::SetWindowText(m_hWnd,"PhongShading");
		break;
	case ESS_CelShading:
		m_pSimpler = new CelShading;
		::SetWindowText(m_hWnd,"CelShading");
		break;
	case ESS_CubeEnvMapping:
		m_pSimpler = new CubeEnvMapping;
		::SetWindowText(m_hWnd,"Cubic Environment Mapping");
		break;
	case ESS_BumpMapping:
		m_pSimpler = new BumpMapping;
		::SetWindowText(m_hWnd,"Bump Mapping ( normal mapping )");
		break;
	case ESS_ParallaxMapping:
		m_pSimpler = new ParallaxMapping;
		::SetWindowText(m_hWnd,"Parallax Mapping");
		break;
	case ESS_DeferredShading:
		m_pSimpler = new DeferredShading;
		::SetWindowText(m_hWnd,"Deferred Shading");
		break;
	case ESS_PP_Bloom:
		m_pSimpler = new PostProcessBloom;
		::SetWindowText(m_hWnd,"PostProcess -- Bloom");
		break;
	}

	//--init
	if(m_pSimpler)
	{
		bool bOK = m_pSimpler->init(m_pD3DDevice);
		if(!bOK)
		{
			MessageBox(m_hWnd, "init failed!", "error", MB_OK);
			delete m_pSimpler;
			m_pSimpler = NULL;

			::SetWindowText(m_hWnd,"Please select a shader ... ");
		}
	}
}

void SimpleD3DApp::CalFPS()
{
	float deltaTime;
	static DWORD lastTime = timeGetTime();
	deltaTime = (timeGetTime()-lastTime)*0.001f;
	lastTime = timeGetTime();

	static float c = 0;
	static float sum = 0;
	if(c < 10)
	{
		c += 1.0f;
		sum += deltaTime;
	}
	else
	{
		FPS = c/sum;

		c=0;
		sum=0;
	}
}

void SimpleD3DApp::onKeyDown(DWORD vk)
{
	if(m_pSimpler)
		m_pSimpler->onKeyDown(vk);
}