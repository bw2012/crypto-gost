#include "DSGost.h"

DSGost::DSGost(BigInteger p, BigInteger a, BigInteger b, BigInteger n, std::vector<byte> xG)
{
    this->a = a;
    this->b = b;
    this->n = n;
    this->p = p;
    this->xG = xG;
}

DSGost::~DSGost()
{
}

BigInteger DSGost::GenPrivateKey(int BitSize)
{
    BigInteger d;
    do {
        d.genRandomBits(BitSize);
    } while ((d < 0) || (d > n));
    return d;
}
