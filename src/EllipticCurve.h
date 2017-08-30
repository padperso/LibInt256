//#23.00A EllipticCurve.h PAD
//

#include "BigPoint2D.h"


// représente une courbe elliptique discrete 
// y2 = (x3 + ax + b) [mod n]
// sekp256k1. https://en.bitcoin.it/wiki/Secp256k1
//p = FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFF FFFFFFFE FFFFFC2F
// y2 = X3 + 7 [ mod p]
class CEllipticCurve
{
protected:
	// coeffient de y2 = (x3 + ax + b) [mod n]
	CBigInt m_clA;
	CBigInt m_clB;
	// modulo n
	CBigInt m_clModulo;

public:
	// contruit une courbe elliptique.. init avec des nombres Hexa ou décimaux.
	CEllipticCurve(PCXSTR pszA, PCXSTR pszB, PCXSTR pszModulo);

	// récup du modulo 
	CBigInt clGetModulo(void) const { return m_clModulo; }


	// Inidique si un point est sur la courbre
	BOOL bPointEstSurLaCourbe(RCCBigPoint2D clPoint) const;

	// Addition de 2 points
	CBigPoint2D Add(RCCBigPoint2D A, RCCBigPoint2D B);

	// Multiplication d'un poiunt pas un entier au sens de l'addition précédente
	CBigPoint2D MultBigInt(RCCBigPoint2D A, CBigInt K);

protected:
	// Addition d'un point avec lui meme
	CBigPoint2D _clPointDoubling(RCCBigPoint2D A) const
		;
};
