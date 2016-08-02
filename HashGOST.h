#ifndef HASHGOST_H
#define HASHGOST_H

#include <vector>
#include <algorithm>

typedef unsigned char byte;
typedef unsigned long ulong;

void byte_vector_copy(std::vector<byte> source, int source_pos, std::vector<byte> target, int target_pos, int count)
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
    unsigned char iv[64];
    unsigned char N[64];
    unsigned char Sigma[64];

    int outLen = 0;

    const static ulong A[];
    const static byte Sbox[];
    const static byte Tau[];
    const static byte C[12][64];

    std::vector<byte> AddModulo512(std::vector<byte> a, std::vector<byte> b)
    {
        std::vector<byte> result;
        result.reserve(64);

        std::vector<byte> tempA = a;
        std::vector<byte> tempB = b;

        int i = 0, t = 0;

        // Array.Copy(a, 0, tempA, 64 - a.Length, a.Length);
        // Array.Copy(b, 0, tempB, 64 - b.Length, b.Length);

        for (i = 63; i >= 0; i--) {
            t = tempA[i] + tempB[i] + (t >> 8);
            result[i] = (unsigned char)(t & 0xFF);
        }

        return result;
    }

    std::vector<byte> AddXor512(std::vector<byte> a, std::vector<byte> b)
    {
        std::vector<byte> result;
        result.reserve(64);

        for (int i = 0; i < 64; i++)
            result[i] = (byte)(a[i] ^ b[i]);
        return result;
    }

    std::vector<byte> S(std::vector<byte> state)
    {
        std::vector<byte> result;
        result.reserve(64);

        for (int i = 0; i < 64; i++) {
            result[i] = Sbox[state[i]];
        }
        return result;
    }

    std::vector<byte> P(std::vector<byte> state)
    {
        std::vector<byte> result;
        result.reserve(64);

        for (int i = 0; i < 64; i++) {
            result[i] = state[Tau[i]];
        }
        return result;
    }

    std::vector<byte> L(std::vector<byte> state)
    {
        std::vector<byte> result;
        result.reserve(64);

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

    std::vector<byte> KeySchedule(std::vector<byte> K, int i)
    {
        const byte* Ci = C[i];
        std::vector<byte> ci;
        ci.assign(Ci, Ci + 64);

        K = AddXor512(K, ci);
        K = S(K);
        K = P(K);
        K = L(K);
        return K;
    }

    std::vector<byte> E(std::vector<byte> K, std::vector<byte> m)
    {
        std::vector<byte> state = AddXor512(K, m);
        for (int i = 0; i < 12; i++) {
            state = S(state);
            state = P(state);
            state = L(state);
            K = KeySchedule(K, i);
            state = AddXor512(state, K);
        }
        return state;
    }

    std::vector<byte> G_n(std::vector<byte> N, std::vector<byte> h, std::vector<byte> m)
    {
        std::vector<byte> K = AddXor512(h, N);
        K = S(K);
        K = P(K);
        K = L(K);
        std::vector<byte> t = E(K, m);
        t = AddXor512(t, h);
        std::vector<byte> newh = AddXor512(t, m);
        return newh;
    }
};

#endif // HASHGOST_H
