#include "rsa.h"
#include <iostream>


int main()
{
	RSA rsa;
	std::cout << "¹«Ô¿£º"<<rsa.GetPkey() << std::endl;
	rsa.EncryptionFile("test.txt", "test.ecrept.txt");
	rsa.DecryptFile("test.ecrept.txt", "test.decrept.txt");
	return 0;
}