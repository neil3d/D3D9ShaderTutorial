
float4x4 matWorldViewPrj;
float4x4 matWorld;

float lightAttenuation = 0.005;
float3 lightPos = float3(0,60,-60);

float3 eyePos = float3(0,200,400);

float4 mtlDiffuseColor = float4(1,1,1,1);

texture texColorMap;
texture texNormalMap;
texture texHeightMap;

struct VS_INPUT
{
	float3 pos : POSITION;
	float2 uv : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float2 uv : TEXCOORD0;
	float3 worldPos : TEXCOORD1;
	float3 lightDirInTexSpace : TEXCOORD2;
	float3 viewDirInTexSpace : TEXCOORD3;
};

VS_OUTPUT my_vs(VS_INPUT vert)
{
	VS_OUTPUT vsout = (VS_OUTPUT)0;
	
	vsout.pos = mul(float4(vert.pos,1),matWorldViewPrj);
	vsout.uv = vert.uv;
	vsout.worldPos = mul(float4(vert.pos,1),matWorld);
	
	float3 lightDir = lightPos - vsout.worldPos;
	float3 viewDir = eyePos - vsout.worldPos;
	
	// compute the 3x3 tranform matrix 
    // to transform from world space to tangent space
    float3x3 worldToTangentSpace;
    worldToTangentSpace[0] = mul(vert.tangent, matWorld);
    worldToTangentSpace[1] = mul(cross(vert.normal, vert.tangent), matWorld);
    worldToTangentSpace[2] = mul(vert.normal, matWorld);
    
    vsout.lightDirInTexSpace = mul(worldToTangentSpace,lightDir);
    vsout.viewDirInTexSpace = mul(worldToTangentSpace,viewDir);
    
	return vsout;
}

sampler ColorMapSampler = sampler_state
{
	Texture = <texColorMap>;
	MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler NormalMapSampler = sampler_state
{
	Texture = <texNormalMap>;
	MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler HeightMapSampler = sampler_state
{
	Texture = <texHeightMap>;
	MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

float4 my_ps(float2 uv : TEXCOORD0,
			float3 worldPos : TEXCOORD1,
			float3 lightDirInTexSpace : TEXCOORD2,
			float3 viewDirInTexSpace : TEXCOORD3,
			uniform bool bUseParallax ) : COLOR
{
	float3 lightDir = normalize(lightDirInTexSpace);
	float3 viewDir = normalize(viewDirInTexSpace);
	
	float2 offsetUV = uv;
	
	if(bUseParallax)
	{
		float h = tex2D(HeightMapSampler,uv).x;
		offsetUV += float2(viewDir.x,viewDir.y)*(h*0.04-0.02);
	}
	
	float4 texColor = tex2D(ColorMapSampler,offsetUV);
	
	//--
	float3 normal = normalize((tex2D(NormalMapSampler,offsetUV) - 0.5)*2);
	//--
	float4 lumColor = float4(0.1,0.1,0.1,1);//ambient
	
	float atten = 1/(lightAttenuation*distance(worldPos,lightPos));
	float diff = saturate(dot(normal, lightDir));
	
	lumColor += mtlDiffuseColor * diff * atten;
	
	float3 R = reflect(lightDir,normal);
	
	float specular = pow(saturate(dot(R,viewDir)),8)*diff*atten;
	lumColor += float4(specular,specular,specular,0);
				
	return lumColor*texColor;
}

technique tech_parallax
{
	pass p0
	{
		cullMode = none;
		VertexShader = compile vs_1_1 my_vs();
		PixelShader = compile ps_2_0 my_ps(true);
	}
}

technique tech_no_parallax
{
	pass p0
	{
		cullMode = none;
		VertexShader = compile vs_1_1 my_vs();
		PixelShader = compile ps_2_0 my_ps(false);
	}
}