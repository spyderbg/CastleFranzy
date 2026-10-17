#include "stdafx.h"
#include "gll.h"
#include "Game.h"
#include "GL3DCamera.h"

fx32 FX_MUL(fx32 v1, fx32 v2)
{
	float fRet = (v1 / 4096.f) * (v2 / 4096.f);
	fRet *= 4096;
	int ret = static_cast<int>(fRet);
	return ret;
}

#if defined(_WINDOWS) && !defined(WIPI)
	const double pi = 3.1415926535897932384626433832795;
#endif

#define GENERATE_ASIN_TABLE 0



#if GENERATE_ASIN_TABLE && defined(_WINDOWS) && !defined(WIPI)

	///---------------------------------------------------------------------
	///	This function generate the following asin table
	/// values in the table are in degree in fixed 16
	///	Uses the ASin function to access the table
	///---------------------------------------------------------------------
	#include <fstream>
	#include <math.h>
	static void GenerateASinTable()
	{		
		std::ofstream out("c:\\asin.txt");
		if(out)
		{
			// precision of the table: 4096/4 = 1024 entry;			
			//
			const int nb = GLT_ANGLE_2PI / 4 - 1;

			for(int i=0;i<=nb;i++)
			{
				const double x = double(i) /double(nb);

				double radian = asin(x);
				double degre = radian * 360.0 / (2*pi);

				double fixedDegre = degre * GLT_FIXED_ONE;

				if( (i%8)==0)
					out << "\n";
				out << int(fixedDegre + 0.5) << ",";
			}
		}
	}
#endif //defined(_WINDOWS)

