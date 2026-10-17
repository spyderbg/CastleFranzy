// Copyright (C) 2005 Gameloft. All rights reserved.
// GAMELOFT PROPRIETARY/CONFIDENTIAL.
//
// Author: Qiu Wei Min

#include "GLLiveStdafx.h"
#if PLATFORM == PLATFORM_KDDI
#include <AEEStdLib.h>
#include "AnimationManager.h"

const TriValue TAN[181] = {	0, 142, 285, 427, 571, 714, 858, 1001, 1145, 1289, 
	1431, 1576, 1721, 1866, 2010, 2156, 2301, 2447, 2593, 2741, 
	2888, 3035, 3183, 3332, 3481, 3630, 3781, 3932, 4084, 4236, 
	4389, 4543, 4697, 4852, 5008, 5164, 5323, 5480, 5641, 5801, 
	5962, 6124, 6288, 6453, 6619, 6786, 6953, 7123, 7294, 7466, 
	7639, 7813, 7990, 8167, 8347, 8527, 8711, 8894, 9081, 9268, 
	9458, 9650, 9843, 10040, 10236, 10436, 10639, 10842, 11051, 11259, 
	11472, 11685, 11902, 12122, 12345, 12571, 12799, 13031, 13266, 13505, 
	13746, 13991, 14240, 14494, 14752, 15012, 15278, 15546, 15820, 16098, 
	16384, 16672, 16965, 17263, 17568, 17879, 18196, 18517, 18846, 19182, 
	19524, 19873, 20230, 20596, 20969, 21351, 21741, 22141, 22549, 22968, 
	23397, 23838, 24289, 24752, 25228, 25716, 26219, 26735, 27266, 27813, 
	28377, 28957, 29556, 30174, 30813, 31472, 32155, 32859, 33592, 34349, 
	35135, 35949, 36798, 37679, 38597, 39554, 40550, 41592, 42680, 43820, 
	45013, 46266, 47582, 48965, 50423, 51961, 53588, 55310, 57137, 59077, 
	61145, 63352, 65711, 68242, 70965, 73903, 77080, 80528, 84287, 88399, 
	92916, 97905, 103443, 109626, 116577, 124447, 133436, 143799, 155882, 170152, 
	187269, 208178, 234301, 267875, 312624, 375254, 469175, 625678, 938637, 1877419, 0x7fffffff
};

#else
#include <stdlib.h>
#endif//PLATFORM == PLATFORM_KDDI

#include "FMath.h"

