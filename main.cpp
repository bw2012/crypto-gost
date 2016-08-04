#include <stdio.h>
#include "BigInteger.h"
#include "HashGOST.h"
#include "ECPoint.h"
#include "DSGost.h"

int char2int(char input)
{
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;

    return 0;
    // throw std::invalid_argument("Invalid input string");
}

std::vector<byte> FromHexStringToByte(std::string input)
{
    std::vector<byte> data; //= new byte[input.Length / 2];
    std::string HexByte = "";

    for (int i = 0; i < input.length() / 2; i++) {
        HexByte = input.substr(i * 2, 2);
        unsigned char temp = char2int(HexByte[0]) * 16 + char2int(HexByte[1]);
        data.push_back(temp);
    }

    return data;
}

// TODO: запилить конструктор и з вектора в бигинтеджер
// TODO: не работает ECPoint multiply

int main(int argc, char** argv)
{

    BigInteger p("6277101735386680763835789423207666416083908700390324961279", 10);
    printf("p  ---> %s\n", p.ToString().c_str());

    BigInteger a("-3", 10);
    printf("a  ---> %s\n", a.ToString().c_str());

    BigInteger b("64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1", 16);
    printf("b  ---> %s\n", b.ToString(16).c_str());

    BigInteger n("ffffffffffffffffffffffff99def836146bc9b1b4d22831", 16);
    printf("n  ---> %s\n", n.ToString(16).c_str());

    std::string xGs = "03188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012";
    std::vector<byte> xG = FromHexStringToByte(xGs);
    printf("xG ---> %s\n", xGs.c_str());

    std::string test = "test";
    std::vector<unsigned char> bytes(test.begin(), test.end());

    HashGOST hash(256);
    std::vector<unsigned char> hash_test = hash.GetHash(bytes);

    std::string hres = "";
    for (auto ttt : hash_test) {
        // printf("hash_test -----> %d\n", ttt);
        char tmp[2];
        sprintf(tmp, "%02x", ttt);
        hres += tmp;
    }

    printf("\n");
    printf("HASH ---> %s\n", hres.c_str());

    DSGost DS(p, a, b, n, xG);
    BigInteger d = DS.GenPrivateKey(192);
    printf("private key = %s\n", d.ToString(10).c_str());

    d = BigInteger("4251051025949052349187729432179324385897356673788666960178", 10);

    ECPoint Q = DS.GenPublicKey(d);
    
    printf("\n\npublic key\n");
    printf("a  ---> %s\n", Q.a.ToString().c_str());
    printf("b  ---> %s\n", Q.b.ToString().c_str());
    printf("FieldChar  ---> %s\n", Q.FieldChar.ToString().c_str());
    printf("x  ---> %s\n", Q.x.ToString().c_str());
    printf("y  ---> %s\n", Q.y.ToString().c_str());

    std::string sign = DS.SingGen(bytes, d);
    printf("\n\n\n");
    printf("Sign --> %s\n", sign.c_str());
    
    exit(0);

    bool result = DS.SingVer(bytes, sign, Q);

    if (result) {
        printf("Correct\n");
    }else{
        printf("Wrong\n");
    }

    /*
    BigInteger t("29515218379636995773351698872144066530463441601111065046740229564272930713805", 10);
    BigInteger n("6277101735386680763835789423207666416083908700390324961279", 10);

    BigInteger res = t % n;
    printf("res  ---> %s\n", res.ToString(10).c_str());
     */

    return 0;
}
