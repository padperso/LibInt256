// points d'entr�e pour ECDSA


#include "BigInt.h"
#include "BigPoint2D.h"

// signature de <HashToSign> avec <PrivateKey>
void ECDSA_Sign(const CBigInt &  Hash,			// hash de la valeur a signer
				const CBigInt &  PrivateKey,	// cl� priv�e (a garder secr�te)
				OUT CBigPoint2D *pPublicKey		// r�sultat
);

// v�rification d'une signature
//de <HashToSign> avec <PrivateKey>
BOOL ECDSA_bCheckSign(  const CBigInt &   Hash,      // hash de la valeur sign�e
						const CBigPoint2D&Signature, // signature a valider
						const CBigPoint2D&PublicKey	 // cl� publique
);

// renvoie la cl� publique pour une cl� priv�e
CBigPoint2D ECDSA_GetPublicKey(const CBigInt &PrivKey);