// BigInt.h PAD
// entier de taille fixe sur <n> bit, n arbitrairement grand.

#pragma once


class CBigIntFixed
{
protected:
	// Little-endian : 0 : poids faible, 7 poids fort
	UINT64 *m_TabVal;
	// Taille  en octets
	int	   m_nSizeInByte;

public:
	// constructeur. init a 0
	CBigIntFixed(int nSizeInByte);
	// constructeur. copie
	CBigIntFixed(const CBigIntFixed &clSrc);	

	// taille en octets
	int nSizeInByte(void) const { return m_nSizeInByte; }
	// taille en I4 = mots de 4 octets = 32 bits
	int nSizeInI4(void) const {	return m_nSizeInByte/4;}
	// taille en I8 = mots de 8 octets = 64 bits
	int nSizeInI8(void) const { return m_nSizeInByte/8; }

	// copie
	void CopieFrom(const CBigIntFixed &clSrc);
	const CBigIntFixed & operator= (const CBigIntFixed &clSrc);
	const CBigIntFixed & operator= (UINT64 nVal);

	// comparation non signée < = >.
	// renvoie -1 si onest < a clNombre2. 0 si égal. 1 si on est supérieur
	int nCompareU(const CBigIntFixed &clNombre2) const;

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
	void Add(const CBigIntFixed &clVal2);
	void AddI4(int nVal); // entier signé
	// Soustraction
	void Substract(const CBigIntFixed &clVal2 );
	// négation : x = -x
	void Negate(void);

	// Multiplication
	void Mult(const CBigIntFixed &clVal2);
	// Multiplication par un entier 32
	void MultUI32(UINT32 nVal);
	// Multiplication par une puissance de 2 (décalage de bits)
	void MultPow2(int nPow2);
	// Renvoie le carré 
	CBigIntFixed Pow2(void) const;
	//  Renvoie le cube 
	CBigIntFixed Pow3(void) const;
	 
	// Division entière de this
	// ex : 2445 / 1000 => (2, 445)
	void Divide(const CBigIntFixed &clDiviseur, OUT CBigIntFixed *pclDivision, OUT CBigIntFixed *pclclReste) const;
	// division par 2
	void DivideBy2(void);
	// renvoie le modulo = reste de la divistion par <clDiviseur>
	CBigIntFixed Modulo(const CBigIntFixed &clDiviseur) const;


	// renvoie la valeur abosolue
	CBigIntFixed Abs(void) const;

protected:
	// constructeur helper
	void _Init(int nSizeInByte);

	// renvoie le n°eme mot de 3é bits. 0 poids faible
	UINT32 _nGetI4(int nNumMot) const;
	void _SetI4(int nNumMot, UINT32 nVal );

	// utilitaires pour la division
	// renvoie la magnitude. cad la puisance de 2 max du nombre
	int _nGetMagnitudeEtDivisteurRapide(OUT UINT32 *pnQuickDivisor) const;
	// Effectue une division rapide.
	CBigIntFixed _clDivQuick(UINT32 nQuickDivisor, int nMagnitude) const;

};