namespace gllive
{
	const unsigned short CFMath::m_sinTable[] =
	{
		   0,   6,  12,  18,  25,  31,  37,  43,  50,  56,  62,  69,  75,  81,  87,  94,
		 100, 106, 113, 119, 125, 131, 138, 144, 150, 157, 163, 169, 175, 182, 188, 194,
		 200, 207, 213, 219, 226, 232, 238, 244, 251, 257, 263, 269, 276, 282, 288, 295,
		 301, 307, 313, 320, 326, 332, 338, 345, 351, 357, 363, 370, 376, 382, 388, 395,
		 401, 407, 413, 420, 426, 432, 438, 445, 451, 457, 463, 470, 476, 482, 488, 495,
		 501, 507, 513, 520, 526, 532, 538, 545, 551, 557, 563, 569, 576, 582, 588, 594,
		 601, 607, 613, 619, 625, 632, 638, 644, 650, 656, 663, 669, 675, 681, 687, 694,
		 700, 706, 712, 718, 725, 731, 737, 743, 749, 755, 762, 768, 774, 780, 786, 792,
		 799, 805, 811, 817, 823, 829, 836, 842, 848, 854, 860, 866, 872, 879, 885, 891,
		 897, 903, 909, 915, 921, 928, 934, 940, 946, 952, 958, 964, 970, 976, 983, 989,
		 995,1001,1007,1013,1019,1025,1031,1037,1043,1050,1056,1062,1068,1074,1080,1086,
		1092,1098,1104,1110,1116,1122,1128,1134,1140,1146,1152,1158,1164,1170,1176,1182,
		1189,1195,1201,1207,1213,1219,1225,1231,1237,1243,1248,1254,1260,1266,1272,1278,
		1284,1290,1296,1302,1308,1314,1320,1326,1332,1338,1344,1350,1356,1362,1368,1373,
		1379,1385,1391,1397,1403,1409,1415,1421,1427,1433,1438,1444,1450,1456,1462,1468,
		1474,1479,1485,1491,1497,1503,1509,1515,1520,1526,1532,1538,1544,1550,1555,1561,
		1567,1573,1579,1584,1590,1596,1602,1608,1613,1619,1625,1631,1636,1642,1648,1654,
		1659,1665,1671,1677,1682,1688,1694,1699,1705,1711,1717,1722,1728,1734,1739,1745,
		1751,1756,1762,1768,1773,1779,1785,1790,1796,1802,1807,1813,1819,1824,1830,1835,
		1841,1847,1852,1858,1864,1869,1875,1880,1886,1891,1897,1903,1908,1914,1919,1925,
		1930,1936,1941,1947,1952,1958,1964,1969,1975,1980,1986,1991,1997,2002,2007,2013,
		2018,2024,2029,2035,2040,2046,2051,2057,2062,2067,2073,2078,2084,2089,2094,2100,
		2105,2111,2116,2121,2127,2132,2138,2143,2148,2154,2159,2164,2170,2175,2180,2186,
		2191,2196,2201,2207,2212,2217,2223,2228,2233,2238,2244,2249,2254,2259,2265,2270,
		2275,2280,2286,2291,2296,2301,2306,2312,2317,2322,2327,2332,2337,2343,2348,2353,
		2358,2363,2368,2373,2379,2384,2389,2394,2399,2404,2409,2414,2419,2424,2429,2434,
		2439,2445,2450,2455,2460,2465,2470,2475,2480,2485,2490,2495,2500,2505,2510,2515,
		2519,2524,2529,2534,2539,2544,2549,2554,2559,2564,2569,2574,2578,2583,2588,2593,
		2598,2603,2608,2613,2617,2622,2627,2632,2637,2641,2646,2651,2656,2661,2665,2670,
		2675,2680,2684,2689,2694,2699,2703,2708,2713,2717,2722,2727,2732,2736,2741,2746,
		2750,2755,2760,2764,2769,2773,2778,2783,2787,2792,2796,2801,2806,2810,2815,2819,
		2824,2828,2833,2837,2842,2847,2851,2856,2860,2865,2869,2874,2878,2882,2887,2891,
		2896,2900,2905,2909,2914,2918,2922,2927,2931,2936,2940,2944,2949,2953,2957,2962,
		2966,2970,2975,2979,2983,2988,2992,2996,3000,3005,3009,3013,3018,3022,3026,3030,
		3034,3039,3043,3047,3051,3055,3060,3064,3068,3072,3076,3080,3085,3089,3093,3097,
		3101,3105,3109,3113,3117,3121,3126,3130,3134,3138,3142,3146,3150,3154,3158,3162,
		3166,3170,3174,3178,3182,3186,3190,3193,3197,3201,3205,3209,3213,3217,3221,3225,
		3229,3232,3236,3240,3244,3248,3252,3255,3259,3263,3267,3271,3274,3278,3282,3286,
		3289,3293,3297,3301,3304,3308,3312,3315,3319,3323,3326,3330,3334,3337,3341,3345,
		3348,3352,3356,3359,3363,3366,3370,3373,3377,3381,3384,3388,3391,3395,3398,3402,
		3405,3409,3412,3416,3419,3423,3426,3429,3433,3436,3440,3443,3447,3450,3453,3457,
		3460,3463,3467,3470,3473,3477,3480,3483,3487,3490,3493,3497,3500,3503,3506,3510,
		3513,3516,3519,3522,3526,3529,3532,3535,3538,3541,3545,3548,3551,3554,3557,3560,
		3563,3566,3570,3573,3576,3579,3582,3585,3588,3591,3594,3597,3600,3603,3606,3609,
		3612,3615,3618,3621,3624,3627,3629,3632,3635,3638,3641,3644,3647,3650,3652,3655,
		3658,3661,3664,3667,3669,3672,3675,3678,3680,3683,3686,3689,3691,3694,3697,3700,
		3702,3705,3708,3710,3713,3716,3718,3721,3723,3726,3729,3731,3734,3736,3739,3742,
		3744,3747,3749,3752,3754,3757,3759,3762,3764,3767,3769,3772,3774,3776,3779,3781,
		3784,3786,3789,3791,3793,3796,3798,3800,3803,3805,3807,3810,3812,3814,3816,3819,
		3821,3823,3826,3828,3830,3832,3834,3837,3839,3841,3843,3845,3848,3850,3852,3854,
		3856,3858,3860,3862,3864,3867,3869,3871,3873,3875,3877,3879,3881,3883,3885,3887,
		3889,3891,3893,3895,3897,3899,3900,3902,3904,3906,3908,3910,3912,3914,3915,3917,
		3919,3921,3923,3925,3926,3928,3930,3932,3933,3935,3937,3939,3940,3942,3944,3945,
		3947,3949,3950,3952,3954,3955,3957,3959,3960,3962,3963,3965,3967,3968,3970,3971,
		3973,3974,3976,3977,3979,3980,3982,3983,3985,3986,3988,3989,3990,3992,3993,3995,
		3996,3997,3999,4000,4001,4003,4004,4005,4007,4008,4009,4011,4012,4013,4014,4016,
		4017,4018,4019,4020,4022,4023,4024,4025,4026,4027,4029,4030,4031,4032,4033,4034,
		4035,4036,4037,4038,4039,4040,4041,4042,4043,4044,4045,4046,4047,4048,4049,4050,
		4051,4052,4053,4054,4055,4056,4057,4057,4058,4059,4060,4061,4062,4062,4063,4064,
		4065,4065,4066,4067,4068,4068,4069,4070,4071,4071,4072,4073,4073,4074,4075,4075,
		4076,4076,4077,4078,4078,4079,4079,4080,4080,4081,4081,4082,4082,4083,4083,4084,
		4084,4085,4085,4086,4086,4087,4087,4087,4088,4088,4089,4089,4089,4090,4090,4090,
		4091,4091,4091,4091,4092,4092,4092,4092,4093,4093,4093,4093,4094,4094,4094,4094,
		4094,4094,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,4095,
		4096
	};

