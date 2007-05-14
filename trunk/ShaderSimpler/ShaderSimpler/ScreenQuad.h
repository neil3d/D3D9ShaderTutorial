#pragma once
#include "inc.h"

class ScreenQuad
{
	struct Vert
	{
		D3DXVECTOR4	pos;
		D3DXVECTOR2	uv;
	};
public:
	ScreenQuad(void);
	~ScreenQuad(void);

	bool init(IDirect3DDevice9 *pD3DDevice);
	void draw();

private:
	IDirect3DDevice9	*m_pD3DDevice;

	IDirect3DVertexBuffer9		*m_pQuadVB;
	IDirect3DVertexDeclaration9	*m_pVertDecl;
};
