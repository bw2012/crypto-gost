#include "BigInteger.h"
#include <stdio.h>

BigInteger::BigInteger()
{
    // data = new unsigned int[maxLength];
    dataLength = 1;
    for (int i = 0; i < maxLength; i++)
        data[i] = 0;
}

BigInteger::BigInteger(long value)
{
    // data = new unsigned int[maxLength];
    for (int i = 0; i < maxLength; i++)
        data[i] = 0;

    long tempVal = value;

    // copy bytes from long to BigInteger without any assumption of
    // the length of the long datatype

    dataLength = 0;
    while (value != 0 && dataLength < maxLength) {
        data[dataLength] = (uint)(value & 0xFFFFFFFF);
        value >>= 32;
        dataLength++;
    }

    if (tempVal > 0) { // overflow check for +ve value
        if (value != 0 || (data[maxLength - 1] & 0x80000000) != 0) {
            // throw(new ArithmeticException("Positive overflow in constructor."));
        }
    } else if (tempVal < 0) { // underflow check for -ve value
        if (value != -1 || (data[dataLength - 1] & 0x80000000) == 0) {
            // throw(new ArithmeticException("Negative underflow in constructor."));
        }
    }

    if (dataLength == 0)
        dataLength = 1;
}

BigInteger::BigInteger(byte inData[], int len)
{
    dataLength = len >> 2;

    int leftOver = len & 0x3;
    if (leftOver != 0) // length not multiples of 4
        dataLength++;

    if (dataLength > maxLength) {
        // throw(new ArithmeticException("Byte overflow in constructor."));
    }

    // data = new uint[maxLength];
    for (int i = 0; i < maxLength; i++)
        data[i] = 0;

    for (int i = len - 1, j = 0; i >= 3; i -= 4, j++) {
        data[j] = (uint)((inData[i - 3] << 24) + (inData[i - 2] << 16) + (inData[i - 1] << 8) + inData[i]);
    }

    if (leftOver == 1)
        data[dataLength - 1] = (uint)inData[0];
    else if (leftOver == 2)
        data[dataLength - 1] = (uint)((inData[0] << 8) + inData[1]);
    else if (leftOver == 3)
        data[dataLength - 1] = (uint)((inData[0] << 16) + (inData[1] << 8) + inData[2]);

    while (dataLength > 1 && data[dataLength - 1] == 0)
        dataLength--;
}

BigInteger::BigInteger(uint inData[], int Length)
{
    dataLength = Length;

    if (dataLength > maxLength) {
        // throw(new ArithmeticException("Byte overflow in constructor."));s
    }

    // data = new uint[maxLength];
    for (int i = 0; i < maxLength; i++)
        data[i] = 0;

    for (int i = dataLength - 1, j = 0; i >= 0; i--, j++)
        data[j] = inData[i];

    while (dataLength > 1 && data[dataLength - 1] == 0)
        dataLength--;
}

BigInteger::BigInteger(const std::string& value, int radix)
{
    BigInteger multiplier(1);
    BigInteger result;
    int limit = 0;

    if (value[0] == '-')
        limit = 1;

    for (int i = value.length() - 1; i >= limit; i--) {
        int posVal = (int)value.at(i);

        if (posVal >= '0' && posVal <= '9')
            posVal -= '0';
        else if (posVal >= 'a' && posVal <= 'z')
            posVal = (posVal - 'a') + 10;
        else
            posVal = 9999999; // arbitrary large

        if (posVal >= radix) {
            // throw(new ArithmeticException("Invalid string in constructor."));
        } else {
            if (value.at(0) == '-')
                posVal = -posVal;

            result = result + (multiplier * posVal);

            if ((i - 1) >= limit)
                multiplier = multiplier * radix;
        }
    }

    if (value.at(0) == '-') // negative values
    {
        if ((result.data[maxLength - 1] & 0x80000000) == 0) {
            // throw(new ArithmeticException("Negative underflow in constructor."));
        }
    } else // positive values
    {
        if ((result.data[maxLength - 1] & 0x80000000) != 0) {
            // throw(new ArithmeticException("Positive overflow in constructor."));
        }
    }

    // data = new uint[maxLength];
    for (int i = 0; i < maxLength; i++)
        data[i] = 0;
    for (int i = 0; i < result.dataLength; i++)
        data[i] = result.data[i];

    dataLength = result.dataLength;
}

