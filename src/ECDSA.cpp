//#23.00 ECDSA.cpp PAD
// points d'entr�e pour ECDSA

#include "central.h"
#include "BigInt.h"
#include "EllipticCurve.h"

//	secp256k1.P = fromHex("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F")
//	secp256k1.N = fromHex("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141")
//	secp256k1.B = fromHex("0000000000000000000000000000000000000000000000000000000000000007")
//	secp256k1.Gx = fromHex("79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798")
//	secp256k1.Gy = fromHex("483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8")

//CBigInt clN_
CEllipticCurve gclsekp256k1("0", "7", "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F", 
									  "0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141");
//										 FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141
//#define nCOORDX_Secp256k1 "55066263022277343669578718895168534326250603453777594175500187360389116729240"
//#define nCOORDY_Secp256k1 "32670510020758816978083085130507043184471273380659243275938904335757337482424"
#define nCOORDX_Secp256k1 "0x79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798"
#define nCOORDY_Secp256k1 "0x483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8"
							 

// Init point d'origine P1 
CBigPoint2D P1;



// G�n�ration d'un nombre random sur 256 bits
CBigInt GetHasardCrypto256(void) 
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
		//printf("Random sequence generated. \n");
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
				OUT CBigPoint2D *pSignature		// r�sultat
			)
{
	P1.SetXY(nCOORDX_Secp256k1, nCOORDY_Secp256k1);

	BOOL bkValide = FALSE;
	CBigInt K; // choisi au hasard
	CBigInt X,Y; // r�sultat
	CBigPoint2D P2;
	const CBigInt N = gclsekp256k1.clGetOrdre();
	while (!bkValide)
	{
		// choix d'un nombre au hasard K
		K = GetHasardCrypto256();
		// doit �tre positif
		while (K.bNegative())
			K = GetHasardCrypto256();

		// calcul de P2 : K * P1
		P2 = gclsekp256k1.MultBigInt(P1, K);
		X = P2.m_clX.Modulo(N);
		// si X mod N == 0 on doit choisir un autre  K
		bkValide = !X.bIsZero();
		// le bit de poids fort ne doit pas �tre a 1, sinon x/y sont consid�r� comme n�gatifs
		// et rejet�.
		if (X.nGetBit(255) == 1)
			bkValide = FALSE;
		if (bkValide)
		{
			// calcul de Y
			Y = K.InvertModulo(N) * (HashToSign + PrivateKey * X);
			Y = Y.Modulo(N);
			// si Y mod N == 0 on doit choisir un autre  K
			bkValide = !Y.bIsZero();
			//@A revoir
			if (Y.nGetBit(255) == 1)
				bkValide = FALSE;
		}
	}//while (bkValide)

	// le bit de poids fort ne doit pas �tre a 1, sinon x/y sont consid�r� comme n�gatifs
	// et rejet�.
	if (X.nGetBit(255) == 1)
	{ 
		//X =  -X;
		//X = X.Modulo(N);
	}
	if (Y.nGetBit(255) == 1)
	{
		//Y =  Y - N;
		//Y = Y.Modulo(N);
	}

	// r�sultat
	pSignature->SetXY( X, Y);
}

// renvoie la cl� publique pour une cl� priv�e
CBigPoint2D ECDSA_GetPublicKey(const CBigInt &PrivKey)
{
	P1.SetXY(nCOORDX_Secp256k1, nCOORDY_Secp256k1);

	// calcul de P2 : K * P1
	CBigPoint2D PubKey = gclsekp256k1.MultBigInt(P1, PrivKey);
	return PubKey;
}


// v�rification d'une signature
//de <HashToSign> avec <PrivateKey>
BOOL ECDSA_bCheckSign(const CBigInt     &Hash,     // valeur sign�e
					  const CBigPoint2D &Signature,   // signature a valider
					  const CBigPoint2D &PublicKey		// cl� publique
					 )
{
	P1.SetXY(nCOORDX_Secp256k1, nCOORDY_Secp256k1);

	const CBigInt N = gclsekp256k1.clGetOrdre();

	// -- Valider la cl�  publique
	// 0,0 est invalide
	if (PublicKey.bIsZero()) return FALSE;
	// le pt doit �tre sur la courbe
	if (!gclsekp256k1.bPointEstSurLaCourbe(PublicKey))
		return FALSE;
	// n*Q doit donner 0	
	CBigPoint2D P0 = gclsekp256k1.MultBigInt(PublicKey, N);
	if (!P0.bIsZero())
		return FALSE;

	// - Valider la signature

	// v�rif que les valeur sont dans [0,n-1]
	if (Signature.m_clX.bIsZero()) return FALSE;
	if (Signature.m_clX.nCompareU(N) != -1) return FALSE;
	if (Signature.m_clY.bIsZero()) return FALSE;
	if (Signature.m_clY.nCompareU(N) != -1) return FALSE;

	CBigInt w = Signature.m_clY.InvertModulo(N);
	CBigInt u1 = (Hash * w).Modulo(N);
	CBigInt u2 = (Signature.m_clX * w).Modulo(N);

	CBigPoint2D PA = gclsekp256k1.MultBigInt(P1, u1);
	CBigPoint2D PB = gclsekp256k1.MultBigInt(PublicKey, u2);
	CBigPoint2D PRes = gclsekp256k1.Add(PA, PB);
	// si on tombe en 0,0, invalide
	if (PRes.bIsZero()) return FALSE;

	// que si X = r mod n
	CBigInt XMod = PRes.m_clX.Modulo(N);
	CBigInt SMod = Signature.m_clX.Modulo(N);

	if (XMod != SMod)
	{
		return FALSE;
	}

	// VALIDE !
	return TRUE;

}
