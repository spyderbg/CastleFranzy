// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IFileStream
//
//		File entity, add features to IStream (like seeking)
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

///////////////////////////////////////////////////////////////////////

#include "Entities/IStream.h"



class IFileStream : public IStreamBase
{
protected:
	// The different impl. should have a factory class that gives the delete/close options
	// (make sure to declare the friendship between your impl. and your factory)
	virtual						~IFileStream() {};

public:
	virtual void				seek(StreamSize offset)			pure;
	virtual StreamSize			tell()					const	pure;
	virtual int					skip(StreamSize offset) pure;
};