	CFMath::CFMath()
	{
		unsigned int i;

		m_recipTable[0] = 0xffffffff;
		for(i = 1; i <= F12_ONE; i++)
			m_recipTable[i] = (0xffffffffu) / i;
	}

	CFMath::~CFMath()
	{
	}

	F12 CFMath::Sin(F12 angle)
	{
		int v = angle & F12_MASK;
		const int d90 = F12_ONE / 4;
		if(v <= d90)
		{
			return m_sinTable[v];
		}
		else if(v <= d90 * 2)
		{
			return m_sinTable[d90*2 - v];
		}
		else if(v <= d90 * 3)
		{
			return -m_sinTable[v - d90*2];
		}
		else
		{
			return -m_sinTable[d90*4 - v];
		}
	}

	#if PLATFORM == PLATFORM_KDDI


	int CFMath::searchInUpArray(TriValue value,  const long *array, int len)
	{
		int first = 0;
		int last = len-1;
		int p = (first+last)/2;
		if(value>=array[first]&&value<=array[last])
		{
			while(last-first>1)
			{
				if(value>array[p])
				{
					first = p;
					p = (first+last)/2;
					continue;
				}
				if(value < array[p])
				{
					last = p;
					p = (first+last)/2;
					continue;
				}
				if(value == array[p])
				{
					first = p;
					break;
				}
			}
			if(value-array[first]>=array[last]-value)
				p = last;
			else
				p = first;
		}
		else
			p = -1;
		return p;
	}

