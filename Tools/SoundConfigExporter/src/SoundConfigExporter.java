import java.io.*;
import java.util.*;
import jxl.*;

// ------------------------------------------------------------------------------------------------
// parse string XML file
// ------------------------------------------------------------------------------------------------
public class SoundConfigExporter
{
	public final static boolean DBG_OUTPUT	= true;	
	public final static String _version = "0.2";
	
	public final static String STRINGS_CHARSET_OUT 		= "Windows-1252";
	public final static String STRINGS_CHARSET_OUT_CHS 	= "Unicode";	
	
	public final static String CONFIGURATION_PREFIX = "soundsconfig";
	public final static String GLOBAL_CONFIGURATION_PREFIX = "Global_SoundConfig";
	
	public static final String CHS_LANG[] = {"CHT", "CHS"}; 
	
	public static final String SOUND_EXTENSION = ".wav";
	
	// command-line parameters
	public final static int ARGUMENT_XML            = 0;	
	public final static int ARGUMENT_OUT_FILE       = 1;
	public final static int LANGUAGE_NUMBERS        = 2;
	
	/// Special symbol
	public final static byte FAKE_SYMBOL = (byte)0x01;
	public final static byte FAKE_SYMBOL_2 = (byte)0xFF;
	
	// columns in Excel table
	public final static int COL_SOUND_NAME			= 0;
	public final static int COL_SOUND_OBLIGATORY	= 3;
	public final static int COL_SOUND_OVERLAP		= 4;
	public final static int COL_SOUND_PRIORITY		= 5;
	public final static int COL_SOUND_VOLUME		= 6;	
	
	/// FORMAT_xxx are used in <format> parameter of FormatSheetStrings()
	public final static int FORMAT_NO_NULL        = 1 << 0; // null strings are replaced by ""
	public final static int FORMAT_NO_COMMENTS    = 1 << 1; // strings starting with "//" are replaced by ""
	public final static int FORMAT_NO_EXCEL_CHARS = 1 << 2; // strings are corrected with correctExcelString()
	public final static int FORMAT_TRIM           = 1 << 3; // whitespaces removed from strings' beginnings and ends
	public final static int FORMAT_DELETE_QUOTES  = 1 << 4; // some strings was bounded with quotes so we got to remove them
	
	// attribute types
	public final static String ATTRIBUTE_TYPE_STRING = "string";
	public final static String ATTRIBUTE_TYPE_INT = "int";
	public final static String ATTRIBUTE_TYPE_FLOAT = "float";
	public final static String ATTRIBUTE_TYPE_BOOL = "bool";
	
	// attribute names
	public final static String ATTRIBUTE_NAME_OBLIGATORY = "Obligatory";
	public final static String ATTRIBUTE_NAME_OVERLAP = "Overlap";
	public final static String ATTRIBUTE_NAME_PRIORITY = "Priority";
	public final static String ATTRIBUTE_NAME_VOLUME = "Volume";
	
	public static void main(String args[])
	{
	
		if(args.length < 2 )
		{
			System.out.println("Usage: java SoundConfigExporter [XML  file] [output file] ");
			System.exit(0);			
		}
		
		System.out.println();
				
		ParseStrings( args[ARGUMENT_XML], args[ARGUMENT_OUT_FILE] );
		//parseXLSFile( args[ARGUMENT_XML] );
		
		System.out.println();
	}
	
	// ------------------------------------------------------------------------------------------------
	/// @brief Parse 
	// ------------------------------------------------------------------------------------------------	
	public static void ParseStrings( String XLS_filename, String OutputFileName )
	{
		Workbook workbook = null;
	
		try
		{
			System.out.println("Parssing " + XLS_filename + " Output: " + OutputFileName );			
			
			if( OutputFileName == null )
				OutputFileName = "";
			
			/// Get workbook
			Locale.setDefault( Locale.US );
			WorkbookSettings workbookSettings = new WorkbookSettings();
			workbookSettings.setLocale( Locale.US );
			workbookSettings.setExcelRegionalSettings( "US" );
			workbookSettings.setExcelDisplayLanguage( "US" );
			workbook = Workbook.getWorkbook( new File(XLS_filename), workbookSettings );
			
			/// Load all sheets names	from XML file			
			String[] all_packs_names = workbook.getSheetNames();
			
			for(int pack_id = 0; pack_id < all_packs_names.length; pack_id++)
			{					
				if(all_packs_names[pack_id].compareTo("Sound Design Document") != 0)
					continue;
			
				/// Load XML file to strings				
				Sheet sheet = workbook.getSheet( pack_id );
				String xls_strings_table[][] = new String[sheet.getRows()][sheet.getColumns()];
				
				for( int row = 0; row < sheet.getRows(); row++ )
				{
					for( int col = 0; col < sheet.getColumns(); col ++ )
					{					
						xls_strings_table[row][col] = sheet.getCell( col, row ).getContents();
					}
				}
			
				FormatSheetStrings( xls_strings_table, FORMAT_NO_NULL | FORMAT_TRIM | FORMAT_NO_EXCEL_CHARS | FORMAT_DELETE_QUOTES );						
				
				WriteSoundConfigToFile(OutputFileName, xls_strings_table);
			}
		}
		catch(Exception e)
		{ 
			System.out.println("SoundConfigExporter.ParseStrings() error: "+e.toString()); 
			e.printStackTrace(); 
			System.exit(1);
		}		
	}
	
