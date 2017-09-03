// 23.00A BigInt.cpp PAD

#include "central.h"
#include "BigInt.h"
#include <algorithm>    // std::max

#define ALLBIT1_64 0xFFFFFFFFFFFFFFFFL // 64 bits a 1.
#define ALLBIT1_32 0xFFFFFFFF		   // 32 bits a 1.
#define BITHIGH_64 0x8000000000000000L // bit de poids fort en 64 bits


// constructeur. init a 0
CBigInt::CBigInt(void)
{
	// alloc pour mettre au moins un I8
	_Init( 8 );
}
// constructeur copie
CBigInt::CBigInt(const CBigInt &clSrc)
{
	// alloc pour mettre au moins un I8
	_Init(8);
	CopieFrom(clSrc);
}
// constructeur a partir d'un INT64
CBigInt::CBigInt(INT64 n)
{
	// alloc pour mettre au moins un I8
	_Init(8);
	*this = n; // opérateur d'affectation
}
// destructeur
CBigInt::~CBigInt()
{
	if (m_TabVal != NULL)
		free(m_TabVal);
}

//C++11 move constructor
CBigInt::CBigInt(CBigInt&& other)
{
	m_nSizeInByte = other.m_nSizeInByte;
	m_TabVal	  = other.m_TabVal;
	other.m_TabVal = NULL;
	other.m_nSizeInByte = 0;
}


// init a 0
void CBigInt::_Init(int nSizeInByte)
{
	m_nSizeInByte = nSizeInByte;
	m_TabVal	  = (UINT64 *)malloc(nSizeInByte);
	ZeroMemory(m_TabVal, nSizeInByte);
}
// realloc 
void CBigInt::_Resize(int nSizeInByte, EResieMode e)
{
	XASSERT(nSizeInByte >= 1);
	if (nSizeInByte % 8 != 0)
	{
		//force a un multiple de 8
		nSizeInByte += 8 - (nSizeInByte % 8);
	}
	XASSERT(nSizeInByte >= 8);
	if (nSizeInByte == m_nSizeInByte)
		return;// rien a faire
	BOOL bNegatif = bNegative();


	// MJA taille du tableau
	m_TabVal	  = (UINT64 *)realloc(m_TabVal, nSizeInByte);


	if (e != eSansRemplissage)
	{
		// mets a 0 les nouveaux bits de poids fort
		BYTE cRemplissage = 0x00;
		if (bNegatif && e == eAvecSigne)
			// on bourre avec OxFF
			cRemplissage = 0xF;
		if (nSizeInByte > m_nSizeInByte)
			memset((char*)m_TabVal + m_nSizeInByte,
				cRemplissage,
				nSizeInByte - m_nSizeInByte
			);
	}

	// apres , carm_nSizeInByte utilisé pour le remplissage  
	m_nSizeInByte = nSizeInByte;
}

// opérateur d'affectaction avec un eniter 64
// ex : CBigInt x; x = 7;
void CBigInt::operator = (INT64 n)
{
	XASSERT(m_nSizeInByte >= 8);
	// tronque a 8 octets. 
	//le signe est le bit de poids fort
	m_nSizeInByte = 8;
	m_TabVal[0]   = n;
}
// opérateur de comparaisons avec un entier 64
// ex : CBigInt x; x == 7;
bool CBigInt::operator == (INT64 n) const
{
	// si sur plus de 8 bits 
	if (m_nSizeInByte != 8)
		return false;

	return m_TabVal[0] == n;
}
// ex : CBigInt x; x != 7;
bool CBigInt::operator != (INT64 n) const
{
	// si sur plus de 8 bits 
	if (m_nSizeInByte != 8)
		return true; // différents

	return m_TabVal[0] != n;
}

// opérateur de comparaisons avec un autre BigInt
bool CBigInt::operator == (const CBigInt& other) const
{
	return nCompareU(other) == 0;
}
bool CBigInt::operator != (const CBigInt& other) const
{
	return nCompareU(other) != 0;
}
// opérateur de copie
//x=y;
const CBigInt & CBigInt::operator =(const CBigInt &clSrc)
{
	CopieFrom(clSrc);
	return *this; // pour a=b=c;
}
// copie
void CBigInt::CopieFrom(const CBigInt &clSrc)
{
	// si la source n'a pas la meme taille
	if (clSrc.nSizeInByte() != nSizeInByte())
	{
		// réalloc
		_Resize( clSrc.nSizeInByte(), eSansRemplissage );
	}
	XASSERT (clSrc.nSizeInByte() == nSizeInByte());

	//copie données src
	memcpy(m_TabVal, clSrc.m_TabVal, nSizeInByte());
}

// = 0 ?
BOOL CBigInt::bIsZero(void) const
{
	return m_nSizeInByte==8 && m_TabVal[0] == 0;
}

