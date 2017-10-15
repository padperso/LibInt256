//#23.00A sha2_256.h PAD

#include "central.h"

// immplémenation de sha2 256 bits
// voir https://en.wikipedia.org/wiki/SHA-2

struct STint512
{
	byte m_bytes[512/8];
};
struct STint256
{
	byte m_bytes[32]; // 256/8 = 32
};

typedef STint512 uint512;
typedef STint256 uint256;
typedef unsigned int  uint32;

#define SHA2_SHFR(x, n)    (x >> n)
#define SHA2_ROTR(x, n)   ((x >> n) | (x << ((sizeof(x) << 3) - n)))
#define SHA2_ROTL(x, n)   ((x << n) | (x >> ((sizeof(x) << 3) - n)))

uint32 rightrotate(uint32 v, int shift) {
	return SHA2_ROTR(v, shift);
    uint32 s =  shift>=0? shift%32 : -((-shift)%32);
    return (v>>s) | (v<<(32-s));
}
uint32 rightshift(uint32 v, int shift) {
	return (v >> shift);
}

#define SHA2_PACK32(str, x)                   \
{                                             \
    *(x) =   ((uint32) *((str) + 3)      )    \
           | ((uint32) *((str) + 2) <<  8)    \
           | ((uint32) *((str) + 1) << 16)    \
           | ((uint32) *((str) + 0) << 24);   \
}

// fonction de base :
void sha2_256_raw(uint512 *pChunk, int nNbChunk, OUT uint256 *pHash)
{
	XASSERT(nNbChunk > 0);
	memset(pHash, 0, 32);

	//Initialize hash values:
	//(first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19):
	uint32 h[8] = { 0x6a09e667,
				    0xbb67ae85,
					0x3c6ef372,
					0xa54ff53a,
					0x510e527f,
					0x9b05688c,
					0x1f83d9ab,
					0x5be0cd19 };

	//	(first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311) :
	uint32 k[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};


	for (int nNumChunk = 0; nNumChunk < nNbChunk; nNumChunk++)
	{
		PBYTE pChunkCur = (byte *)pChunk + nNumChunk * 512;
		//    create a 64-entry message schedule array w[0..63] of 32-bit words
		//(The initial values in w[0..63] don't matter, so many implementations zero them here)
		uint32 w[64] = {};//0
		//copy chunk into first 16 words w[0..15] of the message schedule array
		//memcpy(w, pChunkCur, 16 * 4);
		for (int j = 0; j < 16; j++) {
			SHA2_PACK32(&pChunkCur[j*4], &w[j]);
		}

		//Extend the first 16 words into the remaining 48 words w[16..63] of the message schedule array:
		#define xor ^
		//#define rightshift >>
		#define and &
		for (int i = 16; i < 64; i++)
		{
			uint32 s0 = rightrotate(w[i - 15],7) xor rightrotate(w[i - 15], 18) xor rightshift(w[i - 15],  3);
			uint32 s1 = rightrotate(w[i - 2],17) xor rightrotate(w[i - 2],  19) xor rightshift(w[i - 2] , 10);
			w[i] = w[i - 16] + s0 + w[i - 7] + s1;
		}
		
		//  Initialize working variables to current hash value:
		uint32 a  = h[0];
		uint32 b  = h[1];
		uint32 c  = h[2];
		uint32 d  = h[3];
		uint32 e  = h[4];
		uint32 f  = h[5];
		uint32 g  = h[6];
		uint32 h_  = h[7];

		// boucle principale de hachage
		for (int i = 0; i < 64; i++) // 64 rondes
		{
			uint32 S1 = rightrotate(e, 6) xor rightrotate(e,11) xor rightrotate(e, 25);
			uint32 ch  = (e and f) xor ((~e) and g);
			uint32 temp1 = h_ + S1 + ch + k[i] + w[i];
			uint32 S0 = rightrotate(a,2) xor rightrotate(a,13) xor rightrotate(a, 22);
			uint32 maj = (a and b) xor (a and c) xor (b and c);
			uint32 temp2 = S0 + maj;

			h_ = g;
			g = f;
			f = e;
			e = d + temp1;
			d = c;
			c = b;
			b = a;
			a = temp1 + temp2;
		}
		//Add the compressed chunk to the current hash value:
		h[0] +=  a;
		h[1] +=  b;
		h[2] +=  c;
		h[3] +=  d;
		h[4] +=  e;
		h[5] +=  f;
		h[6] +=  g;
		h[7] +=  h_;

	}

	// résultat en big endian
	memcpy(pHash, h, 32);

}

// point d'entrée de sha2 , 256 bits
// <pHash> doit faire 256 bits = 32 octets
void sha2_256(byte *pData, int nSizeData, OUT byte *pHash)
{
	// calcul de la taille du buffer de donner brut
	// +1 : bit a 1
	// +8 : 64 bits pour la taille;
	int nSizeRaw = ((nSizeData + 1 + 8));
	// doit être un multiple de 512 bits = 64 octest
	if ((nSizeRaw % 64) != 0)
		nSizeRaw = nSizeRaw + 64 - (nSizeRaw % 64);
	byte *pRaw = (byte *)malloc(nSizeRaw);
	// init buffer
	memcpy(pRaw, pData, nSizeData);
	// bourrage :
	// bit a 1
	pRaw[nSizeData] = 0x80;
	// bits à 0
	int nNbByte0 = nSizeRaw - 8 - 1 - nSizeData;
	if (nNbByte0 > 0)
		memset(pRaw+nSizeData+1, 0, nNbByte0);
	// taille en i8
	*(__int64 *)(pRaw + nSizeRaw - 8) = (__int64)nSizeData;

	// rebond
	sha2_256_raw( (uint512 *) pRaw, nSizeRaw / 64, (uint256 *)pHash);


	free(pRaw);

}