	public static void WriteSoundConfigToFile(String fileName, String strings_table[][])
	{
		try
		{
			FileOutputStream file = new FileOutputStream( fileName, false );
			file.write( "<?xml version=\"1.0\"?>\n\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "<!--This file is autogenerated-->\n\n".getBytes(STRINGS_CHARSET_OUT) );
			
			//write global soundconfig
			file.write( ("<" + CONFIGURATION_PREFIX + " id=\"" + GLOBAL_CONFIGURATION_PREFIX + "\">\n").getBytes(STRINGS_CHARSET_OUT) );
			WriteAttribute(file, ATTRIBUTE_TYPE_INT, ATTRIBUTE_NAME_OBLIGATORY, "0");
			WriteAttribute(file, ATTRIBUTE_TYPE_INT, ATTRIBUTE_NAME_OVERLAP, "2");
			WriteAttribute(file, ATTRIBUTE_TYPE_INT, ATTRIBUTE_NAME_PRIORITY, "3");
			WriteAttribute(file, ATTRIBUTE_TYPE_FLOAT, ATTRIBUTE_NAME_VOLUME, "1");			
			WriteClosingTag(file, CONFIGURATION_PREFIX);
			
			int max_texts_num = strings_table.length;
				
			/// Go through all lines
			for(int i = 0; i < max_texts_num; i++)
			{
				if(strings_table[i][COL_SOUND_NAME].startsWith("sfx_"))
				{
					file.write(("<" + CONFIGURATION_PREFIX + " id=\"" + strings_table[i][COL_SOUND_NAME] + SOUND_EXTENSION + "\" extends=\"" + GLOBAL_CONFIGURATION_PREFIX + "\">\n").getBytes(STRINGS_CHARSET_OUT) );
					String isObligatory = strings_table[i][COL_SOUND_OBLIGATORY].compareTo("Yes") == 0 ? "1" : "0";
					WriteAttribute(file, ATTRIBUTE_TYPE_INT, ATTRIBUTE_NAME_OBLIGATORY, isObligatory);
					WriteAttribute(file, ATTRIBUTE_TYPE_INT, ATTRIBUTE_NAME_OVERLAP, strings_table[i][COL_SOUND_OVERLAP]);
					WriteAttribute(file, ATTRIBUTE_TYPE_INT, ATTRIBUTE_NAME_PRIORITY, strings_table[i][COL_SOUND_PRIORITY]);
					WriteAttribute(file, ATTRIBUTE_TYPE_FLOAT, ATTRIBUTE_NAME_VOLUME, strings_table[i][COL_SOUND_VOLUME]);
					WriteClosingTag(file, CONFIGURATION_PREFIX);
				}
			}
			
			file.close();
		}
		catch(Exception e) 
		{ 
			System.out.println("StringParser.WriteSoundConfigToFile() error: "+e.toString()); 
			e.printStackTrace(); 
			System.exit(1);
		}		
	}
	
	public static void WriteAttribute(FileOutputStream file, String type, String name, String value)
	{
		if(value == "")
			return;
			
		try
		{
			if(file != null)
				file.write(("\t<" + type + " name=\"" + name + "\" value=\"" + value + "\" />\n").getBytes(STRINGS_CHARSET_OUT));
		}
		catch(Exception e) 
		{ 
			System.out.println("StringParser.WriteAttribute() error: "+e.toString()); 
			e.printStackTrace(); 
			System.exit(1);
		}		
	}
	
	public static void WriteClosingTag(FileOutputStream file, String name)
	{
		try
		{
			if(file != null)
				file.write(("</" + name + ">\n\n").getBytes(STRINGS_CHARSET_OUT));
		}
		catch(Exception e) 
		{ 
			System.out.println("StringParser.WriteClosingTag() error: "+e.toString()); 
			e.printStackTrace();
			System.exit(1);			
		}		
	}
	
	public static byte[] GetBytes( String str, String encoding) throws Exception
	{
		byte ret[] = str.getBytes( encoding );

		return ret;
	}
	
	public static void PrintLine( String line )
	{
		if( DBG_OUTPUT )
			System.out.println( line );
	}		
	public static void PrintListStrings( ArrayList list )
	{
		System.out.println();
		Object[] list_obj = list.toArray();
		for( int i=0; i<list_obj.length; i++ )
		{
			System.out.println( "   " + (String)list_obj[i] );
		}
		System.out.println();		
	}

	// ------------------------------------------------------------------------------------------------
	/// @brief Writes to an already open file the int as little-endian
	// ------------------------------------------------------------------------------------------------
	public static void WriteToFile_U32 (FileOutputStream file, int value) throws Exception
	{
		file.write((byte)((value) & 0xFF));
		file.write((byte)((value >> 8) & 0xFF));
		file.write((byte)((value >> 16) & 0xFF));
		file.write((byte)((value >> 24) & 0xFF));
	}

	
	// ------------------------------------------------------------------------------------------------
	/// @brief Writes to an already open file the int as little-endian
	// ------------------------------------------------------------------------------------------------
	public static void WriteToFile_U16 (FileOutputStream file, int value) throws Exception
	{
		file.write((byte)((value) & 0xFF));
		file.write((byte)((value >> 8) & 0xFF));
	}	

	// ------------------------------------------------------------------------------------------------
	/// @brief Parses all strings in <strings> and performs operations given by <format>
	/// @param format can be FORMAT_NO_NULL | FORMAT_NO_COMMENTS | FORMAT_NO_EXCEL_CHARS | FORMAT_TRIM
	// ------------------------------------------------------------------------------------------------
	static void FormatSheetStrings (String[][] strings, int format)
	{
			for(int line = 0; line < strings.length; line++)
				for(int col = 0; col < strings[line].length; col++)
				{
					if ((format & FORMAT_NO_NULL) != 0 && strings[line][col] == null)
						strings[line][col] = "";
					
					if ((format & FORMAT_NO_COMMENTS) != 0)
						strings[line][col] = RemoveComment(strings[line][col]);
					
					if ((format & FORMAT_NO_EXCEL_CHARS) != 0)
						strings[line][col] = correctExcelString(strings[line][col]);
					
					if ((format & FORMAT_TRIM) != 0)
						strings[line][col] = strings[line][col].trim();
						
					if ((format & FORMAT_DELETE_QUOTES) != 0)
						strings[line][col] = DeleteStartEndQuotes( strings[line][col] );
						
				}
	}
	
	// ------------------------------------------------------------------------------------------------
	/// @brief Removes " characters from the start and end of a string but only if there are such characters on both the start and end of a string
	/// @param s is the string to trim the "s from
	// ------------------------------------------------------------------------------------------------
	static String DeleteStartEndQuotes (String s)
	{
		if(s!=null && s.startsWith("\"") && s.endsWith("\"") && s.length()>2)
			s = s.substring(1, s.length() - 1);

		return s;
	}
	
	// ------------------------------------------------------------------------------------------------
	/// @brief Removes a comment starting with "//" and ending at the end of string
	/// @param s is the string to trim the "s from
	// ------------------------------------------------------------------------------------------------
	static String RemoveComment (String s)
	{
		int comment_pos = s.indexOf("//");
		
		if (comment_pos == -1) return s;
		
		return s.substring(0, comment_pos);
	}

	// ------------------------------------------------------------------------------------------------
	/// @brief Replaces some common "Excel" characters like "…" with "normal" characters ("..." in this very case)
	/// @param s is the string to parse
	// ------------------------------------------------------------------------------------------------
	static String correctExcelString (String s)
	{
		if (s == null) return null;

		s = s.replace('“','"');
		s = s.replace('”','"');

		s = s.replace('’','\'');
		s = s.replaceAll("…","...");

		return s;
	}
		
 }