//indique si on est un nombre négatif
BOOL CBigInt::bNegative(void) const
{
	return (m_TabVal[nSizeInI8() - 1] & BITHIGH_64) != 0;
}
// addition de 3 I4 avec gestion du carry
UINT32 static _nAddI4WithCarry_DBG(UINT32  n1, UINT32 n2, UINT32 n3, OUT UINT32 *pnCarry)
{
	INT64 nRes64 = (UINT64)n1 + (UINT64)n2 + (UINT64)n3;
	*pnCarry = nRes64 > 0xFFFFFFFF;
	return     nRes64 & 0xFFFFFFFF;
}
// addition de 3 I4 avec gestion du carry
UINT32 static _nAddI4WithCarry(UINT32  n1, UINT32 n2, UINT32 n3, OUT UINT32 *pnCarry)
{
	UINT32 nRes32 = n1 + n2;
	*pnCarry = (nRes32 < n1);
	nRes32 += n3;
	*pnCarry |= ((nRes32 < n1) || (nRes32 < n2));

		/*
#ifdef _DEBUG
	UINT32 nCarryDBG = 0;
	UINT32 nResDBG = _nAddI4WithCarry_DBG(n1, n2, n3, &nCarryDBG);
	XASSERT(nCarryDBG == *pnCarry);
	XASSERT(nResDBG ==     nRes32);
#endif//!_DEBUG
	*/
	return     nRes32;
}
// addition de 3 I8 avec gestion du carry
UINT64 static _nAddI8WithCarry(UINT64  n1, UINT64 n2, UINT64 n3, OUT UINT64 *pnCarry)
{
	UINT64 nRes64 = n1 + n2;
	*pnCarry = (nRes64 < n1);
	nRes64 += n3;
	*pnCarry |= ((nRes64 < n1) || (nRes64 < n2));

	return nRes64;
}
void static _MultI4(UINT32  n1, UINT32 n2, OUT UINT32 *pnLow, UINT32 *pnHigh)
{
	// on passe en 64 bits pour avoir les poids forts et faibles
	UINT64 a = n1;
	UINT64 b = n2;
	UINT64 nRes64 = a*b;// (UINT64)n1 * (UINT64)n2;
	*pnLow = (nRes64 & 0xFFFFFFFFL);
	*pnHigh = (nRes64 >> 32);
}

// renvoie le n°eme mot de 32 bits. 0 poids faible
UINT32  CBigInt::_nGetI4(int nNumMot) const
{
	XASSERT(nNumMot >= 0);
	XASSERT(nNumMot <  nSizeInI4());
	UINT32 *pnTabVal32 = (UINT32 *)m_TabVal;
	return pnTabVal32[nNumMot];
}
// récup du bit n° I
int CBigInt::nGetBit(int nNumBit) const
{
	UINT32 nDword = _nGetI4Add(nNumBit / 32);
	int nDecale = nNumBit % 32;
	int nRes = (nDword >> nDecale) % 2;
	// 0 ou 1
	return nRes;
}

// renvoie le n°eme mot de 32 bits. 0 poids faible
//version pour addition, gère les cas  ou nNumMot est plus grand que le max
UINT32  CBigInt::_nGetI4Add(int nNumMot) const
{
	XASSERT(nNumMot >= 0);
	if (nNumMot >= nSizeInI4())
	{
		if (bNegative()) return 0XFFFFFFFF;
		else             return 0;
	}
	UINT32 *pnTabVal32 = (UINT32 *)m_TabVal;
	return pnTabVal32[nNumMot];
}
UINT64  CBigInt::_nGetI8Add(int nNumMot) const
{
	XASSERT(nNumMot >= 0);
	if (nNumMot >= nSizeInI8())
	{
		if (bNegative()) return 0XFFFFFFFFFFFFFFFF;
		else             return 0;
	}
	return m_TabVal[nNumMot];
}

// renvoie le n°eme mot de 64 bits. 0 poids faible
UINT64  CBigInt::_nGetI8(int nNumMot) const
{
	XASSERT(nNumMot >= 0);
	XASSERT(nNumMot <  nSizeInI8());
	return m_TabVal[nNumMot];
}

// suppression des mots avec 0 au début
void CBigInt::_Trim0(void)
{
	// calcule le nombre de mots a 0
	int nNbMotA0 = 0;
	UINT64 nZero = 0;
	UINT64 bBitMotSuivant = 0; // cas 0x0000 0000 8741 3000
	if (bNegative())
	{
		nZero = ALLBIT1_64;
		bBitMotSuivant = BITHIGH_64;
	}


	int i = nSizeInI8()-1;
	while (    i >= 1 
			&& _nGetI8(i) == nZero
		    && ((_nGetI8(i-1) & BITHIGH_64) == bBitMotSuivant) // pour garder le signe
		 )
	{
		i--;
		nNbMotA0++;
	}
	// MAJ taille
	m_nSizeInByte -= nNbMotA0 * 8;
	XASSERT(m_nSizeInByte >= 8);
}

// affectation n em mot. alloc si nécessaire
void CBigInt::_SetI4(int nNumMot, UINT32 nVal)
{
	/// sipas assez grand
	if (nSizeInI4() <= nNumMot)
	{
		// realloc
		_Resize((nNumMot+1) * 4, eSansSigne );
	}
	// affectation
	UINT32 *pnTabVal32 = (UINT32 *)m_TabVal;
	pnTabVal32[nNumMot] = nVal;
}
// affectation n em mot. alloc si nécessaire
void CBigInt::_SetI8(int nNumMot, UINT64 nVal)
{
	/// sipas assez grand
	if (nSizeInI8() <= nNumMot)
	{
		// realloc
		_Resize((nNumMot+1) * 8, eSansSigne );
	}
	// affectation
	m_TabVal[nNumMot] = nVal;
}
// affectation n em octet. 
void CBigInt:: _SetI1(int nNumMot, UINT8 nVal)
{
	/// sipas *assez grand
	if (nSizeInByte() <= nNumMot)
	{
		// realloc
		_Resize((nNumMot + 1), eSansSigne);
	}
	// affectation
	UINT8 *pnTabVal8 = (UINT8 *)m_TabVal;
	pnTabVal8[nNumMot] = nVal;
}

