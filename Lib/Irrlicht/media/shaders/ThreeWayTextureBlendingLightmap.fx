
/*** Generated through Lumonix shaderFX  by: alexandre.chekroun in 3dsmax at: 2009-08-13 11:26:31  ***/ 

// This FX shader was built to support 3ds Max's standard shader compiler. 


float3 UIColor_7265
<
	string UIName = "Ambient Color";
	string UIType = "ColorSwatch";
> = {0.0901961f, 0.145098f, 0.192157f};
 
int texcoord0 : Texcoord 
<
	int Texcoord = 0;
	int MapChannel = 1;
	string UIType = "None"; 
>;

int texcoord1 : Texcoord 
<
	int Texcoord = 1;
	int MapChannel = 2;
	string UIType = "None"; 
>;

texture TextureMap_6553 : LightMap
<
	string Name = "terrainLightingMap.tga";
	string UIName = "Lightmap";
	string ResourceType = "2D";
>;
 
sampler2D TextureMap_6553Sampler = sampler_state
{
	Texture = <TextureMap_6553>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
//Vertex Color and Alpha in from Max 
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

texture TextureMap_374
<
	string Name = "grass_01_D.tga";
	string UIName = "TextureOne";
	string ResourceType = "2D";
>;
 
sampler2D TextureMap_374Sampler = sampler_state
{
	Texture = <TextureMap_374>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
texture TextureMap_8119
<
	string Name = "ground_01.tga";
	string UIName = "TextureTwo";
	string ResourceType = "2D";
>;
 
sampler2D TextureMap_8119Sampler = sampler_state
{
	Texture = <TextureMap_8119>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
texture TextureMap_7679
<
	string Name = "rock_01_COLOR.tga";
	string UIName = "TextureThree";
	string ResourceType = "2D";
>;
 
sampler2D TextureMap_7679Sampler = sampler_state
{
	Texture = <TextureMap_7679>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
float UIConst_1106
<
	string UIType = "FloatSpinner";
	float UIMin = 0.0;
	float UIMax = 100.0;
	float UIStep = 0.01;
	string UIName = "Spec Level";
> = 0.0;
 
float UIConst_4831
<
	string UIType = "FloatSpinner";
	float UIMin = 0.0;
	float UIMax = 100.0;
	float UIStep = 0.1;
	string UIName = "Glossiness";
> = 1.0;
 
texture NormalMap_3074 : NormalMap
<
	string Name = "Normal_neutral.tga";
	string UIName = "NormalMapOne";
	string ResourceType = "2D";
>;
 
sampler2D NormalMap_3074Sampler = sampler_state
{
	Texture = <NormalMap_3074>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
texture NormalMap_9019 : NormalMap
<
	string Name = "Normal_neutral.tga";
	string UIName = "NormalMapTwo";
	string ResourceType = "2D";
>;
 
sampler2D NormalMap_9019Sampler = sampler_state
{
	Texture = <NormalMap_9019>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
texture NormalMap_4156 : NormalMap
<
	string Name = "rock_01_NRM.tga";
	string UIName = "NormalMapThree";
	string ResourceType = "2D";
>;
 
sampler2D NormalMap_4156Sampler = sampler_state
{
	Texture = <NormalMap_4156>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 

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
	float4 tangent		: TANGENT; 
	float4 binormal		: BINORMAL; 
	float4 normal		: NORMAL; 

	float2 texCoord1	: TEXCOORD1; 
	float3 vertcol		: TEXCOORD3; 
	float vertalpha 	: TEXCOORD4; 
	float2 texCoord		: TEXCOORD0; 

}; 

// output to fragment program 
struct v2f { 
        float4 position    		: POSITION; 
        float3 light1Vec    	: TEXCOORD0; 
        float3 eyeVec	    	: TEXCOORD1; 

	float2 texCoord1		: TEXCOORD2; 
        float4 color    		: COLOR0; 
	float2 texCoord			: TEXCOORD3; 

}; 

//Vertex Shader
v2f v(a2v In, uniform float3 lightPos, uniform int lightType, uniform float3 lightDir) 
{ 
	v2f Out = (v2f)0; 
	float3x3 objTangentXf;								//build object to tangent space transform matrix 
	#ifdef YUP 
	objTangentXf[0] = In.tangent.xyz; 
	objTangentXf[1] = -In.binormal.xyz; 
	#else 
	objTangentXf[0] = In.binormal.xyz; 
	objTangentXf[1] = -In.tangent.xyz; 
	#endif 
	objTangentXf[2] = In.normal.xyz; 
	float3 worldSpacePos = mul(In.position, world).xyz;	//world space position 
	Out.light1Vec = lightVec_func(worldSpacePos, light1Pos, objTangentXf, light1Type); 
	float4 osIPos = mul(viewInv[3], worldI);			//put world space eye position in object space 
	float3 osIVec = osIPos.xyz - In.position.xyz;		//object space eye vector 
	Out.eyeVec = mul(objTangentXf, osIVec);				//tangent space eye vector passed out 
	Out.position = mul(In.position, wvp);				//transform vert position to homogeneous clip space 

	In.texCoord1 += float2(0.0,1.0);		//this fixes Max's V texcoord which is off by one 
	Out.texCoord1 = In.texCoord1;						//pass through texture coordinates from channel 2 
	//this code was added by the Vertex Color Node 
	float4 VertColor = float4(In.vertcol.rgb, In.vertalpha.r); 
	Out.color = VertColor; 

	In.texCoord += float2(0.0,1.0);		//this fixes Max's V texcoord which is off by one 
	Out.texCoord = In.texCoord;						//pass through texture coordinates from channel 1 

	return Out; 
} 

//Pixel Shader
float4 f(v2f In, uniform float3 lightDir, uniform float4 lightColor, uniform float4 lightAttenuation, uniform float lightHotspot, uniform float lightFalloff, uniform int lightType, uniform int lightattenType, uniform int lightconeType, uniform bool lightCastShadows) : COLOR 
{ 
	float3 V = normalize(In.eyeVec);		//creating the eye vector  

	float4 TextureMap_6553 = tex2D(TextureMap_6553Sampler, In.texCoord1.xy);
	float4 VertColor = In.color; 		//bringing in the vertex color 
	float3 BreakColorIntoRGB = (VertColor.rgb / ((VertColor.r + VertColor.g) + VertColor.b));
	float4 TextureMap_374 = tex2D(TextureMap_374Sampler, In.texCoord.xy);
	float4 TextureMap_8119 = tex2D(TextureMap_8119Sampler, In.texCoord.xy);
	float4 TextureMap_7679 = tex2D(TextureMap_7679Sampler, In.texCoord.xy);
	float3 MathOperator_7745 = TextureMap_6553.rgb * (((BreakColorIntoRGB.r * TextureMap_374.rgb) + (BreakColorIntoRGB.g * TextureMap_8119.rgb)) + (BreakColorIntoRGB.b * TextureMap_7679.rgb));
	float3 input1 = (UIColor_7265.rgb * MathOperator_7745); 


	float3 input2 = MathOperator_7745; 


	float input4 = UIConst_1106; 


	float input5 = UIConst_4831; 


	float4 NormalMap_3074 = tex2D(NormalMap_3074Sampler, In.texCoord.xy);
	NormalMap_3074.xyz = NormalMap_3074.xyz * 2 - 1;		//expand to -1 to 1 range 
	NormalMap_3074.rgb = normalize(NormalMap_3074.rgb); 		//normalized the normal vector 
	float4 NormalMap_9019 = tex2D(NormalMap_9019Sampler, In.texCoord.xy);
	NormalMap_9019.xyz = NormalMap_9019.xyz * 2 - 1;		//expand to -1 to 1 range 
	NormalMap_9019.rgb = normalize(NormalMap_9019.rgb); 		//normalized the normal vector 
	float4 NormalMap_4156 = tex2D(NormalMap_4156Sampler, In.texCoord.xy);
	NormalMap_4156.xyz = NormalMap_4156.xyz * 2 - 1;		//expand to -1 to 1 range 
	NormalMap_4156.rgb = normalize(NormalMap_4156.rgb); 		//normalized the normal vector 
	float3 input8 = (((BreakColorIntoRGB.r * NormalMap_3074.rgb) + (BreakColorIntoRGB.g * NormalMap_9019.rgb)) + (NormalMap_4156.rgb * BreakColorIntoRGB.b)); 

	float3 ret =  float3(0,0,0); 
	float3 AmbIllum = input1; 
	ret = AmbIllum; 
	float3 diffuseColor = input2;  
	float3 N = input8;						//using the Normal socket  
	float3 spec = float3(0,0,0); 
	float specularColor = input4; 		//the Specular Color socket was empty - using Specular Level instead
	float glossiness = input5;				//using the Glossiness socket  

	float3 L1 = normalize(In.light1Vec.xyz); 
	float3 light1 = float3(0.0, 0.0, 0.0); 
	float3 NdotL1 = dot(N, L1) ;  
	float3 diffuse1 = saturate(NdotL1);  
	float3 H1 = normalize(L1 + V); 
	float NdotH1 = saturate(dot(N,H1));		//Compute NdotH1  
	float3 specularColor1 =  pow(NdotH1, glossiness)  * specularColor;//Raise to glossiness power and compute final specular color  
	light1 += specularColor1 + (diffuseColor * diffuse1);			//add specular and diffuse color together
	light1 *= light1Color.xyz; 
	float3 light1Dir = light1Dir;	//use the light direction from the light header information  
	float attenuation1 = attenuation_func(light1attenType, light1Attenuation, In.light1Vec); 					//calculate the light attenuation  
	float coneangle1 = coneangle_func(light1coneType, light1Hotspot, light1Falloff, In.light1Vec, light1Dir); 	//calculate the light's cone angle 
	light1 *= attenuation1 * coneangle1;			//multiply by the light decay  
	ret +=  light1; 

	float4 done = float4(ret, 1); 
	return done; 
} 

technique Complete  
{  
	pass one  
    {		 
		VertexShader = compile vs_3_0 v(light1Pos,  light1Type, light1Dir); 
		ZEnable = true; 
		ZWriteEnable = true; 
		CullMode = cw; 
		ShadeMode = Gouraud;
		AlphaBlendEnable = false; 
		AlphaTestEnable = FALSE; 
		PixelShader = compile ps_3_0 f(light1Dir, light1Color, light1Attenuation, light1Hotspot, light1Falloff, light1Type, light1attenType, light1coneType, light1CastShadows); 
	}  

}   