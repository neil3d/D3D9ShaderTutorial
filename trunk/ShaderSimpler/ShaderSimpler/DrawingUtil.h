#pragma once

#include "inc.h"

class DrawingUtil
{
public:
	DrawingUtil(void);
	~DrawingUtil(void);

	static DrawingUtil* getInst();
	void init(IDirect3DDevice9 *pD3DDevice);
	void destroy();

	ID3DXEffect *loadEffect(IDirect3DDevice9 *pD3DDevice,const char* szFileName);

	void drawLight(D3DXVECTOR3 pos);
	void drawText(const char* szText,int x, int y, D3DCOLOR color = 0xFFFFFFFF);
private:
	ID3DXFont	*m_pD3DFont;
	ID3DXMesh	*m_pLightMesh;
	ID3DXEffect	*m_pSimpleEffect;
};
