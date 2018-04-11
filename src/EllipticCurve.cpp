//#23.00A EllipticCurve.cpp PAD


#include "central.h"

#include "EllipticCurve.h"

// contruit une courbe elliptique.. init avec des nombres Hexa ou décimaux.
CEllipticCurve::CEllipticCurve(PCXSTR pszA, PCXSTR pszB, PCXSTR pszModulo, PCXSTR pszOrdre)
{
	m_clA.FromStrBasePrefix(pszA);
	m_clB.FromStrBasePrefix(pszB);
	m_clModulo.FromStrBasePrefix(pszModulo);
	XASSERT(!m_clModulo.bNegative());
	m_clOrdre.FromStrBasePrefix(pszOrdre);
	XASSERT(!m_clModulo.bNegative());
}


// Indique si un point est sur la courbre
// ie : y2 = (x3 + ax + b)[mod n]
BOOL CEllipticCurve::bPointEstSurLaCourbe(RCCBigPoint2D clPoint) const
{
	XASSERT(m_clA.bIsZero()); // cas non géré

	CBigInt clY2 = clPoint.m_clY.Pow2();
//	CBigInt clX3 = clPoint.m_clX.Pow3() + m_clB ;
	CBigInt clX3 = clPoint.m_clX.Pow2().Modulo(m_clModulo);
	clX3 = clX3*clPoint.m_clX + m_clB;

	CBigInt clY2_Mod = clY2.Modulo(m_clModulo);
	CBigInt clX3_Mod = clX3.Modulo(m_clModulo);

	// le point si sur la courbe si égalité
	return clY2_Mod == clX3_Mod;
}

// Addition d'un point avec lui meme
CBigPoint2D CEllipticCurve::_clPointDoubling(RCCBigPoint2D A) const
{
	// formule trouvé sur le net :
	// L = (3x^2 + a) / (2*y)-1  Modulo P
	// NB : ici a=0.
	CBigInt N = (3 * A.m_clX*A.m_clX).Modulo(m_clModulo);
	CBigInt D = (2 * A.m_clY);
	CBigInt L = N * D.InvertModulo(  m_clModulo );
	L = L.Modulo(m_clModulo);

	CBigPoint2D clRes;
	CBigInt RX  = (L*L - ( A.m_clX + A.m_clX )).Modulo(m_clModulo);
	clRes.m_clX = RX; 
	clRes.m_clY = -(L*(RX - A.m_clX ) + A.m_clY);
	clRes.Modulo(m_clModulo);

#ifdef	DEBUG_LENT
	XASSERT(bPointEstSurLaCourbe(clRes));
#endif//DEBUG_LENT
	return clRes;
}

// Addition de 2 points
CBigPoint2D CEllipticCurve::Add(RCCBigPoint2D A, RCCBigPoint2D B)
{
	// cas du 0
	if (A.bIsZero()) return B;
	if (B.bIsZero()) return A;

#ifdef DEBUG_LENT
	 XASSERT(bPointEstSurLaCourbe(A));
	 XASSERT(bPointEstSurLaCourbe(B));
#endif//DEBUG_LENT

	// si les points sont confondus
	if (A == B)
	{
		// A+A
		return _clPointDoubling( A );
	}

	// si A et B sont inverse
	if (   A.m_clX ==  B.m_clX
		//&& A.m_clY == (-B.m_clY).Modulo(m_clModulo))
		&& A.m_clY != B.m_clY
	  )
	{
		// le résultat est 0.
		//printf("=0\n");
		CBigPoint2D pt0;
		return pt0;
	}

	XASSERT(A.m_clX != B.m_clX);


	// formule trouvé sur le net :
	// L = (y1 - y2) / (x1 - x2)-1  Modulo P
	// NB : ici a=0.
	CBigInt N = A.m_clY - B.m_clY;
	CBigInt D = A.m_clX - B.m_clX;
	CBigInt L = (N * D.InvertModulo(m_clModulo)).Modulo(m_clModulo);

	CBigPoint2D clRes;
	clRes.m_clX = L*L - A.m_clX - B.m_clX;
	clRes.Modulo(m_clModulo);
	clRes.m_clY = -(A.m_clY + L * (clRes.m_clX -  A.m_clX ));
	clRes.Modulo(m_clModulo);

#ifdef DEBUG_LENT
	XASSERT(bPointEstSurLaCourbe(clRes));
#endif//#ifdef DEBUG_LENT
	return clRes;

}

