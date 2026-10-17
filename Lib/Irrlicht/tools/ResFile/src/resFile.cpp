#include "stdafx.h"
#include "resFile.h"

namespace irr
{
namespace res
{

int File::Init(FileReader *pFile)
{
	// Get File Size and header size
	int iSize = pFile->getSize();
	int iHeaderSize = sizeof(struct FileHeaderData);

	// Read the data header to find the size of the OffsetTable
	struct FileHeaderData *pHeader = new FileHeaderData;
	pFile->read(pHeader, iHeaderSize);
	int iSizeOffsetTable =  pHeader->numOffsets * sizeof(int);

	// Allocate the buffers (file and offsetTable)
	char *pOffsetBuffer = new char[iSizeOffsetTable];
	char *pBuffer = new char[iSize-iSizeOffsetTable];

	// Copy the file header to the buffer
	memcpy(pBuffer,pHeader,iHeaderSize);
	// Read the rest of the file in our buffer placing the offset table in a second buffer
	pFile->read( pOffsetBuffer, iSizeOffsetTable );
	pFile->read( &pBuffer[iHeaderSize], iSize - iSizeOffsetTable - iHeaderSize );
	
	// delete the old header as it's not needed anymore
	delete pHeader;

	// Run the real init
	*this = File(pBuffer,pOffsetBuffer);

	// offset correction done, discard the offset table
	delete pOffsetBuffer;
	OffsetTable = NULL;

	return IsValid != 1;
}

int File::Init()
{
	res::FileHeaderData* p = ptr();

	if((((char*)&p->signature)[0] != 'B')
		|| (((char*)&p->signature)[1] != 'R')
		|| (((char*)&p->signature)[2] != 'E')
		|| (((char*)&p->signature)[3] != 'S'))
	{
		return -1;
	}

	if(p && (p->version & 0x8000) == 0)
	{
		p->version |= 0x8000;

		// Check if we used a deletable buffer for the offset table
		if(OffsetTable)
		{
			//replace the adress of the offset table in the struct to point to our deletable offsetTable buffer
			(&p->offsets)[0] = OffsetTable;
			int offsetTableLength = p->numOffsets * sizeof(int);
			int offsetTableEnd = offsetTableLength + p->sizeOfHeader;

			for(size_t i = 0; i < p->numOffsets; ++i)
			{
				res::Access<res::Access<int> >& offset = p->offsets[i];

				// if the location befor the normal offset table location, don't apply the location correction.
				if(reinterpret_cast<int>(offset.ptr()) >= offsetTableEnd)
					offset.OffsetToPtr(reinterpret_cast<char*>(p)-offsetTableLength);
				else
					offset.OffsetToPtr(p);

				if(i > 0)
				{
					// Ignore the offset table's length as it's already been extracted
					// if the location befor the normal offset table location, don't apply the location correction.
					if(reinterpret_cast<int>(offset.ptr()->ptr()) >= offsetTableEnd)
						offset.ptr()->OffsetToPtr(reinterpret_cast<char*>(p)-offsetTableLength);
					else
						offset.ptr()->OffsetToPtr(p);
				}
			}
		}
		else
		{
			// Offset table is included in the memory chunk and can't be deleted...
			p->offsets.OffsetToPtr(p);

			for(size_t i = 0; i < p->numOffsets; ++i)
			{
				res::Access<res::Access<int> >& offset = p->offsets[i];

				offset.OffsetToPtr(p);

				if(i > 0) offset.ptr()->OffsetToPtr(p);
			}
		}
	}

	return 0;
}

};
};
