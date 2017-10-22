//#23.00A sha2_256.h PAD

#include "central.h"

// immplémenation de sha2 256 bits
// voir https://en.wikipedia.org/wiki/SHA-2


// Ecrit un entier non signé 32 bits, en big endian
void SetUI32BigEndian(byte *pAddr, UINT32 nVal)
{
	*((pAddr) + 3) = (UINT8) ((nVal)      );       
    *((pAddr) + 2) = (UINT8) ((nVal) >>  8);       
    *((pAddr) + 1) = (UINT8) ((nVal) >> 16);       
    *((pAddr) + 0) = (UINT8) ((nVal) >> 24);       
}
// Ecrit un entier non signé 64 bits, en big endian
void SetUI64BigEndian(byte *pAddr, UINT64 nVal)
{
	*((pAddr) + 7) = (UINT8) ((nVal)      );       
    *((pAddr) + 6) = (UINT8) ((nVal) >>  8);       
    *((pAddr) + 5) = (UINT8) ((nVal) >> 16);       
    *((pAddr) + 4) = (UINT8) ((nVal) >> 24);       
	*((pAddr) + 3) = (UINT8) ((nVal) >> 32);       
    *((pAddr) + 2) = (UINT8) ((nVal) >> 40);       
    *((pAddr) + 1) = (UINT8) ((nVal) >> 48);       
    *((pAddr) + 0) = (UINT8) ((nVal) >> 56);    
}

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

// valeur constante utitlisée par sha

// valueur initiale dans le hash avant injection des valeurs source
//(first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19):
static const uint32 hStartConst[8] = 
{   0x6a09e667,
	0xbb67ae85,
	0x3c6ef372,
	0xa54ff53a,
	0x510e527f,
	0x9b05688c,
	0x1f83d9ab,
	0x5be0cd19 };

//	(first 32 bits of the fractional parts of the cube roots of the first 64 primes 2..311) :
static const uint32 sha_const[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

// fonction de base :
void sha2_256_raw(uint512 *pChunk, int nNbChunk, OUT uint256 *pHash)
{
	XASSERT(nNbChunk > 0);
	memset(pHash, 0, 32);

	// init de la hash avec les valeurs constantes de <hStartConst>
	//(first 32 bits of the fractional parts of the square roots of the first 8 primes 2..19):
	uint32 h[8];
	memcpy(h, hStartConst, 32);

	// pour chaque bloc de 512 bits a hacher = 64 octets
	for (int nNumChunk = 0; nNumChunk < nNbChunk; nNumChunk++)
	{
		// donneée a hacher
		PBYTE pChunkCur = (byte *)pChunk + nNumChunk * 64; // 64 = 512/8
		// Init de 64 mots de 32 bits a partir de <pChunkCur>
		uint32 wSrc[64];
		// copie des 16 premiers mots
		for (int j = 0; j < 16; j++) {
			SetUI32BigEndian( (byte *)&wSrc[j], *(UINT32 *)(pChunkCur + j*4) );
		}
		// extension sur les 4_ mots restant
		#define xor ^
		#define and &
		for (int i = 16; i < 64; i++)
		{
			uint32 s0 = rightrotate(wSrc[i - 15],7) xor rightrotate(wSrc[i - 15], 18) xor rightshift(wSrc[i - 15],  3);
			uint32 s1 = rightrotate(wSrc[i - 2],17) xor rightrotate(wSrc[i - 2],  19) xor rightshift(wSrc[i - 2] , 10);
			wSrc[i] = wSrc[i - 16] + s0 + wSrc[i - 7] + s1;
		}
		
		// Init des var de travail hTemp avec h courant
		uint32 hTemp[8];
		for (int i = 0; i < 8; i++)
			hTemp[i] = h[i];



		// boucle principale de hachage
		// 64 rondes de MAJ de hTemp, avec des opération des décalage de bit, xor et addition
		for (int i = 0; i < 64; i++) 
		{
			// calcul de temp1 utilisé pour la MAJ de hTemp0 et hTemp4
			uint32 e     = hTemp[4];
			uint32 t1p1    = rightrotate(e, 6) xor rightrotate(e,11) xor rightrotate(e, 25);
			uint32 t1p2    = (e and  hTemp[5]) xor ((~e) and  hTemp[6]);
			uint32 temp1 = hTemp[7] + t1p1 + t1p2 + sha_const[i] + wSrc[i];

			// calcul de temp2 utilisé pour la MAJ de hTemp0 
			uint32 a	 = hTemp[0];
			uint32 t2p1  = rightrotate(a,2) xor rightrotate(a,13) xor rightrotate(a, 22);
			uint32 t2p2  = (a and hTemp[1]) xor (a and hTemp[2]) xor (hTemp[1] and hTemp[2]);
			uint32 temp2 = t2p1 + t2p2;

			hTemp[7] = hTemp[6];
			hTemp[6] = hTemp[5];
			hTemp[5] = hTemp[4];
			hTemp[4] = hTemp[3] + temp1;
			hTemp[3] = hTemp[2];
			hTemp[2] = hTemp[1];
			hTemp[1] = hTemp[0];
			hTemp[0] = temp1 + temp2;
		}
		//Add the compressed chunk to the current hash value:
		for (int i = 0; i < 8; i++)
			h[i] += hTemp[i];


	}

	// résultat 
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
	
	// taille en bits a mettre dans les 64 derniers bits au format big-endian
	UINT64 nTailleEnBit = nSizeData * 8;
	SetUI64BigEndian(pRaw + nSizeRaw - 8, nTailleEnBit);

	// rebond
	sha2_256_raw( (uint512 *) pRaw, nSizeRaw / 64, (uint256 *)pHash);


	free(pRaw);

}