const GLfixed kASinTable[] = 
{
	0, 3671, 7341, 11012, 14682, 18353, 22023, 25694, 
		29364, 33035, 36706, 40376, 44047, 47718, 51389, 55060, 
		58731, 62402, 66073, 69744, 73415, 77086, 80758, 84429, 
		88100, 91772, 95444, 99115, 102787, 106459, 110131, 113803, 
		117476, 121148, 124820, 128493, 132166, 135839, 139512, 143185, 
		146858, 150531, 154205, 157879, 161552, 165226, 168901, 172575, 
		176249, 179924, 183599, 187274, 190949, 194624, 198300, 201976, 
		205652, 209328, 213004, 216681, 220357, 224034, 227711, 231389, 
		235066, 238744, 242422, 246101, 249779, 253458, 257137, 260816, 
		264496, 268175, 271855, 275536, 279216, 282897, 286578, 290260, 
		293941, 297623, 301305, 304988, 308671, 312354, 316037, 319721, 
		323405, 327089, 330774, 334459, 338144, 341830, 345516, 349202, 
		352889, 356576, 360263, 363951, 367639, 371327, 375016, 378705, 
		382394, 386084, 389774, 393465, 397156, 400847, 404539, 408231, 
		411923, 415616, 419310, 423003, 426697, 430392, 434087, 437782, 
		441478, 445174, 448871, 452568, 456266, 459964, 463662, 467361, 
		471060, 474760, 478461, 482161, 485863, 489564, 493266, 496969, 
		500672, 504376, 508080, 511785, 515490, 519195, 522901, 526608, 
		530315, 534023, 537731, 541440, 545149, 548859, 552569, 556280, 
		559992, 563704, 567416, 571129, 574843, 578557, 582272, 585987, 
		589703, 593420, 597137, 600855, 604573, 608292, 612011, 615732, 
		619452, 623174, 626896, 630618, 634341, 638065, 641790, 645515, 
		649241, 652967, 656694, 660422, 664150, 667879, 671609, 675339, 
		679070, 682802, 686534, 690267, 694001, 697736, 701471, 705207, 
		708943, 712680, 716418, 720157, 723897, 727637, 731378, 735119, 
		738862, 742605, 746349, 750093, 753839, 757585, 761332, 765079, 
		768828, 772577, 776327, 780078, 783829, 787582, 791335, 795089, 
		798843, 802599, 806355, 810113, 813871, 817630, 821389, 825150, 
		828911, 832673, 836436, 840200, 843965, 847731, 851497, 855265, 
		859033, 862802, 866572, 870343, 874115, 877888, 881661, 885436, 
		889211, 892988, 896765, 900543, 904322, 908102, 911883, 915665, 
		919448, 923232, 927017, 930803, 934589, 938377, 942166, 945955, 
		949746, 953538, 957330, 961124, 964918, 968714, 972511, 976308, 
		980107, 983907, 987708, 991509, 995312, 999116, 1002921, 1006727, 
		1010534, 1014342, 1018151, 1021962, 1025773, 1029585, 1033399, 1037213, 
		1041029, 1044846, 1048664, 1052483, 1056303, 1060124, 1063947, 1067770, 
		1071595, 1075421, 1079247, 1083076, 1086905, 1090735, 1094567, 1098400, 
		1102234, 1106069, 1109905, 1113743, 1117581, 1121421, 1125262, 1129105, 
		1132948, 1136793, 1140639, 1144486, 1148335, 1152184, 1156035, 1159888, 
		1163741, 1167596, 1171452, 1175309, 1179168, 1183028, 1186889, 1190751, 
		1194615, 1198480, 1202347, 1206215, 1210084, 1213954, 1217826, 1221699, 
		1225573, 1229449, 1233326, 1237205, 1241084, 1244966, 1248848, 1252732, 
		1256618, 1260505, 1264393, 1268282, 1272173, 1276066, 1279960, 1283855, 
		1287752, 1291650, 1295550, 1299451, 1303353, 1307257, 1311163, 1315070, 
		1318978, 1322888, 1326800, 1330713, 1334627, 1338543, 1342461, 1346380, 
		1350300, 1354222, 1358146, 1362071, 1365998, 1369926, 1373856, 1377788, 
		1381721, 1385656, 1389592, 1393530, 1397469, 1401410, 1405353, 1409297, 
		1413243, 1417191, 1421140, 1425091, 1429043, 1432998, 1436953, 1440911, 
		1444870, 1448831, 1452794, 1456758, 1460724, 1464692, 1468661, 1472633, 
		1476606, 1480580, 1484557, 1488535, 1492515, 1496497, 1500480, 1504466, 
		1508453, 1512442, 1516432, 1520425, 1524419, 1528415, 1532413, 1536413, 
		1540415, 1544419, 1548424, 1552431, 1556440, 1560451, 1564464, 1568479, 
		1572496, 1576515, 1580535, 1584558, 1588582, 1592609, 1596637, 1600667, 
		1604699, 1608734, 1612770, 1616808, 1620848, 1624891, 1628935, 1632981, 
		1637029, 1641080, 1645132, 1649186, 1653243, 1657301, 1661362, 1665424, 
		1669489, 1673556, 1677625, 1681696, 1685769, 1689845, 1693922, 1698002, 
		1702083, 1706167, 1710253, 1714342, 1718432, 1722525, 1726620, 1730717, 
		1734816, 1738918, 1743021, 1747127, 1751236, 1755346, 1759459, 1763574, 
		1767691, 1771811, 1775933, 1780057, 1784184, 1788313, 1792444, 1796578, 
		1800714, 1804852, 1808993, 1813136, 1817282, 1821430, 1825580, 1829733, 
		1833888, 1838046, 1842206, 1846368, 1850534, 1854701, 1858871, 1863044, 
		1867219, 1871396, 1875576, 1879759, 1883944, 1888132, 1892322, 1896515, 
		1900711, 1904909, 1909110, 1913313, 1917519, 1921728, 1925939, 1930153, 
		1934369, 1938589, 1942811, 1947035, 1951263, 1955493, 1959726, 1963961, 
		1968199, 1972441, 1976684, 1980931, 1985181, 1989433, 1993688, 1997946, 
		2002207, 2006470, 2010737, 2015006, 2019279, 2023554, 2027832, 2032113, 
		2036397, 2040684, 2044974, 2049266, 2053562, 2057861, 2062163, 2066468, 
		2070776, 2075086, 2079400, 2083717, 2088038, 2092361, 2096687, 2101016, 
		2105349, 2109685, 2114024, 2118366, 2122711, 2127059, 2131411, 2135766, 
		2140124, 2144485, 2148849, 2153217, 2157588, 2161963, 2166340, 2170721, 
		2175106, 2179493, 2183884, 2188279, 2192677, 2197078, 2201483, 2205891, 
		2210302, 2214717, 2219136, 2223558, 2227983, 2232412, 2236845, 2241281, 
		2245721, 2250164, 2254611, 2259061, 2263515, 2267973, 2272434, 2276899, 
		2281368, 2285840, 2290317, 2294796, 2299280, 2303767, 2308259, 2312754, 
		2317252, 2321755, 2326261, 2330772, 2335286, 2339804, 2344326, 2348852, 
		2353382, 2357915, 2362453, 2366995, 2371541, 2376091, 2380644, 2385202, 
		2389764, 2394331, 2398901, 2403475, 2408054, 2412636, 2417223, 2421814, 
		2426410, 2431009, 2435613, 2440221, 2444834, 2449450, 2454071, 2458697, 
		2463327, 2467961, 2472599, 2477243, 2481890, 2486542, 2491199, 2495860, 
		2500525, 2505195, 2509870, 2514549, 2519233, 2523922, 2528615, 2533313, 
		2538015, 2542723, 2547435, 2552152, 2556873, 2561600, 2566331, 2571067, 
		2575808, 2580554, 2585305, 2590061, 2594822, 2599588, 2604358, 2609134, 
		2613915, 2618701, 2623492, 2628289, 2633090, 2637897, 2642709, 2647526, 
		2652348, 2657176, 2662009, 2666847, 2671691, 2676540, 2681394, 2686254, 
		2691120, 2695991, 2700867, 2705749, 2710637, 2715530, 2720429, 2725333, 
		2730243, 2735159, 2740081, 2745008, 2749941, 2754880, 2759825, 2764776, 
		2769733, 2774695, 2779664, 2784639, 2789620, 2794606, 2799599, 2804598, 
		2809604, 2814615, 2819633, 2824656, 2829687, 2834723, 2839766, 2844815, 
		2849871, 2854933, 2860002, 2865077, 2870159, 2875247, 2880342, 2885444, 
		2890553, 2895668, 2900790, 2905918, 2911054, 2916197, 2921346, 2926502, 
		2931666, 2936836, 2942014, 2947199, 2952390, 2957589, 2962796, 2968009, 
		2973230, 2978458, 2983694, 2988937, 2994188, 2999446, 3004711, 3009985, 
		3015266, 3020554, 3025851, 3031155, 3036467, 3041787, 3047115, 3052451, 
		3057794, 3063146, 3068506, 3073875, 3079251, 3084636, 3090029, 3095430, 
		3100840, 3106258, 3111685, 3117120, 3122564, 3128017, 3133478, 3138948, 
		3144427, 3149915, 3155412, 3160918, 3166432, 3171956, 3177490, 3183032, 
		3188584, 3194144, 3199715, 3205295, 3210884, 3216483, 3222092, 3227710, 
		3233338, 3238976, 3244624, 3250281, 3255949, 3261627, 3267315, 3273014, 
		3278722, 3284441, 3290171, 3295911, 3301661, 3307422, 3313194, 3318977, 
		3324771, 3330575, 3336391, 3342217, 3348055, 3353904, 3359765, 3365637, 
		3371520, 3377415, 3383322, 3389240, 3395171, 3401113, 3407067, 3413033, 
		3419012, 3425003, 3431006, 3437022, 3443050, 3449091, 3455144, 3461211, 
		3467290, 3473383, 3479488, 3485607, 3491739, 3497885, 3504044, 3510217, 
		3516404, 3522604, 3528819, 3535048, 3541290, 3547548, 3553819, 3560106, 
		3566407, 3572722, 3579053, 3585399, 3591760, 3598136, 3604528, 3610935, 
		3617358, 3623797, 3630252, 3636722, 3643210, 3649713, 3656233, 3662770, 
		3669324, 3675895, 3682482, 3689088, 3695710, 3702350, 3709008, 3715684, 
		3722378, 3729091, 3735822, 3742571, 3749339, 3756126, 3762933, 3769759, 
		3776604, 3783469, 3790354, 3797259, 3804185, 3811131, 3818097, 3825085, 
		3832094, 3839124, 3846176, 3853250, 3860346, 3867464, 3874605, 3881768, 
		3888954, 3896164, 3903397, 3910655, 3917936, 3925241, 3932571, 3939926, 
		3947306, 3954712, 3962143, 3969600, 3977084, 3984594, 3992131, 3999696, 
		4007288, 4014908, 4022557, 4030234, 4037940, 4045676, 4053441, 4061236, 
		4069062, 4076919, 4084807, 4092727, 4100679, 4108664, 4116681, 4124732, 
		4132817, 4140937, 4149091, 4157281, 4165506, 4173768, 4182067, 4190404, 
		4198778, 4207191, 4215643, 4224135, 4232667, 4241241, 4249855, 4258513, 
		4267213, 4275957, 4284745, 4293578, 4302458, 4311384, 4320357, 4329378, 
		4338449, 4347570, 4356741, 4365964, 4375240, 4384570, 4393954, 4403394, 
		4412891, 4422445, 4432059, 4441733, 4451468, 4461266, 4471127, 4481054, 
		4491048, 4501110, 4511241, 4521443, 4531719, 4542068, 4552493, 4562997, 
		4573580, 4584245, 4594993, 4605827, 4616749, 4627761, 4638866, 4650066, 
		4661363, 4672761, 4684262, 4695869, 4707585, 4719414, 4731358, 4743423, 
		4755610, 4767925, 4780371, 4792953, 4805675, 4818544, 4831563, 4844739, 
		4858077, 4871583, 4885266, 4899131, 4913186, 4927441, 4941902, 4956582, 
		4971488, 4986633, 5002029, 5017689, 5033627, 5049860, 5066403, 5083276, 
		5100501, 5118100, 5136099, 5154528, 5173418, 5192808, 5212740, 5233262, 
		5254430, 5276312, 5298984, 5322540, 5347095, 5372787, 5399792, 5428337, 
		5458722, 5491358, 5526840, 5566077, 5610602, 5663403, 5732199, 5898240,
};

