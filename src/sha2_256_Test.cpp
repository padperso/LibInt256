//23.00A sha2_256_test.cpp PAD
// tests de surface pour sha2.cpp

#include "central.h"

#include "sha2_256.h"
#include "BigInt.h"



#include <iostream>
// dans l'autre cpp
//std::string sha256(std::string input);

void  _ShaTest1(PCXSTR szValToHash, PCXSTR szResultOK)
{
	// calcul de sha256(szTestVal) avec un autre algo
//	std::string input = szValToHash;
//	std::string output = sha256(input);

	/*
	const char *p = szValToHash;
		while (*p)
		{
			XASSERT(*p < 127);
			XASSERT(*p >= 32);
			p++;
		}
		*/
	
	// vérif du résultat
	CBigInt clResHash;
	clResHash.InitFromSha3_256(szValToHash);
	CBigInt clTest;
	clTest.FromStrBasePrefix(szResultOK, CBigInt::eHexComplementA2 );
	//clTest.DBG_Print();
	//clResHash.DBG_Print();
	XASSERT(clTest == clResHash);
}


void SHA_Test()
{
	_ShaTest1("",
		"0xe3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");

	_ShaTest1("A",
		"0x559aead08264d5795d3909718cdd05abd49572e84fe55590eef31a88a08fdffd");
		
	_ShaTest1("abcdefghijklmnop1234",
		"0xa9a9dc1836c67f5e5e40af383d262f9bf3aefd0b310066c80417fec712ca8902");

	_ShaTest1("abcd",
		"0x88d4266fd4e6338d13b845fcf289579d209c897823b9217da3e161936f031589");
		
	_ShaTest1("The SHA(Secure Hash Algorithm) is one of a number of cryptographic hash function",
		"0xfd612ae8d2e1c64291d13742d96c3374c06cbf425f79d0b7913b6f760033dd19");

	_ShaTest1("The SHA(Secure Hash Algorithm) is one of a number of cryptographic hash function"
			  ".A cryptographic hash is like a signature for a "
			  "text or a data file.SHA - 256 algorithm generates " // 
			  "an almost - unique, fixed size 256 - bit(32 - byte) hash."//Hash is a one way function – it cannot be decrypted back.This makes it suitable for password validation, challenge hash authentication, anti - tamper, digital signatures.",
		,"0xbb877b1ee9bceae553a746e312782a66da130afffacc2251a8860a81a445e09e");

	_ShaTest1("The SHA(Secure Hash Algorithm) is one of a number of cryptographic hash function"
			  ".A cryptographic hash is like a signature for a "
			  "text or a data file.SHA - 256 algorithm generates " // 
			  "an almost - unique, fixed size 256 - bit(32 - byte) hash."//
			  "Hash is a one way function - it cannot be decrypted back."//This makes it suitable for password validation, challenge hash authentication, anti - tamper, digital signatures.",
		,"0X8b2d6cf8b109d351c87e955f4ac7fd8d5cfef96920906631e3c12437c86cb8d5");

	_ShaTest1("The SHA(Secure Hash Algorithm) is one of a number of cryptographic hash functions.A cryptographic hash is like a signature for a text or a data file.SHA - 256 algorithm generates an almost - unique, fixed size 256 - bit(32 - byte) hash.Hash is a one way function - it cannot be decrypted back.This makes it suitable for password validation, challenge hash authentication, anti - tamper, digital signatures.",
		"0x75491df65e238d42b5aaf143bec0c1631f85742b59d6604b12f6b15731ac3e49");

	printf("SHA test PASSED\n");

}

