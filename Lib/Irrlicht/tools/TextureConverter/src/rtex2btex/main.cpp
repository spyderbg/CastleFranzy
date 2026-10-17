#include "os.h"
#include "IReadFile.h"
#include "CXMLReader.h"
#include "revolution/gx/GXEnum.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <string>
#include <algorithm>

void* IrrAlloc(size_t size, irr::MemHint hint);
void IrrFree(void* p);

void* operator new(size_t size, irr::MemHint hint) {
	return IrrAlloc(size, hint);
}
void* operator new[](size_t size, irr::MemHint hint) {
	return IrrAlloc(size, hint);
}
void operator delete(void* p, irr::MemHint hint) {
	IrrFree(p);
}
void operator delete[](void* p, irr::MemHint hint) {
	IrrFree(p);
}

void* IrrAlloc(size_t size, irr::MemHint hint, int block, const char* filename, int line);

void* operator new(size_t size, irr::MemHint hint, int block, const char* filename, int line) {
	return IrrAlloc(size, hint, block, filename, line);
}
void* operator new[](size_t size, irr::MemHint hint, int block, const char* filename, int line) {
	return IrrAlloc(size, hint, block, filename, line);
}
void operator delete(void* p, irr::MemHint hint, int block, const char* filename, int line) {
	IrrFree(p);
}
void operator delete[](void* p, irr::MemHint hint, int block, const char* filename, int line) {
	IrrFree(p);
}

void* IrrAlloc(size_t size, irr::MemHint hint, int block, const char* filename, int line)
{
	return IrrAlloc(size, hint);
}

void* IrrAlloc(size_t size, irr::MemHint hint) 
{
	return malloc(size);
}

void IrrFree(void* p)
{
	free(p);
}

namespace texcvt {

class read_file : public ::irr::io::IReadFile
{
public :

	read_file( const char* filename )
		: m_in( filename ) ,
		  m_size( 0 ) ,
		  m_filename( filename )
	{
		if ( m_in.good() )
		{
			m_size = m_in.seekg( 0 , ::std::ios_base::end ).tellg() ;
			m_in.seekg( 0 , ::std::ios_base::beg ) ;
		}
	}

	~read_file()
	{
		m_in.close() ;
	}

	virtual s32 read( void* buffer , u32 size )
	{
		long pos = m_in.tellg() ;
		m_in.read( reinterpret_cast< char* >( buffer ) , size ) ;
		return static_cast< s32 >( m_in.tellg() - pos ) ;
	}

	virtual bool seek( long finalpos , bool relative = false )
	{
		if ( relative )
		{
			return m_in.seekg( finalpos , ::std::ios_base::cur ).good() ;
		}
		return m_in.seekg( finalpos ).good() ;
	}

	virtual long getSize() const
	{
		return m_size ;
	}

	virtual long getPos() const
	{
		return const_cast< ::std::ifstream& >( m_in ).tellg() ;
	}

	virtual const char* getFileName() const
	{
		return m_filename.c_str() ;
	}

private :

	::std::ifstream m_in ;
	long m_size ;
	::std::string m_filename ;

} ; // end class read_file

namespace tag {

char const* const image = "image" ;
char const* const palette = "palette" ;

}

namespace attr {

char const* const format = "format" ;
char const* const width = "width" ;
char const* const height = "height" ;
char const* const mipmap_level = "mipmap_level" ;
char const* const data_unit = "data_unit" ;
char const* const data_size = "data_size" ;
char const* const palette_name = "palette_name" ;
char const* const color_size = "color_size" ;

}

bool is_big_endian()
{
	union { char bytes[ 2 ] ; u16 val ; } endian_test ;
	endian_test.val = 0x0102 ;
	return endian_test.bytes[ 0 ] == 0x01 ;
}

struct btex_header
{
	char magick[ 8 ] ;
	u16 width ;
	u16 height ;
	int format ; // u32 in truth
	f32 min_lod ;
	f32 max_lod ;
	u32 size ;
	u16 palette_size ;
	s16 palette_format ; // u16 in truth

	btex_header()
		: width( 0 ) ,
		  height( 0 ) ,
		  format( 0 ) ,
		  min_lod( 0.0f ) ,
		  max_lod( 0.0f ) ,
		  size( 0 ) ,
		  palette_size( 0 ) ,
		  palette_format( 0 )
	{
		::std::strcpy( magick , "BTEXwii" ) ;
	}

