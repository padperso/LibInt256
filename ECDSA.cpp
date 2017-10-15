//#23.00 ECDSA.cpp PAD
// points d'entr�e pour ECDSA

#include "central.h"
#include "BigInt.h"
#include "EllipticCurve.h"

CEllipticCurve clsekp256k1("0", "7", "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
#define nCOORDX_Secp256k1 "55066263022277343669578718895168534326250603453777594175500187360389116729240"
#define nCOORDY_Secp256k1 "32670510020758816978083085130507043184471273380659243275938904335757337482424"

// G�n�ration d'un nombre random sur 256 bits
CBigInt GetHasardCrypto256(void) const
{
	// r�cup d'une cl� priv�e
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

	// cr�e un entier a partir de cette valeur al�atoire
	CBigInt SecretKey;
	SecretKey.FromRawBytes(pbData,32);
	return SecretKey;
}


// signature de <HashToSign> avec <PrivateKey>
void ECDSA_Sign(const CBigInt &HashToSign,   // valeur a signer
	const CBigInt &PrivateKey,		// cl� priv�e (a garder secr�te)
	OUT CBigPoint2D *pPublicKey		// r�sultat
			)
{
	// Init point 
	CBigPoint2D P1;
	P1.SetXY(nCOORDX_Secp256k1, nCOORDY_Secp256k1);

	// choix d'un nombre au hasard K
	CBigInt K = GetHasardCrypto256();

	// calcul de P2 : K * P1
	CBigPoint2D P2 = clsekp256k1.MultBigInt(P1, K);

	PublicKey.m_clX.DBG_Print();
	PublicKey.m_clY.DBG_Print();

}
