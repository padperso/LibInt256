
#include "central.h"
#include "BigIntFixed.h"

#define ALLBIT1_64 0xFFFFFFFFFFFFFFFFL // 64 bits a 1.
#define BITHIGH_64 0x8000000000000000L // bit de poids fort en 64 bits

// constructeur.
// Init a 0
CBigIntFixed::CBigIntFixed(int nSizeInByte)
{
	_Init(nSizeInByte);
}
// constructeur. copie
CBigIntFixed::CBigIntFixed(const CBigIntFixed &clSrc)
{
	_Init(clSrc.nSizeInByte());
	CopieFrom(clSrc);
}


// init a 0
void CBigIntFixed::_Init(int nSizeInByte)
{
	XASSERT(nSizeInByte % 4 == 0);
	XASSERT(nSizeInByte > 32);
	m_nSizeInByte = nSizeInByte;
	m_TabVal = (UINT64 *)malloc(nSizeInByte);
	ZeroMemory(m_TabVal, nSizeInByte);
}

// copie / affectation
const CBigIntFixed & CBigIntFixed::operator= (const CBigIntFixed &clSrc)
{
	CopieFrom(clSrc);
	return *this;
}
const CBigIntFixed & CBigIntFixed::operator= (UINT64 nVal)
{
	FromUI8(nVal);
	return *this;
}

// copie
void CBigIntFixed::CopieFrom(const CBigIntFixed &clSrc)
{
	XASSERT(clSrc.nSizeInByte() == nSizeInByte());
	memcpy(m_TabVal, clSrc.m_TabVal, nSizeInByte());
}

// init a 0
void CBigIntFixed::SetToZero(void)
{
	ZeroMemory(m_TabVal, nSizeInByte());
}
BOOL CBigIntFixed::bIsZero(void) const
{
	int nSizeI8 = nSizeInI8();
	for (int i = 0; i < nSizeI8; i++)
	{
		if (m_TabVal[i] != 0)
			return FALSE;
	}
	// 0 
	return TRUE;
}
BOOL CBigIntFixed::bNegative(void) const
{
	return (m_TabVal[nSizeInI8() - 1] & BITHIGH_64) != 0;
}


// depuis un INT64 non singé
void CBigIntFixed::FromUI8(UINT64 nVal)
{
	SetToZero();
	m_TabVal[0] = nVal;
}
// init a partir d'un entier signé
void CBigIntFixed::FromI4(INT32 nVal)
{
	if (nVal >= 0)
	{
		FromUI8(nVal);
		return;
	}
	// complément a 1
	m_TabVal[0] = (UINT64)nVal ;
	int nSizeI8 = nSizeInI8();
	for (int i = 1; i < nSizeI8; i++)
	{
		m_TabVal[i] = ALLBIT1_64;
	}

}

// vers un INT64
UINT64 CBigIntFixed::nToUI8(void) const
{
	return m_TabVal[0];
}


// addition de 3 I4 avec gestion du carry
UINT32 static _nAddI4WithCarry(UINT32  n1, UINT32 n2, UINT32 n3, OUT UINT32 *pnCarry)
{
	INT64 nRes64 = (UINT64)n1 + (UINT64)n2 + (UINT64)n3;
	*pnCarry = nRes64 > 0xFFFFFFFF;
	return     nRes64 & 0xFFFFFFFF;
}
void static _MultI4(UINT32  n1, UINT32 n2, OUT UINT32 *pnLow, UINT32 *pnHigh)
{
	// on passe en 64 bits pour avoir les poids forts et faibles
	UINT64 nRes64 = (UINT64)n1 * (UINT64)n2;
	*pnLow  = (nRes64		& 0xFFFFFFFFL);
	*pnHigh = (nRes64 >> 32				 );
}

// Addition
void CBigIntFixed::Add(const CBigIntFixed &clVal2)
{
	UINT32 nCarry = 0; // retenue
	// ajout des mots de 32 nits en commencant par le poids faible
	int nNbI4 = nSizeInI4();
	for (int i = 0; i < nNbI4; i++)
	{
		XASSERT(nCarry == 0 || nCarry == 1);
		UINT32 nRes = _nAddI4WithCarry( _nGetI4(i), clVal2._nGetI4(i), nCarry, &nCarry);
		_SetI4(i, nRes );
	}
}
// ajuot d'un entier signé
void CBigIntFixed::AddI4(int nVal)
{
	// ajout via l'opérateur sur 2  CBigIntFixed
	CBigIntFixed clVal(nSizeInByte());
	clVal = nVal;
	Add(clVal);
}
// Soustraction
void CBigIntFixed::Substract(const CBigIntFixed &clVal)
{
	// ajout de -clVal
	CBigIntFixed clValMoins(nSizeInByte());
	clValMoins = clVal;
	clValMoins.Negate(); // v = -v;
	Add(clValMoins);
}
// négation : x = -x
void CBigIntFixed::Negate(void)
{
	// complément a 2
	int nSizeI8 = nSizeInI8();
	for (int i = 0; i < nSizeI8; i++)
	{
		m_TabVal[i] ^= ALLBIT1_64;
	}
	AddI4(1);
}


