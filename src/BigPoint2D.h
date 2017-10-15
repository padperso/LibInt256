//#23.00A BigPoint2D.h PAD
// point ZD en coord BigInt.
// pour ECDSA : Elliptic Curve Digital Signature Algorithm

#ifndef __BIGPOINT2D_H
#define __BIGPOINT2D_H

#include "BigInt.h"


typedef const class CBigPoint2D &RCCBigPoint2D;

class CBigPoint2D
{
public:
	// coordonées X/Y
	CBigInt m_clX;
	CBigInt m_clY;
public:
	// constructeur du point 0,0;
	CBigPoint2D(void) {};
	// Init a partir de coord x,y en chaines décimales
	void SetXY(PCXSTR pszX, PCXSTR pszY);
	// Init a partir de coord x,y en bigint
	void SetXY(const CBigInt X, const CBigInt Y);
	// opérateur d"galité de 2 points
	friend bool operator == (RCCBigPoint2D A, RCCBigPoint2D B);
	// appliquer le mme modulo a X et Y
	void Modulo(CBigInt mod);
	// le point est  0,0 ?
	BOOL bIsZero(void) const;

#ifdef _DEBUG
	void DBG_Print(void) const;
#endif//_DEBUG		
};


#endif//!__BIGPOINT2D_H
