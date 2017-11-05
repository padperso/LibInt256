//#23.00 ECDSA_Test.cpp PAD
// tests de surface pour ECDSA

#include "central.h"

#include "EllipticCurve.h"
#include "ECDSA.h"

#define TESTME( X )  if (!(X))  { XASSERT(FALSE); printf("** FAILED ** \n" ); exit(-1); }

CEllipticCurve clsekp256k1("0", "7", "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F",
									 "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");

// test addition/multiplication....
void ECDSA_Test_BaseFunc(void)
{
	//	CEllipticCurve clsekp256k1("0","7","17");



	CBigPoint2D P1;
	P1.SetXY("55066263022277343669578718895168534326250603453777594175500187360389116729240",
			 "32670510020758816978083085130507043184471273380659243275938904335757337482424");
	//P1.SetXY("1",  "12");



	TESTME(clsekp256k1.bPointEstSurLaCourbe(P1));

	CBigPoint2D MoinsP1 = P1;
	MoinsP1.m_clY = -MoinsP1.m_clY;
	MoinsP1.Modulo(clsekp256k1.clGetModulo());
	CBigPoint2D P0 = clsekp256k1.Add(P1, MoinsP1);
	TESTME(P0.bIsZero());


	// test : (P+P) + (-P) == P
	CBigPoint2D P2 = clsekp256k1.Add(P1, P1);
	CBigPoint2D P1_2 = clsekp256k1.Add(P2, MoinsP1);;
	TESTME(P1_2 == P1);

	// test : (P+P) + P == P + (P + P)
	CBigPoint2D P3 = clsekp256k1.Add(P2, P1);
	CBigPoint2D P3_2 = clsekp256k1.Add(P1, P2);
	TESTME(P3_2 == P3);

	// test : (P+P) + (P + P) == (P + P + P) + P
	CBigPoint2D P4 = clsekp256k1.Add(P3, P1);
	TESTME(clsekp256k1.bPointEstSurLaCourbe(P4));
	CBigPoint2D P4_2 = clsekp256k1.Add(P2, P2);
	TESTME(P4_2 == P4);

	// Test K * P
	CBigPoint2D P4_K = clsekp256k1.MultBigInt(P1, 4);
	TESTME(P4_K == P4);
	CBigPoint2D P3_K = clsekp256k1.MultBigInt(P1, 3);
	TESTME(P3_K == P3);


}



// test signature
void ECDSA_Test_SignParam(PCXSTR pszK, PCXSTR pszX, PCXSTR pszY)
{
	CBigInt HashToSign(99);
	CBigInt PrivKey(pszK); //"0x8eee2c6fe6c375dda3f5897149b5aa133ae63d53dba817bcb8e464bb36074b86");
	
	// récup de la clé publique
	CBigPoint2D PublicKey;
	PublicKey = ECDSA_GetPublicKey( PrivKey );
	//PublicKey.m_clX.DBG_Print();
	//PublicKey.m_clY.DBG_Print();
	// résultat a trouver ( trouvé via ecda ptyhon)
	CBigInt clPKRes_X(pszX); // "0xaf909720eb1d1aa3b9f9c9878f8758990349f8d4a87989af798bef3f8227e461");
	CBigInt clPKRes_Y(pszY); //"0xc5b285e2482a87667f4ce1e2da69dca4745fdf69daeff76a3d642c5a6ea34bef");
	TESTME(PublicKey.m_clX == clPKRes_X);
	TESTME(PublicKey.m_clY == clPKRes_Y);


	// -- Signature ---
	CBigPoint2D Signature;
	ECDSA_Sign(HashToSign,PrivKey, &Signature );
	//Signature.m_clX.DBG_Print();
	//Signature.m_clY.DBG_Print();


	// vérif
	BOOL bOK = ECDSA_bCheckSign(HashToSign, Signature, PublicKey );
	TESTME(bOK);
	if (!bOK)
	{
		printf("ECDSA_bCheckSign FAILED\n");
	}



	/*
	CBigPoint2D P1;
	P1.SetXY("55066263022277343669578718895168534326250603453777594175500187360389116729240",
			 "32670510020758816978083085130507043184471273380659243275938904335757337482424");


	// récup d'une clé privée
	HCRYPTPROV   hCryptProv = NULL;
	BYTE         pbData[32] = {};

	CryptAcquireContext(
		&hCryptProv,               // handle to the CSP
		NULL,                  // container name 
		NULL,                      // use the default provider
		PROV_RSA_FULL,             // provider type
		CRYPT_VERIFYCONTEXT);                        // flag values
	XASSERT(hCryptProv != NULL);

	if (CryptGenRandom(
		hCryptProv,
		32,
		pbData))
	{
		printf("Random sequence generated. \n");
	}
	else
	{
		printf("Error during CryptGenRandom.\n");
		exit(1);
	}

	// crée un entier a partir de cette valeur aléatoire
	CBigInt SecretKey;
	SecretKey.FromRawBytes(pbData,32);
	// -- affiche la cél secrete
	SecretKey.DBG_Print();
	// crée la clé publique : K * P
	CBigPoint2D PublicKey = clsekp256k1.MultBigInt(P1, SecretKey);
	PublicKey.m_clX.DBG_Print();
	PublicKey.m_clY.DBG_Print();
	*/
}

// test signature
void ECDSA_Test_Sign( )
{
	ECDSA_Test_SignParam(
		  "0x8eee2c6fe6c375dda3f5897149b5aa133ae63d53dba817bcb8e464bb36074b86"
		, "0xaf909720eb1d1aa3b9f9c9878f8758990349f8d4a87989af798bef3f8227e461"
		, "0xc5b285e2482a87667f4ce1e2da69dca4745fdf69daeff76a3d642c5a6ea34bef");

	// valeurs validée avec https://bitcore.io/playground/#/address
	ECDSA_Test_SignParam(
		  "0x0514fd4c12e20b91cbaafaa47318ce467da10309dc2ef052fd425d4614a63d01"
		, "0x82e964a742f419ccd9a6a9bacd361feecb3be7f0bb468d392ec5000fea6d6833"
		, "0xe4e601c15a1c693362bac4cb32e2c640e6a16dd2cf8826eccc9c6e0b977a4ac2");


}


void ECDSA_Test_ALL(void)
{
	ECDSA_Test_Sign();

	ECDSA_Test_BaseFunc();

	printf("ECDSA test PASSED\n");
}
