#include "ECPoint.h"

ECPoint::ECPoint()
{
}

ECPoint::ECPoint(const ECPoint& p)
{
    x = p.x;
    y = p.y;
    a = p.a;
    b = p.b;
    FieldChar = p.FieldChar;
}

ECPoint::~ECPoint()
{
}

ECPoint ECPoint::Double() const
{
    ECPoint p = *this;

    ECPoint p2;
    p2.a = p.a;
    p2.b = p.b;
    p2.FieldChar = p.FieldChar;

    BigInteger dy = p.x * p.x * 3 + p.a;
    BigInteger dx = p.y * 2;

    if (dx < 0)
        dx += p.FieldChar;
        
    if (dy < 0)
        dy += p.FieldChar;

    BigInteger m = (dy * dx.modInverse(p.FieldChar)) % p.FieldChar;
    
    p2.x = (m * m - p.x - p.x) % p.FieldChar;
    p2.y = (m * (p.x - p2.x) - p.y) % p.FieldChar;
    
    if (p2.x < 0)
        p2.x += p.FieldChar;
        
    if (p2.y < 0)
        p2.y += p.FieldChar;

    return p2;
}

ECPoint ECPoint::operator+(const ECPoint& p2) const
{
    ECPoint p1 = *this;

    ECPoint p3;
    p3.a = p1.a;
    p3.b = p1.b;
    p3.FieldChar = p1.FieldChar;

    BigInteger dy = p2.y - p1.y;
    BigInteger dx = p2.x - p1.x;

    if (dx < 0)
        dx += p1.FieldChar;

    if (dy < 0)
        dy += p1.FieldChar;

    BigInteger m = (dy * dx.modInverse(p1.FieldChar)) % p1.FieldChar;

    if (m < 0)
        m += p1.FieldChar;

    p3.x = (m * m - p1.x - p2.x) % p1.FieldChar;
    p3.y = (m * (p1.x - p3.x) - p1.y) % p1.FieldChar;

    if (p3.x < 0)
        p3.x += p1.FieldChar;

    if (p3.y < 0)
        p3.y += p1.FieldChar;

    return p3;
}

ECPoint ECPoint::Multiply(const BigInteger& m) const
{
    ECPoint p = *this;
    BigInteger x = m;
    ECPoint temp = *this;
    x = x - 1;
    while (x != 0) {

        if ((x % 2) != 0) {
            if ((temp.x == p.x) || (temp.y == p.y))
                temp = temp.Double();
            else
                temp = temp + p;
            x = x - 1;
        }
        x = x / 2;
        p = p.Double();
    }
    return temp;
}