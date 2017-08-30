//23.00A BigInt_Test.cpp PAD
// tests de surface pour BigInt.cpp

#include "central.h"

#include "BigInt.h"



void TEST_Base()
{
	CBigInt x;
	XASSERT(x == 0);
	XASSERT(x.bIsZero());
	x = 77;
	XASSERT(x == 77);
	x = -1;
	XASSERT(x == -1);

	CBigInt y;
	x = 555;
	y = x;
	XASSERT(y == 555);
}
void TEST_AdditionInt( int nx, int ny, int  res)
{
	CBigInt x,y,z;

	x = nx;
	y = ny;
	z = x + y;
	XASSERT(z == res);
}

void TEST_PlusEgal(int nx, UINT32 ny, int  res)
{
	CBigInt x;

	x = nx;
	x += ny;
	
	XASSERT(x == res);


}
void TEST_Addition()
{
	TEST_AdditionInt(0,   0,  0);
	TEST_AdditionInt(2,   3,  5);
	TEST_AdditionInt(2,  -3, -1);
	TEST_AdditionInt(-2, -3, -5);
	TEST_AdditionInt(9,   3, 12);

	TEST_PlusEgal(2,  1,  3);
	TEST_PlusEgal(5, 18, 23);
	TEST_PlusEgal(2,  0,  2);
	TEST_PlusEgal(7,  1,  8);


}

void TEST_1Neg(int nx)
{
	CBigInt x;

	x = nx;
	x.Negate();

	XASSERT(x == -nx);
}

void TEST_Negation()
{
	TEST_1Neg(0);
	TEST_1Neg(1);
	TEST_1Neg(2);
	TEST_1Neg(-1);
	TEST_1Neg(-2);
	TEST_1Neg(12345);
}

void TEST_SoustractionInt(int na, int nb)
{
	CBigInt x,y,z;

	x = na;
	y = nb;
	z = x - y;
	XASSERT(z == (na - nb));
	
}
void TEST_Sub1_Str(PCXSTR psza, PCXSTR pszb, PCXSTR pszRes)
{
	// init opérandes
	CBigInt x, y, z;
	x.FromStrBasePrefix(psza);
	y.FromStrBasePrefix(pszb);

	// calcul
	z = x - y;

	// verif
	char szBuf[32000] = {};
	z.ToStrBase10(szBuf, 32000);
	XASSERT(strcmp(szBuf, pszRes) == 0);

}

void TEST_Soustraction()
{

	TEST_SoustractionInt(0, 0);
	TEST_SoustractionInt(9, 3);
	TEST_SoustractionInt(2, 3);
	TEST_SoustractionInt(2, -3);
	TEST_SoustractionInt(-2, -3);


	TEST_Sub1_Str("0xFFFFFFFFFFFFFFFF8", "0xFFFFFFFFFFFFFFFF1", "7");

	TEST_Sub1_Str("9096879970699258149176655839548040467220386434145626447723889017111182015140877686503696263564847063551872941024380603471146130769567925511021321259057152",
		"9096879970699258149176655839548040467220386434145626447723889017111182015140877686503696263564847063551872941024380603471146130769567925511021321259057152",
		"0");
	/*
	TEST_Sub1_Str("9096879969715887394191281126224076706179546486010875701472248896657053899503113553599556820590024283086264984352079439993111553479636577925323306412932800",
		"9096879970699258149176655839548040467220386434145626447723889017111182015140877686503696263564847063551872941024380603471146130769567925511021321259057152",
		"-983370754985374713323963761040839948134750746251640120454128115637764132904139442974822780465607956672301163478034577289931347585698014846124352");
	*/
}

void TEST_Mult1Int(int na, int nb)
{
	CBigInt x;

	x = na;
	x.MultUI32( nb );
	INT64 nRes = (INT64)na * (INT64)nb;
	XASSERT(x == nRes);
}
void TEST_MultI4()
{
	TEST_Mult1Int(0, 0);
	TEST_Mult1Int(10, 0);
	TEST_Mult1Int(10, 1);
	TEST_Mult1Int(7, 3);
	TEST_Mult1Int(-7, 1);
	TEST_Mult1Int(-7, 3);
	TEST_Mult1Int(9999999, 10000000);
}	

void TEST_Mult1(int na, int nb)
{
	CBigInt x,y,z;

	x = na;
	y = nb;
	z = x * y;
	INT64 nRes = (INT64)na * (INT64)nb;
	XASSERT(z == nRes);
}
void TEST_Mult()
{
	TEST_Mult1(0, 0);
	TEST_Mult1(10, 0);
	TEST_Mult1(10, 1);
	TEST_Mult1(10, -1);
	TEST_Mult1(7, 3);
	TEST_Mult1(-7, 1);
	TEST_Mult1(-7, 3);
	TEST_Mult1(-7, -30);
	TEST_Mult1(9999999, 10000000);
	TEST_Mult1(99999899, 10000000);
}

