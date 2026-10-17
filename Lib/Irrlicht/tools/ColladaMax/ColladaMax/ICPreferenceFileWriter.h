/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_PREFERENCE_FILE_WRITER_H_
#define _IC_PREFERENCE_FILE_WRITER_H_

class ICPreferenceFile;

class ICPreferenceFileWriter
{
private:
public:
	ICPreferenceFileWriter(){}
	~ICPreferenceFileWriter(){}

	// returns 0 if read ok, -1 if error
	int Write( const fm::string &filePath, ICPreferenceFile &preferenceFile );
};

#endif // _IC_PREFERENCE_FILE_WRITER_H_