BigInteger::BigInteger(const BigInteger& bi)
{
    for (int i = 0; i < maxLength; i++)
        data[i] = 0;

    dataLength = bi.dataLength;

    for (int i = 0; i < dataLength; i++)
        data[i] = bi.data[i];
}

//==========================================================================================================

std::string BigInteger::ToString() const
{
    return ToString(10);
}

std::string BigInteger::ToString(int radix) const
{

    std::string charSet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string result = "";

    BigInteger a = *this;

    bool negative = false;
    if ((a.data[maxLength - 1] & 0x80000000) != 0) {
        negative = true;
        try
        {
            a = -a;
        }
        catch (std::exception)
        {
        }
    }

    BigInteger quotient;
    BigInteger remainder;
    BigInteger biRadix(radix);

    if (a.dataLength == 1 && a.data[0] == 0) {
        result = "0";
    } else {
        while (a.dataLength > 1 || (a.dataLength == 1 && a.data[0] != 0)) {
            BigInteger::singleByteDivide(a, biRadix, quotient, remainder);

            if (remainder.data[0] < 10)
                result = std::to_string(remainder.data[0]) + result;
            else
                result = charSet[(int)remainder.data[0] - 10] + result;

            a = quotient;
        }

        if (negative)
            result = "-" + result;
    }

    return result;
}

BigInteger::~BigInteger()
{
}

void BigInteger::singleByteDivide(const BigInteger& bi1,
                                  const BigInteger& bi2,
                                  BigInteger& outQuotient,
                                  BigInteger& outRemainder)
{
    uint result[maxLength];
    int resultPos = 0;

    // copy dividend to reminder
    for (int i = 0; i < maxLength; i++)
        outRemainder.data[i] = bi1.data[i];

    outRemainder.dataLength = bi1.dataLength;

    while (outRemainder.dataLength > 1 && outRemainder.data[outRemainder.dataLength - 1] == 0)
        outRemainder.dataLength--;

    ulong divisor = (ulong)bi2.data[0];
    int pos = outRemainder.dataLength - 1;
    ulong dividend = (ulong)outRemainder.data[pos];

    if (dividend >= divisor) {
        ulong quotient = dividend / divisor;
        result[resultPos++] = (uint)quotient;

        outRemainder.data[pos] = (uint)(dividend % divisor);
    }
    pos--;

    while (pos >= 0) {
        dividend = ((ulong)outRemainder.data[pos + 1] << 32) + (ulong)outRemainder.data[pos];
        ulong quotient = dividend / divisor;
        result[resultPos++] = (uint)quotient;

        outRemainder.data[pos + 1] = 0;
        outRemainder.data[pos--] = (uint)(dividend % divisor);
    }

    outQuotient.dataLength = resultPos;
    int j = 0;
    for (int i = outQuotient.dataLength - 1; i >= 0; i--, j++)
        outQuotient.data[j] = result[i];
    for (; j < maxLength; j++)
        outQuotient.data[j] = 0;

    while (outQuotient.dataLength > 1 && outQuotient.data[outQuotient.dataLength - 1] == 0)
        outQuotient.dataLength--;

    if (outQuotient.dataLength == 0)
        outQuotient.dataLength = 1;

    while (outRemainder.dataLength > 1 && outRemainder.data[outRemainder.dataLength - 1] == 0)
        outRemainder.dataLength--;
}

BigInteger BigInteger::operator-() const
{

    BigInteger bi1 = *this;

    if (bi1.dataLength == 1 && bi1.data[0] == 0)
        return BigInteger();

    BigInteger result(bi1);

    // 1's complement
    for (int i = 0; i < maxLength; i++)
        result.data[i] = (uint)(~(bi1.data[i]));

    // add one to result of 1's complement
    long val, carry = 1;
    int index = 0;

    while (carry != 0 && index < maxLength) {
        val = (long)(result.data[index]);
        val++;

        result.data[index] = (uint)(val & 0xFFFFFFFF);
        carry = val >> 32;

        index++;
    }

    if ((bi1.data[maxLength - 1] & 0x80000000) == (result.data[maxLength - 1] & 0x80000000)) {
        // throw(new ArithmeticException("Overflow in negation.\n"));
    }

    result.dataLength = maxLength;

    while (result.dataLength > 1 && result.data[result.dataLength - 1] == 0)
        result.dataLength--;
    return result;
}

