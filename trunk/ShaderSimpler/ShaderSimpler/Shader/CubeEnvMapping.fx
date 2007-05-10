float4x4 matWorldViewPrj;
float4x4 matWorld;
float4x4 matView;
float3 eyePos;

texture cubeMapTexture;
texture sphereMapTexture;

//--
struct VS_INPUT
{
	float3 pos		: POSITION;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float3 cubeTex	: TEXCOORD0;
};

VS_OUTPUT my_vs(VS_INPUT vert)
{
	VS_OUTPUT vsout;
	
	vsout.pos = mul(float4(vert.pos,1),matWorldViewPrj);
	
	float3 worldPos = mul(float4(vert.pos,1),matWorld);
	float3 worldNormal = normalize(mul(vert.normal,matWorld));
		
	float3 viewDir = normalize(worldPos - eyePos);
	
	vsout.cubeTex = reflect(viewDir, worldNormal);
	
	return vsout;
}

//--
samplerCUBE cubeMap = sampler_state
{
	Texture = <cubeMapTexture>;
	MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

float4 my_ps(float3 cubeTex	: TEXCOORD0
			) : COLOR
{
	return texCUBE(cubeMap,cubeTex);
}

//--
technique myCubeEnvMap
{
	pass p0
	{
		VertexShader = compile vs_1_1 my_vs();
		PixelShader = compile ps_2_0 my_ps();
	}
}

//--

struct VS_OUTPUT_Sphere
{
	float4 pos : POSITION;
	float2 sphereTex	: TEXCOORD0;
};

VS_OUTPUT_Sphere my_sphere_vs(VS_INPUT vert)
{
	VS_OUTPUT_Sphere vsout;
	
	vsout.pos = mul(float4(vert.pos,1),matWorldViewPrj);
	
	float3 viewNormal = mul(vert.normal,matWorld);
	viewNormal = mul(viewNormal,matView);
	viewNormal = normalize(viewNormal);
		
	vsout.sphereTex = float2(viewNormal.x/2+0.5,viewNormal.y/2+0.5);
	
	return vsout;
}

sampler sphereMap = sampler_state
{
	Texture = <sphereMapTexture>;
	MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

float4 my_sphere_ps(float2 sphereTex	: TEXCOORD0
			) : COLOR
{
	return tex2D(sphereMap,sphereTex);
}


technique mySphereEnvMap
{
	pass p0
	{
		VertexShader = compile vs_1_1 my_sphere_vs();
		PixelShader = compile ps_2_0 my_sphere_ps();
	}
}