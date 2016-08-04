#ifndef DSGOST_H
#define DSGOST_H

#include "BigInteger.h"
#include "ECPoint.h"

class DSGost
{
public:
    BigInteger p;
    BigInteger a;
    BigInteger b;
    BigInteger n;
    std::vector<byte> xG;
    ECPoint G;

    DSGost(BigInteger p, BigInteger a, BigInteger b, BigInteger n, std::vector<byte> xG);
    ~DSGost();

    BigInteger GenPrivateKey(int BitSize);
    ECPoint GenPublicKey(BigInteger d);
    
    std::string SingGen(std::vector<byte> h, BigInteger d);
    bool SingVer(std::vector<byte> msg, std::string sing, ECPoint Q);

private:
    ECPoint GDecompression();
    BigInteger ModSqrt(const BigInteger& a, const BigInteger& q);
    BigInteger Legendre(const BigInteger& a, const BigInteger& q);
    
    std::string padding(std::string input, int size);
};

#endif // DSGOST_H
