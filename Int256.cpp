
#include "central.h"
#include "int256.h"

#define ALLBIT1_64 0xFFFFFFFFFFFFFFFFL // 64 bits a 1.
#define BITHIGH_64 0x8000000000000000L // bit de poids fort en 64 bits

// init a 0
CInt256::CInt256()
{
	SetToZero();
}
// init a partir d'un entier
CInt256::CInt256(UINT64 nVal)
{
	FromUI8(nVal);
}
// init a partir d'un entier signé
CInt256::CInt256(INT32 nVal)
{
	FromI4(nVal);
}

// opération de copie
CInt256::CInt256(const CInt256 &clSrc)
{
	CopieFrom( clSrc);
}
const CInt256 & CInt256::operator= (const CInt256 &clSrc)
{
	CopieFrom(clSrc);
	return *this;
}
const CInt256 & CInt256::operator= (UINT64 nVal)
{
	FromUI8(nVal);
	return *this;
}

// copie
void CInt256::CopieFrom(const CInt256 &clSrc)
{
	memcpy(m_TabVal, clSrc.m_TabVal, nSizeInByte);

}

// init a 0
void CInt256::SetToZero(void)
{
	XASSERT(sizeof(m_TabVal) == nSizeInByte);
	ZeroMemory(m_TabVal, nSizeInByte);
}
BOOL CInt256::bIsZero(void) const
{
	for (int i = 0; i < nSizeI8; i++)
	{
		if (m_TabVal[i] != 0)
			return FALSE;
	}
	// 0 
	return TRUE;
}
BOOL CInt256::bNegative(void) const
{
	return (m_TabVal[nSizeI8 - 1] & BITHIGH_64) != 0;
}




// depuis un INT64
void CInt256::FromUI8(UINT64 nVal)
{
	SetToZero();
	m_TabVal[0] = nVal;
}
// init a partir d'un entier signé
void CInt256::FromI4(INT32 nVal)
{
	if (nVal >= 0)
	{
		FromUI8(nVal);
		return;
	}
	// complément a 1
	m_TabVal[0] = (UINT64)nVal ;
	for (int i = 1; i < nSizeI8; i++)
	{
		m_TabVal[i] = ALLBIT1_64;
	}

}

// vers un INT64
UINT64 CInt256::nToUI8(void) const
{
	return m_TabVal[0];
}


// addition de 3 I4 avec gestion du carry
UINT32 _nAddI4WithCarry(UINT32  n1, UINT32 n2, UINT32 n3, OUT UINT32 *pnCarry)
{
	INT64 nRes64 = (UINT64)n1 + (UINT64)n2 + (UINT64)n3;
	*pnCarry = nRes64 > 0xFFFFFFFF;
	return     nRes64 & 0xFFFFFFFF;
}
void _MultI4(UINT32  n1, UINT32 n2, OUT UINT32 *pnLow, UINT32 *pnHigh)
{
	// on passe en 64 bits pour avoir les poids forts et faibles
	UINT64 nRes64 = (UINT64)n1 * (UINT64)n2;
	*pnLow  = (nRes64		& 0xFFFFFFFFL);
	*pnHigh = (nRes64 >> 32				 );
}

// Addition
void CInt256::Add(const CInt256 &clVal2)
{
	UINT32 nCarry = 0; // retenue
	// ajout des mots de 32 nits en commencant par le poids faible
	for (int i = 0; i < nSizeI4; i++)
	{
		XASSERT(nCarry == 0 || nCarry == 1);
		UINT32 nRes = _nAddI4WithCarry( _nGetI4(i), clVal2._nGetI4(i), nCarry, &nCarry);
		_SetI4(i, nRes );
	}
}
// ajuot d'un entier signé
void CInt256::AddI4(int nVal)
{
	CInt256 clVal(nVal);
	Add(clVal);
}
// Soustraction
void CInt256::Substract(const CInt256 &clVal2)
{
	CInt256 clValMoins2(clVal2);
	clValMoins2.Negate(); // v = -v;
	Add(clValMoins2);
}
// négation : x = -x
void CInt256::Negate(void)
{
	// complément a 2
	for (int i = 0; i < nSizeI8; i++)
	{
		m_TabVal[i] ^= ALLBIT1_64;
	}
	AddI4(1);
}