BigInteger BigInteger::operator-(const BigInteger& bi2) const
{
    BigInteger bi1 = *this;
    BigInteger result;

    result.dataLength = (bi1.dataLength > bi2.dataLength) ? bi1.dataLength : bi2.dataLength;

    long carryIn = 0;
    for (int i = 0; i < result.dataLength; i++) {
        long diff;

        diff = (long)bi1.data[i] - (long)bi2.data[i] - carryIn;
        result.data[i] = (uint)(diff & 0xFFFFFFFF);

        if (diff < 0)
            carryIn = 1;
        else
            carryIn = 0;
    }

    // roll over to negative
    if (carryIn != 0) {
        for (int i = result.dataLength; i < maxLength; i++)
            result.data[i] = 0xFFFFFFFF;
        result.dataLength = maxLength;
    }

    // fixed in v1.03 to give correct datalength for a - (-b)
    while (result.dataLength > 1 && result.data[result.dataLength - 1] == 0)
        result.dataLength--;

    // overflow check

    int lastPos = maxLength - 1;
    if ((bi1.data[lastPos] & 0x80000000) != (bi2.data[lastPos] & 0x80000000) &&
        (result.data[lastPos] & 0x80000000) != (bi1.data[lastPos] & 0x80000000)) {
        // throw(new ArithmeticException());
    }

    return result;
}

bool BigInteger::operator<(const BigInteger& bi2) const
{
    int pos = maxLength - 1;
    BigInteger bi1 = *this;

    // bi1 is negative, bi2 is positive
    if ((bi1.data[pos] & 0x80000000) != 0 && (bi2.data[pos] & 0x80000000) == 0)
        return true;

    // bi1 is positive, bi2 is negative
    else if ((bi1.data[pos] & 0x80000000) == 0 && (bi2.data[pos] & 0x80000000) != 0)
        return false;

    // same sign
    int len = (bi1.dataLength > bi2.dataLength) ? bi1.dataLength : bi2.dataLength;
    for (pos = len - 1; pos >= 0 && bi1.data[pos] == bi2.data[pos]; pos--)
        ;

    if (pos >= 0) {
        if (bi1.data[pos] < bi2.data[pos])
            return true;
        return false;
    }
    return false;
}

BigInteger BigInteger::operator+(const BigInteger& bi2) const
{
    BigInteger result;
    BigInteger bi1 = *this;

    result.dataLength = (bi1.dataLength > bi2.dataLength) ? bi1.dataLength : bi2.dataLength;

    long carry = 0;
    for (int i = 0; i < result.dataLength; i++) {
        long sum = (long)bi1.data[i] + (long)bi2.data[i] + carry;
        carry = sum >> 32;
        result.data[i] = (uint)(sum & 0xFFFFFFFF);
    }

    if (carry != 0 && result.dataLength < maxLength) {
        result.data[result.dataLength] = (uint)(carry);
        result.dataLength++;
    }

    while (result.dataLength > 1 && result.data[result.dataLength - 1] == 0)
        result.dataLength--;

    // overflow check
    int lastPos = maxLength - 1;
    if ((bi1.data[lastPos] & 0x80000000) == (bi2.data[lastPos] & 0x80000000) &&
        (result.data[lastPos] & 0x80000000) != (bi1.data[lastPos] & 0x80000000)) {
        // throw(new ArithmeticException());
    }

    return result;
}

BigInteger BigInteger::operator+=(const BigInteger& bi2) const
{
    return *this + bi2;
}

BigInteger BigInteger::operator-=(const BigInteger& bi2) const
{
    return *this - bi2;
}

