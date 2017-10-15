//#23.00A sha2_256.h PAD

#pragma once


// <pHash> doit faire 256 bits = 32 octets
void sha2_256(byte *pData, int nSizeData, OUT byte *pHash);