//#23.00A BigPoint2D.h PAD
// point ZD en coord BigInt.
// pour ECDSA : Elliptic Curve Digital Signature Algorithm

#include "central.h"

#include "BigPoint2D.h"

void CBigPoint2D::SetXY(PCXSTR pszX, PCXSTR pszY)
{
	m_clX.FromStrBase10(pszX);
	m_clY.FromStrBase10(pszY);
}
// Init a partir de coord x,y en bigint
void CBigPoint2D::SetXY(const CBigInt X, const CBigInt Y)
{
	m_clX = X;
	m_clY = Y;
}

// opérateur d"galité de 2 points
bool operator == (RCCBigPoint2D A, RCCBigPoint2D B)
{
	return    A.m_clX == B.m_clX
		   && A.m_clY == B.m_clY;
}
// appliquer le mme modulo a X et Y
void CBigPoint2D::Modulo(CBigInt mod)
{
	m_clX = m_clX.Modulo(mod);
	m_clY = m_clY.Modulo(mod);
}
// le point est  0,0 ?
BOOL CBigPoint2D::bIsZero(void) const
{
	return	   m_clX.bIsZero()
			&& m_clY.bIsZero();
}

#ifdef _DEBUG
void CBigPoint2D::DBG_Print(void) const
{
	printf("(x=");
	m_clX.DBG_Print( DBGPRINT_NORC + DBGPRINT_NORALIGN );
	printf(",y=");
	m_clY.DBG_Print( DBGPRINT_NORC + DBGPRINT_NORALIGN );
	printf(")\n");
}
#endif//_DEBUG
