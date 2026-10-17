#ifndef _FONT_TABLE_H
#define _FONT_TABLE_H

#define UNDEF_CHAR 160
#define MAP_CHAR_EXT_START (127)

// maps an ASCII char to a sprite FModule
const unsigned char _map_char_ext[] =
{
	UNDEF_CHAR,		// 128 80 '€'
	UNDEF_CHAR,		// 129 81 '?
	UNDEF_CHAR,		// 130 82 '?
	UNDEF_CHAR,		// 131 83 '?
	UNDEF_CHAR,		// 132 84 '?
	UNDEF_CHAR,		// 133 85 '?
	UNDEF_CHAR,		// 134 86 '?
	UNDEF_CHAR,		// 135 87 '?
	UNDEF_CHAR,		// 136 88 '?
	UNDEF_CHAR,		// 137 89 '?
	UNDEF_CHAR,		// 138 8A '?
	UNDEF_CHAR,		// 139 8B '?
	UNDEF_CHAR,		// 140 8C '?
	UNDEF_CHAR,		// 141 8D '?
	UNDEF_CHAR,		// 142 8E '?
	UNDEF_CHAR,		// 143 8F '?
	UNDEF_CHAR,		// 144 90 '?
	UNDEF_CHAR,		// 145 91 '?
	UNDEF_CHAR,		// 146 92 '?
	UNDEF_CHAR,		// 147 93 '?
	UNDEF_CHAR,		// 148 94 '?
	UNDEF_CHAR,		// 149 95 '?
	UNDEF_CHAR,		// 150 96 '?	//DOWN ARROW
	UNDEF_CHAR,		// 151 97 '?	//UP ARROW
	UNDEF_CHAR,		// 152 98 '?
	UNDEF_CHAR,		// 153 99 '?
	UNDEF_CHAR,		// 154 9A '?
	UNDEF_CHAR,		// 155 9B '?
	UNDEF_CHAR,		// 156 9C '?
	UNDEF_CHAR,		// 157 9D '?
	UNDEF_CHAR,		// 158 9E '?
	UNDEF_CHAR,		// 159 9F '?

	UNDEF_CHAR,		// 160 A0 '? No-Break Space
	212,		    // 161 A1 
	UNDEF_CHAR,		// 162 A2 
	214,		    // 163 A3
	213,		    // 164 A4
	215,		    // 165 A5
	UNDEF_CHAR,		// 166 A6
	UNDEF_CHAR,     // 167 A7 
	UNDEF_CHAR,		// 168 A8
	UNDEF_CHAR,	    // 169 A9 
	UNDEF_CHAR,		// 170 AA
	UNDEF_CHAR,		// 171 AB
	UNDEF_CHAR,		// 172 AC
	UNDEF_CHAR,		// 173 AD
	UNDEF_CHAR,		// 174 AE
	UNDEF_CHAR,		// 175 AF
	218,		    // 176 B0
	UNDEF_CHAR,		// 177 B1
	UNDEF_CHAR,		// 178 B2
	UNDEF_CHAR,		// 179 B3
	UNDEF_CHAR,		// 180 B4
	UNDEF_CHAR,		// 181 B5
	UNDEF_CHAR,		// 182 B6
	UNDEF_CHAR,		// 183 B7
	UNDEF_CHAR,		// 184 B8
	UNDEF_CHAR,		// 185 B9
	UNDEF_CHAR,		// 186 BA
	UNDEF_CHAR,		// 187 BB
	UNDEF_CHAR,		// 188 BC
	UNDEF_CHAR,		// 189 BD
	UNDEF_CHAR,		// 190 BE

    209,    // 191 BE

	161,	// 192 C0 
	162,	// 193 C1 
	163,	// 194 C2 
	164,	// 195 C3 
	165,	// 196 C4 

	UNDEF_CHAR,	// 197
	UNDEF_CHAR,	// 198

	210,	// 199 C7 
	166,	// 200 C8 
	167,	// 201 C9 
	168,	// 202 CA 
	169,	// 203 CB 
	170,	// 204 CC 
	171,	// 205 CD 
	172,	// 206 CE 
	173,	// 207 CF 

	UNDEF_CHAR,	// 208
	183,	// 209 D1 

	174,	// 210 D2 
	175,	// 211 D3 
	176,	// 212 D4 
	177,	// 213 D5 
	178,	// 214 D6 

	UNDEF_CHAR,	// 215
	UNDEF_CHAR,	// 216

	179,	// 217 D9 
	180,	// 218 DA 
	181,	// 219 DB 
	182,	// 220 DC 

	UNDEF_CHAR,	// 221
	UNDEF_CHAR,	// 222
	207,	// 223 DF 

	184,	// 224 E0 
	185,	// 225 E1 
	186,	// 226 E2 
	187,	// 227 E3 
	188,	// 228 E4 

	UNDEF_CHAR,	// 229
	UNDEF_CHAR,	// 230

	211,	// 231 E7 

	189,	// 232 E8 
	190,	// 233 E9 
	191,	// 234 EA 
	192,	// 235 EB 
	193,	// 236 EC 
	194,	// 237 ED 
	195,	// 238 EE 
	196,	// 239 EF 

	UNDEF_CHAR,	// 240
	206,	    // 241 F1 

	197,	// 242 F2 
	198,	// 243 F3 
	199,	// 244 F4 
	200,	// 245 F5 
	201,	// 246 F6 

	UNDEF_CHAR,	// 247
	UNDEF_CHAR,	// 248

	202,	// 249 F9
	203,	// 250 FA
	204,	// 251 FB
	205,	// 252 FC

	57,	    // 253 FD 
	48,	    // 254 FE 
	34	    // 255 FF 
};
#endif