#pragma once

static const int nSizeInByte = 32;
static const int nSizeI4     = 8;  // 32 * 8 = 256
static const int nSizeI8     = 4;  // 64 * 4 = 256



class CInt256
{
	friend class CBigIntFixed;

protected:
	// Little-endian : 0 : poids faible, 7 poids fort
	UINT64 m_TabVal[nSizeI8];

public:
	// init a 0
	CInt256(); 
	// init a partir d'un entier non signé
	CInt256(UINT64 nVal );
	// init a partir d'un entier signé
	CInt256(INT32 nVal);	
	// copie
	CInt256(const CInt256 &clSrc);

	// copie
	void CopieFrom(const CInt256 &clSrc);
	const CInt256 & operator= (const CInt256 &clSrc);
	const CInt256 & operator= (UINT64 nVal);

	// comparation non signée < = >.
	// renvoie -1 si onest < a clNombre2. 0 si égal. 1 si on est supérieur
	int nCompareU(const CInt256 &clNombre2) const;

	// init a 0
	void SetToZero(void);
	BOOL bIsZero(void) const;
	BOOL bNegative(void) const;

	// ---- convertions ----

	// depuis un INT64
	void FromUI8(UINT64 nVal);
	// init a partir d'un entier signé
	void FromI4(INT32 nVal);

	// vers un INT64
	UINT64 nToUI8(void) const;
	// depuis une chaine en base 10.
	// ex : "32670510020758816978083085130507043184471273380659243275938904335757337482424"
	void FromStrBase10(PCXSTR pszVal);
	// vers une chaine en base 10.
	// nLenInChar doit faire au moins 79 charatères.
	void ToStrBase10(OUT PXSTR pszVal, int nLenInChar);
	// depuis une chaine en base 16
	// ex : "1E99423A4ED27608A15A2616A2B0E9E52CED330AC530EDCC32C8FFC6A526AEDD"
	void FromStrHexa(PCXSTR pszVal);

	// ---- opération mathématiques 

	// Addition
	void Add(const CInt256 &clVal2);
	void AddI4(int nVal); // entier signé
	// Soustraction
	void Substract(const CInt256 &clVal2 );
	// négation : x = -x
	void Negate(void);

	// Multiplication
	void Mult(const CInt256 &clVal2);
	// Multiplication par un entier 32
	void MultUI32(UINT32 nVal);
	// Multiplication par une puissance de 2 (décalage de bits)
	void MultPow2(int nPow2);
	// Renvoie le carré 
	CInt256 Pow2(void) const;
	//  Renvoie le cube 
	CInt256 Pow3(void) const;
	 
	// Division entière de this
	// ex : 2445 / 1000 => (2, 445)
	void Divide(const CInt256 &clDiviseur, OUT CInt256 *pclDivision, OUT CInt256 *pclclReste) const;
	// division par 2
	void DivideBy2(void);
	// renvoie le modulo = reste de la divistion par <clDiviseur>
	CInt256 Modulo(const CInt256 &clDiviseur) const;


	// renvoie la valeur abosolue
	CInt256 Abs(void) const;

protected:
	// renvoie le n°eme mot de 3é bits. 0 poids faible
	UINT32 _nGetI4(int nNumMot) const;
	void _SetI4(int nNumMot, UINT32 nVal );

	// utilitaires pour la division
	// renvoie la magnitude. cad la puisance de 2 max du nombre
	int _nGetMagnitudeEtDivisteurRapide(OUT UINT32 *pnQuickDivisor) const;
	// Effectue une division rapide.
	CInt256 _clDivQuick(UINT32 nQuickDivisor, int nMagnitude) const;

};