// addition
//c=a+b;
 CBigInt operator +(RCBigInt A, RCBigInt B)
{
	CBigInt clRes;

	UINT64 nCarry = 0; // retenue
					  
	// ajout des mots de 32 nits en commencant par le poids faible
	int nNbI8 = std::max<int>( A.nSizeInI8(), B.nSizeInI8() ) ;
	int i;
	for ( i = 0; i < nNbI8; i++)
	{
		XASSERT(nCarry == 0 || nCarry == 1);
		// addition de this+ paramr + carry précédent
		UINT64 nA = A._nGetI8Add(i);
		UINT64 nB = B._nGetI8Add(i);
		UINT64 nRes = _nAddI8WithCarry(nA, nB, nCarry, &nCarry);
		//MAJ dans le resultat
		clRes._SetI8(i, nRes);
	}
	// gestion du carry si l'addtion donne un nombre qui dépasse
	// et que A et B sont de meme signe zet pas du signe du résultat
	if (   nCarry 
		&& (A.bNegative() == B.bNegative()    )
		&& (A.bNegative() != clRes.bNegative())
	   )
	{
		XASSERT(nCarry == 1);
		if (A.bNegative())
		{
			// si résultat positif et pas le cas (-1+-2)
			if (!clRes.bNegative()
				//&& A.Abs().nCompareU(B.Abs())!=-1 
				)
			{
				clRes._SetI8(i, ALLBIT1_64);
				// si on est que le poids faible on met aussi le poids fort.
				if (i % 2 == 0)
					clRes._SetI8(i + 1, ALLBIT1_64);
			}

		}
		else
			clRes._SetI8(i, 1);
		i++;
	}
	// ajoute un bloc de poids fort si on tombe sur un négatif
	if (clRes.bNegative() && !A.bNegative() && !B.bNegative())
		clRes._SetI8(i, 0);

	// suppression des mots avec 0 au début
	clRes._Trim0();
	return clRes;
}
 //a+= 4
 void CBigInt::operator +=(UINT32 n)
 {
	 AddUI4(n, eDefaut);
 }
 //a+= 4
 void CBigInt::operator +=(INT32 n)
 {
	 if (n>=0)
		AddUI4(n, eDefaut);
	 else
		 *this = *this - (UINT64)(-n);

 }
 // a += 4,
 void CBigInt::AddUI4(UINT32 n, EOptionAdd eOption)
 {
	 UINT32 nCarry = 0;
	 // addition de this+ paramr 
	 UINT32 nRes = _nAddI4WithCarry(_nGetI4Add(0), n, 0, &nCarry);
	 //MAJ dans this
	 _SetI4(0, nRes);

	 // ajout aux poids forts si dépassement
	 int nNbI4 = nSizeInI4();
	 int i;
	 for (i = 1; i < nNbI4 && nCarry; i++)
	 {
		 XASSERT(nCarry == 0 || nCarry == 1);
		 // addition de this + carry précédent
		 UINT32 nRes = _nAddI4WithCarry(_nGetI4Add(i), 0, nCarry, &nCarry);
		 //MAJ dans this
		 _SetI4(i, nRes);
	 }
	 // gestion du carry si l'addtion donne un nombre qui dépasse
	 if (nCarry && eOption != eAllowOverflow)
	 {
		 _SetI4(i, nCarry);
	 }

	 // suppression des mots avec 0 au début
	 _Trim0();
 }

 // négation : x = -x
 void CBigInt::Negate(void)
 {
	 // complément a 2
	 int nSizeI8 = nSizeInI8();
	 for (int i = 0; i < nSizeI8; i++)
	 {
		 m_TabVal[i] ^= ALLBIT1_64;
	 }
	 //+1 sans agrandir this
	 AddUI4(1, eAllowOverflow);
 }
 CBigInt operator -(RCBigInt A)
 {
	 CBigInt clRes = A;
	 clRes.Negate();
	 return clRes;
 }

 // soustraction
 //c=a-b;
 CBigInt operator -(RCBigInt A, RCBigInt B)
 {
	 CBigInt MoinsB(B);
	 MoinsB.Negate();
	 // renvoie A + (-B)
	 return A + MoinsB;
 }

 // init a 0
 void CBigInt::SetToZero(void)
 {
	 *this = 0;
 }


 // Multiplication par un entier 32
 void CBigInt::MultUI32(UINT32 nMultiplicateur)
 {
	 UINT32 nCarry = 0; // retenue
	 CBigInt clSrcCopie(*this);
	 BOOL bNegatif = clSrcCopie.bNegative();
	 if (bNegatif)
	 {
		 clSrcCopie.Negate();
	 }

	 SetToZero();
	 // cas *0
	 if (nMultiplicateur==0 || clSrcCopie.bIsZero())
	 {
		 XASSERT(bIsZero());
		 return;
	 }

	 // ajout des mots de 32 nits en commencant par le poids faible
	 int nSizeI4 = clSrcCopie.nSizeInI4();
	 int i;
	 for ( i = 0; i < nSizeI4; i++)
	 {
		 UINT32 nResLow;
		 UINT32 nResHigh;
		 //  (h,l)  = a * m
		 UINT32 nA = clSrcCopie._nGetI4(i);
		 _MultI4(nA, nMultiplicateur, &nResLow, &nResHigh);

		 UINT32 nRes = _nAddI4WithCarry(0, nResLow, nCarry, &nCarry);
		 _SetI4(i, nRes);
		 nCarry += nResHigh;
	 }
	 // ajout du carry
	 if (nCarry)
	 {
		 _SetI4(i, nCarry);
		 i++;
	 }
		
	 // si on mis le bit de poids fort a 1
	 if (bNegative())
		 _SetI4(i, 0); // pour quel'on reste positif


	 // restauration du signe
	 if (bNegatif)
		Negate();
	 XASSERT(!bIsZero());
 }

 // Multiplication par une puissance de 2 (décalage de bits)
 void CBigInt::MultPow2(int nPow2)
 {
	 if (nPow2 <= 0) return;
	 _Resize(nSizeInByte() + nPow2 / 8, eAvecSigne);
	 XASSERT (nPow2 <= (int)nSizeInBit()); //  trop grand, 

	 if (bIsZero()) return;
	 // déplacement par otect si plus de 8
	 if (nPow2 >= 8)
	 {
		 int NbByte = nPow2 / 8;
		 memmove((byte *)m_TabVal + NbByte, // dest
			 (byte *)m_TabVal,	       // src : poids faibles
			 nSizeInByte() - NbByte); // len
		 ZeroMemory(m_TabVal, NbByte); // des 0 sur les poids faibles
		 nPow2 = nPow2 - NbByte * 8;
		 if (nPow2 <= 0)
		 {
			 _Trim0();
			 return;
		 }
		 XASSERT(FALSE);
	 }
//@@A voir
/*
	 // commence avecs les poids faibles
	 UINT64 nCarry = 0;
	 int nSizeI8 = nSizeInI8();
	 for (int i = 0; i < nSizeI8; i++)
	 {
		 UINT64 nCarryNext = m_TabVal[i] >> (64 - nPow2); // sauve les bits de poids fort
		 m_TabVal[i] = m_TabVal[i] << nPow2;
		 m_TabVal[i] |= nCarryNext;
		 nCarry = nCarryNext;
	 }
*/
	 _Trim0();
 }
 
