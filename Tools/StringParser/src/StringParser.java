import java.io.*;
import java.util.*;
import jxl.*;

// ------------------------------------------------------------------------------------------------
// parse string XML file
// ------------------------------------------------------------------------------------------------
public class StringParser
{
	public final static boolean DBG_OUTPUT	= true;	
	public final static String _version = "0.1";
	
	public final static String STRINGS_CHARSET_OUT 		= "Windows-1252";
	public final static String STRINGS_CHARSET_OUT_CHS 	= "Unicode";	
	
	public final static String EXPORT_PREFIX = "FortressDefender_";
	
	public static final String CHS_LANG[] = {"CHT", "CHS", "JP"}; 
	
	// command-line parameters
	public final static int ARGUMENT_XML            = 0;	
	public final static int ARGUMENT_OUT_DIR        = 1;
	public final static int LANGUAGE_NUMBERS        = 2;

	
	// columns in Excel table
	public final static int LANG_LINE_ID    = 0;	
	public final static int COL_STR_ID        = 0;
	
	/// ID string
	public final static String ID_STRING  		= "::ID::";
	
	/// Export file ext.
	public final static String EXPORT_EXT		= ".hpp";
	public final static String TEXT_EXT			= ".txt";
	public final static String TEXT_GEN			= ".gen";
	
	/// Special symbol
	public final static byte FAKE_SYMBOL = (byte)0x01;
	public final static byte FAKE_SYMBOL_2 = (byte)0xFF;
	
	public static int MAX_LANGUAGES_NUMBER = 1;
	
	/// FORMAT_xxx are used in <format> parameter of FormatSheetStrings()
	public final static int FORMAT_NO_NULL        = 1 << 0; // null strings are replaced by ""
	public final static int FORMAT_NO_COMMENTS    = 1 << 1; // strings starting with "//" are replaced by ""
	public final static int FORMAT_NO_EXCEL_CHARS = 1 << 2; // strings are corrected with correctExcelString()
	public final static int FORMAT_TRIM           = 1 << 3; // whitespaces removed from strings' beginnings and ends
	public final static int FORMAT_DELETE_QUOTES  = 1 << 4; // some strings was bounded with quotes so we got to remove them

	
	public static void main(String args[])
	{
	
		if(args.length < 2 )
		{
			System.out.println("Usage: java StringParser [XML  file] [output dir (no final \\)] ");
			System.exit(0);			
		}
		
		System.out.println();
		
		MAX_LANGUAGES_NUMBER = Integer.parseInt( args[LANGUAGE_NUMBERS] );
				
		ParseStrings( args[ARGUMENT_XML], args[ARGUMENT_OUT_DIR], EXPORT_PREFIX );
		//parseXLSFile( args[ARGUMENT_XML] );
		
		System.out.println();
	}

	// ------------------------------------------------------------------------------------------------
	/// @brief Parse XLS file
	// ------------------------------------------------------------------------------------------------
	public static void parseXLSFile( String szInputXLSFile )
	{
		Workbook workbook = null;
		try
		{
			/// Get workbook
			Locale.setDefault(Locale.US);
			WorkbookSettings workbookSettings = new WorkbookSettings();
			workbookSettings.setLocale(Locale.US);
			workbookSettings.setExcelRegionalSettings("US");
			workbookSettings.setExcelDisplayLanguage("US");
			workbook = Workbook.getWorkbook(new File(szInputXLSFile), workbookSettings);


			/// Go trough worksheets
			System.out.println( "Sheets number: " + workbook.getSheets().length );

			Sheet sheet = workbook.getSheet( 0 );
		
			String sheet_str[][] = new String[sheet.getRows()][sheet.getColumns()];
			
			System.out.println( "Rows: " + sheet.getRows() );
			System.out.println( "Columns: " + sheet.getColumns() );
			for (int row = 0; row < sheet.getRows(); row++)
			{
				for( int col = 0; col < sheet.getColumns(); col ++ )
				{					
					sheet_str[row][col] = sheet.getCell( col, row ).getContents();
				}
			}
			
			System.out.println();
			System.out.println( "First column: " );
			for( int col = 0; col < sheet.getColumns(); col ++ )
			{
				System.out.println( sheet_str[0][col] );
			}
		}
		catch(Exception e)
		{
			System.out.println("StringParser.parseXLSFile() error: "+e.toString()); 
			e.printStackTrace(); 
			System.exit(1);
		}		
	}
	