const int kASinTableSize = sizeof(kASinTable) / sizeof(kASinTable[0]);



///---------------------------------------------------------------------
///
///---------------------------------------------------------------------
GLfixed	GL3DCamera::ASin(GLfixed x) // static
{
	const int kTableShiftVal = GLT_ANGLE_SHIFT - 2;

	GLL_ASSERT(kASinTableSize == (1<<kTableShiftVal));


//	GLL_ASSERT(x >= -GLT_FIXED_ONE && x <= GLT_FIXED_ONE);

	if(x>=GLT_FIXED_ONE)
		x=GLT_FIXED_ONE-1;
	else if(x <= -GLT_FIXED_ONE)
		x = -GLT_FIXED_ONE + 1;


	if(x<0)
	{
		int val = (-x) >> ( GLT_FIXED_SHIFT - kTableShiftVal);
		GLL_ASSERT(val >=0 && val <= kASinTableSize);
		if(val==kASinTableSize)
			val=kASinTableSize-1;
		return -kASinTable[val];
	}
	else
	{
		int val = x >> ( GLT_FIXED_SHIFT - kTableShiftVal);
		GLL_ASSERT(val >=0 && val < kASinTableSize);
		return kASinTable[val];
	}
}





//---------------------------------------------------------------------
//--
//--	class GL3DCamera
//--
//---------------------------------------------------------------------

