
/*** Generated through Lumonix shaderFX  by: alexandre.chekroun in 3dsmax at: 2009-08-14 11:06:01  ***/ 

// This FX shader was built to support 3ds Max's standard shader compiler. 


float3 UIColor_3864 : Ambient
<
	string UIName = "Ambient Color";
	string UIType = "ColorSwatch";
> = {0.0f, 0.0f, 0.0f};
 
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

int texcoord2 : Texcoord 
<
	int Texcoord = 2;
	int MapChannel = 3;
	string UIType = "None"; 
>;

texture TextureMap_9619 : LightMap
<
	string Name = "Lightmap_testCompleteMap.tga";
	string UIName = "LightMap";
	string ResourceType = "2D";
>;
 
sampler2D TextureMap_9619Sampler = sampler_state
{
	Texture = <TextureMap_9619>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
float UIConst_2762
<
	string UIType = "FloatSpinner";
	float UIMin = -100.0;
	float UIMax = 100.0;
	float UIStep = 0.01;
	string UIName = "LightMap Brightness";
> = 0.0;
 
float UIConst_6251
<
	string UIType = "FloatSpinner";
	float UIMin = 0.0;
	float UIMax = 100.0;
	float UIStep = 0.01;
	string UIName = "LightMap Contrast";
> = 1.0;
 
texture TextureMap_3386 : DiffuseMap
<
	string Name = "D_cobblestones.dds";
	string UIName = "Diffuse Texture";
	string ResourceType = "2D";
>;
 
sampler2D TextureMap_3386Sampler = sampler_state
{
	Texture = <TextureMap_3386>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
float UIConst_9315
<
	string UIType = "FloatSpinner";
	float UIMin = 1.0;
	float UIMax = 50.0;
	float UIStep = 1.0;
	string UIName = "Diffuse Detail Tile";
> = 8.0;
 
texture TextureMap_8879
<
	string Name = "noise_01.tga";
	string UIName = "Detail Texture";
	string ResourceType = "2D";
>;
 
sampler2D TextureMap_8879Sampler = sampler_state
{
	Texture = <TextureMap_8879>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
float3 UIColor_1359 : Diffuse
<
	string UIName = "Diffuse Color";
	string UIType = "ColorSwatch";
> = {1.0f, 1.0f, 1.0f};
 
texture SpecMap : SpecularMap
<
	string Name = "D_cobblestones.dds";
	string UIName = "Specular Map";
	string ResourceType = "2D";
>;
 
sampler2D SpecMapSampler = sampler_state
{
	Texture = <SpecMap>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
float UIConst_8059
<
	string UIType = "FloatSpinner";
	float UIMin = 0.0;
	float UIMax = 10.0;
	float UIStep = 0.1;
	string UIName = "Specular Level Factor";
> = 1.0;
 
float UIConst_9233
<
	string UIType = "FloatSpinner";
	float UIMin = 0.0;
	float UIMax = 999.0;
	float UIStep = 0.1;
	string UIName = "Glossiness";
> = 20.0;
 
texture TextureMap_9148 : NormalMap
<
	string Name = "N_cobblestones_ati.dds";
	string UIName = "Normal Map";
	string ResourceType = "2D";
>;
 
sampler2D TextureMap_9148Sampler = sampler_state
{
	Texture = <TextureMap_9148>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};
 
float UIConst_5481
<
	string UIType = "FloatSpinner";
	float UIMin = 1.0;
	float UIMax = 50.0;
	float UIStep = 1.0;
	string UIName = "NM Detail Tile";
> = 8.0;
 
texture TextureMap_3091
<
	string Name = "noise_01_N.tga";
	string UIName = "Detail Normal Map";
	string ResourceType = "2D";
>;
 
sampler2D TextureMap_3091Sampler = sampler_state
{
	Texture = <TextureMap_3091>;
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

	float2 texCoord2	: TEXCOORD2; 
	float2 texCoord		: TEXCOORD0; 

}; 

// output to fragment program 
struct v2f { 
        float4 position    		: POSITION; 
        float3 light1Vec    	: TEXCOORD0; 
        float3 eyeVec	    	: TEXCOORD1; 

	float2 texCoord2		: TEXCOORD2; 
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

	In.texCoord2 += float2(0.0,1.0);		//this fixes Max's V texcoord which is off by one 
	Out.texCoord2 = In.texCoord2;						//pass through texture coordinates from channel 3 
	In.texCoord += float2(0.0,1.0);		//this fixes Max's V texcoord which is off by one 
	Out.texCoord = In.texCoord;						//pass through texture coordinates from channel 1 

	return Out; 
} 

//Pixel Shader
float4 f(v2f In, uniform float3 lightDir, uniform float4 lightColor, uniform float4 lightAttenuation, uniform float lightHotspot, uniform float lightFalloff, uniform int lightType, uniform int lightattenType, uniform int lightconeType, uniform bool lightCastShadows) : COLOR 
{ 
	float3 V = normalize(In.eyeVec);		//creating the eye vector  

	float4 TextureMap_9619 = tex2D(TextureMap_9619Sampler, In.texCoord2.xy);
	float3 MathBrightnessContrast_7026 = (TextureMap_9619.rgb - 0.5) * UIConst_6251 + 0.5 + UIConst_2762;
	float4 TextureMap_3386 = tex2D(TextureMap_3386Sampler, In.texCoord.xy);
	float2 IncomingUVCoords = In.texCoord.xy; 
	float4 TextureMap_8879 = tex2D(TextureMap_8879Sampler, (IncomingUVCoords * UIConst_9315).xy);
	float3 MultLightmapDiff = MathBrightnessContrast_7026 * ((TextureMap_3386.rgb * TextureMap_8879.rgb) * UIColor_1359.rgb);
	float3 input1 = (UIColor_3864.rgb * MultLightmapDiff); 


	float3 input2 = MultLightmapDiff; 


	float4 SpecMap = tex2D(SpecMapSampler, In.texCoord.xy);
	float3 input3 = SpecMap.rgb; 


	float input4 = (SpecMap.r * UIConst_8059); 


	float input5 = UIConst_9233; 


	float4 TextureMap_9148 = tex2D(TextureMap_9148Sampler, In.texCoord.xy);
	TextureMap_9148.xyz = TextureMap_9148.xyz * 2 - 1;		//expand to -1 to 1 range 
	TextureMap_9148.rgb = normalize(TextureMap_9148.rgb); 		//normalized the normal vector 
	float2 IncomingUVs = In.texCoord.xy; 
	float4 TextureMap_3091 = tex2D(TextureMap_3091Sampler, (IncomingUVs * UIConst_5481).xy);
	TextureMap_3091.xyz = TextureMap_3091.xyz * 2 - 1;		//expand to -1 to 1 range 
	TextureMap_3091.rgb = normalize(TextureMap_3091.rgb); 		//normalized the normal vector 
	float3 Normalize = normalize((TextureMap_9148.rgb + TextureMap_3091.rgb));		//Normalize 
	float3 input8 = Normalize; 

	float3 ret =  float3(0,0,0); 
	float3 AmbIllum = input1; 
	ret = AmbIllum; 
	float3 diffuseColor = input2;  
	float3 N = input8;						//using the Normal socket  
	float3 spec = float3(0,0,0); 
	float3 specularColor = input3;			//using the Specular Color socket 
	specularColor *= input4;				//Multiplying Specular Color by the Specular Level  
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