/*
	Copyright (C) 2005-2007 Feeling Software Inc.
	Portions of the code are:
	Copyright (C) 2005-2007 Sony Computer Entertainment America
	
	MIT License: http://www.opensource.org/licenses/mit-license.php
*/

#ifndef _IC_PREFERENCE_FILE_PARSER_H_
#define _IC_PREFERENCE_FILE_PARSER_H_

class ICPreferenceFile;

class ICPreferenceFileParser
{
private:
public:
	ICPreferenceFileParser(){}
	~ICPreferenceFileParser(){}

	// returns 0 if read ok, -1 if error
	int Read( const fm::string &filePath, ICPreferenceFile &preferenceFile );
};

#endif // _IC_PREFERENCE_FILE_PARSER_H_