BigInteger BigInteger::modInverse(const BigInteger& modulus) const
{
    BigInteger p[2] = { BigInteger(0), BigInteger(1) };
    BigInteger* q = new BigInteger[2];                  // quotients
    BigInteger r[2] = { BigInteger(0), BigInteger(0) }; // remainders

    int step = 0;

    BigInteger a = modulus;
    BigInteger b = *this;

    while (b.dataLength > 1 || (b.dataLength == 1 && b.data[0] != 0)) {
        BigInteger quotient;
        BigInteger remainder;

        if (step > 1) {
            BigInteger pval = (p[0] - (p[1] * q[0])) % modulus;
            p[0] = p[1];
            p[1] = pval;
        }

        if (b.dataLength == 1)
            singleByteDivide(a, b, quotient, remainder);
        else
            multiByteDivide(a, b, quotient, remainder);

        q[0] = q[1];
        r[0] = r[1];
        q[1] = quotient;
        r[1] = remainder;

        a = b;
        b = remainder;

        step++;
    }

    if (r[0].dataLength > 1 || (r[0].dataLength == 1 && r[0].data[0] != 1)) {
        // throw(new ArithmeticException("No inverse!"));
    }

    BigInteger result = ((p[0] - (p[1] * q[0])) % modulus);

    if ((result.data[maxLength - 1] & 0x80000000) != 0)
        result += modulus; // get the least positive modulus

    return result;
}

BigInteger BigInteger::operator*(const BigInteger& bi) const
{
    BigInteger bi1 = *this;
    BigInteger bi2 = bi;

    int lastPos = maxLength - 1;
    bool bi1Neg = false, bi2Neg = false;

    // take the absolute value of the inputs
    try
    {
        if ((bi1.data[lastPos] & 0x80000000) != 0) // bi1 negative
        {
            bi1Neg = true;
            bi1 = -bi1;
        }
        if ((bi2.data[lastPos] & 0x80000000) != 0) // bi2 negative
        {
            bi2Neg = true;
            bi2 = -bi2;
        }
    }
    catch (std::exception)
    {
    }

    BigInteger result;

    // multiply the absolute values
    try
    {
        for (int i = 0; i < bi1.dataLength; i++) {
            if (bi1.data[i] == 0)
                continue;

            ulong mcarry = 0;
            for (int j = 0, k = i; j < bi2.dataLength; j++, k++) {
                // k = i + j
                ulong val = ((ulong)bi1.data[i] * (ulong)bi2.data[j]) + (ulong)result.data[k] + mcarry;

                result.data[k] = (uint)(val & 0xFFFFFFFF);
                mcarry = (val >> 32);
            }

            if (mcarry != 0)
                result.data[i + bi2.dataLength] = (uint)mcarry;
        }
    }
    catch (std::exception)
    {
        // sthrow(new ArithmeticException("Multiplication overflow."));
    }

    result.dataLength = bi1.dataLength + bi2.dataLength;
    if (result.dataLength > maxLength)
        result.dataLength = maxLength;

    while (result.dataLength > 1 && result.data[result.dataLength - 1] == 0)
        result.dataLength--;

    // overflow check (result is -ve)
    if ((result.data[lastPos] & 0x80000000) != 0) {
        if (bi1Neg != bi2Neg && result.data[lastPos] == 0x80000000) // different sign
        {
            // handle the special case where multiplication produces
            // a max negative number in 2's complement.

            if (result.dataLength == 1)
                return result;
            else {
                bool isMaxNeg = true;
                for (int i = 0; i < result.dataLength - 1 && isMaxNeg; i++) {
                    if (result.data[i] != 0)
                        isMaxNeg = false;
                }

                if (isMaxNeg)
                    return result;
            }
        }

        // throw(new ArithmeticException("Multiplication overflow."));
    }

    // if input has different signs, then result is -ve
    if (bi1Neg != bi2Neg)
        return -result;

    return result;
}

BigInteger BigInteger::operator%(const BigInteger& bi) const
{
    BigInteger bi1 = *this;
    BigInteger bi2 = bi;
    BigInteger quotient;
    BigInteger remainder = bi1;

    int lastPos = maxLength - 1;
    bool dividendNeg = false;

    if ((bi1.data[lastPos] & 0x80000000) != 0) // bi1 negative
    {
        bi1 = -bi1;
        dividendNeg = true;
    }
    if ((bi2.data[lastPos] & 0x80000000) != 0) // bi2 negative
        bi2 = -bi2;

    if (bi1 < bi2) {
        return remainder;
    } else {
        if (bi2.dataLength == 1) {
            singleByteDivide(bi1, bi2, quotient, remainder);
        } else
            multiByteDivide(bi1, bi2, quotient, remainder);

        if (dividendNeg)
            return -remainder;

        return remainder;
    }
}