	AngDegree CFMath::atan(TriValue value)
	{
		AngDegree ang;
		int p = searchInUpArray(Math::myabs(value),TAN,181);
		ang = 5*p;
		if(p==179 && Math::myabs(value)>TAN[179]*100)
		{
			ang+=5;
		}
		if(value<0)
			ang = -ang;
		return ang;
	}

	#define DEGREE_EXT			10
	#define TRI_EXT				14 //2^14=16384

	AngDegree CFMath::Atan2(PhType y, PhType x)
	{
		TriValue value;
		if(x==0)
		{
			if(y>0)
				return 90*DEGREE_EXT;
			if(y<0)
				return -90*DEGREE_EXT;
			if(y==0)
				return 0;
		}
		value = Math::myabs((TriValue)(y<<TRI_EXT)/x);
		AngDegree ang=atan(value);
		if(x<0&&y>=0)
			ang = 1800-ang;
		if(x<0&&y<0)
			//ang += 1800;
			ang -= 1800;
		if(x>0&&y<0)
			//ang = 3600-ang;
			ang = -ang;
		return ang;
	}


	AngDegree CFMath::Asin(TriValue value)
	{ 
		AngDegree ang;
		int p = searchInUpArray(abs(value),(const long *)m_sinTable,1024);
		if(p<0)
			return 0xffffffff;

		ang = (900*p)>>7;
		if(value<0)
			ang = -ang;
		return ang;

		/*AngDegree ang;
		int p = searchInUpArray(abs(value),SIN,181);
		if(p<0)
		return 0xffffffff;
		ang = 5*p;
		if(value<0)
		ang = -ang;
		return ang;*/
	}


	#endif//PLATFORM == PLATFORM_KDDI

	void CFMath::Rotate(Point2D &dst, const Point2D &src, const Point2D &pivot, F12 angle)
	{
		Point2D tmp;
		tmp.x = src.x - pivot.x;
		tmp.y = src.y - pivot.y;

		F12 valcos = Cos(angle);
		F12 valsin = Sin(angle);
		dst.x = pivot.x + ((tmp.x * valcos - tmp.y * valsin) >> F12_SHIFT);
		dst.y = pivot.y + ((tmp.x * valsin + tmp.y * valcos) >> F12_SHIFT);
	}

	void CFMath::FloorDivMod(int Numerator, int Denominator, int &Floor, int &Mod)
	{
		ASSERT(Denominator > 0);
		if(Numerator >= 0) {
			Floor = Numerator / Denominator;
			Mod = Numerator % Denominator;
		} else {
			Floor = -((-Numerator) / Denominator);
			Mod = (-Numerator) % Denominator;
			if(Mod) {
				// there is a remainder
				Floor--; Mod = Denominator - Mod;
			}
		}
	}

	int CFMath::FastDiv(int Numerator, int Denominator)
	{
		ASSERT(Denominator > 0);
		ASSERT(Denominator < sizeof(m_recipTable) / sizeof(m_recipTable[0]));
		if(Numerator >= 0)
		{
			int q = (int)(((u64)Numerator * m_recipTable[Denominator]) >> 32);
			int r = Numerator - q * Denominator;
			if(r >= Denominator)
				q++;
			return q;
		}
		else
		{
			int q = (int)(((u64)(-Numerator) * m_recipTable[Denominator]) >> 32);
			int r = Numerator - q * Denominator;
			if(r >= Denominator)
				q++;
			return -q;
		}
	}

	void CFMath::SetRandomSeed(u64 seed)
	{
		m_randomSeed = ((((seed >> 32) ^ 0x5ul) & 0xfffful) << 32) | ((seed ^ 0xdeece66dul) & 0xfffffffful);
	// 	m_randomSeed = (seed ^ 0x5deece66dul) & 0xfffffffffffful;
	}