// Division par une puissance de 2 (décalage de bits)
 void CBigInt::DivPow2(int nPow2)
{
	 XDBG(CBigInt clCopie_DBG = *this);

	 if (nPow2 <= 0) return;
	 int nNewSizeInByte = nSizeInByte() - nPow2 / 8;
	 // si on décalde de plus que du nombre de bits dispo
	 if (nNewSizeInByte <= 0)
	 {
		 SetToZero();
		 return;
	 }
	 // Optim : "0 << n = 0"
	 if (bIsZero()) return;

	 _Resize(nNewSizeInByte, eAvecSigne);
	 XASSERT (nPow2 <= (int)nSizeInBit()); //  trop grand, 

	 // déplacement par otect si plus de 8
	 if (nPow2 >= 8)
	 {
		 int NbByteDecal = nPow2 / 8;
		 memmove((byte *)m_TabVal , // dest
				 (byte *)m_TabVal + NbByteDecal,	       // src : poids faibles
				 nNewSizeInByte); // len
		 ZeroMemory((PBYTE)m_TabVal+nNewSizeInByte, NbByteDecal); // des 0 sur les poids forts
		 nPow2 = nPow2 - NbByteDecal * 8;
		 if (nPow2 <= 0)
		 {
			 _Trim0();
			 return;
		 }
		 XASSERT(FALSE);
	 }

	 _Trim0();
	 //@@A voir
}


 // Multiplication
 //c=a*b;
 CBigInt operator *(RCBigInt A, RCBigInt B)
 {
	 // cas des négatifs
	 if (A.bNegative() || B.bNegative())
	 {
		 if (A.bNegative() && B.bNegative())
			 return A.Abs() * B.Abs();
		 return -(A.Abs() * B.Abs());
	 }

	 CBigInt clResult;
	 if (A.bIsZero()) return clResult; //0
	 if (B.bIsZero()) return clResult; //0

	 // on multiplie chaque DWORD par clVal2

	 CBigInt clMulI;
	 int nSizeI4 = std::max<int>(A.nSizeInI4(), B.nSizeInI4());
	 for (int i = 0; i < nSizeI4; i++)
	 {
		 // x = m [ dword n°i ]
		 UINT32 nIB = B._nGetI4Add(i);
		 clMulI = A;
		 // x = t * m
		 clMulI.MultUI32(nIB);
		 // x = x * 2^(i*32)
		 clMulI.MultPow2(i * 32);
		 // res= res + x
		 clResult += clMulI;
	 }
	 clResult._Trim0();
	 //  résultat
	 return clResult;

 }

 // Renvoie le carré 
 CBigInt CBigInt::Pow2(void) const
 {
	 return (*this) * (*this);
 }
 //  Renvoie le cube 
 CBigInt CBigInt::Pow3(void) const
 {
	 return (*this) * (*this) * (*this);
 }

 // renvoie la valeur abosolue
 CBigInt CBigInt::Abs(void) const
 {
	 if (!bNegative())
		 return *this;

	 // renvoie -this
	 CBigInt clRes(*this);
	 clRes.Negate();
	 return clRes;
 }

 // division par 2
 void CBigInt::DivideBy2(void)
 {
	 BOOL bNegatif = bNegative();
	 if (bNegatif)
		 Negate(); // passe en positif
	 // commence avecs les poids forts
	 int nCarry = 0;
	 int nSizeI8 = nSizeInI8();
	 for (int i = nSizeI8 - 1; i >= 0; i--)
	 {
		 int nCarryNext = ((m_TabVal[i] & 0x1) != 0); // sauvel dernier bit
		 m_TabVal[i] = m_TabVal[i] >> 1;
		 if (nCarry)
			 m_TabVal[i] += 0x8000000000000000L;
		 nCarry = nCarryNext;
	 }
	 // suppression des mots avec 0 au début
	 _Trim0();

	 if (bNegatif)
		 Negate(); // repasse en negatif 

 }



 // renvoie la magnitude. cad la puisance de 2 max du nombre et sa valeur dans cette puissance de 2.
 // ex : "0x344A3" => 20,3
 void CBigInt::_GetPow2AndCoef(OUT UINT32 *pnPow2, OUT UINT32 *pnCoef) const
 {
	 // recherche le 1er 0 en partant du poids fort
	 int nSizeI4 = nSizeInI4();
	 for (int i = nSizeI4 - 1; i >= 0; i--)
	 {
		 UINT32 nVal = _nGetI4(i);
		 if (nVal != 0)
		 {
			 *pnCoef = nVal;
			 *pnPow2 = i * 32;
			 return;
		 }
	 }
	 // arrive si *this= 0
	 *pnCoef = 0;
	 *pnPow2 = 0;
 }

 struct STQuickDiv
 {
	 // puisance de 2 nombre qui divise
	 // ex: 0x6186 => 16 
	 UINT32 nMagnitudeDivisor;
	 // plus gos chiffe significatif
	 // ex: 0x61386 => 6
	 UINT32 nQuickDivisor;
 };

 // renvoie les info pour effecter une division rapide appproximative
 void CBigInt::_GetQuickDiv(OUT STQuickDiv *pstQuickDiv) const
 {
	 _GetPow2AndCoef(&pstQuickDiv->nMagnitudeDivisor, &pstQuickDiv->nQuickDivisor);

	 //	int nSizeI4 = nSizeInI4();
	 //	if (nMagnitude / 32 < nSizeI4 - 1)
	 //{
	 //	// prends aussi le poids fort
	 //	nNumerateur +=  (UINT64)_nGetI4(nMagnitude / 32 + 1) << 32;
	 //}
 }
 // Effectue une division rapide.
 CBigInt CBigInt::_clDivQuick(const STQuickDiv &stQuickDiv) const
 {
	 CBigInt clResultat;

	 if (bNegative())
	 {
		 CBigInt clCopie;
		 clCopie.Negate();
		 CBigInt clRes = clCopie._clDivQuick(stQuickDiv);
		 clRes.Negate();
		 XASSERT(clRes.bNegative() || clRes.bIsZero());// 0 aussi possible
		 return clRes;
	 }


	 XASSERT(stQuickDiv.nMagnitudeDivisor >= 0);
//	 if (stQuickDiv.nMagnitudeDivisor > (UINT)nSizeInBit())
//		 return 0;
//	 XASSERT(stQuickDiv.nMagnitudeDivisor < (UINT)nSizeInBit());

	 // récup info du numérateur = this
	 UINT32 nMagnitudeNumerator;
	 UINT32 nCoefNumerator;
	 _GetPow2AndCoef(&nMagnitudeNumerator, &nCoefNumerator);
	 UINT64 nNumerateur = nCoefNumerator;
	 // récup du numérateur en sur 64 bits si possible
	 if (nMagnitudeNumerator >= 32)
	 {
		 // prends aussi le poids fabile du suivant
		 UINT64 nPoidFaible = _nGetI4((nMagnitudeNumerator - 32) / 32);
		 nNumerateur = (nNumerateur << 32) + nPoidFaible;// (UINT64)_nGetI4((nMagnitudeNumerator - 32) / 32);
		 nMagnitudeNumerator -= 32;
	 }
	 //si on divisive par un nombre nettement plus grand
	 if ((nMagnitudeNumerator+32) < stQuickDiv.nMagnitudeDivisor)
	 {
		 // le résultat de la division est 0
		 clResultat.SetToZero();
		 return clResultat;

	 }

	 XASSERT(nMagnitudeNumerator >= 0);
	 XASSERT(nMagnitudeNumerator < nSizeInBit());
	// XASSERT(nMagnitudeNumerator >= stQuickDiv.nMagnitudeDivisor);

	 UINT64 nQuotient = nNumerateur / (UINT64)stQuickDiv.nQuickDivisor;
	 // MAJ résultat
	// clResultat.FromUI8(nQuotient);
	 clResultat = nQuotient;
	 // ex: 5600 / 100 :  4 - 3 => 1
	 int nMagnitudeRes = nMagnitudeNumerator - stQuickDiv.nMagnitudeDivisor;
	 if (nMagnitudeRes<0)
		 clResultat.DivPow2(-nMagnitudeRes);
	 else
		 clResultat.MultPow2(nMagnitudeRes);
	 clResultat._Trim0();
	 return clResultat;
 }


 int static _nCmpUI64(UINT64 n1, UINT64 n2)
 {
	 if (n1 < n2) return -1;
	 if (n1 > n2) return 1;
	 return 0;
 }

 // comparation < = >.
 // renvoie -1 si on est < a clNombre2. 0 si égal. 1 si on est supérieur
 int CBigInt::nCompareU(const CBigInt &clNombre2) const
 {
	 // si on est pkus grande en nombre de chiffre
	 if (nSizeInI8() != clNombre2.nSizeInI8())
	 {
		 if (nSizeInI8() > clNombre2.nSizeInI8())
			 return 1;
		 else
			 return -1;

	 }

	 // commence en comparant les poids forts
	 int nSizeI8 = nSizeInI8();
	 for (int i = nSizeI8 - 1; i >= 0; i--)
	 {
		 int nRes = _nCmpUI64(m_TabVal[i], clNombre2.m_TabVal[i]);
		 if (nRes != 0)
		 {
			 return nRes;
		 }
	 }
	 // egaux
	 return 0;
 }

 void CBigInt::operator +=(RCBigInt A)
 {
	 CBigInt clRes = *this + A;
	 *this = clRes;
 }
 void CBigInt::operator *=(RCBigInt A)
 {
	 CBigInt clRes = *this * A;
	 *this = clRes;
 }
 void CBigInt::operator -=(RCBigInt A)
 {
	 CBigInt clRes = *this - A;
	 *this = clRes;
 }

 // Division entière de this
 // ex : 2445 / 1000 => (2, 445)
 void CBigInt::Divide(const CBigInt &clDiviseur, OUT CBigInt *pclQuotient, OUT CBigInt* pclReste) const
 {


	 if (clDiviseur.bIsZero())
	 {
		 // division par 0
		 XASSERT(FALSE);
		 return;
	 }
	 if (bIsZero())
	 {
		 // 0 / X = 0
		 *pclQuotient = 0L;
		 *pclReste    = 0L;
		 return;
	 }
	 // cas des négatif
	 if (bNegative())
	 {
		 CBigInt clCopiePositive(*this);
		 clCopiePositive.Negate();
		 clCopiePositive.Divide(clDiviseur, pclQuotient, pclReste);
		 pclQuotient->Negate();
		 pclReste->Negate();
		 return;
	 }
	 if (clDiviseur.bNegative())
	 {
		 CBigInt clCopiePositive(clDiviseur);
		 clCopiePositive.Negate();
		 Divide(clCopiePositive, pclQuotient, pclReste);
		 pclQuotient->Negate();
		 return;
	 }
	 // si on est plus petit que le diviseur
	 int nResCmp = nCompareU(clDiviseur);
	 if (nResCmp == -1)
	 {
		 // T / X = 0 si T < X
		 *pclQuotient = 0;
		 *pclReste    = *this;
		 return;
	 }
	 if (nResCmp == 0)
	 {
		 // X / X = 1
		 *pclQuotient = 1;
		 *pclReste    = 0L;
		 return;
	 }

	 // --- algo itératif trouvé ici 
	 // http://justinparrtech.com/JustinParr-Tech/an-algorithm-for-arbitrary-precision-integer-division/

	 //M = length in digits of D, minus 1.  So if D is 1234, there are 4 digits, M = 3.
	 STQuickDiv stQuickDivisorA;
	 clDiviseur._GetQuickDiv(&stQuickDivisorA);
	 //A = D – ( D MOD 10^M ). ex 64367 => 60000, mais en Puissance de 2
	 //= clDiviseur._nGetQuickDivisor(nMagnitude);
	 CBigInt clQuotient = _clDivQuick(stQuickDivisorA);
	// clQuotient.DBG_Print();

	 XASSERT(!clQuotient.bNegative());
	 CBigInt clReste	= clDiviseur +1;
	 XDBG(CBigInt clRestePrec = clReste);
	 CBigInt clNewQ;
	 CBigInt clQuotientN;
	 CBigInt clTemp;
	 // While  ABS(R)>=D
	 int nNbTour = 0;
	 while (clReste.Abs().nCompareU(clDiviseur) != -1)
	 {
		 // R = N – (Q * D)
		// clQuotient.DBG_Print();
		 //CBigInt clTemp(clQuotient);
		// clTemp *= clDiviseur;//@test
		 //clTemp.DBG_Print();
		 //clReste = *this - clTemp;
		 clReste = *this - clQuotient*clDiviseur;
		 //clReste.DBG_Print();
		 XASSERT(clReste.Abs().nCompareU(*this) != 1); // plus grand que le divisé
		 XASSERT(nNbTour==0 || clReste.Abs().nCompareU(clRestePrec.Abs()) == -1); // le reste dtoi décroitre
		 XDBG(clRestePrec = clReste);
		// clReste -= clTemp;

		 // Qn = Q + R / A
		 if (clReste.bNegative())
			 clTemp = -clReste.Abs()._clDivQuick(stQuickDivisorA);
		 else
			 clTemp = clReste._clDivQuick(stQuickDivisorA);
		 clQuotientN = clQuotient + clTemp;// clReste._clDivQuick(stQuickDivisorA);


		 // Q = (Q + Qn) / 2
		 clNewQ = clQuotient + clQuotientN;
		 clNewQ.DivideBy2();

		 // anti-boucle infinie si en /2 on retombe sur le meme
		 if (clNewQ.nCompareU(clQuotient) == 0 && clReste.Abs().nCompareU(clDiviseur) != -1)
		 {
			 if (clReste.bNegative())
				clNewQ -= 1; // Q--
			 else
				clNewQ += 1; // Q++
		 }

#ifdef _DEBUG
		 //@TEST
		 nNbTour++;
		 if (nNbTour > m_nSizeInByte * 10+10)
		 {
			this->DBG_Print();
			clDiviseur.DBG_Print();
			clQuotient.DBG_Print();
			clReste.DBG_Print();
		 }
#endif//_DEBUG
		 XASSERT(nNbTour <= nSizeInByte() * 200);
		 clQuotient = clNewQ;

	 }

	 //R = N – (Q * D)    Calculate a final value for R
	 //CBigInt clTemp = clQuotient* clDiviseur;
	 //clTemp *= clDiviseur;
	 clReste = *this - (clQuotient* clDiviseur);

	 if (clReste.bNegative())
	 {
		 // Q = Q - 1
		 clQuotient -= 1;
		 // R = R + D
		 clReste += clDiviseur;
	 }
	 XASSERT( !clReste.bNegative() );

	 // MAJ résultat
	 *pclQuotient = clQuotient;
	 *pclReste    = clReste;

#ifdef _DEBUG
	 // on teste en debug le résultat
	 CBigInt clDBG_Tst = clQuotient * clDiviseur;
	 clDBG_Tst += clReste;
	 XASSERT(nCompareU(clDBG_Tst) == 0);
#endif//__DEBUG
 }

 //divistion
 //c=a+b;
 CBigInt operator /(RCBigInt A, RCBigInt B)
 {
	 CBigInt clQuotient;
	 CBigInt clReste;
	 A.Divide(B, &clQuotient, &clReste);
	 return clQuotient;
 }

 // depuis une chaine en base 10 ou 16 si commence par 0x
 void CBigInt::FromStrBasePrefix(PCXSTR pszVal)
 {
	 // si hexa
	 if (pszVal[0] == '0'
		 && pszVal[1] == 'x')
	 {
		 // base 16
		 FromStrHexa(pszVal);
		 return;
	 }
	 // base 10
	 FromStrBase10(pszVal);
 }



 // depuis une chaine en base 10.
 // ex : "32670510020758816978083085130507043184471273380659243275938904335757337482424"
 void CBigInt::FromStrBase10(PCXSTR pszVal)
 {
	 //cas négatif
	 BOOL  bNegatif = (*pszVal == '-');
	 if (bNegatif)
		 pszVal++;

	 SetToZero();
	 //CInt256 cl10(10);
	 PCXSTR pCur = pszVal;
	 while (*pCur)
	 {
		 // récup poids fort en base 10 : p
		 char c = *pCur;
		 XASSERT(c >= '0');
		 XASSERT(c <= '9');
		 int nValPfBase10 = (c - '0');
		 CBigInt clP;
		 clP = nValPfBase10;
		 // x = x * 10 + p
		 MultUI32(10);
		 XASSERT(!bNegative());
		 *this += clP;

		 pCur++;
	 }

	 if (bNegatif)
		 Negate();
 }

 // convetion d'un char d'une chaine hexa en un byte non signé
 int _nGetValCharHexa(char c)
 {
	 if (c >= '0' && c <= '9')
		 return c - '0';
	 if (c >= 'a' && c <= 'f')
		 return c - 'a' + 10;
	 if (c >= 'A' && c <= 'F')
		 return c - 'A' + 10;
	 return 0;
 }

