
/*** Generated through Lumonix shaderFX  by: alexandre.chekroun in 3dsmax at: 2009-08-12 17:59:29  ***/ 

// This FX shader was built to support 3ds Max's standard shader compiler. 


int texcoord0 : Texcoord 
<
	int Texcoord = 0;
	int MapChannel = 1;
	string UIType = "None"; 
>;

float UIConst_5265
<
	string UIType = "FloatSpinner";
	float UIMin = -999.0;
	float UIMax = 999.0;
	float UIStep = 0.01;
	string UIName = "Wave size";
> = 3.0;
 
float Time : TIME < string UIType = "None"; >;
 
float UIConst_4273
<
	string UIType = "FloatSpinner";
	float UIMin = -999.0;
	float UIMax = 999.0;
	float UIStep = 0.001;
	string UIName = "UV Offset";
> = 0.02;
 
float UIConst_9692
<
	string UIType = "FloatSpinner";
	float UIMin = -999.0;
	float UIMax = 999.0;
	float UIStep = 0.01;
	string UIName = "UV Rotation";
> = 0.0;
 
//function to rotate UVs 
float2 rotateUVs(float2 Texcoords, float2 center, float theta) { 
 
	// compute sin and cos for this angle 
	float2 sc; 
	sincos( theta, sc.x, sc.y ); 
	 
	// move the rotation center to the origin 
	float2 uv = Texcoords - center; 
	 
	// rotate the uv 
	float2 rotateduv; 
	rotateduv.x = dot( uv, float2( sc.y, -sc.x ) ); 
	rotateduv.y = dot( uv, sc.xy );	 
	 
	// move the uv's back to the correct place 
	rotateduv += center; 
	 
	return rotateduv; 
} 
 
texture NormalMap_1685 : NormalMap
<
	string Name = "water_bump_map.tga";
	string UIName = "Normal Map";
	string ResourceType = "2D";
>;
 