GL3DCamera::GL3DCamera(bool isLandScape/* = false*/)/*
	: 
		m_zNear(DEFAULT_ZNEAR),
		m_zFar(DEFAULT_ZFAR),
		m_viewportRatio(GLT_INT_TO_FIXED(1)),	
		m_fov((isLandScape ? DEFAULT_LAND_SCAPE_FOV : DEFAULT_FOV)),
		m_dirty(true),
		m_pos(0,0,0),
		m_rot(0,0,0)*/
	
{	
	m_fov = GLT_INT_TO_FIXED(5);
	m_pos = GLTvec3D(0,0,0);
	m_rot = GLTvec3D(0,0,0);

	//Init(isLandScape);
	/*
	m_zNear = DEFAULT_ZNEAR;
		m_zFar = DEFAULT_ZFAR;
		m_viewportRatio = GLT_INT_TO_FIXED(1);	
		m_fov = isLandScape ? DEFAULT_LAND_SCAPE_FOV : DEFAULT_FOV;
		m_dirty = true;
		
	
		m_matrix.SetIdentity();
	//	m_mtx_inv.SetIdentity();
		m_mtx_inv_notrans.SetIdentity();
		m_viewportCenterX = GLT_FLOAT_TO_FIXED(Game::Instance()->viewport_cx);
		m_viewportCenterY = GLT_FLOAT_TO_FIXED(Game::Instance()->viewport_cy);
		m_viewportW = (Game::Instance())->viewport_w;
		m_viewportH = (Game::Instance())->viewport_h;*/
	

}