// convetion de 2 premiers char d'une chaine heaxe en un byte non signé
int _nGetValByteHexa(PCXSTR pszVal)
{
	return   _nGetValCharHexa(*pszVal) * 16
		   + _nGetValCharHexa(*(pszVal + 1));
}

 // depuis une chaine en base 16
 // ex : "1E99423A4ED27608A15A2616A2B0E9E52CED330AC530EDCC32C8FFC6A526AEDD"
 void CBigInt::FromStrHexa(PCXSTR pszVal)
 {
	 int nNbChar = strlen(pszVal);
	 if (nNbChar < 2)
		 return;// invalide

	 //taille en octet : "0xA4" => 1 octets pour 2 char
	 int nNbTaille = nNbChar / 2;
	 BOOL bImpair = nNbChar % 2 == 1;
	 // init/reinit taille modulo 8
	 _Resize(nNbTaille + bImpair, eSansSigne);

	 PCXSTR pszCur = pszVal+ nNbChar -2; // commen a la fin = poids faible
	 for (int i = 0; i < nNbTaille; i++)
	 {

		 BYTE nVal = _nGetValByteHexa(pszCur);
		 _SetI1(i, nVal);
		 pszCur -= 2;
	 }
	 // si nombre impair de char
	 if (bImpair)
	 {
		 // on prend juste le 1er char = poids fort
		 int n1erChar = _nGetValCharHexa(*pszVal);
		 _SetI1(nNbTaille, n1erChar);
	 }

	 _Trim0();// cas de "000000000000000001"
 }

 // renvoie le modulo = reste de la divistion par <clDiviseur>
 CBigInt CBigInt::Modulo(const CBigInt &clDiviseur) const
 {
	 CBigInt clDivision;
	 CBigInt clReste;
	 Divide(clDiviseur, &clDivision, &clReste);
	 // pour que le modulo soit dans [0,modulo[
	 if (clReste.bNegative())
		 clReste += clDiviseur;

	 return clReste;
 }

 // vers un INT64
 UINT64 CBigInt::_nToUI8(void) const
 {
	 XASSERT(m_nSizeInByte == 8);
	 return m_TabVal[0];
 }

 // insérer 1 char a début de <pszVal>