	void ensure_big_endian()
	{
		if ( ! is_big_endian() )
		{
			width = irr::os::Byteswap::byteswap( width ) ;
			height = irr::os::Byteswap::byteswap( height ) ;
			format = irr::os::Byteswap::byteswap( format ) ;
			min_lod = irr::os::Byteswap::byteswap( min_lod ) ;
			max_lod = irr::os::Byteswap::byteswap( max_lod ) ;
			size = irr::os::Byteswap::byteswap( size ) ;
			palette_size = irr::os::Byteswap::byteswap( palette_size ) ;
			palette_format = irr::os::Byteswap::byteswap( palette_format ) ;
		}
	}

} ; // end struct btex_header

int get_format( const char* format_name )
{
	if ( ::std::strcmp( format_name , "i4" ) == 0 )
	{
		return GX_TF_I4 ;
	}
	if ( ::std::strcmp( format_name , "i8" ) == 0 )
	{
		return GX_TF_I8 ;
	}
	if ( ::std::strcmp( format_name , "ia4" ) == 0 )
	{
		return GX_TF_IA4 ;
	}
	if ( ::std::strcmp( format_name , "ia8" ) == 0 )
	{
		return GX_TF_IA8 ;
	}
	if ( ::std::strcmp( format_name , "rgb565" ) == 0 )
	{
		return GX_TF_RGB565 ;
	}
	if ( ::std::strcmp( format_name , "rgb5a3" ) == 0 )
	{
		return GX_TF_RGB5A3 ;
	}
	if ( ::std::strcmp( format_name , "rgba8" ) == 0 )
	{
		return GX_TF_RGBA8 ;
	}
	if ( ::std::strcmp( format_name , "cmpr" ) == 0 )
	{
		return GX_TF_CMPR ;
	}
	if ( ::std::strcmp( format_name , "c4" ) == 0 )
	{
		return GX_TF_C4 ;
	}
	if ( ::std::strcmp( format_name , "c8" ) == 0 )
	{
		return GX_TF_C8 ;
	}
	if ( ::std::strcmp( format_name , "c14" ) == 0 )
	{
		return GX_TF_C14X2 ;
	}
	return -1 ;
}

int get_data_unit( const char* data_unit_name )
{
	if ( ::std::strcmp( data_unit_name , "u8" ) == 0 )
	{
		return 1 ;
	}
	if ( ::std::strcmp( data_unit_name , "u16" ) == 0 )
	{
		return 2 ;
	}
	if ( ::std::strcmp( data_unit_name , "u32" ) == 0 )
	{
		return 4 ;
	}
	return -1 ;
}

typedef ::std::vector< u8 > buffer_type ;

u32 strtou32x(char const* str, char const*& end)
{
	u32 val = 0 ;
	const char* str0 = str ;
	for ( char c = * str ; c && ::std::isspace( c ) ; c = * str )
	{
		++ str ;
	}
	const char* s = str ;
	for ( char c = * s ; c && s - str < sizeof( u32 ) * 2 ; c = * s )
	{
		if ( ::std::isdigit( c ) )
		{
			c -= '0' ;
		}
		else
		{
			c = ::std::tolower( c ) ;
			if ( c >= 'a' && c <= 'f' )
			{
				c += 10 - 'a' ;
			}
			else
			{
				break ;
			}
		}
		val = val << 4 | c ;
		++ s ;
	}
	end = s == str ? str0 : s ;
	return val ;
}

template < typename Data >
bool read_data( const char* text_data ,
				buffer_type::iterator out ,
				buffer_type::iterator out_end )
{
	if ( text_data == 0 )
	{
		return true ; // or false? ...
	}

 	const bool needs_swap = ! is_big_endian() ;
	const char* end ;
	u32 val = strtou32x( text_data , end ) ;
	while ( end != text_data )
	{
		// validation
		buffer_type::iterator next = out + sizeof( Data ) ;
		if ( out_end - next < 0 )
		{
			return false ;
		}

		// copy
		* reinterpret_cast< Data* >( & (* out) ) = static_cast< Data >( val ) ;
		if ( sizeof( Data ) > 1 && needs_swap )
		{
			* reinterpret_cast< Data* >( & (* out) ) = 
				irr::os::Byteswap::byteswap(
					* reinterpret_cast< Data* >( & (* out) )
				) ;
		}
		out = next ;

		text_data = end ;
		val = strtou32x( text_data , end ) ;
	}
	// ensure only trailing spaces are at the end
	while ( * end && ::std::isspace( * end ) )
	{
		++ end ;
	}
	return * end == 0 ;
}

enum read_state
{
	seeking ,
	processing ,
	finished ,
	error
} ;

::std::string progname ;

#define set_error( msg )											\
	state = texcvt::error ;											\
	::std::cerr << ::texcvt::progname << ": " << msg << ::std::endl	\

#define set_brk_error_if( cond , msg )									\
	if ( cond )															\
	{																	\
		set_error( msg ) ;												\
		break ;															\
	}

s16 get_palette_format( const char* format )
{
	if ( ::std::strcmp( format , "ia8" ) == 0 )
	{
		return GX_TL_IA8 ;
	}
	if ( ::std::strcmp( format , "rgb565" ) == 0 )
	{
		return GX_TL_RGB565 ;
	}
	if ( ::std::strcmp( format , "rgb5a3" ) == 0 )
	{
		return GX_TL_RGB5A3 ;
	}
	return -1 ;
}

bool get_palette_data( const char* filename ,
					   const char* palette_name ,
					   u16& palette_size ,
					   s16& palette_format ,
					   buffer_type& palette_data )
{
	::std::string palette_filename = filename ;
	::std::size_t pos = palette_filename.find_last_of( "\\/" ) ;
	if ( pos == ::std::string::npos )
	{
		pos = 0 ;
	}
	palette_filename.replace( pos == 0 ? 0 : pos + 1 ,
							  palette_filename.length() - pos ,
							  palette_name ) ;
	palette_filename += ".rplt" ;

	read_file* input_file = new read_file( palette_filename.c_str() ) ;
	irr::io::IXMLReaderUTF8* reader = irr::io::createIXMLReaderUTF8( input_file ) ;

	read_state state = seeking ;
	buffer_type::iterator palette_data_out ;

	while ( reader->read() )
	{
		switch ( reader->getNodeType() )
		{
			case irr::io::EXN_ELEMENT :
			{
				set_brk_error_if(
					state != seeking ,
					"unexpected palette element: " << reader->getNodeName()
				) ;
				if ( ::std::strcmp( reader->getNodeName() ,
									tag::palette ) == 0 )
				{
					palette_size = static_cast< u16 >(
						reader->getAttributeValueAsInt( attr::color_size )
					) ;
					set_brk_error_if( palette_size == 0 ,
									  "missing color_size" ) ;

					palette_format = get_palette_format(
						reader->getAttributeValue( attr::format )
					) ;
					set_brk_error_if( palette_format < 0 ,
									  "invalid palette format" ) ;

					// all paletted formats are u16
					palette_data.resize( palette_size * sizeof( u16 ) ) ;
					palette_data_out = palette_data.begin() ;
					
					state = processing ;
				}
			}
			break ;

			case irr::io::EXN_ELEMENT_END :
			{
				if ( state == processing )
				{
					state = finished ;
				}
			}
			break ;

			case irr::io::EXN_TEXT :
			{
				if ( state == processing )
				{
					bool ok = read_data< u16 >( reader->getNodeData() ,
												palette_data_out ,
												palette_data.end() ) ;
					set_brk_error_if( ! ok , "corrupt palette data" ) ;
				}
			}
			break ;

			// default: just skip
		}

		if ( state == finished || state == error )
		{
			break ;
		}
	}

	reader->drop() ;
	input_file->drop() ;

	if ( state != finished )
	{
		palette_data.clear() ;
		palette_size = 0 ;
		if ( state == error )
		{
			return false ;
		}
		else
		{
			set_error( "error processing palette xml data" ) ;
		}
	}
	else if ( palette_data.size() != palette_size * 2 )
	{
		palette_data.clear() ;
		palette_size = 0 ;
		set_error( "palette size error" ) ;
	}

	return state == finished ;
}

} // end namespace texcvt

