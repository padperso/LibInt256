// LibInt256.cpp : définit le point d'entrée pour l'application console.
//

#include "central.h"
#include <stdio.H>

#include "int256.h"

int main()
{
	CInt256 x;



	x.FromUI8(44);
	x.FromI4(-44);
	x.DivideBy2();
	x.FromI4(-22);

	x.FromI4(713);
	CInt256 clPow10Cur(10);
	CInt256 clDivision;
	CInt256 clReste;
	x.Divide(clPow10Cur, &clDivision, &clReste);


	x.FromStrBase10("321");
	x.FromStrBase10("3212132335");
	x.FromStrBase10("3267051002075881697");

	x.FromStrBase10("32670510020758816978083085130507043184471273380659243275938904335757337482424");

	char szBuf[80] = {};
	x.ToStrBase10(szBuf, 80);
	printf(szBuf);

    return 0;

}

