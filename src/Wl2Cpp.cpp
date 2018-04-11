// Wl2cpp.cpp : point d'entrée pour le WL
//

#include "central.h"

#include "BigInt.h"
#include "sha2_256.h"
#include "EllipticCurve.h"
#include "ECDSA.h"

typedef CBigInt *PCBigInt;
#define EXPORT  extern "C" __declspec(dllexport) 

// crée un entier "big" = alloue
EXPORT PCBigInt BI_pNew(void)
{
	return new CBigInt(); // 0
}
// libère un entier "big"
EXPORT void BI_pDelete(PCBigInt p)
{
	delete p;
}

// -- Affectation --

// affecte un entier big avec unee chaine base 10 ou hexa (selon le préfixe)
EXPORT void BI_FromStr(PCBigInt p, PCXSTR pszVal)
{
	p->FromStrBasePrefix(pszVal);
}
// affecte un entier big a partir de sa représentation binaire sur x byte
EXPORT void BI_FromRawBytes(PCBigInt p, PBYTE pBBuffer, int nLenByte)
{
	p->FromRawBytes(pBBuffer, nLenByte);
}
// affectation par copie
EXPORT void BI_FromBigInt(PCBigInt p, PCBigInt psrc)
{
	p->CopieFrom(*psrc);
}
// affectation par un entier sur 4
EXPORT void BI_FromI4(PCBigInt p, int n)
{ 


	// appel de bool operator == (INT64 n) const;
	*p = (INT64)n;
}
// affectation par un entier sans signe sur 4
EXPORT void BI_FromUI4(PCBigInt p, UINT32 n)
{
	*p = n;
}


// -- Lecture

// récup de la valeur au format chaine (base 10)
EXPORT char* BI_ToStr(PCBigInt p)
{
	static char szBuf[1024];
	p->ToStrBase10(szBuf, 1024);
	return szBuf;
}
// récup de la valeur au format chaine hexadécimale (base 16)
EXPORT char* BI_ToHexa(PCBigInt p)
{
	static char szBuf[1024];
	p->ToStrBase16(szBuf, 1024);

	char *pRes = szBuf;
	//supprimer les 0 inutiles au début
//	while (*pRes == '0' && pRes[1] != 0)
//		pRes++;
	return pRes;
}
// récup des 32 bits de poids faible
EXPORT UINT32 BI_GetLowI4(PCBigInt p)
{
	return p->nGetLow32Bit();
}

// -- Test

// x ext-il négatif (strict, ie <0)?
EXPORT BOOL BI_IsNegative(PCBigInt p)
{
	return p->bNegative();
}
// x ext-il posifif (strict, ie >0) ?
EXPORT BOOL BI_IsPositive(PCBigInt p)
{
	return !(p->bNegative() || p->bIsZero());
}
// x ext-il égal à 0 ?
EXPORT BOOL BI_IsZero(PCBigInt p)
{
	return p->bIsZero();
}

// -- Opération
 
// ajout d'un entier non signé
EXPORT void BI_AddUI4(PCBigInt p, UINT32 n)
{
	// aggrandir si overflow
	p->AddUI4(n, CBigInt::eDefaut);
}
// multiplication par un entier non signé
EXPORT void BI_MultUI4(PCBigInt p, UINT32 n)
{
	p->MultUI32(n);
}
// division entière par <pDiviseur> avec reste (modulo)
EXPORT void BI_DivModulo(PCBigInt p, PCBigInt pDiviseur, OUT PCBigInt pQuotient, OUT  PCBigInt  pReste )
{
	p->Divide(*pDiviseur, pQuotient, pReste);
}
// x est-il impair ?
EXPORT BOOL BI_IsOdd(PCBigInt p)
{
	return p->bIsOdd();
}


// -- ECDSA
 
// récup de la clé publique X,Y a partir de la clé privée K
EXPORT void ECDA_PrivateToPublicKey(PCBigInt pK, OUT PCBigInt pX, OUT PCBigInt pY)
{
	CBigPoint2D PublicKey;
	PublicKey = ECDSA_GetPublicKey(*pK);
	*pX = PublicKey.m_clX;
	*pY = PublicKey.m_clY;
}

// signer Hash avec la clé privée K
EXPORT void ECDA_SignWithPrivateKey(  PCBigInt pK, PCBigInt pHash,
									  OUT PCBigInt pSignatureX, OUT PCBigInt pSignatureY )
{
	// pour que le bit de poids fort soit a 1
	CBigInt PrivateK = *pK;
/*	CBigInt N;
	N.FromStrBasePrefix("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");
	if (pHash->nGetBit(255) == 1)
		PrivateK = N - *pK;
	else
		PrivateK = *pK;
//PrivateK = *pK;
*/

	CBigPoint2D clSignature;
	ECDSA_Sign(*pHash, PrivateK, &clSignature);
	CBigPoint2D PKey;

	//@TEST
	CBigPoint2D PublicKey;
	PublicKey = ECDSA_GetPublicKey(*pK);
	if (!ECDSA_bCheckSign(*pHash, clSignature, PublicKey))
	{
		return;
	}



	// renvoyie x,y
	*pSignatureX = clSignature.m_clX;
	*pSignatureY = clSignature.m_clY;
}

// verifier qu'une signature est valide
EXPORT bool ECDSA_CheckSignature(PCBigInt pHash, 
								PCBigInt pPubKeyX,   PCBigInt pPubKeyY,
								PCBigInt pSignatureX,PCBigInt pSignatureY)
{
	// init clé publique
	CBigPoint2D PublicKey;
	PublicKey.m_clX = *pPubKeyX;
	PublicKey.m_clY = *pPubKeyY;
	// init signature
	CBigPoint2D clSignature;
	clSignature.m_clX = *pSignatureX;
	clSignature.m_clY = *pSignatureY;

	// vérif signature
	if (!ECDSA_bCheckSign(*pHash, clSignature, PublicKey))
	{
		// invalide
		return false;
	}
	// signatur OK
	return true;
}

// https://en.wikipedia.org/wiki/MurmurHash
EXPORT UINT32  murmur3_32(PBYTE key, size_t len, UINT32 seed) 
{
  UINT32 h = seed;
  // block de 4 octets
  if (len > 3) 
  {
    const UINT32* key_x4 = (const UINT32*) key;
    size_t i = len >> 2;
    do {
      UINT32 k = *key_x4++;
      k *= 0xcc9e2d51;
      k = (k << 15) | (k >> 17);
      k *= 0x1b873593;
      h ^= k;
      h = (h << 13) | (h >> 19);
      h = (h * 5) + 0xe6546b64;
    } while (--i);
    key = (PBYTE) key_x4;
  }
  // reste
  if (len & 3) 
  {
    size_t i = len & 3;
    UINT32 k = 0;
    key = &key[i - 1];
    do {
      k <<= 8;
      k |= *key--;
    } while (--i);
    k *= 0xcc9e2d51;
    k = (k << 15) | (k >> 17);
    k *= 0x1b873593;
    h ^= k;
  }
  h ^= len;
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;
  return h;
}