sampler2D NormalMap_1685Sampler = sampler_state
{
	Texture = <NormalMap_1685>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
float UIConst_3468
<
	string UIType = "FloatSpinner";
	float UIMin = -999.0;
	float UIMax = 999.0;
	float UIStep = 0.01;
	string UIName = "Wave size";
> = 1.0;
 
float UIConst_2046
<
	string UIType = "FloatSpinner";
	float UIMin = -999.0;
	float UIMax = 999.0;
	float UIStep = 0.001;
	string UIName = "UV Offset";
> = -0.004;
 
float UIConst_9165
<
	string UIType = "FloatSpinner";
	float UIMin = -999.0;
	float UIMax = 999.0;
	float UIStep = 0.01;
	string UIName = "UV Rotation";
> = 0.3;
 
texture NormalMap_8499 : NormalMap
<
	string Name = "water_bump_map.tga";
	string UIName = "Normal Map";
	string ResourceType = "2D";
>;
 
sampler2D NormalMap_8499Sampler = sampler_state
{
	Texture = <NormalMap_8499>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
texture ReflectCube : environment
<
	string Name = "sunol_cubemap.dds";
	string UIName = "Reflect Cube";
	string type = "Cube";
>;
 
samplerCUBE ReflectCubeSampler = sampler_state
{
	Texture = <ReflectCube>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};
 
float IndexofRefraction
<
	string UIType = "FloatSpinner";
	float UIMin = 0.0;
	float UIMax = 10.0;
	float UIStep = 0.1;
	string UIName = "IndexofRefraction";
> = 1.33;
 
texture RefractCube : environment
<
	string Name = "sunol_cubemap.dds";
	string UIName = "Refract Cube";
	string type = "Cube";
>;
 
samplerCUBE RefractCubeSampler = sampler_state
{
	Texture = <RefractCube>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = CLAMP;
	AddressV = CLAMP;
};
 
float FresnelPower
<
	string UIType = "FloatSpinner";
	float UIMin = 0.0;
	float UIMax = 100.0;
	float UIStep = 0.01;
	string UIName = "Fresnel Power";
> = 1.0;
 
float FresnelScale
<
	string UIType = "FloatSpinner";
	float UIMin = 0.0;
	float UIMax = 100.0;
	float UIStep = 0.01;
	string UIName = "Fresnel Scale";
> = 1.0;
 
float FresnelBias
<
	string UIType = "FloatSpinner";
	float UIMin = -100.0;
	float UIMax = 100.0;
	float UIStep = 0.01;
	string UIName = "Fresnel Bias";
> = 0.0;
 
//Vertex Color and Alpha in from Max 
int texcoord1 : Texcoord 
<
	int Texcoord = 1;
	int MapChannel = 2;
	string UIType = "None"; 
>;

int texcoord2 : Texcoord 
<
	int Texcoord = 2;
	int MapChannel = 3;
	string UIType = "None"; 
>;

int texcoord3 : Texcoord 
<
	int Texcoord = 3;
	int MapChannel = 0;
	string UIType = "None"; 
>;

int texcoord4 : Texcoord 
<
	int Texcoord = 4;
	int MapChannel = -2;
	string UIType = "None"; 
>;


// this function does the different types of light attenuation 
float attenuation_func(int lightattenType, float4 lightAttenuation, float3 lightVec) 
{ 
	float att = 1.0; 
	return att; 
} 
	 
// this function does the different types of cone angle 
float coneangle_func(int lightconeType, float lightHotspot, float lightFalloff, float3 lightVec, float3 lightDir) 
{ 
	float cone = 1.0; 
	return cone; 
} 

/************** light info **************/ 

float3 light1Dir : Direction 
< 
	string UIName = "Light 1 Direction"; 
	string Object = "TargetLight"; 
	string Space = "World"; 
		int refID = 1; 
> = {100.0f, 100.0f, 100.0f}; 

float3 light1Pos : POSITION 
< 
	string UIName = "Light 1 Position"; 
	string Object = "PointLight"; 
	string Space = "World"; 
		int refID = 1; 
> = {100.0f, 100.0f, 100.0f}; 

float4 light1Color : LIGHTCOLOR <int LightRef = 1; string UIWidget = "None"; > = { 1.0f, 1.0f, 1.0f, 1.0f}; 
float4 light1Attenuation : LightAttenuation <int LightRef = 1; string UIWidget = "None"; > = { 20.0f, 30.0f, 0.0f, 100.0f}; 
float light1Hotspot : LightHotSpot <int LightRef = 1; string UIWidget = "None"; > = { 43.0f }; 
float light1Falloff : LightFallOff <int LightRef = 1; string UIWidget = "None"; > = { 45.0f }; 

#define light1Type 1
#define light1attenType 0
#define light1coneType 0
#define light1CastShadows false

//---------------------------------- 

float4x4 wvp : WorldViewProjection < string UIType = "None"; >;  
float4x4 worldI : WorldInverse < string UIType = "None"; >;  
float4x4 worldIT : WorldInverseTranspose < string UIType = "None"; >;  
float4x4 viewInv : ViewInverse < string UIType = "None"; >;  
float4x4 world : World < string UIType = "None"; >;  
// create the light vector 
float3 lightVec_func(float3 worldSpacePos, float3 lightVector, float3x3 objTangentXf, int lightType) 
{ 
	float3 lightVec = mul(objTangentXf, (mul((lightVector - worldSpacePos), worldI).xyz)); 
	return lightVec; 
} 


// input from application 
	struct a2v { 
	float4 position		: POSITION; 

	float2 texCoord		: TEXCOORD0; 
	float4 tangent		: TANGENT; 
	float4 binormal		: BINORMAL; 
	float4 normal		: NORMAL; 
	float3 vertcol		: TEXCOORD3; 
	float vertalpha 	: TEXCOORD4; 

}; 

// output to fragment program 
struct v2f { 
        float4 position    		: POSITION; 

	float2 texCoord			: TEXCOORD0; 
        float3 worldTangent  	: TEXCOORD1; 
        float3 worldBinormal 	: TEXCOORD2; 
        float3 worldNormal   	: TEXCOORD3; 
        float3 eyeVec	    	: TEXCOORD4; 
        float4 color    		: COLOR0; 

}; 

//Ambient and Self-Illum Pass Vertex Shader
v2f av(a2v In, uniform float3 lightPos, uniform int lightType, uniform float3 lightDir) 
{ 
    v2f Out = (v2f)0; 
	Out.position = mul(In.position, wvp);				//transform vert position to homogeneous clip space 

	In.texCoord += float2(0.0,1.0);		//this fixes Max's V texcoord which is off by one 
	Out.texCoord = In.texCoord;						//pass through texture coordinates from channel 1 
	//this code was added by the World Space Transform Node 
	Out.worldNormal = mul(In.normal, worldIT).xyz;		//compute world space normal 
	Out.worldBinormal = mul(In.binormal, worldIT).xyz;	//compute world space binormal 
	Out.worldTangent = mul(In.tangent, worldIT).xyz;		//compute world space tangent 
	//this code was added by the Eye Vector Node 
	float3x3 objTangentXf;								//build object to tangent space transform matrix 
	#ifdef YUP 
	objTangentXf[0] = In.tangent.xyz; 
	objTangentXf[1] = -In.binormal.xyz; 
	#else 
	objTangentXf[0] = In.binormal.xyz; 
	objTangentXf[1] = -In.tangent.xyz; 
	#endif 
	objTangentXf[2] = In.normal.xyz; 
	//these three lines were added by the Eye Vector Node 
	float4 osIPos = mul(viewInv[3], worldI);			//put world space eye position in object space 
	float3 osIVec = osIPos.xyz - In.position.xyz;		//object space eye vector 
	Out.eyeVec = mul(objTangentXf, osIVec);				//tangent space eye vector passed out 
	//this code was added by the World Space Transform Node 
	//this code was added by the Vertex Color Node 
	float4 VertColor = float4(In.vertcol.rgb, In.vertalpha.r); 
	Out.color = VertColor; 


	return Out; 
} 

//Ambient and Self-Illum Pass Pixel Shader
float4 af(v2f In, uniform float3 lightDir, uniform float4 lightColor, uniform float4 lightAttenuation, uniform float lightHotspot, uniform float lightFalloff, uniform int lightType, uniform int lightattenType, uniform int lightconeType, uniform bool lightCastShadows, uniform int shadowPassCount) : COLOR 
{ 

	float2 INUV_9611 = In.texCoord.xy; 
	float2 UVRotator_3084_UVs = ((INUV_9611 * UIConst_5265) + (Time * UIConst_4273)).xy; 
	float2 UVRotator_3084_center = float2(0.0, 0.0);	//no center values wired to UV Rotate Node - using defaults 
	float UVRotator_3084_rotation = UIConst_9692; 
	float2 UVRotator_3084 = rotateUVs(UVRotator_3084_UVs, UVRotator_3084_center, UVRotator_3084_rotation); 
	float4 NormalMap_1685 = tex2D(NormalMap_1685Sampler, UVRotator_3084.xy);
	NormalMap_1685.xyz = NormalMap_1685.xyz * 2 - 1;		//expand to -1 to 1 range 
	float2 INUV_8301 = In.texCoord.xy; 
	float2 UVRotator_5997_UVs = ((INUV_8301 * UIConst_3468) + (Time * UIConst_2046)).xy; 
	float2 UVRotator_5997_center = float2(0.0, 0.0);	//no center values wired to UV Rotate Node - using defaults 
	float UVRotator_5997_rotation = UIConst_9165; 
	float2 UVRotator_5997 = rotateUVs(UVRotator_5997_UVs, UVRotator_5997_center, UVRotator_5997_rotation); 
	float4 NormalMap_8499 = tex2D(NormalMap_8499Sampler, UVRotator_5997.xy);
	NormalMap_8499.xyz = NormalMap_8499.xyz * 2 - 1;		//expand to -1 to 1 range 
	float3 MathNormalize_2356 = normalize((NormalMap_1685.rgb + NormalMap_8499.rgb));		//Normalize 
	//this code was added by the World Space Transform Node 
	float3 Nn = normalize(In.worldNormal); 		//input the vectors required for tangent to world space transform 
	float3 Tn = normalize(In.worldTangent); 
	float3 Bn = normalize(In.worldBinormal); 
	#ifdef YUP 
		float3 WorldXformNormal = ( Nn * MathNormalize_2356.z ) + (MathNormalize_2356.x * Tn ) + ( MathNormalize_2356.y * -Bn); 
	#else 
		float3 WorldXformNormal = ( Nn * MathNormalize_2356.z ) + (MathNormalize_2356.x * Bn ) + ( MathNormalize_2356.y * -Tn); 
	#endif 
	float3 viewDir = mul(viewInv[3], world) ;	// Adjust for world space position of object 
	float3 Reflect = reflect(viewDir, WorldXformNormal);		//Compute the reflection vector 
	float3 ReflectCubeInputVector = Reflect.xyz;
	#ifdef YUP 
	#else 
	ReflectCubeInputVector.xyz = ReflectCubeInputVector.xzy;  //swizzle required for Max 
	#endif 
	float4 ReflectCube = texCUBE(ReflectCubeSampler, ReflectCubeInputVector);
	float3 EyeVector = In.eyeVec.xyz;		//unnormalized eye vector 
	#ifdef YUP 
		float3 WorldXformEye = ( Nn * EyeVector.z ) + (EyeVector.x * Tn ) + ( EyeVector.y * -Bn); 
	#else 
		float3 WorldXformEye = ( Nn * EyeVector.z ) + (EyeVector.x * Bn ) + ( EyeVector.y * -Tn); 
	#endif 
    float3 Refract = refract( WorldXformEye, -WorldXformNormal, IndexofRefraction );		//Compute the refraction vector 
    Refract.x = -Refract.x;		//negate 
    Refract.y = -Refract.y;		//negate 
	float3 RefractCubeInputVector = Refract.xyz;
	#ifdef YUP 
	#else 
	RefractCubeInputVector.xyz = RefractCubeInputVector.xzy;  //swizzle required for Max 
	#endif 
	float4 RefractCube = texCUBE(RefractCubeSampler, RefractCubeInputVector);
	float3 V = normalize(In.eyeVec.xyz);		//normalized eye vector 
	float NdotV_fresnel = saturate(dot(V,WorldXformNormal)); 		//clamped dot product 
	float Pow_NdotV = pow(NdotV_fresnel,FresnelPower);		//x to the power of y 
	float ClampFresnel = saturate(((Pow_NdotV * FresnelScale) + FresnelBias)); 
	float3 LerpReflectionAndRefraction = lerp(ReflectCube.rgb, RefractCube.rgb, ClampFresnel); 
	float3 input1 = LerpReflectionAndRefraction; 
	float4 INVertColor_9974 = In.color; 		//bringing in the vertex color 
	float input7 = INVertColor_9974.a; 

	float4 ret =  float4(0,0,0,1); 
	ret = float4(input1, 1); 
	ret.a = input7; 
	return ret; 
} 

technique Complete  
{  
	pass ambient  
    {		 
		VertexShader = compile vs_3_0 av(light1Pos,  light1Type, light1Dir); 
		ZEnable = true; 
		ZWriteEnable = true; 
		CullMode = cw; 
		ShadeMode = Gouraud;
		AlphaBlendEnable = true; 
		SrcBlend = SrcAlpha; 
		DestBlend = InvSrcAlpha; 
		AlphaTestEnable = FALSE; 
	PixelShader = compile ps_3_0 af(light1Dir, light1Color, light1Attenuation, light1Hotspot, light1Falloff, light1Type, light1attenType, light1coneType, light1CastShadows, 1); 
	}  

}    