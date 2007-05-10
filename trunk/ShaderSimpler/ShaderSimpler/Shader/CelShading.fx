
float4x4 matWorldViewPrj;
float4x4 matWorld;
float3 lightPos = float3(0,60,-60);
float lightAttenuation = 0.01;

float3 eyePos = float3(0,200,400);

float4 mtlDiffuseColor = float4(1,1,1,1);

texture texCartoon;
texture texCartoonEdge;

//--
struct VS_INPUT
{
	float3 pos : POSITION;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float3 worldPos : TEXCOORD0;
	float3 normal : TEXCOORD1;
};

VS_OUTPUT my_vs(VS_INPUT vert)
{
	VS_OUTPUT vsout;
	
	vsout.pos = mul(float4(vert.pos,1),matWorldViewPrj);
	vsout.worldPos = mul(float4(vert.pos,1),matWorld);
	vsout.normal = normalize(mul(vert.normal, matWorld));
	
	return vsout;
}

sampler CartoonSampler = sampler_state
{
	Texture = <texCartoon>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU  = Clamp;
	AddressV  = Clamp;
};

sampler CartoonEdgeSampler = sampler_state
{
	Texture = <texCartoonEdge>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = None;
	AddressU  = Clamp;
	AddressV  = Clamp;
};

//--
float4 my_ps(float3 worldPos : TEXCOORD0,
			 float3 normal : TEXCOORD1) : COLOR
{
	float4 color;
	
	//-- ambient
	float lum = 0.2f;
	
	//-- diffuse
	normal = normalize(normal);
	
	float3 L = lightPos - worldPos;
	float d = length(L);
	L /= d; // normalize L
	
	float atten = 1/(lightAttenuation*d);
	float diff = saturate(dot(normal, L));
	
	lum += diff * atten;
	
	float4 cartoonColorDiff = tex2D(CartoonSampler,lum);
	
	color = cartoonColorDiff*mtlDiffuseColor;
		
	//-- edge
	float3 V = normalize(eyePos - worldPos);
	float e = (dot(normal,V));
	float4 edgeColor = tex2D(CartoonEdgeSampler,e);
	
	color *= edgeColor;
	
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