void BigInteger::multiByteDivide(const BigInteger& bi1,
                                 const BigInteger& bi2_,
                                 BigInteger& outQuotient,
                                 BigInteger& outRemainder)
{
    BigInteger bi2 = bi2_;

    printf("---> %s \n", bi1.ToString().c_str());

    uint result[maxLength];
    int remainderLen = bi1.dataLength + 1;
    uint remainder[remainderLen];

    uint mask = 0x80000000;
    uint val = bi2.data[bi2.dataLength - 1];
    int shift = 0, resultPos = 0;

    while (mask != 0 && (val & mask) == 0) {
        shift++;
        mask >>= 1;
    }

    for (int i = 0; i < bi1.dataLength; i++)
        remainder[i] = bi1.data[i];
    shiftLeft(remainder, shift, remainderLen);
    bi2 = bi2 << shift;

    int j = remainderLen - bi2.dataLength;
    int pos = remainderLen - 1;

    ulong firstDivisorByte = bi2.data[bi2.dataLength - 1];
    ulong secondDivisorByte = bi2.data[bi2.dataLength - 2];

    int divisorLen = bi2.dataLength + 1;
    uint dividendPart[divisorLen];

    while (j > 0) {
        ulong dividend = ((ulong)remainder[pos] << 32) + (ulong)remainder[pos - 1];
        ulong q_hat = dividend / firstDivisorByte;
        ulong r_hat = dividend % firstDivisorByte;

        bool done = false;
        while (!done) {
            done = true;

            if (q_hat == 0x100000000 || (q_hat * secondDivisorByte) > ((r_hat << 32) + remainder[pos - 2])) {
                q_hat--;
                r_hat += firstDivisorByte;

                if (r_hat < 0x100000000)
                    done = false;
            }
        }

        for (int h = 0; h < divisorLen; h++)
            dividendPart[h] = remainder[pos - h];

        BigInteger kk(dividendPart, divisorLen);
        BigInteger ss = bi2 * (long)q_hat;

        while (ss > kk) {
            q_hat--;
            ss -= bi2;
        }

        BigInteger yy = kk - ss;

        for (int h = 0; h < divisorLen; h++)
            remainder[pos - h] = yy.data[bi2.dataLength - h];

        result[resultPos++] = (uint)q_hat;

        pos--;
        j--;
    }

    outQuotient.dataLength = resultPos;
    int y = 0;
    for (int x = outQuotient.dataLength - 1; x >= 0; x--, y++)
        outQuotient.data[y] = result[x];
    for (; y < maxLength; y++)
        outQuotient.data[y] = 0;

    while (outQuotient.dataLength > 1 && outQuotient.data[outQuotient.dataLength - 1] == 0)
        outQuotient.dataLength--;

    if (outQuotient.dataLength == 0)
        outQuotient.dataLength = 1;

    outRemainder.dataLength = shiftRight(remainder, shift, remainderLen);

    for (y = 0; y < outRemainder.dataLength; y++)
        outRemainder.data[y] = remainder[y];
    for (; y < maxLength; y++)
        outRemainder.data[y] = 0;
}

BigInteger BigInteger::operator<<(int shiftVal) const
{
    BigInteger bi1 = *this;
    BigInteger result = bi1;
    result.dataLength = shiftLeft(result.data, shiftVal, maxLength);

    return result;
}

int BigInteger::shiftLeft(uint buffer[], int shiftVal, int Length)
{
    int shiftAmount = 32;
    int bufLen = Length;

    while (bufLen > 1 && buffer[bufLen - 1] == 0)
        bufLen--;

    for (int count = shiftVal; count > 0;) {
        if (count < shiftAmount)
            shiftAmount = count;

        ulong carry = 0;
        for (int i = 0; i < bufLen; i++) {
            ulong val = ((ulong)buffer[i]) << shiftAmount;
            val |= carry;

            buffer[i] = (uint)(val & 0xFFFFFFFF);
            carry = val >> 32;
        }

        if (carry != 0) {
            if (bufLen + 1 <= Length) {
                buffer[bufLen] = (uint)carry;
                bufLen++;
            }
        }
        count -= shiftAmount;
    }
    return bufLen;
}