int
main( int argc ,
	  char** argv )
{
	// Process command line arguments ------------------------------------------
	{
		texcvt::progname = argv[ 0 ] ;
		::std::size_t pos = texcvt::progname.find_last_of( "\\/" ) ;
		if ( pos != ::std::string::npos )
		{
			texcvt::progname = texcvt::progname.substr( pos + 1 ) ;
		}
	}
	for ( int i = 1 ; i < argc ; ++ i )
	{
		if ( ::std::strcmp( "-h" , argv[ i ] ) == 0 )
		{
			::std::cerr << "Usage: "
						<< texcvt::progname
						<< " input-file output-file"
						<< ::std::endl ;
			return 0 ;
		}
	}
	// if no help, check validity
	if ( argc != 3 )
	{
		::std::cerr << texcvt::progname
					<< ": invalid command line arguments; expected two."
					<< ::std::endl ;
		return 3 ;
	}

	const char* filename = argv[ 1 ] ;
	const char* output_filename = argv[ 2 ] ;

	texcvt::read_file* input_file = new texcvt::read_file( filename ) ;
	irr::io::IXMLReaderUTF8* reader = irr::io::createIXMLReaderUTF8( input_file ) ;

	texcvt::read_state state = texcvt::seeking ;
	texcvt::btex_header hdr ;
	int data_unit ;
	texcvt::buffer_type image_data ;
	texcvt::buffer_type::iterator image_data_out ;
	texcvt::buffer_type palette_data ;

	// Process XML file --------------------------------------------------------
	while ( reader->read() )
	{
		switch ( reader->getNodeType() )
		{
			case irr::io::EXN_ELEMENT :
			{
				set_brk_error_if(
					state != texcvt::seeking ,
					"unexpected element: " << reader->getNodeName()
				) ;
				if ( ::std::strcmp( texcvt::tag::image ,
									reader->getNodeName() ) == 0 )
				{
					const char* format
						= reader->getAttributeValue( texcvt::attr::format ) ;
					hdr.format = texcvt::get_format( format ) ;
					set_brk_error_if( hdr.format < 0 ,
									  "invalid format: " << format ) ;

					hdr.width = static_cast< u16 >(
						reader->getAttributeValueAsInt( texcvt::attr::width )
					) ;
					set_brk_error_if( hdr.width == 0 , "missing width" ) ;

					hdr.height = static_cast< u16 >(
						reader->getAttributeValueAsInt( texcvt::attr::height )
					) ;
					set_brk_error_if( hdr.height == 0 , "missing width" ) ;

					hdr.max_lod = ::std::max(
						0.0f ,
						static_cast< f32 >(
							reader->getAttributeValueAsInt( texcvt::attr::mipmap_level )
						) - 1.0f
					) ;

					hdr.size = static_cast< u32 >(
						reader->getAttributeValueAsInt( texcvt::attr::data_size )
					) ;
					set_brk_error_if( hdr.size == 0 , "missing data size" ) ;

					image_data.resize( hdr.size ) ;
					image_data_out = image_data.begin() ;

					const char* data_unit_name
						= reader->getAttributeValue( texcvt::attr::data_unit ) ;
					data_unit = texcvt::get_data_unit( data_unit_name ) ;
					set_brk_error_if( data_unit < 0 ,
									  "invalid data unit: " << data_unit_name ) ;

					const char* palette_name
						= reader->getAttributeValue( texcvt::attr::palette_name ) ;
					if ( palette_name && palette_name[ 0 ] )
					{
						bool ok = texcvt::get_palette_data(
							filename ,
							palette_name ,
							hdr.palette_size ,
							hdr.palette_format ,
							palette_data
						) ;
						set_brk_error_if( ! ok ,
										  "invalid palette: " << palette_name ) ;
					}

					state = texcvt::processing ;
				}
			}
			break ;

			case irr::io::EXN_ELEMENT_END :
			{
				if ( state == texcvt::processing )
				{
					state = texcvt::finished ;
				}
			}
			break ;

			case irr::io::EXN_TEXT :
			{
				if ( state == texcvt::processing )
				{
					bool ok = true ;
					switch ( data_unit )
					{
						case 1 :
						{
							ok = texcvt::read_data< u8 >(
								reader->getNodeData() ,
								image_data_out ,
								image_data.end()
							) ;
						}
						break ;

						case 2 :
						{
							ok = texcvt::read_data< u16 >(
								reader->getNodeData() ,
								image_data_out ,
								image_data.end()
							) ;
						}
						break ;

						case 4 :
						{
							ok = texcvt::read_data< u32 >(
								reader->getNodeData() ,
								image_data_out ,
								image_data.end()
							) ;
						}
						break ;
					}
					set_brk_error_if( ! ok , "corrupt data" ) ;
				}
			}
			break ;

			// default: just skip
		}

		if ( state == texcvt::finished || state == texcvt::error )
		{
			break ;
		}
	}

	reader->drop() ;
	input_file->drop() ;

	if ( state == texcvt::error )
	{
		return 1 ;
	}
	if ( state != texcvt::finished )
	{
		set_error( "error processing xml data" ) ;
		return 2 ;
	}
	else if ( hdr.size != image_data.size() )
	{
		set_error( "invalid data size" ) ;
		return 1 ;
	}
	else if ( hdr.palette_size != palette_data.size() / 2 )
	{
		set_error( "invalid palette size" ) ;
		return 1 ;
	}

	// Write binary file -------------------------------------------------------
	::std::ofstream out( output_filename ,
						 ::std::ios_base::out | ::std::ios_base::binary ) ;
	if ( ! out.good() )
	{
		set_error( "could not open " << output_filename ) ;
		return 4 ;
	}

	hdr.ensure_big_endian() ;
	out.write( reinterpret_cast< const char* >( & hdr ) ,
			   sizeof( hdr ) ) ;
	// from here on, hdr info is not necessary valid is swapping occured
	out.write( reinterpret_cast< const char* >( & image_data[ 0 ] ) ,
			   image_data.size() ) ;
	if ( ! palette_data.empty() )
	{
		out.write( reinterpret_cast< const char* >( & palette_data[ 0 ] ) ,
				   palette_data.size() ) ;
	}
	out << ::std::flush ;
	out.close() ;
	
	return 0 ;
}
