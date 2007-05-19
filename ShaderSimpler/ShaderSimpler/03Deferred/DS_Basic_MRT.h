#pragma once
#include "ds_basic.h"

class DS_Basic_MRT : public DS_Basic
{
public:
	DS_Basic_MRT(void);
	virtual ~DS_Basic_MRT(void);

private:
	virtual bool createPosRT();
	virtual void buildGBuffers();
	virtual const char* getShadingTech();
	virtual void setEffectTexture();

private:
	IDirect3DTexture9*	m_posRT_Z;
};
