#pragma once
#include <fstream>
#include <iostream>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/multiprecision/random.hpp>
#include <boost/multiprecision/miller_rabin.hpp>

#define NUM 10
namespace bordm = boost::random;

typedef boost::multiprecision::int1024_t DataType;

class RSA
{
	
public:
	RSA()
	{
		Key();
	}
	DataType GetPkey()
	{
		return m_pkey;//获取公钥
	}
	void EncryptionFile(const char * filename, const char * fileout)//对文件进行分块加密
	{
		std::ifstream fin(filename, std::ifstream::binary);
		std::ofstream fout(fileout, std::ofstream::binary);
		if (!fin.is_open())
		{
			perror("input file open failed!\n");
			return;
		}
		char* buffer = new char[NUM];
		DataType* bufferout = new DataType[NUM];
		while (!fin.eof())
		{
			fin.read(buffer, NUM);
			int curNum = fin.gcount();
			for (int i = 0; i < curNum; i++)
			{
				bufferout[i] = ecrept(buffer[i], m_ekey, m_pkey);
			}
			fout.write((char*)bufferout, curNum * sizeof(DataType));
		}
		delete[] buffer;
		delete[] bufferout;
		fin.close();
		fout.close();
	}
	void DecryptFile(const char * filename, const char * fileout)//对文件进行分块解密
	{
		std::ifstream fin(filename, std::ifstream::binary);
		std::ofstream fout(fileout, std::ofstream::binary);
		if (!fin.is_open())
		{
			perror("file open failed!\n");
			return;
		}
		DataType* buffer = new DataType[NUM];

		char* bufferout = new char[NUM];
		while (!fin.eof())
		{
			fin.read((char*)buffer, NUM * sizeof(DataType));
			int num = fin.gcount();
			num /= sizeof(DataType);
			for (int i = 0; i < num; i++)
			{
				bufferout[i] = (char)decrept(buffer[i], m_dkey, m_pkey);
			}
			fout.write(bufferout, num);
		}
		delete[] buffer;
		delete[] bufferout;
		fin.close();
		fout.close();
	}
private:
	DataType m_ekey;
	DataType m_dkey;
	DataType m_pkey;

	void Key()//生成公钥、私钥
	{
		DataType prime1 = Prime();
		DataType prime2 = Prime();

		while (prime1 == prime2)
		{
			prime2 = Prime();
		}
		DataType orla = Orla(prime1, prime2);
		m_pkey = Pkey(prime1, prime2);
		m_ekey = Ekey(orla);
		m_dkey = Dkey(orla, m_ekey);
	}

	DataType Prime()//得到素数 ※
	{
		bordm::mt19937 Gen(time(nullptr));//随机数发生器
		bordm::uniform_int_distribution<DataType> Breadth(0, DataType(1) << 512);//随机数的范围
		DataType prime;
		while (true)
		{
			prime = Breadth(Gen);
			if (isPrime(prime))
				break;
		}
		return prime;
	}
	DataType Pkey(DataType prime1, DataType prime2)//计算n
	{
		return prime1 * prime2;
	}
	DataType Orla(DataType p, DataType q)//计算f(n)
	{
		return (p - 1) * (q - 1);
	}
	DataType Ekey(DataType orla)
	{
		bordm::mt19937 Gen(time(nullptr));
		bordm::uniform_int_distribution<DataType> dist(2, orla);
		DataType ekey;
		while (true)
		{
			ekey = dist(Gen);
			if (getGcd(orla, ekey) == 1)
				return ekey;
		}
	}
	DataType Dkey(DataType orla, DataType ekey)
	{
		DataType x = 0, y = 0;
		exGcd(ekey, orla, x, y);
		return (x % orla + orla) % orla;
	}
	DataType ecrept(DataType data, DataType ekey, DataType pkey)//蒙哥马利算法
	{
		DataType a = data;
		DataType tmp = 1;
		while (ekey)
		{
			if (ekey & 1)
				tmp = (tmp * a) % pkey;
			ekey >>= 1;
			a = (a * a) % pkey;
		}
		return tmp;
	}
	DataType decrept(DataType data, DataType dkey, DataType pkey)
	{
		return ecrept(data, dkey, pkey);
	}
	bool isPrime(DataType data)
	{
		bordm::mt11213b gen(time(nullptr));
		if (miller_rabin_test(data, 25, gen))
		{
			if (miller_rabin_test((data - 1) / 2, 25, gen))//0.25的25次方的可能是错误的
			{
				return true;
			}
		}
		return false;
	}
	DataType getGcd(DataType data1, DataType data2)//辗转相除法求最大公约数(欧几里得算法)
	{
		DataType ret;
		while (ret = data1 % data2)
		{
			data1 = data2;
			data2 = ret;
		}
		return data2;
	}
	DataType exGcd(DataType a, DataType b, DataType& x, DataType& y)//扩展的欧几里得算法 ※
	{
		if (b == 0)
		{
			x = 1;
			y = 0;
			return a;
		}
		DataType gcd = exGcd(b, a % b, x, y);
		DataType x1 = x, y1 = y;
		x = y1;
		y = x1 - a / b * y1;
		return gcd;
	}
};