	// ------------------------------------------------------------------------------------------------
	/// @brief Parse 
	// ------------------------------------------------------------------------------------------------	
	public static void ParseStrings( String XLS_filename, String OutputDirName, String ExportFilePrefix )
	{
		Workbook workbook = null;
	
		try
		{
			System.out.println("Parssing " + XLS_filename + " Output: " + OutputDirName );			
			
			if( OutputDirName == null )
				OutputDirName = "";
			if( !OutputDirName.endsWith( "\\" ) );
				OutputDirName = OutputDirName + "\\"; 
			
			int current_lang_num = 0;
			Hashtable language_id_map = new Hashtable();
			ArrayList id_list = new ArrayList();
			
			int prev_added_ids = 0;			
			
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
				String pack_name = all_packs_names[pack_id];
				PrintLine( "Pack name: " + pack_name );
			
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
								
				/// Check for missing ID column
				if( xls_strings_table[LANG_LINE_ID][COL_STR_ID].compareTo( ID_STRING ) != 0 )													
					throw new Exception( "Missing ID column in " + pack_name );							
								
					
				/// Export text, from current sheet, for all languages 
				int first_col = COL_STR_ID+1;
				int max_col_num = xls_strings_table[LANG_LINE_ID].length;				
				int max_texts_num = xls_strings_table.length;															
				int languages_num = 0;			
				for( int i=first_col; i<max_col_num; i++ )
					if( xls_strings_table[LANG_LINE_ID][i].length() > 0 )
						languages_num++;
				
				/// Check for new language in this sheet and add it 
				for( int i=first_col; i<max_col_num; i++ )
					if( xls_strings_table[LANG_LINE_ID][i].length() > 0 )
						if( language_id_map.get( xls_strings_table[LANG_LINE_ID][i] ) == null )
						{
							PrintLine( "  Add language: " + xls_strings_table[LANG_LINE_ID][i] );
							
							ArrayList lang = new ArrayList();
							for( int j=0; j<prev_added_ids; j++ )
								lang.add( "" );
							language_id_map.put( xls_strings_table[LANG_LINE_ID][i], lang );
							current_lang_num++;
						}
					
				/// Check for missing language or some more
				if( MAX_LANGUAGES_NUMBER != languages_num )				
					System.out.println( "Missing language in " + pack_name );
				if( MAX_LANGUAGES_NUMBER < current_lang_num )
					System.out.println( "Too many languages in " + pack_name );				
				
												
				/// Go through all languages
				for( int lang=first_col; lang<languages_num+first_col; lang++ )
				{
					String current_lang = xls_strings_table[LANG_LINE_ID][lang];
					ArrayList strings_list = (ArrayList)language_id_map.get( current_lang );
					if( strings_list == null )
						throw new Exception( "Error parsing. Check language IDs in " + pack_name + " Lang: " + current_lang );
					
					/// Go throgh all IDs
					for( int id=LANG_LINE_ID+1; id<max_texts_num; id++ )
					{
						if( xls_strings_table[id][COL_STR_ID].length() > 0 )
						{
							String text = new String( xls_strings_table[id][lang] );
							text = text.replaceAll("\\\\n", "\n");
							strings_list.add( text );
						}
					}
				}

				/// Go throgh all IDs
				for( int id=LANG_LINE_ID+1; id<max_texts_num; id++ )
					if( xls_strings_table[id][COL_STR_ID].length() > 0 )
						id_list.add( new String( pack_name.toUpperCase() + "_" + xls_strings_table[id][COL_STR_ID].toUpperCase() ) );
				
			}
			
			ExportID( OutputDirName + ExportFilePrefix + "ID", id_list );
			ExportSID( OutputDirName + ExportFilePrefix + "ID", id_list );
			
			for (Enumeration e = language_id_map.keys() ; e.hasMoreElements() ;) 
			{
				String key_str = (String)e.nextElement();
				ArrayList list = (ArrayList)language_id_map.get( key_str );
//				PrintListStrings( list );
				SaveStringsData( OutputDirName + ExportFilePrefix + key_str, list.toArray(), STRINGS_CHARSET_OUT_CHS, OutputDirName + ExportFilePrefix + "All_Strings" );			
				SaveStringMapping(OutputDirName + "FontMapping_" + key_str, list.toArray(), STRINGS_CHARSET_OUT_CHS);
			}
		}
		catch(Exception e)
		{ 
			System.out.println("StringParser.ParseStrings() error: "+e.toString()); 
			e.printStackTrace(); 
			System.exit(1);
		}		
	}
	
	// ------------------------------------------------------------------------------------------------
	/// @brief Saves an array of strings into a text file (each string will be on a new line)
	/// @note For convenience the <lines> parameter is declared as Object[] (not String[]); each line is cast to (String)
	// ------------------------------------------------------------------------------------------------
	public static void SaveStringsData( String filename, Object[] lines, String encoding, String all_file )
	{	
		int size = lines.length;
		if( size < 1 ) return;

		try
		{
			FileOutputStream file = new FileOutputStream(filename + TEXT_EXT);
			FileOutputStream all_str = new FileOutputStream(all_file + TEXT_GEN, true);
			all_str.write( 0xFF );
			all_str.write( 0xFE );
			
			int offset = 0;
			if( encoding.compareTo( STRINGS_CHARSET_OUT_CHS ) == 0 )
				offset = 2;
			
			/// Write stirngs size
			WriteToFile_U32( file, size );
						
			/// Write strings offsets
			int cur_offset = 4 + size * 4; /// 4 byte for C pointer
			for(int i = 0; i < size; i++)			
			{
				WriteToFile_U32( file, cur_offset );			
				cur_offset += GetBytes( (String)lines[i] + "\0", encoding).length - offset;
			}						
			
			/// Write strings			
			for(int i = 0; i < size; i++)
			{										
				byte tmp[] = GetBytes((String)lines[i] + "\0", encoding);
				file.write( tmp, offset, tmp.length-offset );
				all_str.write( tmp, offset, tmp.length-offset );
			}
		
			all_str.close();
			file.close();
		}
		catch(Exception e) 
		{ 
			System.out.println("StringParser.SaveStringsData() error: "+e.toString()); 
			e.printStackTrace(); 
			System.exit(1);
		}
	}
	
	public static void SaveStringMapping( String filename, Object[] lines, String encoding)
	{	
		int size = lines.length;
		if( size < 1 ) return;

		try
		{
			FileOutputStream file = new FileOutputStream(filename + TEXT_EXT);
			file.write( 0xFF );
			file.write( 0xFE );
			
			int offset = 0;
			if( encoding.compareTo( STRINGS_CHARSET_OUT_CHS ) == 0 )
				offset = 2;
			
			/// find all characters
			boolean[] characters = new boolean[2*Short.MAX_VALUE];
			String str;
			for(int i = 0; i < size; i++)
			{										
				str = (String)lines[i];
				for(int j = 0; j < str.length(); j++)
				{
					characters[str.charAt(j)] = true;
				}
			}
			
			//write characters
			String strMap = "";
			for(int i = 32; i < 2*Short.MAX_VALUE; i++)
			{
				if(characters[i])
				{
					strMap += (char)i;
				}
			}
			
			byte tmp[] = GetBytes(strMap, encoding);
			file.write( tmp, offset, tmp.length-offset );
		
			file.close();
		}
		catch(Exception e) 
		{ 
			System.out.println("StringParser.SaveStringsData() error: "+e.toString()); 
			e.printStackTrace(); 
			System.exit(1);
		}
	}

	public static void ExportID( String filename, ArrayList id_list )
	{
		if( id_list.isEmpty() ) return;
		
		PrintLine( "ExportID filename: " + filename );	
		try
		{
			FileOutputStream file 		= new FileOutputStream( filename+EXPORT_EXT, false );
			
			/// Write all lines of exported file		
			file.write( "///////////////////////////////////////////////////////////\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "// WARNING: This file was generated by 'StringParser' tool!\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "///////////////////////////////////////////////////////////\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "BEGIN_STRINGS\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );

			String str = "";
			Object[] lines = id_list.toArray();
			for(int i = 0; i < lines.length; i++)
			{
				str = "STRING_( " + i + ",\t" + (String)lines[i] + " );\n";					
					
				file.write( str.getBytes(STRINGS_CHARSET_OUT) );
			}

			
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "END_STRINGS".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );			
			file.write( "///////////////////////////////////////////////////////////\n".getBytes(STRINGS_CHARSET_OUT) );			
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );			
			file.close();
		}
		catch(Exception e) 
		{ 
			System.out.println("StringParser.ExportID() error: "+e.toString()); 
			e.printStackTrace(); 
			System.exit(1);
		}		
	}
	
	public static void ExportSID( String filename, ArrayList id_list )
	{
		if( id_list.isEmpty() ) return;
		
		PrintLine( "ExportID filename: " + filename );	
		try
		{
			FileOutputStream file 		= new FileOutputStream( filename + "_string" + EXPORT_EXT, false );
			
			/// Write all lines of exported file		
			file.write( "///////////////////////////////////////////////////////////\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "// WARNING: This file was generated by 'StringParser' tool!\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "///////////////////////////////////////////////////////////\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "BEGIN_STRINGS\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );

			String str = "";
			Object[] lines = id_list.toArray();
			for(int i = 0; i < lines.length; i++)
			{
				str = "S_STRING_( " + i + ",\t" + "\"" + (String)lines[i] +"\"" + " );\n";					
					
				file.write( str.getBytes(STRINGS_CHARSET_OUT) );
			}

			
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "END_STRINGS".getBytes(STRINGS_CHARSET_OUT) );
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );			
			file.write( "///////////////////////////////////////////////////////////\n".getBytes(STRINGS_CHARSET_OUT) );			
			file.write( "\n".getBytes(STRINGS_CHARSET_OUT) );			
			file.close();
		}
		catch(Exception e) 
		{ 
			System.out.println("StringParser.ExportSID() error: "+e.toString()); 
			e.printStackTrace(); 
			System.exit(1);
		}		
	}

	
	public static boolean IsChsLang( String lang )
	{
		for( int i=0; i<CHS_LANG.length; i++ )
		{
			if( lang.compareTo( CHS_LANG[i] ) == 0 )
				return true;
		}
		
		return false;
	}
	
	public static byte[] GetBytes( String str, String encoding) throws Exception
	{
		byte ret[] = str.getBytes( encoding );
		
/*		
		if( encoding.compareTo( STRINGS_CHARSET_OUT_CHS ) == 0 )
		{
			for( int i=1; i < ret.length-2; i += 2 )
				if( ret[i] == 0)
					ret[i] = FAKE_SYMBOL;

			for( int i=0; i < ret.length-2; i += 2 )
				if( ret[i] == 0)
					ret[i] = FAKE_SYMBOL_2;	
		}
*/				
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