///---------------------------------------------------------------------
///
///---------------------------------------------------------------------
void GL3DCamera::Init(CGame* pGame, bool isLandScape)
{
	m_zNear = DEFAULT_ZNEAR;
	m_zFar = DEFAULT_ZFAR;
	m_viewportRatio = GLT_INT_TO_FIXED(1);
	m_dirty = true;
	m_matrix.SetIdentity();
	m_mtx_inv_notrans.SetIdentity();
	m_viewportCenterX = GLT_FLOAT_TO_FIXED(0);
	m_viewportCenterY = GLT_FLOAT_TO_FIXED(0);

#if IPHONE
	m_fov = GLT_FLOAT_TO_FIXED(21.0f);
	m_viewportW = GL_VIEWPORT_WIDTH;
	m_viewportH = GL_VIEWPORT_HEIGHT;
#else
	m_fov = GLT_INT_TO_FIXED(pGame->GetViewportWidth() == 240 ? 21 : 15);
	m_viewportW = pGame->GetViewportWidth();
	m_viewportH = pGame->GetViewportHeight();
#endif

}

///---------------------------------------------------------------------
///
///---------------------------------------------------------------------

void GL3DCamera::Update(GLfixed deltaT)
{
}
///---------------------------------------------------------------------
///
///---------------------------------------------------------------------


void GL3DCamera::SetFov(GLfixed fov_in)
{
	m_fov = fov_in;
	m_dirty = true;
} 
///---------------------------------------------------------------------
///
///---------------------------------------------------------------------

void GL3DCamera::Set(const GLTvec3D& pos,const GLTvec3D& rot)
{
	m_pos = pos;
	m_rot = rot;
	m_dirty = true;

//WJ	_pos = m_pos;
//WJ	_rot = m_rot;
}

///---------------------------------------------------------------------
/// static
///---------------------------------------------------------------------
int GL3DCamera::GetAlignmentRotation(const GLTvec3D& subject,const GLTvec3D& target)
{
	const GLTvec3D vec = target - subject;

	const GLfixed z2 = GLTMul(vec.z,vec.z);
	
	GLfixed  lengthXZ2 = GLTMul(vec.x,vec.x) + z2;
    
	if(lengthXZ2!=0)
	{
		GLfixed  lengthX = GLTSqrt(lengthXZ2);
		GLfixed x = GLTDiv( vec.x , lengthX );
		
		int y = ASin(x);
	    
		if(vec.z > 0)
			y+=GLT_INT_TO_FIXED(180);
		else
			y = -y;

		return y;
	}
	else
		return 0;
}


///---------------------------------------------------------------------
///
///---------------------------------------------------------------------
int GL3DCamera::GetRotationYToAllignToCamera(const GLTvec3D& pos) const
{
	return GetAlignmentRotation(m_pos ,pos);	
}



///---------------------------------------------------------------------
///
///---------------------------------------------------------------------

void GL3DCamera::SetLookAt( const GLTvec3D& eyepos,const GLTvec3D& lookat )
{	
//	m_lookAt = lookat;

	const GLTvec3D vec = lookat - eyepos;
	const int z2 = GLTMul(vec.z,vec.z);
	GLfixed  lengthXZ2 = GLTMul(vec.x,vec.x) + z2;
	GLfixed  lengthX = GLTSqrt(lengthXZ2);
	if(lengthX == 0)
		return;
	GLfixed x = GLTDiv( vec.x , lengthX );

	GLTvec3D rot;

#if IPHONE
	rot.z = GLT_FLOAT_TO_FIXED(90);//0;
#else
	rot.z = 0;
#endif

	if (x > 65536)
	{
		x = 65536;
	}
	else if (x < -65536)
	{
		x = -65536;
	}
#if PLATFORM ==  PLATFORM_KDDI
	rot.y = GL3DCamera::ASin(x);
#else
	rot.y = GLT_FLOAT_TO_FIXED(asin(x / 65536.0f) * 180 / 3.1415926535798);
#endif
	GLfixed lengthY = GLTSqrt(GLTMul(vec.y,vec.y) + lengthXZ2 );
	GLfixed y = GLTDiv( vec.y , lengthY );

	if (y > 65536)
	{
		y = 65536;
	}
	else if (y < -65536)
	{
		y = -65536;
	}
#if PLATFORM == PLATFORM_KDDI
	rot.x = GL3DCamera::ASin(y);
#else
	rot.x = GLT_FLOAT_TO_FIXED(asin(y / 65536.0f) * 180 / 3.1415926535798);
#endif

	if(vec.z > 0)
		rot.y += GLT_INT_TO_FIXED(180);
	else
		rot.y *= -1;

	Set( eyepos, rot );
}

