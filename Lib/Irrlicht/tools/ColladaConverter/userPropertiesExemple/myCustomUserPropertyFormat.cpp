#include "resFileWriter.h"

void
WriteUserProperties(Chunk &chk, std::string &userProperties)
{
	chk.WriteStr(userProperties.c_str());	
}