// renvoie le n°eme mot de 3é bits. 0 poids faible
UINT32 CBigIntFixed::_nGetI4(int nNumMot) const
{
	XASSERT(nNumMot >= 0);
	XASSERT(nNumMot <  nSizeInI4());
	UINT32 *pnTabVal32 = (UINT32 *)m_TabVal;
	return pnTabVal32[nNumMot];
}
// mofigie le n°eme mot de 3é bits. 0 poids faible
void CBigIntFixed::_SetI4(int nNumMot, UINT32 nVal)
{
	XASSERT(nNumMot >= 0);
	XASSERT(nNumMot <  nSizeInI4());
	UINT32 *pnTabVal32 = (UINT32 *)m_TabVal;
	pnTabVal32[nNumMot] = nVal;
}

//#include <math.h>
//use Math::BigInt;

// Multiplication par un entier 32
void CBigIntFixed::MultUI32(UINT32 nMultiplicateur )
{
	UINT32 nCarry = 0; // retenue
	CBigIntFixed clRes(nSizeInByte());
					 
	// ajout des mots de 32 nits en commencant par le poids faible
	int nSizeI4 = nSizeInI4();
	for (int i = 0; i < nSizeI4; i++)
	{
		UINT32 nResLow;
		UINT32 nResHigh;
		//  (h,l)  = a * m
		UINT32 nA = _nGetI4(i);
		_MultI4(nA, nMultiplicateur, &nResLow, &nResHigh);
		
		UINT32 nRes = _nAddI4WithCarry(0, nResLow, nCarry, &nCarry);
		_SetI4(i, nRes);
		nCarry = nResHigh;

	}
}
// Multiplication par une autre CBigIntFixed
void CBigIntFixed::Mult(const CBigIntFixed &clVal2)
{
	// on multiplie chaque DWORD par clVal2
	CBigIntFixed clResut(nSizeInByte());
	int nSizeI4 = nSizeInI4();
	for (int i = 0; i < nSizeI4; i++)
	{
		// x = x * i
		UINT32 nI = clVal2._nGetI4(i);
		CBigIntFixed clMulI(*this);
		clMulI.MultUI32(nI);
		// x = x << i
		clMulI.MultPow2(i * 32);
		// res= res + x
		clResut.Add(clMulI);
	}
	// copie résultat
	*this = clResut;
}

int static _nCmpUI64(UINT64 n1, UINT64 n2 )
{
	if (n1 < n2) return -1;
	if (n1 > n2) return 1;
	return 0;
}

// comparation < = >.
// renvoie -1 si on est < a clNombre2. 0 si égal. 1 si on est supérieur
int CBigIntFixed::nCompareU(const CBigIntFixed &clNombre2) const
{
	// commence en comparant les poids forts
	int nSizeI8 = nSizeInI8();
	for (int i = nSizeI8-1; i >=0 ; i--)
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

// division par 2
void CBigIntFixed::DivideBy2(void)
{
	BOOL bNegatif = bNegative();
	// commence avecs les poids forts
	int nCarry = 0;
	int nSizeI8 = nSizeInI8();
	for (int i = nSizeI8 - 1; i >= 0; i--)
	{
		int nCarryNext = ((m_TabVal[i] & 0x1) != 0); // sauvel dernier bit
		m_TabVal[i]  = m_TabVal[i] >> 1;
		if (nCarry)
			m_TabVal[i] += 0x8000000000000000L;
		nCarry = nCarryNext;
	}
	if (bNegatif)
	{
		// remet le bit de poids fort à 0
		m_TabVal[nSizeI8 - 1] |= BITHIGH_64;
		XASSERT(bNegative());
	}
}
// Multiplication par une puissance de 2 (décalage de bits)
void CBigIntFixed::MultPow2(int nPow2)
{
	if (nPow2 <= 0) return;
	if (nPow2 >= 256)
	{
		SetToZero();
		return;
	}
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
		if (nPow2 <= 0) return;
	}



	// commence avecs les poids faibles
	UINT64 nCarry = 0;
	int nSizeI8 = nSizeInI8();
	for (int i = 0; i < nSizeI8; i++)
	{
		UINT64 nCarryNext = m_TabVal[i] >> (64-nPow2); // sauve les bits de poids fort
		m_TabVal[i] = m_TabVal[i] << nPow2;
		m_TabVal[i] |= nCarryNext;
		nCarry = nCarryNext;
	}

}