	unsigned int CFMath::NextRandom(int bits)
	{
		m_randomSeed = m_randomSeed * 0xdeece66dul + ((m_randomSeed * 0x5ul) << 32) + 11ul;
		m_randomSeed = (m_randomSeed & 0xfffffffful) | (((m_randomSeed >> 32) & 0xfffful) << 32);
	// 	m_randomSeed = m_randomSeed * 0x5deece66dul + 11ul & 0xfffffffffffful;
		return (unsigned int)(m_randomSeed >> (48 - bits));
	}


	int Math::Random(int nMax)
	{
	#if PLATFORM == PLATFORM_KDDI
		int nRet = rand_int();
	#else
		int nRet = rand();
	#endif
		nRet %= (nMax+1);
		//TRACE("Random:%d\n",nRet);
		return nRet;
	}

	int Math::Random(int nMin,int nMax)
	{
		if (nMax == nMin)
			return nMin;
		else if (nMax < nMin)
		{
			int temp = nMin;
			nMin = nMax;
			nMax = temp;
		}
		int r = Random(nMax - nMin);
		return r + nMin;
	}

	#if PLATFORM == PLATFORM_KDDI

	int Math::_rand(int n)
	{
		if (n <= 1)
			return 0;

		int random;

		unsigned char randbuffer[3] = {0};
		GETRAND(randbuffer, sizeof(randbuffer));
		random = randbuffer[0]*0x010000+randbuffer[1]*0x0100+randbuffer[2];

		return (random%n);
	}

	int Math::_rand(int min,int max)
	{	
		if(min > max)
		{
			int n = max;
			max = min;
			min = n;
		}

		return (_rand(max - min + 1) + min);
	}

	int Math::rand_int()
	{
		return _rand( 0, 0x7FFFFFFE );
	}

	int Math::next_int()
	{
		return _rand( 0, 0x7FFFFFFE );
	}

	int Math::next_int(int value)
	{
		return _rand( 0, value);
	}

	#define USE_SYSTEM_SQRT_FUNC	1
	double Math::mysqrt(double value)
	{
	#if USE_SYSTEM_SQRT_FUNC
		return FSQRT(value);
	#else
		double a, b;

		if ( value == 0 )
			return 0;

		a = value/2;
		do
		{
			if(a==0)
			{
				a = 1;
				break;
			}
			b = value/a;
			b = (b - a)/2;
			a = a + b;
		} while ( ( b > 1 ) || ( b < -1 ) );
		return a;
	#endif
	}

	void Math::Qsort(void* s, const int cnt, const int s_size, s32 _PtFuncCompare(const void*, const void*))
	{
		_PlayerDistanceSort S[PLAYER_NUM_ONETEAM*2+1], *pS = (_PlayerDistanceSort*)s, T;
		T.nIndex = -1; T.nDistance = -1;
		memcpy(&S[1],pS,sizeof(_PlayerDistanceSort)*(PLAYER_NUM_ONETEAM*2));
		S[0] = T;
		_PlayerDistanceSort* R = S;
		int n = cnt;
		int i,j;
		for(i = 2; i <= n; i++) //依次插入R[2]，…，R[n]
		{
			if(R[i].nDistance < R[i-1].nDistance) //若R[i].key大于等于有序区中所有的keys，则R[i] //_PtFuncCompare
			{
				//应在原有位置上
				R[0] = R[i];
				j = i-1; //watcher是哨兵，且是R[i]的副本
				do { //从右向左在有序区R[1..i-1]中查找R[i]的插入位置
					R[j+1] = R[j]; //将关键字大于R[i].key的记录后移
					j--;
				} while(R[0].nDistance < R[j].nDistance); //当R[i].key≥R[j].key时终止
				R[j+1] = R[0]; //R[i]插入到正确的位置上
			}
		}
		memcpy(pS, &S[1],sizeof(_PlayerDistanceSort)*(PLAYER_NUM_ONETEAM*2));
	}

	#endif//PLATFORM == PLATFORM_KDDI
}