// renvoie le n°eme mot de 3é bits. 0 poids faible
UINT32 CInt256::_nGetI4(int nNumMot) const
{
	XASSERT(nNumMot >= 0);
	XASSERT(nNumMot <  nSizeI4);
	UINT32 *pnTabVal32 = (UINT32 *)m_TabVal;
	return pnTabVal32[nNumMot];
}
// mofigie le n°eme mot de 3é bits. 0 poids faible
void CInt256::_SetI4(int nNumMot, UINT32 nVal)
{
	XASSERT(nNumMot >= 0);
	XASSERT(nNumMot <  nSizeI4);
	UINT32 *pnTabVal32 = (UINT32 *)m_TabVal;
	pnTabVal32[nNumMot] = nVal;
}

//#include <math.h>
//use Math::BigInt;

// Multiplication par un entier 32
void CInt256::MultUI32(UINT32 nMultiplicateur )
{
	UINT32 nCarry = 0; // retenue
	CInt256 clRes;
					 
	// ajout des mots de 32 nits en commencant par le poids faible
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
// Multiplication par une autre int256
void CInt256::Mult(const CInt256 &clVal2)
{
	// on multiplie chaque DWORD par clVal2
	CInt256 clResut;
	for (int i = 0; i < nSizeI4; i++)
	{
		// x = x * i
		UINT32 nI = clVal2._nGetI4(i);
		CInt256 clMulI(*this);
		clMulI.MultUI32(nI);
		// x = x << i
		clMulI.MultPow2(i * 32);
		// res= res + x
		clResut.Add(clMulI);
	}
	// copie résultat
	*this = clResut;
}

int _nCmpUI64(UINT64 n1, UINT64 n2 )
{
	if (n1 < n2) return -1;
	if (n1 > n2) return 1;
	return 0;
}

// comparation < = >.
// renvoie -1 si on est < a clNombre2. 0 si égal. 1 si on est supérieur
int CInt256::nCompareU(const CInt256 &clNombre2) const
{
	// commence en comparant les poids forts
	for (int i = nSizeI8-1; i >=0 ; i--)
	{
		int nRes = _nCmpUI64(m_TabVal[i], clNombre2.m_TabVal[i]);
		if (nRes != 0)
		{
			// cas particulier des <
			/*if (clNombre2.bNegative() != bNegative())
			{
				if (bNegative()) return -1;
				//if (clNombre2.bNegative()) return 1;
			}*/


			return nRes;
		}
	}
	// egaux
	return 0;
}

// division par 2
void CInt256::DivideBy2(void)
{
	BOOL bNegatif = bNegative();
	// commence avecs les poids forts
	int nCarry = 0;
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
void CInt256::MultPow2(int nPow2)
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
			    nSizeInByte - NbByte); // len
		ZeroMemory(m_TabVal, NbByte); // des 0 sur les poids faibles
		nPow2 = nPow2 - NbByte * 8;
		if (nPow2 <= 0) return;
	}



	// commence avecs les poids faibles
	UINT64 nCarry = 0;
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
int CInt256::_nGetMagnitudeEtDivisteurRapide(OUT UINT32 *pnQuickDivisor) const
{
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
CInt256 CInt256::_clDivQuick(UINT32 nQuickDivisor, int nMagnitude) const
{
	if (bNegative())
	{
		CInt256 clCopie(*this);
		clCopie.Negate();
		CInt256 clRes = clCopie._clDivQuick(nQuickDivisor, nMagnitude);
		clRes.Negate();
		XASSERT(clRes.bNegative() || clRes.bIsZero() );// 0 aussi possible
		return clRes;
	}


	XASSERT(nMagnitude >= 0);
	XASSERT(nMagnitude < 256);
	// récup du numérateur divisé par la la magnitude
	// ex : 56584 => '5'
	UINT64 nNumerateur = _nGetI4(nMagnitude / 32);
	if (nMagnitude / 32 < nSizeI4 - 1)
	{
		// prends aussi le poids fort
		nNumerateur +=  (UINT64)_nGetI4(nMagnitude / 32 + 1) << 32;
	}
	UINT64 nQuotient = nNumerateur /(UINT64)nQuickDivisor;
	// MAJ résultat
	CInt256 clResultat;
	clResultat.FromUI8(nQuotient);
	return clResultat;
}


// renvoie la valeur abosolue
CInt256 CInt256::Abs(void) const
{
	if (!bNegative())
		return *this;

	// renvoie -this
	CInt256 clRes(*this);
	clRes.Negate();
	return clRes;
}

// Division entière de this
// ex : 2445 / 1000 => (2, 445)
void CInt256::Divide(const CInt256 &clDiviseur, OUT CInt256 *pclDivision, OUT CInt256* pclReste) const
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
	// --- algo itératif trouvé ici 
	// http://justinparrtech.com/JustinParr-Tech/an-algorithm-for-arbitrary-precision-integer-division/

	//M = length in digits of D, minus 1.  So if D is 1234, there are 4 digits, M = 3.
	UINT32 nQuickDivisorA;
	int nMagnitude = clDiviseur._nGetMagnitudeEtDivisteurRapide(&nQuickDivisorA);
	//A = D – ( D MOD 10^M ). ex 64367 => 60000, mais en Puissance de 2
	//= clDiviseur._nGetQuickDivisor(nMagnitude);
	CInt256 clQuotient = _clDivQuick(nQuickDivisorA, nMagnitude);
	CInt256 clReste = clDiviseur;
	clReste.AddI4(1);

	// While  ABS(R)>=D
	while (  clReste.Abs().nCompareU(clDiviseur) == 1 )
	{
		// R = N – (Q * D)
		CInt256 clTemp(clQuotient);
		clTemp.Mult(clDiviseur);
		clReste = *this;
		clReste.Substract(clTemp);

		// Qn = Q + R / A
		CInt256 clQuotientN = clReste._clDivQuick(nQuickDivisorA, nMagnitude);
		clQuotientN.Add(clQuotient);

		// Q = (Q + Qn) / 2
		CInt256 clNewQ = clQuotient;
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
	CInt256 clTemp(clQuotient);
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
void CInt256::FromStrBase10(PCXSTR pszVal)
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
		CInt256 clP(nValPfBase10);
		// x = x * 10 + p
		MultUI32(10);
		Add(clP);

		pCur++;
	}

}


