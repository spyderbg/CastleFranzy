const char* g_basicshader="										\n\
struct SHADER_MATRIX											\n\
{																\n\
	float4x4 wvp;												\n\
};																\n\
float4x4 tex0mat;												\n\
struct SHADER_MATERIAL											\n\
{																\n\
	float4 diffuse;												\n\
	float4 ambient;												\n\
	float4 specular;											\n\
	float4 emissive;											\n\
	float4 specularPower;										\n\
};																\n\
///////////////////////////////////////////						\n\
// vertex shader												\n\
uniform SHADER_MATRIX mat : register(c0);						\n\
																\n\
struct VS_IN{													\n\
	float3 pos : POSITION;										\n\
	float4 diffuse : COLOR;										\n\
	float2 texCoord : TEXCOORD;									\n\
};																\n\
																\n\
struct VS_OUT{													\n\
	float4 pos : POSITION;										\n\
	float4 diffuse : COLOR;										\n\
	float2 texCoord : TEXCOORD;									\n\
};																\n\
																\n\
VS_OUT vs_main(VS_IN vertex)									\n\
{																\n\
	VS_OUT r;													\n\
																\n\
	r.pos = mul(float4(vertex.pos,1.0f),mat.wvp);				\n\
	r.diffuse=vertex.diffuse;									\n\
	r.texCoord=mul(float4(vertex.texCoord, 0, 1.0f), tex0mat);	\n\
																\n\
	return r;													\n\
};																\n\
																\n\
/////////////////////////////////////////////////////			\n\
// pixel shader													\n\
uniform bool ps_bDiffuse = true;								\n\
uniform bool ps_bTexture = false;								\n\
uniform bool ps_bAlpha = false;									\n\
uniform float ps_alphaValue = 1.0;								\n\
uniform float  RGB_OP = 1.0;	//0:REPLACE 1:MODULE				\n\
uniform float  ALPHA_OP = 0;										\n\
																\n\
const sampler2D s : register(s0);								\n\
const sampler2D s1 : register(s1);								\n\
struct PS_IN{													\n\
	float4 diffuse : COLOR0;									\n\
	float2 texCoord : TEXCOORD0;								\n\
};																\n\
																\n\
float4 ps_main(PS_IN pixel) : COLOR								\n\
{																\n\
	float4 rd={ 1.0, 1.0, 1.0, 1.0 };							\n\
	float4 rt={ 1.0, 1.0, 1.0, 1.0 };							\n\
	float4 ca = 0;												\n\
																\n\
	if(ps_bDiffuse)												\n\
	{															\n\
		rd=pixel.diffuse;ca	= rd;								\n\
	}															\n\
	if(ps_bTexture)												\n\
    {															\n\
		rt=tex2D(s,pixel.texCoord);	ca	= rt;					\n\
    }															\n\
	int rgb_op = RGB_OP;										\n\
	int a_op = ALPHA_OP;										\n\
	switch(rgb_op)												\n\
	{															\n\
	case 1://Texture Replace									\n\
	{															\n\
		ca = rt;break;											\n\
	}															\n\
	case 2://Diffuse Replace									\n\
	{															\n\
		ca = rd;break;											\n\
	}															\n\
	case 3://color module										\n\
	{															\n\
		ca=rd*rt;break;											\n\
	}															\n\
	}															\n\
	switch(a_op)											\n\
	{															\n\
	case 1://Texture Replace									\n\
	{															\n\
		ca.a=rt.a;												\n\
		break;													\n\
	}															\n\
	case 2://Diffuse Replace									\n\
	{															\n\
		ca.a=rd.a;												\n\
		break;													\n\
	}															\n\
	case 3://color module										\n\
	{															\n\
		ca.a=rd.a*rt.a;break;									\n\
	}															\n\
	}															\n\
	if(ps_bAlpha)												\n\
    {															\n\
		ca.a = ps_alphaValue * ca.a;							\n\
    }															\n\
																\n\
	return ca;													\n\
};																\n\
";									