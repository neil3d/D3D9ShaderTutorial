float4x4 matWorldViewPrj;

//--
struct VS_INPUT
{
	float3 pos : POSITION;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
};

VS_OUTPUT my_vs(VS_INPUT vert)
{
	VS_OUTPUT vsout;
	
	vsout.pos = mul(float4(vert.pos,1),matWorldViewPrj);
	
	return vsout;
}

//--
float4 my_ps() : COLOR
{
	return float4(1,1,0,1.0f);
}

//--
technique my_tech
{
	pass p0
	{
		VertexShader = compile vs_1_1 my_vs();
		PixelShader = compile ps_2_0 my_ps();
	}
}