void GL3DCamera::GetDirectionFromRot(GLTvec3D &dV)
{
	GLTmatrix rotMat;
	rotMat.SetIdentity();
	rotMat.RotateX(m_rot.x);
	rotMat.RotateY(m_rot.y);
	rotMat.Mul(GLTvec3D(0, 0, -GLT_FIXED_ONE), dV);
}

///---------------------------------------------------------------------
///
///---------------------------------------------------------------------

void GL3DCamera::ComputeMatrix() const
{
	if(m_dirty)
	{
		m_matrix.SetIdentity();
		//m_matrix.RotateZ(-m_rot.z);
		//m_matrix.RotateX(-m_rot.x);
		//m_matrix.RotateY(-m_rot.y);

		GLTmatrix m;
		m.SetIdentity();
		m.SetRotate(-m_rot.z,GLT_INT_TO_FIXED(0),GLT_INT_TO_FIXED(0),GLT_INT_TO_FIXED(1));
		m_matrix = m_matrix * m;
		
		m.SetIdentity();
		m.SetRotate(-m_rot.x,GLT_INT_TO_FIXED(1),GLT_INT_TO_FIXED(0),GLT_INT_TO_FIXED(0));
		m_matrix = m_matrix * m;

		m.SetIdentity();
		m.SetRotate(-m_rot.y,GLT_INT_TO_FIXED(0),GLT_INT_TO_FIXED(1),GLT_INT_TO_FIXED(0));
		m_matrix = m_matrix * m;

		//GLTmatrix m;
		m.SetIdentity();
		m.SetTranslate(-m_pos.x,-m_pos.y,-m_pos.z);
		m_matrix = m_matrix  * m;


//		m_mtx_inv.SetTranslate(m_pos.x, m_pos.y, m_pos.z);
//		m_mtx_inv.RotateY(m_rot.y);
//		m_mtx_inv.RotateX(m_rot.x);
//		m_mtx_inv.RotateZ(m_rot.z);

		m_mtx_inv_notrans.SetIdentity();
		m_mtx_inv_notrans.RotateY(m_rot.y);
		m_mtx_inv_notrans.RotateX(m_rot.x);
		m_mtx_inv_notrans.RotateZ(m_rot.z);	
		m_dirty = false;
	}
}

GLTmatrix GL3DCamera::GetMatrix(void) const
{
	ComputeMatrix();
	return m_matrix;
}

///---------------------------------------------------------------------
///
///---------------------------------------------------------------------

