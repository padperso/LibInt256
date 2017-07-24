// LibInt256.cpp : définit le point d'entrée pour l'application console.
//

#include "central.h"
#include <stdio.H>

#include "int256.h"



CInt256 f(const CInt256 &x, const CInt256 &y)
{
	CInt256 P;
	P.FromStrBase10("115792089237316195423570985008687907853269984665640564039457584007908834671663");

	CInt256 clRes = x.Pow3();
	clRes.AddI4(7);
	CInt256 Y2 = y.Pow2();
	clRes.Substract(Y2);
	
	
	return clRes.Modulo( P );
}


int main()
{
	CInt256 x;
	CInt256 y;
	CInt256 fxy;



	x.FromUI8(44);
	x.FromI4(-44);
	x.DivideBy2();
	x.FromI4(-22);

	x.FromI4(713);
	CInt256 clPow10Cur(10);
	CInt256 clDivision;
	CInt256 clReste;
	x.Divide(clPow10Cur, &clDivision, &clReste);


	x.FromStrBase10("327");
	y.FromStrBase10("10");

	char szBuf[80] = {};
	x.Modulo(y).ToStrBase10(szBuf, 80);
	printf(szBuf);

	x.FromStrBase10("3212132335");
	x.FromStrBase10("3267051002075881697");

	x.FromStrBase10("55066263022277343669578718895168534326250603453777594175500187360389116729240");
	y.FromStrBase10("32670510020758816978083085130507043184471273380659243275938904335757337482424");

	fxy = f(x, y);
	


	//char szBuf[80] = {};
	x.ToStrBase10(szBuf, 80);
	printf(szBuf);

    return 0;

}

