#pragma once

typedef const class CBigInt & RCBigInt;

class CBigInt
{
protected:
	// Little-endian : 0 : poids faible, 7 poids fort
	UINT64 *m_TabVal;
	// Taille  en octets
	int	   m_nSizeInByte;
	// OPTIM : Tant que  moins de 256 bits =  4 QWORD, pas d'alloc dyn
	#define NLIMIT_PREALLOCINBYTE (4*8)
	UINT64 m_PreAlloc[NLIMIT_PREALLOCINBYTE / 8];


public:
	// constructeur. init a 0
	CBigInt(void);
	// constructeur. copie
	CBigInt(const CBigInt &clSrc);
	// constructeur a partir d'un INT64
	CBigInt(INT64 n);
	// constructeur a partir d'un chaine avec un nombre en base 10 ou 16 (si commence par 0x)
	CBigInt(PCXSTR pszValAsString);
	// destructeur
	~CBigInt();
	//C++11 move constructor
	CBigInt(CBigInt&& other);

	// Init a partir du hash d'une chaine
	void InitFromSha3_256(PCXSTR pszValToHash);


	// = 0 ?
	BOOL bIsZero(void) const;
	//indique si on est un nombre négatif
	BOOL bNegative(void) const;
	// comparation non signée < = >.
	// renvoie -1 si onest < a clNombre2. 0 si égal. 1 si on est supérieur
	int nCompareU(RCBigInt clNombre2) const;

	// init a 0
	void SetToZero(void);

	// opérateur d'affectaction avec un entier 64
	// ex : CBigInt x; x = 7;
	void operator = (INT64 n);

	// opérateur de comparaisons avec un entier 64
	// ex : CBigInt x; x == 7;
	bool operator == (INT64 n) const;
	bool operator != (INT64 n) const;
	// opérateur de comparaisons avec un autre BigInt
	bool operator == (const CBigInt& other) const;
	bool operator != (const CBigInt& other) const;
	// opérateur de copie
	//x=y;
	const CBigInt& operator =(const CBigInt &clSrc);
	void CopieFrom(const CBigInt &clSrc);// idem

	// --- oprations -----

	// addition
	enum EOptionAdd
	{
		eDefaut,
		eAllowOverflow  //  sans aggrandir this en cas d'overflow
	};
	//c=a+b;
	friend CBigInt operator +(RCBigInt A, RCBigInt B);
	void operator +=(RCBigInt A);
	void operator -=(RCBigInt A);
	void operator *=(RCBigInt A);
	//a+= 4
	// a += 4,
	void AddUI4(UINT32 n, EOptionAdd eOption);
	void operator +=(UINT32 n);
	void operator +=(INT32 n); // cas signé
	// soustraction
	//c=a-b;
	friend CBigInt operator -(RCBigInt A, RCBigInt B);


	// Multiplication
	//c=a*b;
	friend CBigInt operator *(RCBigInt A, RCBigInt B);
	// Multiplication par un entier 32
	void MultUI32(UINT32 nVal);
	// Multiplication par un entier 64
	void MultUI64(UINT64 nMultiplicateur);
	// Multiplication par une puissance de 2 (décalage de bits)
	void MultPow2(int nPow2);
	// Division par une puissance de 2 (décalage de bits)
	void DivPow2(int nPow2);

	// Renvoie le carré 
	CBigInt Pow2(void) const;
	//  Renvoie le cube 
	CBigInt Pow3(void) const;


	// négation : x = -x
	friend CBigInt operator -(RCBigInt A);
	void Negate(void);
	
	//divistion
	//c=a+b;
	friend CBigInt operator /(RCBigInt A, RCBigInt B);

	// Division entière de this
	// ex : 2445 / 1000 => (2, 445)
	void Divide(const CBigInt &clDiviseur, OUT CBigInt *pclQuotient, OUT CBigInt *pclclReste) const;
	// division par 2
	void DivideBy2(void);
	// renvoie le modulo = reste de la divistion par <clDiviseur>
	CBigInt Modulo(const CBigInt &clDiviseur) const;
	// log2 
	int nGetLog2(void) const;



	// renvoie la valeur abosolue
	CBigInt Abs(void) const;

	// calcul du PGCD de 2 nombre par l'algorithme d'eclide étendu
	// renvoie le PGCD et x,y tels que A*x + B*y = PGCD
	static void sPGDCExt(RCBigInt A, RCBigInt B,
		OUT CBigInt *pgdc, OUT CBigInt *px, OUT CBigInt *py);
	// calcul de l'invere modulaire
	// cas x tel que x *(*this) == 1 modulo mod
	CBigInt InvertModulo(RCBigInt mod) const;