static void _Insert1Char(PXSTR pszVal, int nLenInChar, char cNum)
{
	 memmove(pszVal + 1, pszVal, nLenInChar - 1);
	 *pszVal = cNum;
}


// vers une chaine en base 10.
 void CBigInt::ToStrBase10(OUT PXSTR pszVal, int nLenInChar) const
 {
	 //fermer la chaine
	 *pszVal = 0;

	 // cas de "-6452"
	 if (bNegative())
	 {
		 Abs().ToStrBase10(pszVal + 1, nLenInChar - 1); // +1, résever la place pour "-"
		 *pszVal = '-';
		 return;
	 }

	 // cas parrticulier du 0
	 if (bIsZero())
	 {
		 *pszVal     = '0';
		 *(pszVal+1) =  0;
		 return;
	 }

	 // 10 = la base
	 CBigInt cl10 = (UINT64)10;

	 // division par  10 tant que non nul
	 CBigInt clCur(*this);
	 while (!clCur.bIsZero())
	 {
		 // Division entière
		 // ex : 2445 / 10 => (2, 445)
		 CBigInt clQuotient;
		 CBigInt clReste;
		 XASSERT(!clCur.bNegative());
		 clCur.Divide(cl10, &clQuotient, &clReste);
		 XASSERT(!clReste.bNegative());
		 // nombre a affecter a la pos courante
		 int nNumber = (int)clReste._nToUI8();
		 XASSERT(nNumber >= 0);
		 XASSERT(nNumber <= 9);
		 // insert du char 
		 char cNum = '0' + nNumber;
		 _Insert1Char(pszVal, nLenInChar, cNum);

		 // suivant
		 clCur.CopieFrom(clQuotient);

	 }

 }

 // vers une chaine en base 16.
 void CBigInt::ToStrBase16(OUT PXSTR pszVal, int nLenInChar) const
 {
	 PXSTR pszCur = pszVal;
	 int nLenRemaining = nLenInChar;
	 int nNbI4 = nSizeInI4();
	 for (int i = 0; i < nNbI4; i++)
	 {
		 if (nLenRemaining < 8)
			 break;
		 // Ajout du mot i en commecant par le poids fort.
		 UINT32 nI = _nGetI4(nNbI4- i-1);
		 snprintf(pszCur, nLenRemaining, "%08x", nI);
		 // mot suivant (pods plus fabile)
		 pszCur		   += 8;
		 nLenRemaining -= 8;
	 }
	 // ferme la chaine
	 *pszCur = 0;
 }
 
 // calcul du PGCD de 2 nombre par l'algorithme d'eclide étendu
 // renvoie le PGCD et x,y tels que A*x + B*y = PGCD
 void CBigInt::sPGDCExt(RCBigInt A, RCBigInt B,
						OUT CBigInt *ppgdc, OUT CBigInt *px, OUT CBigInt *py)
 {
	 // Algorithme d'euclide étendu
	 CBigInt s, t, r;
	 CBigInt sOld, tOld, rOld;

	 // Init
	 s	  = 0;
	 sOld = 1;
	 t	  = 1;
	 tOld = 0;
	 r	  = B;
	 rOld = A;

	 // var temps. hors la boucle pour optim d'allocs
	 CBigInt quotient, reste;
	 while (!r.bIsZero())
	 {
		 // Division euclidiene
		 // quotient = rOld / r + reste_unused;
		 CBigInt quotient, resteCur;
		 rOld.Divide(r, &quotient, &resteCur);
		 XASSERT(!resteCur.bNegative());
		 // sauver les valeur précécentes
		 CBigInt rTemp = rOld;
		 CBigInt sTemp = sOld;
		 CBigInt tTemp = tOld;
		 rOld = r;
		 sOld = s;
		 tOld = t;
		 // MAJ var
		 XASSERT(resteCur == rTemp - quotient * r);
		 r = resteCur;// rTemp - quotient * r;
		 s = sTemp - quotient * sOld;
		 t = tTemp - quotient * t;
		
#ifdef _DEBUG
		 // les égalités r = a*u+b*v et r' = a*u'+b*v' sont des invariants de boucle
		CBigInt TestVal = A * s + B * t;
		XASSERT(TestVal.Modulo(B) == r);
#endif//_DEBUG
	 }

	 *ppgdc = rOld;
	 *px    = sOld;
	 *py	= tOld;

#ifdef _DEBUG
	 CBigInt TestVal = A * (*px) + B * (*py);
	 XASSERT(TestVal.Modulo(B) == *ppgdc);
/*	  CBigInt TestVal_Mod = TestVal.Modulo(B);
	 if (TestVal_Mod != *ppgdc)
	 {
		 A.DBG_Print();
		 B.DBG_Print();

		 CBigInt A0 = A * (*px);
		 A0.DBG_Print();
		 CBigInt A0Mod = A0.Modulo(B);
		 A0Mod.DBG_Print();

		 ppgdc->DBG_Print();
		 px->DBG_Print();
		 TestVal.DBG_Print();
		 TestVal_Mod.DBG_Print();

		 XASSERT(FALSE);
	 }

	 /*
	 A.DBG_Print();
	 px->DBG_Print();
	 CBigInt A0 = A * (*px);
	 A0.DBG_Print();
	 B.DBG_Print();
	 CBigInt A0Mod = A0.Modulo(B);
	 A0Mod.DBG_Print();
	 XASSERT(A0Mod == 1);


	 XASSERT( *ppgdc == 1);

	 CBigInt B0 = B * (*py);
	 B0.DBG_Print();
	 XASSERT(B0.Modulo(B) == 0);

	  TestVal = A0 + B0;
	 TestVal.DBG_Print();

	 A0.DBG_Print();
	 */

#endif/_DEBUG
 }

 // calcul de l'invere modulaire
 // cas x tel que x *(*this) == 1 modulo mod
 CBigInt CBigInt::InvertModulo(RCBigInt mod)
 {
	 XASSERT(!bIsZero());

	 // on calcule avec la valeur modulo
	 CBigInt val = Modulo(mod);

	 // alorithme d'euclide étendu : pgdc + facteurs
	 CBigInt pgdc, x, y;
	 sPGDCExt(val, mod, &pgdc, &x, &y);
	 
	 // on ne doit être premier avec <modulo> sinon l'inver modulaire n'exite pas
	 if (pgdc != 1)
	 {
		 XASSERT(FALSE);
		 return 0;
	 }
	 // cas particulier des négatifs.
	 if (x.bNegative())
		 x = mod - ((-x).Modulo(mod));
	 CBigInt inverse = x.Modulo(mod);


#ifdef _DEBUG
	 CBigInt TestVal = inverse * val;
	 XASSERT(TestVal.Modulo(mod) == 1);
#endif/_DEBUG
	 return inverse;
 }




#ifdef _DEBUG
 bool gbInPrint = 0;
 // nOption : DBGPRINT_NORC par ex/
 void CBigInt::DBG_Print(int nOption) const
 {
	 if (gbInPrint) return;

	 gbInPrint = 1;

	 char szBuf[1320] = {};
	 ToStrBase16(szBuf, 1320);
	 
	 // on aligne a droite
	 int nLen = strlen(szBuf);
	 if ((nOption & DBGPRINT_NORALIGN) == 0)
	 {
		 int nPadding = 170 - nLen;
		 for (int i = 0; i < nPadding; i++)
			 printf(" ");
	 }


	 printf(szBuf);
	 if ((nOption & DBGPRINT_NORC) == 0)
		printf("\r\n");

	 gbInPrint = 0;
 }
#endif/_DEBUG
