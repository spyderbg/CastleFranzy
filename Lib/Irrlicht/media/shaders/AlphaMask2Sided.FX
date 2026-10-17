
/*** Generated through Lumonix shaderFX  by: alexandre.chekroun in 3dsmax at: 2009-08-14 11:16:52  ***/ 

// This FX shader was built to support 3ds Max's standard shader compiler. 


float3 UIColor_1859 : Ambient
<
	string UIName = "Ambient Color";
	string UIType = "ColorSwatch";
> = {0.345098f, 0.345098f, 0.345098f};
 
texture TextureMap_5309 : DiffuseMap
<
	string Name = "D_grass.dds";
	string UIName = "Diffuse Map";
	string ResourceType = "2D";
>;
 
sampler2D TextureMap_5309Sampler = sampler_state
{
	Texture = <TextureMap_5309>;
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

	float2 texCoord		: TEXCOORD0; 

}; 

// output to fragment program 
struct v2f { 
        float4 position    		: POSITION; 
        float3 light1Vec    	: TEXCOORD0; 

	float2 texCoord			: TEXCOORD1; 

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
	Out.position = mul(In.position, wvp);				//transform vert position to homogeneous clip space 

	In.texCoord += float2(0.0,1.0);		//this fixes Max's V texcoord which is off by one 
	Out.texCoord = In.texCoord;						//pass through texture coordinates from channel 1 

	return Out; 
} 

//Pixel Shader
float4 f(v2f In, uniform float3 lightDir, uniform float4 lightColor, uniform float4 lightAttenuation, uniform float lightHotspot, uniform float lightFalloff, uniform int lightType, uniform int lightattenType, uniform int lightconeType, uniform bool lightCastShadows) : COLOR 
{ 

	float4 TextureMap_5309 = tex2D(TextureMap_5309Sampler, In.texCoord.xy);
	float3 input1 = (UIColor_1859.rgb * TextureMap_5309.rgb); 


	float3 input2 = TextureMap_5309.rgb; 


	float input7 = TextureMap_5309.a; 

	float3 ret =  float3(0,0,0); 
	float3 AmbIllum = input1; 
	ret = AmbIllum; 
	float3 diffuseColor = input2;  
	float3 N = float3(0.0, 0.0, 1.0);	//the Normal socket was empty - using default value 

	float3 L1 = normalize(In.light1Vec.xyz); 
	float3 light1 = float3(0.0, 0.0, 0.0); 
	float3 NdotL1 = dot(N, L1) ;  
	float3 diffuse1 = saturate(NdotL1);  
	float3 specularColor1 = float3(0,0,0);	//Specular Color socket is empty so using black
	light1 += specularColor1 + (diffuseColor * diffuse1);			//add specular and diffuse color together
	light1 *= light1Color.xyz; 
	float3 light1Dir = light1Dir;	//use the light direction from the light header information  
	float attenuation1 = attenuation_func(light1attenType, light1Attenuation, In.light1Vec); 					//calculate the light attenuation  
	float coneangle1 = coneangle_func(light1coneType, light1Hotspot, light1Falloff, In.light1Vec, light1Dir); 	//calculate the light's cone angle 
	light1 *= attenuation1 * coneangle1;			//multiply by the light decay  
	ret +=  light1; 

	float Opacity = input7; 
	float4 done = float4(ret, Opacity); 
	return done; 
} 

technique Complete  
{  
	pass one  
    {		 
		VertexShader = compile vs_3_0 v(light1Pos,  light1Type, light1Dir); 
		ZEnable = true; 
		ZWriteEnable = true; 
		CullMode = none; 
		ShadeMode = Gouraud;
		AlphaBlendEnable = true; 
		SrcBlend = SrcAlpha; 
		DestBlend = InvSrcAlpha; 
		AlphaTestEnable = TRUE; 
		AlphaRef = 64; 
		AlphaFunc = GREATER; 
		PixelShader = compile ps_3_0 f(light1Dir, light1Color, light1Attenuation, light1Hotspot, light1Falloff, light1Type, light1attenType, light1coneType, light1CastShadows); 
	}  

}   