// vers une chaine en base 10.
// nLenInChar doit faire au moins 79 charatères.
void CInt256::ToStrBase10(OUT PXSTR pszVal, int nLenInChar)
{
	// cas parrticulier du 0
	if (bIsZero())
	{
		*pszVal = '0';
		return;
	}

	// 1E77, valeur max puissance de 10
	static const int nPow2Max = 77;
	CInt256 i256Pow10Cur(1);
	// calcul de toutes les pussance de 10. de 1 à 10^77
	CInt256 Tabi256Pow10[nPow2Max + 1];
	//Tabi256Pow10[0] = 1L;
	for (int i = 0; i <= nPow2Max; i++)
	{
		Tabi256Pow10[i] = i256Pow10Cur;
		i256Pow10Cur.MultUI32(10);
	}
	// division par toutes les puissances de 10
	BOOL bChiffreAjoute = FALSE;
	CInt256 clCur(*this);
	int nPow2 = nPow2Max;
	PXSTR pCur = pszVal;
	while (nPow2 >= 0)
	{
		// Division entière
		// ex : 2445 / 1000 => (2, 445)
		const CInt256 &clPow10Cur = Tabi256Pow10[nPow2];
		CInt256 clDivision;
		CInt256 clReste;
		XASSERT(!clCur.bNegative());
		clCur.Divide(clPow10Cur, &clDivision, &clReste);
		XASSERT(!clReste.bNegative());
		// nombre a affecter a la pos courante
		int nNumber = (int)clDivision.nToUI8();
		XASSERT(nNumber >= 0);
		XASSERT(nNumber <= 9);
		// ajout du char si c'est 0 et pas encore ajouté
		if (nNumber != 0 || bChiffreAjoute)
		{
			char cNum = '0' + nNumber;
			*pCur = cNum;
			pCur++;
			bChiffreAjoute = TRUE;
		}

		// suivant
		clCur.CopieFrom( clReste );
		nPow2--;
	}

	//fermer la chaine
	*pCur = 0;
}

// Renvoie le carré 
CInt256 CInt256::Pow2(void) const
{
	CInt256 clRes(*this);
	CInt256 clRes2(*this);
	clRes.Mult(clRes2);
	return clRes2;
}
// Renvoie le cube 
CInt256 CInt256::Pow3(void) const
{
	CInt256 clRes(*this);
	CInt256 clRes2(*this);
	clRes.Mult(clRes2);
	clRes.Mult(clRes2); 
	return clRes2;
}
// renvoie le modulo = reste de la divistion par <clDiviseur>
CInt256 CInt256::Modulo(const CInt256 &clDiviseur) const
{
	CInt256 clDivision;
	CInt256 clReste;
	Divide(clDiviseur, &clDivision, &clReste);
	return clReste;
}