// renvoie la magnitude. cad la puisance de 2 max du nombre
// <pnQuickDivisor>: diviseur rapide ( a multipler par cette puissance de 2 )
int CBigIntFixed::_nGetMagnitudeEtDivisteurRapide(OUT UINT32 *pnQuickDivisor) const
{
	int nSizeI4 = nSizeInI4();
	for (int i = nSizeI4 - 1; i >= 0; i--)
	{
		UINT32 nVal = _nGetI4(i);
		if (nVal != 0)
		{
			*pnQuickDivisor = nVal;
			return i * 32;
		}
	}
	XASSERT(FALSE);
	*pnQuickDivisor = 1;
	return 0;
}
// Effectue une division rapide.
CBigIntFixed CBigIntFixed::_clDivQuick(UINT32 nQuickDivisor, int nMagnitude) const
{
	if (bNegative())
	{
		CBigIntFixed clCopie(*this);
		clCopie.Negate();
		CBigIntFixed clRes = clCopie._clDivQuick(nQuickDivisor, nMagnitude);
		clRes.Negate();
		XASSERT(clRes.bNegative() || clRes.bIsZero() );// 0 aussi possible
		return clRes;
	}


	XASSERT(nMagnitude >= 0);
	XASSERT(nMagnitude < 256);
	// récup du numérateur divisé par la la magnitude
	// ex : 56584 => '5'
	UINT64 nNumerateur = _nGetI4(nMagnitude / 32);
	int nSizeI4 = nSizeInI4();
	if (nMagnitude / 32 < nSizeI4 - 1)
	{
		// prends aussi le poids fort
		nNumerateur +=  (UINT64)_nGetI4(nMagnitude / 32 + 1) << 32;
	}
	UINT64 nQuotient = nNumerateur /(UINT64)nQuickDivisor;
	// MAJ résultat
	CBigIntFixed clResultat(nSizeInByte());
	clResultat.FromUI8(nQuotient);
	clResultat.MultPow2(nMagnitude);
	return clResultat;
}


// renvoie la valeur abosolue
CBigIntFixed CBigIntFixed::Abs(void) const
{
	if (!bNegative())
		return *this;

	// renvoie -this
	CBigIntFixed clRes(*this);
	clRes.Negate();
	return clRes;
}

// Division entière de this
// ex : 2445 / 1000 => (2, 445)
void CBigIntFixed::Divide(const CBigIntFixed &clDiviseur, OUT CBigIntFixed *pclDivision, OUT CBigIntFixed* pclReste) const
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
		*pclDivision = 0L;
		*pclReste    = 0L;
		return;
	}
	// si on est plus petit que le diviseur
	int nResCmp = nCompareU(clDiviseur);
	if (nResCmp == -1)
	{
		// T / X = 0 si T < X
		*pclDivision = 0;
		*pclReste    = *this;
		return;
	}
	if (nResCmp == 0)
	{
		// X / X = 1
		*pclDivision = 1;
		*pclReste    = 0L;
		return;
	}
	// cas des négatif
	if (bNegative())
	{
		CBigIntFixed clCopiePositive(*this);
		clCopiePositive.Negate();
		clCopiePositive.Divide(clDiviseur, pclDivision, pclReste);
		pclDivision->Negate();
		return;
	}

	// --- algo itératif trouvé ici 
	// http://justinparrtech.com/JustinParr-Tech/an-algorithm-for-arbitrary-precision-integer-division/

	//M = length in digits of D, minus 1.  So if D is 1234, there are 4 digits, M = 3.
	UINT32 nQuickDivisorA;
	int nMagnitude = clDiviseur._nGetMagnitudeEtDivisteurRapide(&nQuickDivisorA);
	//A = D – ( D MOD 10^M ). ex 64367 => 60000, mais en Puissance de 2
	//= clDiviseur._nGetQuickDivisor(nMagnitude);
	CBigIntFixed clQuotient = _clDivQuick(nQuickDivisorA, nMagnitude);
	CBigIntFixed clReste( clDiviseur );
	clReste.AddI4(1);

	// While  ABS(R)>=D
	while (  clReste.Abs().nCompareU(clDiviseur) == 1 )
	{
		// R = N – (Q * D)
		CBigIntFixed clTemp(clQuotient);
		clTemp.Mult(clDiviseur);
		clReste = *this;
		clReste.Substract(clTemp);

		// Qn = Q + R / A
		CBigIntFixed clQuotientN = clReste._clDivQuick(nQuickDivisorA, nMagnitude);
		clQuotientN.Add(clQuotient);

		// Q = (Q + Qn) / 2
		CBigIntFixed clNewQ = clQuotient;
		clNewQ.Add(clQuotientN);
		clNewQ.DivideBy2();
		// anti-boulce infinie si en /2 on retombe sur le meme
		//if (clNewQ.nCompareU(clQuotient) == 0)
		//{
			//clNewQ.AddI4(1); // Q++
		//}
		clQuotient = clNewQ;

	}
	
	//R = N – (Q * D)    Calculate a final value for R
	CBigIntFixed clTemp(clQuotient);
	clTemp.Mult(clDiviseur);
	clReste = *this;
	clReste.Substract(clTemp);
	
	if (clReste.bNegative())
	{
		// Q = Q - 1
		clQuotient.AddI4(-1);
		// R = R + D
		clReste.Add(clDiviseur);
	}

	// MAJ résultat
	*pclDivision = clQuotient;
	*pclReste    = clReste;
}






