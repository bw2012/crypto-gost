#ifndef HASHGOST_H
#define HASHGOST_H

#include <vector>
#include <algorithm>
#include <array>

typedef unsigned char byte;
typedef unsigned long ulong;

typedef std::array<byte, 64> barr64;

#include <stdio.h>


static void byte_vector_copy(const std::vector<byte>& source, int source_pos, std::vector<byte>& target, int target_pos, int count)
{
    for (auto aaa = 0; aaa < count; aaa++) {
        target[target_pos + aaa] = source[source_pos + aaa];
    }
}

static void byte_vector_copy(const std::vector<byte>& source, int source_pos, barr64& target, int target_pos, int count)
{
    for (auto aaa = 0; aaa < count; aaa++) {
        target[target_pos + aaa] = source[source_pos + aaa];
    }
}

static void byte_vector_copy(const barr64& source, int source_pos, barr64& target, int target_pos, int count)
{
    for (auto aaa = 0; aaa < count; aaa++) {
        target[target_pos + aaa] = source[source_pos + aaa];
    }
}

class HashGOST
{

public:
    HashGOST(int outputLenght);
    ~HashGOST();
    
    std::vector<byte> GetHash(std::vector<byte> message);

private:
    barr64 iv;
    barr64 N;
    barr64 Sigma;

    int outLen = 0;

    const static ulong A[];
    const static byte Sbox[];
    const static byte Tau[];
    const static byte C[12][64];

    barr64 AddModulo512(barr64 a, std::vector<byte> b)
    {
        barr64 result;
        barr64 tempA;
        barr64 tempB;

        int i = 0, t = 0;

        byte_vector_copy(a, 0, tempA, 64 - a.size(), a.size());
        byte_vector_copy(b, 0, tempB, 64 - b.size(), b.size());

        for (i = 63; i >= 0; i--) {
            t = tempA[i] + tempB[i] + (t >> 8);
            result[i] = (unsigned char)(t & 0xFF);
        }

        return result;
    }

    barr64 AddXor512(barr64 a, barr64 b)
    {
        barr64 result;
        for (int i = 0; i < 64; i++)
            result[i] = (byte)(a[i] ^ b[i]);
        return result;
    }

    barr64 S(barr64 state)
    {
        barr64 result;

        for (int i = 0; i < 64; i++) {
            result[i] = Sbox[state[i]];
        }
        return result;
    }

    barr64 P(barr64 state)
    {
        barr64 result;
        for (int i = 0; i < 64; i++) {
            result[i] = state[Tau[i]];
        }
        return result;
    }

    barr64 L(barr64 state)
    {
        barr64 result;

        unsigned long long v = 0;
        int i = 0, j = 0, k = 0;

        for (i = 0; i < 8; i++) {
            v = 0;
            for (k = 0; k < 8; k++) {
                for (j = 0; j < 8; j++) {
                    if ((state[i * 8 + k] & (1 << (7 - j))) != 0)
                        v ^= A[k * 8 + j];
                }
            }
            for (k = 0; k < 8; k++) {
                result[i * 8 + k] = (v & ((unsigned long long)0xFF << (7 - k) * 8)) >> (7 - k) * 8;
            }
        }

        return result;
    }

    barr64 KeySchedule(barr64 K, int i)
    {
        const byte* Ci = C[i];
        barr64 ci;
        for(i=0; i<64; i++) ci[i] = Ci[i];
        //ci.assign(Ci, Ci + 64);

        barr64 k = AddXor512(K, ci);
        k = S(k);
        k = P(k);
        k = L(k);
        return k;
    }

    barr64 E(barr64 K, barr64 m)
    {
        barr64 state = AddXor512(K, m);
        for (int i = 0; i < 12; i++) {
            state = S(state);
            state = P(state);
            state = L(state);
            K = KeySchedule(K, i);
            state = AddXor512(state, K);
        }
        return state;
    }

    barr64 G_n(barr64 N, barr64 h, barr64 m)
    {
        barr64 K = AddXor512(h, N);
        K = S(K);
        K = P(K);
        K = L(K);
        
        barr64 t = E(K, m);
        t = AddXor512(t, h);
        
        return AddXor512(t, m);
    }
};

#endif // HASHGOST_H
