// points d'entrée pour ECDSA


#include "BigInt.h"
#include "BigPoint2D.h"

// signature de <HashToSign> avec <PrivateKey>
void ECDSA_Sign(const CBigInt &  Hash,			// hash de la valeur a signer
				const CBigInt &  PrivateKey,	// clé privée (a garder secrète)
				OUT CBigPoint2D *pPublicKey		// résultat
);

// vérification d'une signature
//de <HashToSign> avec <PrivateKey>
BOOL ECDSA_bCheckSign(  const CBigInt &   Hash,      // hash de la valeur signée
						const CBigPoint2D&Signature, // signature a valider
						const CBigPoint2D&PublicKey	 // clé publique
);

// renvoie la clé publique pour une clé privée
CBigPoint2D ECDSA_GetPublicKey(const CBigInt &PrivKey);