void GL3DCamera::ApplyTransform() const
{	
	if(true)//todo:
	{
		const int angle4096 = GetFoVAtScale();
		
		GLfixed sinFov		= GLTSin(angle4096);
		GLfixed cosFov		= GLTCos(angle4096);
		GLfixed tanFov		= GLTDiv(sinFov, cosFov);
		GLfixed fH			= GLTMul(tanFov, m_zNear);
		GLfixed	ratio		= GLTDiv(m_viewportW, m_viewportH);
		GLfixed fW			= GLTMul(fH, ratio );
		
		glMatrixMode( GL_PROJECTION );
		glLoadIdentity();
		glFrustumx( -fW, fW, -fH, fH, m_zNear,m_zFar);
		ComputeMatrix();

#if !IPHONE
		ComputeFrustrumBTLR(tanFov, tanFov, m_zNear, m_zFar);
#else
		ComputeFrustrumBTLR(fH, fH, m_zNear, m_zFar);
#endif
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
 	glRotatex(-m_rot.z,GLT_INT_TO_FIXED(0),GLT_INT_TO_FIXED(0),GLT_INT_TO_FIXED(1));
	glRotatex(-m_rot.x,GLT_INT_TO_FIXED(1),GLT_INT_TO_FIXED(0),GLT_INT_TO_FIXED(0));
	glRotatex(-m_rot.y,GLT_INT_TO_FIXED(0),GLT_INT_TO_FIXED(1),GLT_INT_TO_FIXED(0));
	glTranslatex(-m_pos.x,-m_pos.y ,-m_pos.z);
}

///---------------------------------------------------------------------
///
///---------------------------------------------------------------------
GL3DCamera::~GL3DCamera()
{
};

///---------------------------------------------------------------------
///
///---------------------------------------------------------------------

unsigned int GL3DCamera::TestVisibility(const GLTvec3D& _worldSpace) const 
{


	unsigned int flags = 0;

#if defined(_WINDOWS) && defined(WIPI) && 0
	const GLTvec3D worldSpace(0,0,0);
	GLTvec3D camSpace;
	m_matrix.Mul(worldSpace,camSpace);


	// test far clip 
	if(camSpace.z < -m_zFar)
		flags |=kFar;

	if(camSpace.z > -m_zNear)
		flags |= kNear;
    
	// Test side:
	const int angle4096 = GetFoVAtScale();


	GLfixed sin = GLTSin(angle4096);
	GLfixed cos = GLTCos(angle4096);

	GLTvec2D v1(-cos,sin);
	GLTvec2D v2(camSpace.x,camSpace.z);

	int d0 = v1 * v2;

	if(d0>0)
		flags |= kRight;

	v1.y = -v1.y;

	int d1 = v1 * v2;
	if(d1<0)
		flags |= kLeft;



	GLfixed ratio = GLT_INT_TO_FIXED(C(Screen::Height)) / C(Screen::Width);

	DebugFixed _ratio = ratio;
    
    v1.y = GLTMul(ratio,sin);

	int d2 = v1 * v2;
	if(d2 > 0)
		flags |= kUp;

	v1.y = -v1.y;
	int d3 = v1 * v2;
	if(d3 < 0)
		flags |= kDown;

#endif

	return flags;
}
/////---------------------------------------------------------------------
/////
/////---------------------------------------------------------------------
//void GL3DCamera::SetViewportRatio(int viewportSizeX,int viewportSizeY)
//{
//	int newRatio = GLTDiv(GLT_INT_TO_FIXED(viewportSizeX),GLT_INT_TO_FIXED(viewportSizeY));
//	
//	if(newRatio != m_viewportRatio)
//	{
//		m_viewportRatio = newRatio;
//		m_dirty = true;
//	}
//}
//  [7/26/2006 baibo.zhang]
// transform the vector array to screen coordinates
// if any outCamSpace[i].z == 0, then the screen position is not computable.
void GL3DCamera::GetScreenPosition(int vectorNum, const GLTvec3D* worldSpace/*,int screenWidth,int sceenHeight*/, GLTvec3D *outCamSpace) const
{
	ComputeMatrix();
	const GLfixed focalLength = GetFocalLength();
	int i;
	GLfixed d;
	for(i=0; i< vectorNum; i++)
	{
		m_matrix.Mul(worldSpace[i], outCamSpace[i]);
		if (outCamSpace[i].z < GLT_INT_TO_FIXED(-1) )
		{
			d= ::GLTDiv(focalLength, outCamSpace[i].z);
			outCamSpace[i].x = GLTMul(outCamSpace[i].x, d);
			outCamSpace[i].y = GLTMul(outCamSpace[i].y, d);
			outCamSpace[i].x = m_viewportCenterX - outCamSpace[i].x;
			outCamSpace[i].y = m_viewportCenterY + outCamSpace[i].y;
		}
		else
		{
			outCamSpace[i].z = 0;
		}
	}

}
///---------------------------------------------------------------------
// if the returned value Z ==0, the screen position is not computable
///---------------------------------------------------------------------
GLTvec3D GL3DCamera::GetScreenPosition(const GLTvec3D& worldSpace/*,int screenWidth,int sceenHeight*/) const
{
	GLTvec3D camSpace;

	ComputeMatrix();
	m_matrix.Mul(worldSpace,camSpace);

	if( camSpace.z < GLT_INT_TO_FIXED(-1))
	{
// 		const unsigned int angle4096 = ::GLTDeg2FixedAngle(m_fov);
// 
// 		const GLfixed s = ::GLTSin(angle4096);
// 		const GLfixed c = ::GLTCos(angle4096);

		const GLfixed focalLength = GetFocalLength();
        
		const GLfixed d = ::GLTDiv(focalLength,camSpace.z);

		const GLTvec3D screenSpace( GLTMul(camSpace.x,d) , GLTMul(camSpace.y,d) , camSpace.z );

		const GLTvec3D rasterSpace(  m_viewportCenterX - screenSpace.x,
									screenSpace.y +   m_viewportCenterY ,
									screenSpace.z);

		return rasterSpace;
	}
	else
	{
		camSpace.z=0;
		return camSpace;
	}	
}

GLfixed GL3DCamera::GetFocalLength(/*int screenWidth*/) const
{
	const unsigned int angle4096 = ::GLTDeg2FixedAngle(m_fov);
	
	const GLfixed s = ::GLTSin(angle4096);
	const GLfixed c = ::GLTCos(angle4096);
	
	return ::GLTDiv((c * /*(Game::Instance())->viewport_h*/SCREEN_HEIGHT) / 2,s);
}

// transform the screenPosition (screenPos.x, screenPos.y) to world space (assume the point is located in the focal plane)
// return value:
//  return TRUE if succesfully transformed.
bool GL3DCamera::ScreenPositionToWorld(const GLTvec3D &screenPos, GLTvec3D &worldPos/*, int screenWidth, int screenHeight*/)
{
	const GLfixed focalLength = GetFocalLength();
	const GLTvec3D camSpace(screenPos.x - m_viewportCenterX,m_viewportCenterY - screenPos.y, -focalLength);
	ComputeMatrix();
	m_mtx_inv_notrans.Mul(camSpace, worldPos);
	worldPos = worldPos + m_pos;


	return true;
}
///---------------------------------------------------------------------
///
///---------------------------------------------------------------------
const GLTmatrix& GL3DCamera::GetInvMatrixNoTrans() const
{
	ComputeMatrix();
	return m_mtx_inv_notrans;
}

/*
///---------------------------------------------------------------------
///
///---------------------------------------------------------------------
const GLTmatrix& GL3DCamera::GetInvMatrix() const 
{
	ComputeMatrix();
	return m_mtx_inv;
}
*/

//GLTcoord GL3DCamera::GetViewCoord(void) const
//{
//	Location location;
//	location.m_pos = m_pos;
//	location.m_rot = m_rot;
//	GLTcoord coord;
//	coord.o = m_pos;
//	coord.u = location.GetLeft();
//	coord.w = location.GetFront();
//	return coord;
//}











void GL3DCamera::SetNearFarPlane(GLfixed zNear, GLfixed zFar)
{
	m_zNear = zNear;
	m_zFar = zFar;
	m_dirty = true;
}

void  GL3DCamera::ComputeFrustrumBTLR(GLfixed w, GLfixed h, GLfixed n, GLfixed f)  const
{
	GLT_ASSERT( w == h );	// SOFTWARE LIMITATION
	
	// we use a 1:1 aspect ratio, so use the width * screen_ratio instead of the height !
#if (PLATFORM != PLATFORM_MAC_OS_X)
	GLfixed		ratio = GLTDiv(m_viewportW, m_viewportH);
#else
	GLfixed		ratio = GLTDiv(m_viewportH, m_viewportW);
#endif
//	h =	GLTMul( w, ratio );
	w = GLTMul( h, ratio );
	
	GLTvec3D	v( GLT_FIXED_ONE, w, 0 );
	v.Normalize();
	
	m_frustrumBTLR[GLT_FRUSTRUM_LEFT][0] = -v.x;
	m_frustrumBTLR[GLT_FRUSTRUM_LEFT][1] = 0;
	m_frustrumBTLR[GLT_FRUSTRUM_LEFT][2] = v.y;
	
	m_frustrumBTLR[GLT_FRUSTRUM_RIGHT][0] = v.x;
	m_frustrumBTLR[GLT_FRUSTRUM_RIGHT][1] = 0;
	m_frustrumBTLR[GLT_FRUSTRUM_RIGHT][2] = v.y;
	
	v.x = GLT_FIXED_ONE;
	v.y = h;
	v.Normalize();
	
	m_frustrumBTLR[GLT_FRUSTRUM_BOTTOM][0] = 0;
	m_frustrumBTLR[GLT_FRUSTRUM_BOTTOM][1] = -v.x;
	m_frustrumBTLR[GLT_FRUSTRUM_BOTTOM][2] = v.y;
	
	m_frustrumBTLR[GLT_FRUSTRUM_TOP][0] = 0;
	m_frustrumBTLR[GLT_FRUSTRUM_TOP][1] = v.x;
	m_frustrumBTLR[GLT_FRUSTRUM_TOP][2] = v.y;

}

void  GL3DCamera::InvertAspect(CGame* pGame, bool isLandScape)
{
	Init(pGame, isLandScape);
}