int BigInteger::shiftRight(uint buffer[], int shiftVal, int Length)
{
    int shiftAmount = 32;
    int invShift = 0;
    int bufLen = Length;

    while (bufLen > 1 && buffer[bufLen - 1] == 0)
        bufLen--;

    for (int count = shiftVal; count > 0;) {
        if (count < shiftAmount) {
            shiftAmount = count;
            invShift = 32 - shiftAmount;
        }

        ulong carry = 0;
        for (int i = bufLen - 1; i >= 0; i--) {
            ulong val = ((ulong)buffer[i]) >> shiftAmount;
            val |= carry;

            carry = ((ulong)buffer[i]) << invShift;
            buffer[i] = (uint)(val);
        }

        count -= shiftAmount;
    }

    while (bufLen > 1 && buffer[bufLen - 1] == 0)
        bufLen--;

    return bufLen;
}

bool BigInteger::operator>(const BigInteger& bi2) const
{
    int pos = maxLength - 1;
    BigInteger bi1 = *this;

    // bi1 is negative, bi2 is positive
    if ((bi1.data[pos] & 0x80000000) != 0 && (bi2.data[pos] & 0x80000000) == 0)
        return false;

    // bi1 is positive, bi2 is negative
    else if ((bi1.data[pos] & 0x80000000) == 0 && (bi2.data[pos] & 0x80000000) != 0)
        return true;

    // same sign
    int len = (bi1.dataLength > bi2.dataLength) ? bi1.dataLength : bi2.dataLength;
    for (pos = len - 1; pos >= 0 && bi1.data[pos] == bi2.data[pos]; pos--)
        ;

    if (pos >= 0) {
        if (bi1.data[pos] > bi2.data[pos])
            return true;
        return false;
    }
    return false;
}

bool BigInteger::operator==(const BigInteger& bi2) const
{
    BigInteger bi1 = *this;
    return bi1.Equals(bi2);
}

bool BigInteger::operator!=(const BigInteger& bi2) const
{
    BigInteger bi1 = *this;
    return !(bi1.Equals(bi2));
}

bool BigInteger::Equals(const BigInteger& bi) const
{

    if (this->dataLength != bi.dataLength)
        return false;

    for (int i = 0; i < this->dataLength; i++) {
        if (this->data[i] != bi.data[i])
            return false;
    }
    return true;
}

BigInteger BigInteger::operator/(const BigInteger& bi) const
{
    BigInteger bi2 = bi;
    BigInteger bi1 = *this;
    BigInteger quotient;
    BigInteger remainder;

    int lastPos = maxLength - 1;
    bool divisorNeg = false, dividendNeg = false;

    if ((bi1.data[lastPos] & 0x80000000) != 0) // bi1 negative
    {
        bi1 = -bi1;
        dividendNeg = true;
    }
    if ((bi2.data[lastPos] & 0x80000000) != 0) // bi2 negative
    {
        bi2 = -bi2;
        divisorNeg = true;
    }

    if (bi1 < bi2) {
        return quotient;
    } else {
        if (bi2.dataLength == 1) {
            ;
            singleByteDivide(bi1, bi2, quotient, remainder);
        } else {
            multiByteDivide(bi1, bi2, quotient, remainder);
        }

        if (dividendNeg != divisorNeg)
            return -quotient;
        return quotient;
    }
}

void BigInteger::genRandomBits(int bits)
{
    int dwords = bits >> 5;
    int remBits = bits & 0x1F;

    if (remBits != 0)
        dwords++;

    if (dwords > maxLength) {
        // throw(new ArithmeticException("Number of required bits > maxLength."));
    }

    for (int i = 0; i < dwords; i++) {
        double RandomDouble = (double)std::rand() / RAND_MAX;
        data[i] = (uint)(RandomDouble * 0x100000000);
    }

    for (int i = dwords; i < maxLength; i++)
        data[i] = 0;

    if (remBits != 0) {
        uint mask = (uint)(0x01 << (remBits - 1));
        data[dwords - 1] |= mask;

        mask = (uint)(0xFFFFFFFF >> (32 - remBits));
        data[dwords - 1] &= mask;
    } else
        data[dwords - 1] |= 0x80000000;

    dataLength = dwords;

    if (dataLength == 0)
        dataLength = 1;
}

