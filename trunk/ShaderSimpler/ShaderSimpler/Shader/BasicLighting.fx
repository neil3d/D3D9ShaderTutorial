
float4x4 matWorldViewPrj;
float4x4 matWorld;
float3 lightPos = float3(0,60,-60);
float lightAttenuation = 0.01;

float4 mtlDiffuseColor = float4(1,0,0,1);

//--
struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float4 color : COLOR0;
};

VS_OUTPUT my_vs(VS_INPUT vert)
{
	VS_OUTPUT vsout;
	
	vsout.pos = mul(float4(vert.pos,1),matWorldViewPrj);
	
	float3 N = normalize(mul(vert.normal, matWorld));
	
	float3 worldPos = mul(float4(vert.pos,1),matWorld);
	float3 L = lightPos - worldPos;
	float d = length(L);
	L /= d; // normalize L
	
	float atten = 1/(lightAttenuation*d);
	float r = saturate(dot(N, L));
	vsout.color = mtlDiffuseColor * r * atten + float4(0.2,0,0,0);
	
	return vsout;
}

//--
float4 my_ps(float4 color : COLOR0) : COLOR
{
	return color;
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
