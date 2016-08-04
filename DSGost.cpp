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
        d.genRandomBits(BitSize, time(0));
    } while ((d < 0) || (d > n));
    return d;
}

ECPoint DSGost::GenPublicKey(BigInteger d)
{
    ECPoint G = GDecompression();
    
    printf("Ga  ---> %s\n", G.a.ToString().c_str());
    printf("Gb  ---> %s\n", G.b.ToString().c_str());
    printf("GFieldChar  ---> %s\n", G.FieldChar.ToString().c_str());
    printf("Gx  ---> %s\n", G.x.ToString().c_str());
    printf("Gy  ---> %s\n", G.y.ToString().c_str());
    printf("------------------------------------------------\n");
    printf("d  ---> %s\n", d.ToString().c_str());
    
    ECPoint Q = G.Multiply(d);
    
    printf("Qa  ---> %s\n", Q.a.ToString().c_str());
    printf("Qb  ---> %s\n", Q.b.ToString().c_str());
    printf("QFieldChar  ---> %s\n", Q.FieldChar.ToString().c_str());
    printf("Qx  ---> %s\n", Q.x.ToString().c_str());
    printf("Qy  ---> %s\n", Q.y.ToString().c_str());
    
    
    return Q;
}

//Восстанавливаем координату y из координаты x и бита четности y
ECPoint DSGost::GDecompression()
{
    byte y = xG.at(0);
    int xLen = xG.size() - 1;
    byte x[xLen];

    for (auto i = 0; i < xLen; i++) {
        x[i] = xG.at(i + 1);
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
        // printf("time %d\n", time(0));
        b.genRandomBits(255, time(0));
        // b = BigInteger("29515218379636995773351698872144066530463441601111065046740229564272930713805",10);
        // printf("b  ---> %d %s %d\n", b.dataLength, b.ToString().c_str(), b.ToString().length());
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
    BigInteger res = a.modPow((q - 1) / 2, q);
    return res;
}

//подписываем сообщение
std::string DSGost::SingGen(std::vector<byte> msg, BigInteger d)
{
    byte h[msg.size()];
    for (auto i = 0; i < msg.size(); i++) {
        h[i] = msg[i];
    }

    BigInteger alpha(h, msg.size());
    BigInteger e = alpha % n;

    if (e == 0)
        e = 1;

    BigInteger k;
    ECPoint C;
    BigInteger r;
    BigInteger s;

    do {
        do {
            k.genRandomBits(n.bitCount(), time(0));
        } while ((k < 0) || (k > n));
        C = G.Multiply(k);
        r = C.x % n;
        s = ((r * d) + (k * e)) % n;
    } while ((r == 0) || (s == 0));

    std::string Rvector = padding(r.ToString(16), n.bitCount() / 4);
    std::string Svector = padding(s.ToString(16), n.bitCount() / 4);

    return Rvector + Svector;
    // return "";
}

//дополняем подпись нулями слева до длины n, где n - длина модуля в битах
std::string DSGost::padding(std::string input, int size)
{
    if (input.length() < size) {
        do {
            input = "0" + input;
        } while (input.length() < size);
    }
    return input;
}

//проверяем подпись
bool DSGost::SingVer(std::vector<byte> msg, std::string sing, ECPoint Q)
{
    std::string Rvector = sing.substr(0, n.bitCount() / 4);
    std::string Svector = sing.substr(n.bitCount() / 4, n.bitCount() / 4);

    BigInteger r(Rvector, 16);
    BigInteger s(Svector, 16);

    if ((r < 1) || (r > (n - 1)) || (s < 1) || (s > (n - 1)))
        return false;

    byte H[msg.size()];
    for (auto i = 0; i < msg.size(); i++) {
        H[i] = msg[i];
    }

    printf("test1\n");

    BigInteger alpha(H, sing.size());
    BigInteger e = alpha % n;

    if (e == 0)
        e = 1;

    printf("test2\n");

    BigInteger v = e.modInverse(n);
    BigInteger z1 = (s * v) % n;
    BigInteger z2 = n + ((-(r * v)) % n);

    printf("test3\n");

    ECPoint G = GDecompression();
    
    printf("Ga  ---> %s\n", G.a.ToString().c_str());
    printf("Gb  ---> %s\n", G.b.ToString().c_str());
    printf("GFieldChar  ---> %s\n", G.FieldChar.ToString().c_str());
    printf("Gx  ---> %s\n", G.x.ToString().c_str());
    printf("Gy  ---> %s\n", G.y.ToString().c_str());

    printf("test4\n");

    ECPoint A = G.Multiply(z1);
    ECPoint B = Q.Multiply(z2);

    printf("test5\n");

    ECPoint C = A + B;
    BigInteger R = C.x % n;

    if (R == r)
        return true;
    else
        return false;
}