bool gbCacheInit = false;
CBigPoint2D *clAKPow2_Cache; // [256];


// VERSION pour génération
/*
#define POUR_GENERATION
void CEllipticCurve::_InitCache(RCCBigPoint2D A)
{
	if (gbCacheInit) return;

	clAKPow2_Cache = new CBigPoint2D[256];

	CBigPoint2D clAKPow2= A;

	for (int i = 0; i < 256; i++)
	{
		clAKPow2_Cache[i] = clAKPow2;

#ifdef POUR_GENERATION
		printf("\r\n // ---- 2^%d = ", i);
		printf("\r\n{\"0x");
		clAKPow2.m_clX.DBG_Print(DBGPRINT_NORALIGN | DBGPRINT_NORC);
		printf("\",\"0x");
		clAKPow2.m_clY.DBG_Print(DBGPRINT_NORALIGN | DBGPRINT_NORC);
		printf("\"},");
#endif//POUR_GENERATION		


		// clAKPow2 = clAKPow2*clAKPow2
		clAKPow2 = Add( clAKPow2, clAKPow2);
	}

	gbCacheInit = true;
}
*/

void GetValCacheEC(int nPow2, OUT CBigInt &X, OUT CBigInt &Y);

bool CEllipticCurve::_bInitCache(RCCBigPoint2D A)
{
	// OK Que si X = 0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798
	//	      et Y = 0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8
	CBigPoint2D clAKPow2;
	GetValCacheEC(0, clAKPow2.m_clX, clAKPow2.m_clY);
	if (!(clAKPow2 == A))
		return false; // cache pas utilisable

	if (gbCacheInit) return true;


	// alloc cache
	clAKPow2_Cache = new CBigPoint2D[256];


	for (int i = 0; i < 256; i++)
	{
		// récup valeur dans tableau pré-calculé
		GetValCacheEC(i, clAKPow2.m_clX, clAKPow2.m_clY);
		// init cache valeur i
		clAKPow2_Cache[i] = clAKPow2;
	}

	gbCacheInit = true;
	return true;
}

// Multiplication d'un point par un entier au sens de l'addition précédente
// version sans cacke
CBigPoint2D CEllipticCurve::_MultBigInt_NoCache(RCCBigPoint2D A, const CBigInt &K)
{
	CBigPoint2D clRes;
	// A * K*2^i
	CBigPoint2D clAKPow2= A;
	int nNbBit = K.nSizeInBit();
	for (int i = 0; i < nNbBit; i++)
	{
		// récup bit I
		int nBitI = K.nGetBit(i);
		// si bit mis
		if (nBitI)
		{
			// R += K*2^î
			clRes = Add( clRes, clAKPow2);
		}

		// APow2 = 2*APow2
		clAKPow2 = Add( clAKPow2, clAKPow2);
	}

	return clRes;
}

// Multiplication d'un point par un entier au sens de l'addition précédente
CBigPoint2D CEllipticCurve::MultBigInt(RCCBigPoint2D A, const CBigInt &K)
{
	CBigPoint2D clRes;

	if (!_bInitCache(A))
	{
		return _MultBigInt_NoCache(A, K);
	}
		


	int nNbBit = K.nSizeInBit();
	for (int i = 0; i < nNbBit; i++)
	{
		

		// récup bit I
		int nBitI = K.nGetBit(i);
		// si bit mis
		if (nBitI)
		{
			//XASSERT(clAKPow2_Cache[i] == clAKPow2);
			// R += K*2^î
			clRes = Add( clRes, clAKPow2_Cache[i]);
		}

		// clAKPow2 = clAKPow2*clAKPow2
		//clAKPow2_Cache[i] = clAKPow2;
		//clAKPow2 = Add( clAKPow2, clAKPow2);

	}

	return clRes;
}

