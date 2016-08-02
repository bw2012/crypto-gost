#ifndef ECPOINT_H
#define ECPOINT_H

#include "BigInteger.h"

class ECPoint
{
public:
    BigInteger x;
    BigInteger y;
    BigInteger a;
    BigInteger b;
    BigInteger FieldChar;

    ECPoint operator+(const ECPoint& p2) const;
    ECPoint Double() const;
    ECPoint Multiply(const BigInteger& x) const;

    ECPoint(const ECPoint& p);
    ECPoint();
    ~ECPoint();

};

#endif // ECPOINT_H
