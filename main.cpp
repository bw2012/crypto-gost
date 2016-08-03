#include <stdio.h>
#include "BigInteger.h"
#include "HashGOST.h"

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
    for (auto ttt : hash_test)
    {
        //printf("hash_test -----> %d\n", ttt);
        char tmp[2];
        sprintf(tmp, "%02x", ttt);
        hres += tmp;
    }
    
    printf("\n");
    printf("HASH ---> %s\n", hres.c_str());
    
    

    return 0;
}
