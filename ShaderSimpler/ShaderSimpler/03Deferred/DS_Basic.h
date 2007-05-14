#pragma once
#include "..\shadersimpler.h"

class DS_Basic  : public ShaderSimpler
{
public:
	DS_Basic(void);
	virtual ~DS_Basic(void);

	virtual bool init(IDirect3DDevice9 *pD3DDevice);
	virtual void render();
};