	// -- convertion chaines

	// depuis une chaine en base 10.
	// ex : "32670510020758816978083085130507043184471273380659243275938904335757337482424"
	void FromStrBase10(PCXSTR pszVal);
	// depuis une chaine en base 16
	// ex : "1E99423A4ED27608A15A2616A2B0E9E52CED330AC530EDCC32C8FFC6A526AEDD"
	enum EStrMode { eHexComplementA2, ePositif };
	void FromStrHexa(PCXSTR pszVal, EStrMode eMode=ePositif);
	// depuis une série d'octet
	void FromRawBytes(PBYTE pByte, int nByteCount);

	// vers une chaine en base 10.
	void ToStrBase10(OUT PXSTR pszVal, int nLenInChar) const;
	// vers une chaine en base 16.
	void ToStrBase16(OUT PXSTR pszVal, int nLenInChar) const;

	// depuis une chaine en base 10 ou 16 si commence par 0x
	void FromStrBasePrefix(PCXSTR pszVal, EStrMode eMode=ePositif);

	// taille en bit
	UINT nSizeInBit(void) const { return m_nSizeInByte * 8; }
	// récup du bit n° I
	int nGetBit(int nNumBit) const;



	// Tests de surface
	static void sTEST_ALL(void);
//#ifdef _DEBUG
	#define DBGPRINT_NORC     0x01
	#define DBGPRINT_NORALIGN 0x02
	
	void DBG_Print(int nOption=0) const;
//#endif//_DEBUG
protected:
	// constructeur helper
	void _Init(int nSizeInByte);
	// realloc 
	enum EResieMode { eSansRemplissage, eAvecSigne, eSansSigne};
	void _Resize(int nSizeInByte, EResieMode e);

	// renvoie le n°eme mot de 3é bits. 0 poids faible
	UINT32 _nGetI4(int nNumMot) const;
	UINT64 _nGetI8(int nNumMot) const;
	//version pour addition, gère les cas  ou nNumMot est plus grand que le max
	UINT32 _nGetI4Add(int nNumMot) const;
	UINT64 _nGetI8Add(int nNumMot) const;
	// affectation n em mot. alloc si nécessaire
	void _SetI8(int nNumMot, UINT64 nVal);
	void _SetI4(int nNumMot, UINT32 nVal);
	// affectation n em octet. alloc si nécessaire
	void _SetI1(int nNumMot, UINT8 nVal);
	// suppression des mots avec 0 au début
	void _Trim0(void);

	// taille en octets
	int nSizeInByte(void) const { return m_nSizeInByte; }
	// taille en I4 = mots de 4 octets = 32 bits
	int nSizeInI4(void) const { return m_nSizeInByte / 4; }
	// taille en I8 = mots de 8 octets = 64 bits
	int nSizeInI8(void) const { return m_nSizeInByte / 8; }

	// utilitaires pour la division
	// renvoie la magnitude. cad la puisance de 2 max du nombre et sa valeur dans cette puissance de 2.
	// ex : "0x344A3" => 20,3
	void _GetPow2AndCoef(OUT UINT32 *pnPow2, OUT UINT32 *pnCoef) const;
	// renvoie les info pour effecter une division rapide appproximative par this
	void _GetQuickDiv(OUT struct STQuickDiv *pstQuickDiv) const;
	// Effectue une division rapide.
	CBigInt _clDivQuick(const struct STQuickDiv &stQuickDiv) const;
	// vers un INT64
	UINT64 _nToUI8(void) const;

	// Division 
	//Algo 1 (Newtown)
	void _Divide_Algo1(const CBigInt &clDiviseur, OUT CBigInt *pclQuotient, OUT CBigInt *pclclReste) const;
	//Algo 2
	void _Divide_Algo2(const CBigInt &clDiviseur, OUT CBigInt *pclQuotient, OUT CBigInt *pclclReste) const;

	// calcul de 2^N/this
	// fonction utilitairep our la division.
	CBigInt _clCalc2PowNSurX(int npow2) const;

	// version rapide de Modulo
	// Barrett reduction : https://en.wikipedia.org/wiki/Barrett_reduction
	CBigInt _FastModulo(const CBigInt &clDiviseur) const;

};
typedef const CBigInt &RCCBigInt;
