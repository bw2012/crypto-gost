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

//Генерируем секретный ключ заданной длины
BigInteger DSGost::GenPrivateKey(int BitSize)
{
    BigInteger d;
    do {
        d.genRandomBits(BitSize);
    } while ((d < 0) || (d > n));
    return d;
}

ECPoint DSGost::GenPublicKey(BigInteger d)
{
    ECPoint G = GDecompression();
    ECPoint Q = G.Multiply(d);
    return Q;
}

//Восстанавливаем координату y из координаты x и бита четности y
ECPoint DSGost::GDecompression()
{
    byte y = xG.at(0);
    int xLen = xG.size() - 1;
    byte x[xLen];

    for (auto i = 0; i < xLen; i++) {
        x[i] = xG.at(i);
    }

    BigInteger Xcord(x, xLen);
    BigInteger temp = (Xcord * Xcord * Xcord + a * Xcord + b) % p;
    BigInteger beta = ModSqrt(temp, p);
    BigInteger Ycord;

    if ((beta % 2) == (y % 2))
        Ycord = beta;
    else
        Ycord = p - beta;

    ECPoint G;
    G.a = a;
    G.b = b;
    G.FieldChar = p;
    G.x = Xcord;
    G.y = Ycord;

    this->G = G;

    return G;
}

//функция вычисления квадратоного корня по модулю простого числа q
BigInteger DSGost::ModSqrt(const BigInteger& a, const BigInteger& q)
{
    BigInteger b;

    do {
        b.genRandomBits(255);
    } while (Legendre(b, q) == 1);

    BigInteger s = 0;
    BigInteger t = q - 1;

    while ((t & 1) != 1) {
        s += 1;
        t = t >> 1;
    }

    BigInteger InvA = a.modInverse(q);
    BigInteger c = b.modPow(t, q);
    BigInteger r = a.modPow(((t + 1) / 2), q);
    BigInteger d;

    for (int i = 1; s > i; i++) {
        BigInteger temp = 2;
        temp = temp.modPow((s - i - 1), q);
        d = (r.modPow(2, q) * InvA).modPow(temp, q);
        if (d == (q - 1))
            r = (r * c) % q;
        c = c.modPow(2, q);
    }
    return r;
}

//Вычисляем символ Лежандра
BigInteger DSGost::Legendre(const BigInteger& a, const BigInteger& q)
{
    return a.modPow((q - 1) / 2, q);
}