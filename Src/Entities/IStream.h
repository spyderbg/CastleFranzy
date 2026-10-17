// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// IStream
//
//		Streamable entity: can be read and/or write to
//
//	Notes:
//		- Cannot use the name "IStream": already defined in the root namespace for the Win32 version
//
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ENTITIES_ISTREAM_H
#define ENTITIES_ISTREAM_H



class IStreamBase
{
protected:
	// The different impl. should have a factory class that gives the delete/close options
	// (make sure to declare the friendship between your impl. and your factory)
	virtual						~IStreamBase() {};

public:
	virtual StreamSize			size()								const	pure;

	virtual bool				canRead()							const	pure;
	virtual bool				canWrite()							const	pure;

	virtual StreamSize			read(byte* data, StreamSize len)			pure;
	virtual byte				readByte()			pure;
	virtual short				readShort()			pure;
	virtual int					readInt()			pure;
	virtual StreamSize			write(const byte* data, StreamSize len)		pure;
};



#endif