// depuis une chaine en base 10.
// ex : "32670510020758816978083085130507043184471273380659243275938904335757337482424"
void CBigIntFixed::FromStrBase10(PCXSTR pszVal)
{
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
		CBigIntFixed clP(nSizeInByte());
		clP = nValPfBase10;
		// x = x * 10 + p
		MultUI32(10);
		Add(clP);

		pCur++;
	}

}

// insérer 1 char a début de <pszVal>
void _Insert1Char( PXSTR pszVal, int nLenInChar, char cNum)
{
	memmove(pszVal + 1, pszVal, nLenInChar - 1);
	*pszVal = cNum;
}


// vers une chaine en base 10.
// nLenInChar doit faire au moins 79 charatères.
void CBigIntFixed::ToStrBase10(OUT PXSTR pszVal, int nLenInChar) const
{

	//fermer la chaine
	*pszVal = 0;

	// cas de "-6452"
	if (bNegative())
	{
		CBigIntFixed clCopiePositive(*this);
		clCopiePositive.ToStrBase10(pszVal + 1,nLenInChar-1); // +1, résever la place pour "-"
		*pszVal = '-';
		return;
	}
	
	// cas parrticulier du 0
	if (bIsZero())
	{
		*pszVal = '0';
		return;
	}

	// 10 = la base
	CBigIntFixed cl10(nSizeInByte());
	cl10 = 10;
	
	// division par  10 tant que > 10
	CBigIntFixed clCur(*this);
	while (clCur.nCompareU(cl10) == 1)
	{
		// Division entière
		// ex : 2445 / 10 => (2, 445)
		CBigIntFixed clDivision(nSizeInByte());
		CBigIntFixed clReste(nSizeInByte());
		XASSERT(!clCur.bNegative());
		clCur.Divide(cl10, &clDivision, &clReste);
		XASSERT(!clReste.bNegative());
		// nombre a affecter a la pos courante
		int nNumber = (int)clDivision.nToUI8();
		XASSERT(nNumber >= 0);
		XASSERT(nNumber <= 9);
		// insert du char 
		char cNum = '0' + nNumber;
		_Insert1Char(pszVal, nLenInChar, cNum);

		// suivant
		clCur.CopieFrom( clReste );

	}

}

// Renvoie le carré 
CBigIntFixed CBigIntFixed::Pow2(void) const
{
	CBigIntFixed clRes(*this);
	CBigIntFixed clRes2(*this);
	clRes.Mult(clRes2);
	return clRes;
}
// Renvoie le cube 
CBigIntFixed CBigIntFixed::Pow3(void) const
{
	CBigIntFixed clRes(*this);
	CBigIntFixed clRes2(*this);
	clRes.Mult(clRes2);
	clRes.Mult(clRes2); 
	return clRes;
}
// renvoie le modulo = reste de la divistion par <clDiviseur>
CBigIntFixed CBigIntFixed::Modulo(const CBigIntFixed &clDiviseur) const
{
	CBigIntFixed clDivision(nSizeInByte());
	CBigIntFixed clReste(nSizeInByte());
	Divide(clDiviseur, &clDivision, &clReste);
	return clReste;
}

//@@TEST
#include "Int256.h"
void CBigIntFixed::InitFromInt256(const class CInt256 &clSrc)
{
	memcpy(m_TabVal, clSrc.m_TabVal, 256/8 );
}





