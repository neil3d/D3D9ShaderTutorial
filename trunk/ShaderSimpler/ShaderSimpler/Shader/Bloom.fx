
texture texSource;
texture texScene;

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float2 uv  : TEXCOORD0;
};

VS_OUTPUT quad_vs(VS_INPUT vert)
{
	VS_OUTPUT vsout = (VS_OUTPUT)0;
	
	vsout.pos = vert.pos;
	vsout.uv = vert.uv;
	
	return vsout;
}

sampler sourceSampler = sampler_state
{	
	Texture = <texSource>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

// down sample
//---------------------------------------------------
const float HighlightThreshold = 0.5;

float luminance(float3 c)
{
	return dot( c, float3(0.3, 0.59, 0.11) );
}

// this function should be baked into a texture lookup for performance
float highlights(float3 c)
{
	return smoothstep(HighlightThreshold, 1.0, luminance(c.rgb));
}

float4 ps_downsample(float2 uv : TEXCOORD0) : COLOR
{
	float4 color = tex2D(sourceSampler, uv);
	
	// store hilights in alpha
	color.a = highlights(color);
	
	return color;
}

// blur
//----------------------------------------------------------------
// blur filter weights
const float weights7[7] = {0.05, 0.1, 0.2, 0.3, 0.2, 0.1, 0.05};	

float4 ps_hblur(float2 uv : TEXCOORD0) : COLOR
{
	float texelSize = 1.0f/512.0f;	//1除以贴图大小

	float4 color = 0;
	for(int i=0; i<7; i++)
	{
		// 把uv的生成放到vs会节省很多运算
		float2 uvi = uv + float2(texelSize*(i-3),0);
		color += tex2D(sourceSampler, uvi) * weights7[i];
	}
	
	return color;
}

float4 ps_vblur(float2 uv : TEXCOORD0) : COLOR
{
	float texelSize = 1.0f/384.0f;	//1除以贴图大小

	float4 color = 0;
	for(int i=0; i<7; i++)
	{
		// 把uv的生成放到vs会节省很多运算
		float2 uvi = uv + float2(0, texelSize*(i-3));
		color += tex2D(sourceSampler, uvi) * weights7[i];
	}
	
	return color;
}

// compose
//------------------------------------------------------------------
sampler sceneSampler = sampler_state
{	
	Texture = <texScene>;
    MinFilter = None;
    MagFilter = None;
    AddressU = CLAMP;
    AddressV = CLAMP;
};

float4 ps_compose(float2 uv : TEXCOORD0) : COLOR
{
	float4 blurColor = tex2D(sourceSampler, uv);
	float4 sceneColor = tex2D(sceneSampler, uv);
	
	return sceneColor*0.8 + blurColor*0.2 + blurColor*blurColor.a;
}


// technique
//------------------------------------------------------------------
technique PP_Bloom
{
	pass DownSample
	{
		CullMode = none;
		ZEnable = false;
		ZWriteEnable = false;
		
		VertexShader = compile vs_1_1 quad_vs();
		PixelShader = compile ps_2_0 ps_downsample();
	}
	
	pass HBlur
	{
		VertexShader = compile vs_1_1 quad_vs();
		PixelShader = compile ps_2_0 ps_hblur();
	}
	
	pass VBlur
	{
		VertexShader = compile vs_1_1 quad_vs();
		PixelShader = compile ps_2_0 ps_vblur();
	}
	
	pass Compose
	{
		VertexShader = compile vs_1_1 quad_vs();
		PixelShader = compile ps_2_0 ps_compose();
	}
}