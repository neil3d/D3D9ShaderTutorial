
float4x4 matWorldViewPrj;
float4x4 matWorld;
float3 lightPos = float3(0,60,-60);
float lightAttenuation = 0.01;

float3 eyePos = float3(0,200,400);

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

//--
float4 my_ps(float3 worldPos : TEXCOORD0,
			 float3 normal : TEXCOORD1) : COLOR
{
	float4 color;
	
	//-- ambient
	color = mtlDiffuseColor*0.2f;
	
	//-- diffuse
	normal = normalize(normal);
	
	float3 L = lightPos - worldPos;
	float d = length(L);
	L /= d; // normalize L
	
	float atten = 1/(lightAttenuation*d);
	float diff = saturate(dot(normal, L));
	
	color += mtlDiffuseColor * diff * atten;
	
	//-- specular
	float3 V = normalize(worldPos - eyePos);
	float3 R = reflect(L,normal);
	
	float specular = pow(saturate(dot(R,V)),8)*diff*atten;
	color += float4(specular,specular,specular,0);
	
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
