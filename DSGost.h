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
};

#endif // DSGOST_H
