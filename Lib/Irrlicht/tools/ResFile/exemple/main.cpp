#include <resFileWriter.h>
#include <resFile.h>

#include <fstream>
#include <sstream>
#include <cassert>

void WriteToFile(const char* filename, const void* data, size_t length)
{
	std::fstream f(filename, std::ios_base::out | std::ios_base::binary);
	f.write(static_cast<const char*>(data), length);
	f.close();
}

int main(int argc, char** argv)
{
	{
		res::ResChunk resfile;

		unsigned char buf[256];
		for(int i = 0; i < 256; ++i)
			buf[i] = i;

		struct Data {
			unsigned int int1;
			unsigned int int2;
			unsigned int int3;
			unsigned int int4;
			res::ResString str;
			unsigned int int5;
			unsigned int int6;
			unsigned int int7;
			unsigned int int8;
			res::ResData<unsigned char> data;
			res::ResString str1;
			res::ResString str2;
			res::ResString strs[6];
			res::ResBuffer<char> file;
		};

		
		resfile.
			Write4s(0).
			Write4s(1).
			Write4s(2).
			Write4s(3).
			WriteStr("hello").
			//AddChunk("test").
				Write4u(4).
				Write4u(5).
				Write4u(6).
				Write4u(7).
				WriteRawExternal(buf, 256).
				//AddChunk().
					WriteStr("blabla").
					WriteStr("test").
					WriteStr("test1").
					WriteStr("test2").
					WriteStr("test3").
					WriteStr("test4").
					WriteStr("test5").
					WriteStr("test6").
					WriteBufferFromFile("../include/token.h");

		std::ostringstream out_be(std::ios_base::out | std::ios_base::binary);
		std::ostringstream out_le(std::ios_base::out | std::ios_base::binary);

		resfile.Write(out_be, res::Token::ENDIAN_BIG);
		resfile.Write(out_le, res::Token::ENDIAN_LITTLE);

		std::string sBe = out_be.str();
		std::string sLe = out_le.str();

		WriteToFile("test.cpp.be.bin", sBe.c_str(), sBe.size());
		WriteToFile("test.cpp.le.bin", sLe.c_str(), sLe.size());

		char* bufferBe = (char*)sBe.c_str();
		char* bufferLe = (char*)sLe.c_str();

		res::ResFile resFile(bufferBe);

		
		

	
		Data* dat = (Data*)resFile->data.ptr();
		size_t strSize = dat->str.GetSize();
		size_t dataSize = dat->data.GetSize();

	}
}