//***********************************************************************
// Overloading of bitwise AND operator
//***********************************************************************
BigInteger BigInteger::operator&(const BigInteger& bi2) const
{
    BigInteger bi1 = *this;
    BigInteger result;

    int len = (bi1.dataLength > bi2.dataLength) ? bi1.dataLength : bi2.dataLength;

    for (int i = 0; i < len; i++) {
        uint sum = (uint)(bi1.data[i] & bi2.data[i]);
        result.data[i] = sum;
    }

    result.dataLength = maxLength;

    while (result.dataLength > 1 && result.data[result.dataLength - 1] == 0)
        result.dataLength--;

    return result;
}

//***********************************************************************
// Overloading of the unary ++ operator
//***********************************************************************
/*
BigInteger BigInteger::operator++() const
{
    BigInteger bi1 = *this;
    BigInteger result = *this;

    long val, carry = 1;
    int index = 0;

    while (carry != 0 && index < maxLength) {
        val = (long)(result.data[index]);
        val++;

        result.data[index] = (uint)(val & 0xFFFFFFFF);
        carry = val >> 32;

        index++;
    }

    if (index > result.dataLength)
        result.dataLength = index;
    else {
        while (result.dataLength > 1 && result.data[result.dataLength - 1] == 0)
            result.dataLength--;
    }

    // overflow check
    int lastPos = maxLength - 1;

    // overflow if initial value was +ve but ++ caused a sign
    // change to negative.

    if ((bi1.data[lastPos] & 0x80000000) == 0 &&
        (result.data[lastPos] & 0x80000000) != (bi1.data[lastPos] & 0x80000000)) {
        //throw(new ArithmeticException("Overflow in ++."));
    }
    return result;
}
*/

//***********************************************************************
// Overloading of unary >> operators
//***********************************************************************
BigInteger BigInteger::operator>>(int shiftVal) const
{
    BigInteger bi1 = *this;
    BigInteger result = bi1;
    result.dataLength = shiftRight(result.data, shiftVal, result.dataLength);

    if ((bi1.data[maxLength - 1] & 0x80000000) != 0) // negative
    {
        for (int i = maxLength - 1; i >= result.dataLength; i--)
            result.data[i] = 0xFFFFFFFF;

        uint mask = 0x80000000;
        for (int i = 0; i < 32; i++) {
            if ((result.data[result.dataLength - 1] & mask) != 0)
                break;

            result.data[result.dataLength - 1] |= mask;
            mask >>= 1;
        }
        result.dataLength = maxLength;
    }

    return result;
}

//***********************************************************************
// Modulo Exponentiation
//***********************************************************************

BigInteger BigInteger::modPow(const BigInteger& exp, const BigInteger& n_) const
{
    BigInteger n = n_;

    if ((exp.data[maxLength - 1] & 0x80000000) != 0) {
        // throw(new ArithmeticException("Positive exponents only."));
    }

    BigInteger resultNum = 1;
    BigInteger tempNum;

    bool thisNegative = false;

    if ((this->data[maxLength - 1] & 0x80000000) != 0) // negative this
    {
        tempNum = -(*this) % n;
        thisNegative = true;
    } else
        tempNum = *this % n; // ensures (tempNum * tempNum) < b^(2k)

    if ((n.data[maxLength - 1] & 0x80000000) != 0) // negative n
        n = -n;

    // calculate constant = b^(2k) / m
    BigInteger constant;

    int i = n.dataLength << 1;
    constant.data[i] = 0x00000001;
    constant.dataLength = i + 1;

    constant = constant / n;
    int totalBits = exp.bitCount();
    int count = 0;

    // perform squaring and multiply exponentiation
    for (int pos = 0; pos < exp.dataLength; pos++) {
        uint mask = 0x01;
        // Console.WriteLine("pos = " + pos);

        for (int index = 0; index < 32; index++) {
            if ((exp.data[pos] & mask) != 0)
                resultNum = BarrettReduction(resultNum * tempNum, n, constant);

            mask <<= 1;

            tempNum = BarrettReduction(tempNum * tempNum, n, constant);

            if (tempNum.dataLength == 1 && tempNum.data[0] == 1) {
                if (thisNegative && (exp.data[0] & 0x1) != 0) // odd exp
                    return -resultNum;
                return resultNum;
            }
            count++;
            if (count == totalBits)
                break;
        }
    }

    if (thisNegative && (exp.data[0] & 0x1) != 0) // odd exp
        return -resultNum;

    return resultNum;
}

