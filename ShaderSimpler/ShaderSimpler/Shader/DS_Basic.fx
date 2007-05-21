
float4x4 matWorldViewPrj;
float4x4 matWorld;
float4 mtlDiffuseColor = float4(1,0,0,1);

struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float4 worldPos : TEXCOORD0;
	float4 normal : TEXCOORD1;
};

VS_OUTPUT init_vs(VS_INPUT vert)
{
	VS_OUTPUT vsout;
	
	vsout.pos = mul(float4(vert.pos,1),matWorldViewPrj);
	vsout.worldPos = mul(float4(vert.pos,1),matWorld);
	vsout.normal = mul(vert.normal, matWorld);	
	
	return vsout;
}

float4 init_pos_ps(float4 worldPos : TEXCOORD0) : COLOR
{
	return worldPos.xyzw;
}

float4 init_diffuse_ps() : COLOR
{
	return mtlDiffuseColor;
}

float4 init_normal_ps(float4 normal : TEXCOORD1) : COLOR
{
	return 0.5f*(normal+1);
}

technique init_pos
{
	pass p0
	{
		VertexShader = compile vs_1_1 init_vs();
		PixelShader = compile ps_2_0 init_pos_ps();
	}
}

technique init_normal
{
	pass p0
	{
		VertexShader = compile vs_1_1 init_vs();
		PixelShader = compile ps_2_0 init_normal_ps();
	}
}

technique init_diffuse
{
	pass p0
	{
		VertexShader = compile vs_1_1 init_vs();
		PixelShader = compile ps_2_0 init_diffuse_ps();
	}
}

//----------------------------------------------------------------------------------------
struct PS_OUTPUT_MRT
{
	float4 posXY : COLOR0;
	float4 posZ : COLOR1;
	float4 normal : COLOR2;
	float4 diffuse : COLOR3;
};

PS_OUTPUT_MRT init_mrt_ps(float4 worldPos : TEXCOORD0,
						  float4 normal : TEXCOORD1)
{
	PS_OUTPUT_MRT ret;
	
	ret.posXY.rgba = worldPos.xyzw;
	ret.posZ.rgba = worldPos.zxyw;
	
	ret.normal = 0.5f*(normal+1);
	ret.diffuse = mtlDiffuseColor;
	
	return ret;
}

technique init_mrt
{
	pass p0
	{
		VertexShader = compile vs_1_1 init_vs();
		PixelShader = compile ps_2_0 init_mrt_ps();
	}
}

//----------------------------------------------------------------------------------------
float3 lightPos;
float3 eyePos = float3(0,200,400);

texture posRT;
texture normalRT;
texture diffuseRT;

struct VS_INPUT_S
{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};

struct VS_OUTPUT_S
{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};

VS_OUTPUT_S quad_vs(VS_INPUT_S vert)
{
	VS_OUTPUT_S vsout = (VS_OUTPUT_S)0;
	
	vsout.pos = vert.pos;
	vsout.uv = vert.uv;
	
	return vsout;
}

sampler PosSampler =  sampler_state
{
	texture = <posRT>;
};

sampler NormalSampler =  sampler_state
{
	texture = <normalRT>;
};

sampler DiffuseSampler =  sampler_state
{
	texture = <diffuseRT>;
};

float4 basic_light(float3 pos, float3 normal, float4 diffuse)
{
	float3 lightDir = lightPos - pos;
	float3 L = normalize(lightDir);
	
	float d = saturate(dot(normal, L));
	
	d += 0.05f; //ambient
	
	float4 color = diffuse*d;
	
	//--
	float3 V = normalize(pos - eyePos);
	float3 R = reflect(L, normal);
	
	float specular = pow(saturate(dot(R,V)),8)*d;
	color += float4(specular,specular,specular,0) * length(diffuse);
	
	return color;
}

float4 shading_ps(float2 uv  : TEXCOORD0) : COLOR0
{
	float3 pos = tex2D(PosSampler, uv);
	
	float3 normal = tex2D(NormalSampler, uv);
	normal = normalize(2*normal-1);
	
	float4 diffuse = tex2D(DiffuseSampler, uv);
	
	return basic_light(pos, normal, diffuse);
}

technique shading_fullscreen
{
	pass p0
	{
		ZWriteEnable = false;
		ZFunc = Always;
		
		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;
		
		VertexShader = compile vs_1_1 quad_vs();
		PixelShader = compile ps_2_0 shading_ps();
	}
}

//---------------------------------------------------------
texture posRT_Z;

sampler PosZSampler =  sampler_state
{
	texture = <posRT_Z>;
};

float4 shading_ps_mrt(float2 uv  : TEXCOORD0) : COLOR0
{
	float3 pos;
	pos.xy = tex2D(PosSampler, uv).rg;
	pos.z = tex2D(PosZSampler, uv).r;
	
	float3 normal = tex2D(NormalSampler, uv);
	normal = normalize(2*normal-1);
	
	float4 diffuse = tex2D(DiffuseSampler, uv);
	
	return basic_light(pos, normal, diffuse);
}

technique shading_fullscreen_mrt
{
	pass p0
	{
		ZWriteEnable = false;
		ZFunc = Always;
		
		AlphaBlendEnable = true;
		SrcBlend = one;
		DestBlend = one;
		
		VertexShader = compile vs_1_1 quad_vs();
		PixelShader = compile ps_2_0 shading_ps_mrt();
	}
}