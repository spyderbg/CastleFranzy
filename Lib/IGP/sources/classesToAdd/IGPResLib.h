////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////

#import "IGPFile.h"
#import "IGPLZMAFile.h"
#import "IGPDefines.h"
class IGPResLibHeader
{
private:		

	u32*	m_offsetsArray;
	u32		m_filesNumber;

public:

	IGPResLibHeader() : m_filesNumber(0), m_offsetsArray(NULL) {};
	~IGPResLibHeader() 
	{ 
		SAFE_DELETE(m_offsetsArray); 
	};

	int LoadHeader(IGPLZMAFile *pResource, u32 baseOffset = 0);

	inline int GetNumberOfFiles()
	{
		return m_filesNumber;
	};

	inline u32 GetSize(u32 id)
	{
		if (id >= m_filesNumber)
			return 0;

		return (m_offsetsArray[id + 1] - m_offsetsArray[id]);
	};

	inline u32 GetOffset(u32 id)
	{
		if (id >= m_filesNumber)
			return 0;

		return m_offsetsArray[id];
	};
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class IGPResLib
{
public:
	static IGPResLib* Open(TFileName fileName, int i_cache, int i_offset, bool bUseLZMA = false);

	static void Close(IGPResLib*& lib);

	bool ReadFile( char*& buffer, int nFileID );

	inline int GetFileSize( int nFileID )	{ return m_LibHeader.GetSize( nFileID ); }
	inline int GetFileOffset( int nFileID ) { return m_LibHeader.GetOffset( nFileID ); }

private:
	IGPResLib();
	~IGPResLib();

protected:
	IGPLZMAFile*		m_pFile;
	IGPResLibHeader	m_LibHeader;
	int				m_nFileOffset;

};


////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////
