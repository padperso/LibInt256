// Wl2cpp.cpp�: point d'entr�e pour le WL
//

#include "central.h"

#include "BigInt.h"
#include "sha2_256.h"
#include "EllipticCurve.h"
#include "ECDSA.h"

typedef CBigInt *PCBigInt;
#define EXPORT  extern "C" __declspec(dllexport) 

// cr�e un entier "big" = alloue
EXPORT PCBigInt BI_pNew(void)
{
	return new CBigInt(); // 0
}
// lib�re un entier "big"
EXPORT void BI_pDelete(PCBigInt p)
{
	delete p;
}

// -- Affectation --

// affecte un entier big avec unee chaine base 10 ou hexa (selon le pr�fixe)
EXPORT void BI_FromStr(PCBigInt p, PCXSTR pszVal)
{
	p->FromStrBasePrefix(pszVal);
}
// affecte un entier big a partir de sa repr�sentation binaire sur x byte
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
EXPORT void BI_FromI4(PCBigInt p, UINT32 n)
{
	*p = n
}




// r�cup de la valeur au format chaine (base 10)
EXPORT char* BI_ToStr(PCBigInt p)
{
	static char szBuf[1024];
	p->ToStrBase10(szBuf, 1024);
	return szBuf;
}
// r�cup de la valeur au format chaine hexad�cimale (base 16)
EXPORT char* BI_ToHexa(PCBigInt p)
{
	static char szBuf[1024];
	p->ToStrBase16(szBuf, 1024);

	char *pRes = szBuf;
	//supprimer les 0 inutiles au d�but
//	while (*pRes == '0' && pRes[1] != 0)
//		pRes++;
	return pRes;
}

// x ext-il n�gatif (strict, ie <0)?
EXPORT BOOL BI_IsNegative(PCBigInt p)
{
	return p->bNegative();
}
// x ext-il posifif (strict, ie >0) ?
EXPORT BOOL BI_IsPositive(PCBigInt p)
{
	return !(p->bNegative() && p->bIsZero());
}
// x ext-il �gal � 0 ?
EXPORT BOOL BI_IsZero(PCBigInt p)
{
	return p->bIsZero();
}

// -- ECDSA

// r�cup de la cl� publique X,Y a partir de la cl� priv�e K
EXPORT void ECDA_PrivateToPublicKey(PCBigInt pK, OUT PCBigInt pX, OUT PCBigInt pY)
{
	CBigPoint2D PublicKey;
	PublicKey = ECDSA_GetPublicKey(*pK);
	*pX = PublicKey.m_clX;
	*pY = PublicKey.m_clY;
}