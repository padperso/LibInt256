//#23.00 ECDSA_Test.cpp PAD
// tests de surface pour ECDSA

#include "central.h"

#include "EllipticCurve.h"


void ECDSA_Test_ALL(void)
{
	CEllipticCurve clsekp256k1("0","7","0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
//	CEllipticCurve clsekp256k1("0","7","17");



	CBigPoint2D P1;
	P1.SetXY("55066263022277343669578718895168534326250603453777594175500187360389116729240",
		     "32670510020758816978083085130507043184471273380659243275938904335757337482424");
	//P1.SetXY("1",  "12");



	XASSERT(clsekp256k1.bPointEstSurLaCourbe( P1 ) );

	CBigPoint2D MoinsP1 = P1;
	MoinsP1.m_clY = -MoinsP1.m_clY;
	MoinsP1.Modulo(clsekp256k1.clGetModulo() );
	CBigPoint2D P0 = clsekp256k1.Add(P1, MoinsP1);
	XASSERT(P0.bIsZero());


	// test : (P+P) + (-P) == P
	CBigPoint2D P2   = clsekp256k1.Add(P1, P1);
	CBigPoint2D P1_2 = clsekp256k1.Add(P2, MoinsP1);;
	XASSERT(P1_2 == P1);

	// test : (P+P) + P == P + (P + P)
	CBigPoint2D P3   = clsekp256k1.Add(P2, P1);
	CBigPoint2D P3_2 = clsekp256k1.Add(P1, P2);
	XASSERT(P3_2 == P3);

	// test : (P+P) + (P + P) == (P + P + P) + P
	CBigPoint2D P4 = clsekp256k1.Add(P3, P1);
	XASSERT(clsekp256k1.bPointEstSurLaCourbe(P4));
	CBigPoint2D P4_2 = clsekp256k1.Add(P2, P2);
	XASSERT(P4_2 == P4);

	// Test K * P
	CBigPoint2D P4_K = clsekp256k1.MultBigInt(P1, 4);
	XASSERT(P4_K == P4);
	CBigPoint2D P3_K = clsekp256k1.MultBigInt(P1, 3);
	XASSERT(P3_K == P3);


	printf("ECDSA test PASSED\n");
}
