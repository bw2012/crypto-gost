#include <stdio.h>
#include "BigInteger.h"

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

    printf("test\n");
    /*
    BigInteger a1(100);
    BigInteger a2(20000);
    BigInteger a3(300);

    printf("a1  a2 ---> %s %s\n", a1.ToString().c_str(), a2.ToString().c_str());
    //printf("a1 + a2 ---> %s \n", (a1 + a2).ToString().c_str());
    //printf("a1 - a2 ---> %s \n", (a1 - a2).ToString().c_str());
    //printf("a1 -= a2 ---> %s \n", (a1 -= a2).ToString().c_str());
    //printf("a1 += a2 ---> %s \n", (a1 += a2).ToString().c_str());
    //printf("a1 * a2 ---> %s \n", (a1 * a2).ToString().c_str());
    printf("a2 %% a3 ---> %s \n", (a2 % a3).ToString().c_str());
    printf("a2  a3 ---> %s %s\n", a2.ToString().c_str(), a3.ToString().c_str());

    printf("a2 / a1 ---> %s \n", (a2 / a1).ToString().c_str());
    printf("a1  a2 ---> %s %s\n", a1.ToString().c_str(), a2.ToString().c_str());
    */
    BigInteger p("6277101735386680763835789423207666416083908700390324961279", 10);
    printf("---> %s\n", p.ToString().c_str());

    BigInteger a("-3", 10);
    printf("---> %s\n", a.ToString().c_str());

    BigInteger b("64210519e59c80e70fa7e9ab72243049feb8deecc146b9b1", 16);
    printf("---> %s\n", b.ToString(16).c_str());

    BigInteger n("ffffffffffffffffffffffff99def836146bc9b1b4d22831", 16);
    printf("---> %s\n", n.ToString(16).c_str());

    std::vector<byte> xG = FromHexStringToByte("03188da80eb03090f67cbf20eb43a18800f4ff0afd82ff1012");

    std::string test = "test";

    std::vector<char> bytes(test.begin(), test.end());
    for (auto& ttt : bytes) // access by reference to avoid copying
    {
        printf("-----> %d\n", ttt);
    }

    std::vector<int> test1;
    test1.reserve(20000);
    test1[10] = 10;


    return 0;
}
