// LibInt256.cpp : définit le point d'entrée pour l'application console.
//

#include "central.h"
#include <stdio.H>

#include "int256.h"
#include "BigIntFixed.h"

#include "src/BigInt.h"
#include "src/sha2_256.h"

//class  CInt256 : public CInt
void _Print(const CBigIntFixed &clNum)
{
	char szBuf[320] = {};
	clNum.ToStrBase10(szBuf, 320);
	printf(szBuf);
	printf("\r\n");

}

CBigIntFixed fEllipticBtc(const CInt256 &x, const CInt256 &y)
{
#define nTAILLE 1024/8
	CBigIntFixed bigX(nTAILLE);
	bigX.InitFromInt256( x );
	CBigIntFixed bigY(nTAILLE);
	bigY.InitFromInt256( y );

	CBigIntFixed P(nTAILLE);
	P.FromStrBase10("115792089237316195423570985008687907853269984665640564039457584007908834671663");
//	_Print(P);


	_Print(bigY);
	CBigIntFixed Y2 = bigY.Pow2();
	_Print(Y2);

	CBigIntFixed clRes(nTAILLE);
	clRes = bigX.Pow3();
//	_Print(clRes);

	clRes.AddI4(7);


	clRes.Substract(Y2);
//	_Print(clRes);

	
	CBigIntFixed clResModP(nTAILLE);
	clResModP = clRes.Modulo( P );
	return clResModP;
}


void ECDSA_Test_ALL(void);
void SHA_Test();


int main()
{
	SHA_Test();


	DWORD nStart = GetTickCount();

	CBigInt::sTEST_ALL();
	
	ECDSA_Test_ALL();

	DWORD nEnd = GetTickCount();
	printf("Time : %d", nEnd - nStart);
	Sleep(2000);



	return 0; 
	/*
	CBigIntFixed b1(512/8);

	b1.FromStrBase10("317");
	_Print(b1);
	b1.FromStrBase10("8913175891");
	_Print(b1);

	b1.FromStrBase10("115792089237316195423570985008687907853269984665640564039457584007908834671663");
	_Print( b1 );
	//printf(szBuf);

	CInt256 x;
	CInt256 y;
	//CInt256 fxy;



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

//	x.Modulo(y).ToStrBase10(szBuf, 80);
//	printf(szBuf);

	x.FromStrBase10("3212132335");
	x.FromStrBase10("3267051002075881697");

	x.FromStrBase10("55066263022277343669578718895168534326250603453777594175500187360389116729240");
	y.FromStrBase10("32670510020758816978083085130507043184471273380659243275938904335757337482424");

	CBigIntFixed fxy(nTAILLE);
	fxy = fEllipticBtc(x, y);
	_Print(fxy);
	
    return 0;
	*/
}

