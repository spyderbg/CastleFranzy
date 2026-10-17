#include "stdafx.h"

#include "Utils/StringUtils.h"
#include "Common/StringTable.h"

#define MAX_DIGITS 20

irr::core::stringw FormatNumber( int number )
{
	char buffer[MAX_DIGITS + 2];
	memset( buffer, '\0' , MAX_DIGITS + 2 );

	bool separateThousands = (number > 9999 && IS_LANG_SPANISH()) |
							 (number > 999  && IS_LANG_DEUTCH()) |
							 (number > 999  && IS_LANG_ENGLISH() |
							 (number > 9999 && IS_LANG_FRENCH()) |
							 (number > 9999 && IS_LANG_JAPANESE()) |
							 (number > 999 && IS_LANG_ITALIAN()) );
	char thousandSeparator = ' ';

	if( IS_LANG_DEUTCH() )
		thousandSeparator = '.';
	else
	if( IS_LANG_ENGLISH() | IS_LANG_JAPANESE() )
		thousandSeparator = ',';
	
	int firstDigitOff = 0;

	if( separateThousands )
	{
		for( int i = MAX_DIGITS; i >= 0; i-- )
		{
			if( number > 0 )
			{
				if( (MAX_DIGITS - i + 1)%4 == 0 )
				{
					buffer[i] = thousandSeparator;
				}else

				{
					buffer[i] = '0' + (number%10);
					number /= 10;
				}
			}else
			{
				firstDigitOff = i + 1;
				break;
			}
		}
	}else
	{
		sprintf( &buffer[firstDigitOff], "%d\0", number );
	}

	irr::core::stringw result( &buffer[firstDigitOff] );
	return result;
}

stringw FormatString (const wchar_t* format, ...)
{
	va_list args;
	va_start(args, format);
	const int maxCharacters = 1024;
	wchar_t result[maxCharacters];
	int index = 0;
	while (format[0] != L'\0')
	{
		if (format[0] == L'%')
		{
			if (format[1] == L'%')
			{
				result[index++] = L'%';
				++format;
			}
			else if (format[1] == L's' || format[1] == L'S')
			{
				const wchar_t* string = va_arg(args, const wchar_t*);
				for (int j = 0; string[j]; ++j)
					result[index++] = string[j];
				++format;
			}
			else if (format[1] == L'd' || format[1] == L'D')
			{
				stringw number = stringw(va_arg(args, int));
				for (int j = 0; j < number.size(); ++j)
					result[index++] = number[j];
				++format;
			}
			else
			{
				result[index++] = L'%';
			}
		}
		else
		{
			result[index++] = *format;
		}
		++format;
	}
	result[index++] = '\0';
	va_end(args);
	return stringw(result);
}
