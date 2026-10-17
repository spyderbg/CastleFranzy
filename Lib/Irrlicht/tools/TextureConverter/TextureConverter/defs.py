import os.path
import sys
import platform

module_path = os.path.join( os.path.dirname( __file__ ) )
system_name = platform.system().lower()
bin_path = os.path.join( module_path , ".." , "bin" , system_name )

def change_extension( filename ,
                      ext ) :
    return os.path.splitext( filename )[ 0 ] + ext

def remove_files( file_list ) :
    for f in file_list :
        if os.path.exists( f ) :
            os.remove( f )

def print_flush( msg ,
                 file = sys.stderr ) :
    print >> file , msg
    file.flush()

def print_command( cmd ,
                   file = sys.stderr ) :
    print_flush( " ".join( cmd ) , file )

def get_child_text( xml_node ,
                    dom = None ) :
    if dom is None :
        import xml.dom.minidom
        dom = xml.dom.minidom
    value = ""
    for child in xml_node.childNodes :
        if child.nodeType == dom.Node.TEXT_NODE :
            value = value + child.nodeValue
    return value

def write_base_header( ofile ,
                       submagick ) :
    import struct
    ofile.write( struct.pack( ("cccc"
                               + "c" * min( 4 , len( submagick ) )
                               + "x" * max( 0 , 4 - len( submagick ) )) ,
                              *("BTEX" + submagick) ) )

def cat_file( ifile ,
              ofile ) :
    "Copies the content of ifile to ofile. If ifile is a string, the named file is opened, same for ofile; otherwise, file objects will not be closed."
    close_ofile = False
    close_ifile = False
    try :
        if type( ofile ) is str or type( ofile ) is unicode :
            ofile = open( ofile , "wb" )
            close_ofile = True
        if type( ifile ) is str or type( ifile ) is unicode :
            ifile = open( ifile , "rb" )
            close_ifile = True
        buf = ifile.read( 16384 )
        while buf != "" :
            ofile.write( buf )
            buf = ifile.read( 16384 )
        if close_ofile :
            ofile.close()
        if close_ifile :
            ifile.close()
    except :
        if close_ofile :
            ofile.close()
        if close_ifile :
            ifile.close()
        raise

def is_power_of_two( n ) :
    return (n & (n - 1)) == 0

def next_power_of_two( n ) :
    r = 1
    while r < n :
        r = r << 1
    return r;
