#ifndef BIGINTEGER_H
#define BIGINTEGER_H

#include <string>
#include <vector>
#include <cstdlib>

typedef unsigned int uint;
typedef char byte;

class BigInteger {
    
public:
    BigInteger();
    BigInteger(long value);
    BigInteger(byte inData[], int Length);
    BigInteger(uint inData[], int Length);
    BigInteger(const std::string& value, int radix);
    BigInteger(const BigInteger& bi);
    
    BigInteger operator-() const;
    
    //BigInteger operator++() const;
    
    BigInteger operator-(const BigInteger& bi2) const;
    BigInteger operator+(const BigInteger& bi2) const;
    BigInteger operator*(const BigInteger& bi2) const;
    BigInteger operator%(const BigInteger& bi2) const;
    BigInteger operator/(const BigInteger& bi2) const;
    BigInteger operator<<(int shiftVal) const;
    BigInteger operator>>(int shiftVal) const;
    
    BigInteger operator+=(const BigInteger& bi2) const;
    BigInteger operator-=(const BigInteger& bi2) const;
    
    bool operator<(const BigInteger& bi2) const;
    
    bool operator>(const BigInteger& bi2) const;
    bool operator>=(const BigInteger& bi2) const;
    
    bool operator==(const BigInteger& bi2) const;
    bool operator!=(const BigInteger& bi2) const;
    
    BigInteger operator&(const BigInteger& bi2) const;

    BigInteger modInverse(const BigInteger& modulus) const;
    
    std::string ToString() const;
    std::string ToString(int radix) const;
    
    void genRandomBits(int bits);
    
    BigInteger modPow(const BigInteger& exp, const BigInteger& n) const;
    
    ~BigInteger();

    
    int dataLength;

private:
    static const unsigned int maxLength = 70;

    //unsigned int* data;     
    unsigned int data[maxLength];

    static void singleByteDivide(const BigInteger& bi1, const BigInteger& bi2, BigInteger& outQuotient, BigInteger& outRemainder);
    static void multiByteDivide(const BigInteger& bi1, const BigInteger& bi2, BigInteger& outQuotient, BigInteger& outRemainder);
    static int shiftLeft(uint buffer[], int shiftVal, int Length);
    static int shiftRight(uint buffer[], int shiftVal, int Length);
    
    bool Equals(const BigInteger& bi) const;
    
    int bitCount() const;
    
    BigInteger BarrettReduction(const BigInteger& x, const BigInteger& n, const BigInteger& constant) const;

};

#endif // BIGINTEGER_H