void TEST_Div1(int na, int nb)
{
	CBigInt x, y, z;

	x = na;
	y = nb;
	z = x / y;
	INT64 nRes = (INT64)na / (INT64)nb;
	XASSERT(z == nRes);

}
void TEST_Div1_Str(PCXSTR psza, PCXSTR pszb, PCXSTR pszRes)
{
	CBigInt x, y, z;
	x.FromStrBasePrefix(psza);
	y.FromStrBasePrefix(pszb);
	// calcul

	z = x / y;
	// verif
	char szBuf[32000] = {};
	z.ToStrBase10(szBuf, 32000);
	XASSERT( strcmp(szBuf, pszRes)== 0);
}
void TEST_Modulo1(int na, int nb, INT64 nRes)
{
	CBigInt x, y, z;

	x = na;
	y = nb;
	z = x.Modulo( y );
	//INT64 nRes = (INT64)na % (INT64)nb;
	XASSERT(z == nRes);

}


void TEST_Div()
{
	TEST_Div1(10, -1);
	TEST_Div1(10, 10);
	TEST_Div1(0, 5);
	TEST_Div1(10, 1);

	TEST_Div1(7, 3);
	TEST_Div1(-127, 3);
	TEST_Div1(21, 3);
	TEST_Div1(12130, -30);
	TEST_Div1(9999999, 100);
	TEST_Div1(99999899, 10);


	TEST_Modulo1(-10, 17, 7);
	TEST_Modulo1(17, 10 , 7);

/*
	TEST_Div1_Str("9096879969715887394191281126224076706179546486010875701472248896657053899503113553599556820590024283086264984352079439993111553479636577925323306412932800",
		"65341020041517633956166170261014086368942546761318486551877808671514674964848",
		"139221578786736675480792904055773408246814158120981180980253974860784918464552");
	*/
		

}


void TEST_DivPar2_1(int na)
{
	CBigInt x;
	x = na;
	x.DivideBy2();
	XASSERT(x== na/2);
}
void TEST_DivPar2()
{
	TEST_DivPar2_1(0);
	TEST_DivPar2_1(10);
	TEST_DivPar2_1(11);
	TEST_DivPar2_1(-1);
	TEST_DivPar2_1(-18);
	TEST_DivPar2_1(-7);
	TEST_DivPar2_1(9999999);
}

void TEST_FromStrBase10_1(char *psz)
{
	CBigInt x;

	x.FromStrBase10(psz);

	INT64 nRes = atoi(psz);
	XASSERT(x == nRes);
}
void TEST_FromStrBase10(void)
{
	TEST_FromStrBase10_1("0");
	TEST_FromStrBase10_1("1");
	TEST_FromStrBase10_1("-1");
	TEST_FromStrBase10_1("2");
	TEST_FromStrBase10_1("9");
	TEST_FromStrBase10_1("10");
	TEST_FromStrBase10_1("11");
	TEST_FromStrBase10_1("1100");
	TEST_FromStrBase10_1("123456789");
	TEST_FromStrBase10_1("-123456789");
}

void TEST_FromStrBase16_1(char *psz)
{
	CBigInt x;

	x.FromStrHexa(psz);

	INT64 nRes = strtoul(psz, NULL, 16);
	XASSERT(x == nRes);
}
void TEST_FromStrBase16(void)
{
	TEST_FromStrBase16_1("00");
	TEST_FromStrBase16_1("10");
	TEST_FromStrBase16_1("100");
	TEST_FromStrBase16_1("0a");
	TEST_FromStrBase16_1("2f");
	TEST_FromStrBase16_1("9AA5");
	TEST_FromStrBase16_1("10FFFF");
	TEST_FromStrBase16_1("af1F1");
	TEST_FromStrBase16_1("12345678");
	TEST_FromStrBase16_1("9abcdef0");

}

// test pour 1 valconvetion enbase  10
void TEST_Base10_1(PCXSTR pszValTest)
{
	CBigInt x;
	x.FromStrBase10(pszValTest);

	char buff[256];
	x.ToStrBase10(buff, 256);

	XASSERT(strcmp(pszValTest, buff) == 0);
}

void TEST_Base10(void)
{
	TEST_Base10_1("0");
	TEST_Base10_1("1");
	TEST_Base10_1("-1");
	TEST_Base10_1("10");
	TEST_Base10_1("65635");
	TEST_Base10_1("1023456789");
	TEST_Base10_1("102345678901234567890");
	TEST_Base10_1("11579208923731619542357098500868790785326998");
	TEST_Base10_1("115792089237316195423570985008687907853269984665640564039457584007908834671663");
	TEST_Base10_1("-115792089237316195423570985008687907853269984665640564039457584007908834671663");
}


//#include <crtdbg.h>  
void CBigInt::sTEST_ALL(void)
{
	//_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);

//	TEST_Div1_Str("9096879969715887394191281126224076706179546486010875701472248896657053899503113553599556820590024283086264984352079439993111553479636577925323306412932800",
//				  "65341020041517633956166170261014086368942546761318486551877808671514674964848",
//				  "139221578786736675480792904055773408246814158120981180980253974860784918464552");


	TEST_Base();
	TEST_Addition();
	TEST_Negation();
	TEST_Soustraction();
	TEST_MultI4();
	TEST_Mult();
	TEST_DivPar2();
	TEST_Div();
	TEST_FromStrBase10();
	TEST_FromStrBase16();
	TEST_Base10();

	printf("BigInt test PASSED\n");
}