//***********************************************************************
// Returns the position of the most significant bit in the BigInteger.
//
// Eg.  The result is 0, if the value of BigInteger is 0...0000 0000
//      The result is 1, if the value of BigInteger is 0...0000 0001
//      The result is 2, if the value of BigInteger is 0...0000 0010
//      The result is 2, if the value of BigInteger is 0...0000 0011
//
//***********************************************************************

int BigInteger::bitCount() const
{
    BigInteger bi = *this;

    while (bi.dataLength > 1 && bi.data[bi.dataLength - 1] == 0)
        bi.dataLength--;

    uint value = bi.data[bi.dataLength - 1];
    uint mask = 0x80000000;
    int bits = 32;

    while (bits > 0 && (value & mask) == 0) {
        bits--;
        mask >>= 1;
    }
    bits += ((bi.dataLength - 1) << 5);

    return bits;
}

//***********************************************************************
// Fast calculation of modular reduction using Barrett's reduction.
// Requires x < b^(2k), where b is the base.  In this case, base is
// 2^32 (uint).
//
// Reference [4]
//***********************************************************************

BigInteger BigInteger::BarrettReduction(const BigInteger& x, const BigInteger& n, const BigInteger& constant) const
{
    int k = n.dataLength, kPlusOne = k + 1, kMinusOne = k - 1;

    BigInteger q1;

    // q1 = x / b^(k-1)
    for (int i = kMinusOne, j = 0; i < x.dataLength; i++, j++)
        q1.data[j] = x.data[i];
    q1.dataLength = x.dataLength - kMinusOne;
    if (q1.dataLength <= 0)
        q1.dataLength = 1;

    BigInteger q2 = q1 * constant;
    BigInteger q3;

    // q3 = q2 / b^(k+1)
    for (int i = kPlusOne, j = 0; i < q2.dataLength; i++, j++)
        q3.data[j] = q2.data[i];
    q3.dataLength = q2.dataLength - kPlusOne;
    if (q3.dataLength <= 0)
        q3.dataLength = 1;

    // r1 = x mod b^(k+1)
    // i.e. keep the lowest (k+1) words
    BigInteger r1;
    int lengthToCopy = (x.dataLength > kPlusOne) ? kPlusOne : x.dataLength;
    for (int i = 0; i < lengthToCopy; i++)
        r1.data[i] = x.data[i];
    r1.dataLength = lengthToCopy;

    // r2 = (q3 * n) mod b^(k+1)
    // partial multiplication of q3 and n

    BigInteger r2;
    for (int i = 0; i < q3.dataLength; i++) {
        if (q3.data[i] == 0)
            continue;

        ulong mcarry = 0;
        int t = i;
        for (int j = 0; j < n.dataLength && t < kPlusOne; j++, t++) {
            // t = i + j
            ulong val = ((ulong)q3.data[i] * (ulong)n.data[j]) + (ulong)r2.data[t] + mcarry;

            r2.data[t] = (uint)(val & 0xFFFFFFFF);
            mcarry = (val >> 32);
        }

        if (t < kPlusOne)
            r2.data[t] = (uint)mcarry;
    }

    r2.dataLength = kPlusOne;
    while (r2.dataLength > 1 && r2.data[r2.dataLength - 1] == 0)
        r2.dataLength--;

    r1 -= r2;
    if ((r1.data[maxLength - 1] & 0x80000000) != 0) // negative
    {
        BigInteger val;
        val.data[kPlusOne] = 0x00000001;
        val.dataLength = kPlusOne + 1;
        r1 += val;
    }

    while (r1 >= n)
        r1 -= n;

    return r1;
}

bool BigInteger::operator>=(const BigInteger& bi2) const
{
    BigInteger bi1 = *this;
    return (bi1 == bi2 || bi1 > bi2);
}