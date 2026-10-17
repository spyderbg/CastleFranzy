from BaseConverter import BaseConverter
import os.path
from defs import *
import xml.dom.minidom

class WiiConverter ( BaseConverter ) :

    def __init__( self ,
                  options ) :
        BaseConverter.__init__( self , options )
        self.__cvt_base_cmd = [ os.path.join( bin_path , "nw4r_texcvtr.exe" ) ]
        self.__bcvt_base_cmd = [ os.path.join( bin_path , "rtex2btex.exe" ) ]
        if options.verbose :
            self.__cvt_base_cmd.append( "-i" )

    def get_format( self ,
                    format ) :
        if "_format_map" not in WiiConverter.__dict__ :
            # values taken from "revolution/gx/GXEnum.h" in RVL_SDK
            WiiConverter._format_map = { "i4" : 0x0 ,
                                         "i8" : 0x1 ,
                                         "ia4" : 0x2 ,
                                         "ia8" : 0x3 ,
                                         "rgb565" : 0x4 ,
                                         "rgb5a3" : 0x5 ,
                                         "rgba8" : 0x6 ,
                                         "cmpr" : 0xE ,
                                         "c4" : 0x8 ,
                                         "c8" : 0x9 ,
                                         "c14" : 0xa }
        return WiiConverter._format_map[ format ]

    def get_unit_spec( self ,
                       unit_size ) :
        if "_unit_code_map" not in WiiConverter.__dict__ :
            WiiConverter._unit_code_map = { "u8" : ( 1 , "B" ) ,
                                            "u16" : ( 2 , "H" ) ,
                                            "u32" : ( 4 , "I" ) }
        return WiiConverter._unit_code_map[ unit_size ]

    def get_palette_format( self ,
                            format ) :
        if "_palette_format_map" not in WiiConverter.__dict__ :
            # values taken from "revolution/gx/GXEnum.h" in RVL_SDK
            WiiConverter._palette_format_map = { "ia8" : 0x0 ,
                                                 "rgb565" : 0x1 ,
                                                 "rgb5a3" : 0x2 }
        return WiiConverter._palette_format_map[ format ]

    def load_palette( self ,
                      input_file ,
                      name ) :
        rplt_doc = xml.dom.minidom.parse(
            os.path.join( os.path.dirname( input_file ) , name ) + ".rplt"
            )
        palette = rplt_doc.getElementsByTagName( "palette" )[ 0 ]
        palette_size = int( palette.getAttribute( "color_size" ) )
        palette_format \
            = self.get_palette_format( palette.getAttribute( "format" ) )

        units = get_child_text( palette ).split()
        if len( units ) != palette_size :
            raise ValueError , "wrong palette size"
        palette_data = []
        for u in units :
            palette_data.append( eval( "0x" + u ) )

        return palette_size , palette_format , palette_data

    def process( self ,
                 input_file ,
                 output_file ,
                 image_info ) :
        tmp_rtex = None
        if os.path.splitext( input_file )[ 1 ] != ".rtex" :
            cmd = self.__cvt_base_cmd + self.extra_options
            tmp_rtex = self.gen_tempname( ".rtex" , input_file , output_file )
            self.add_temporary( os.path.splitext( tmp_rtex )[ 0 ] + ".rplt" )
            cmd.append( "-o" )
            cmd.append( tmp_rtex )
            cmd.append( input_file )
            self.run_command( cmd )
        else :
            tmp_rtex = input_file

        if not output_file :
            output_file = change_extension( input_file , ".wii.btex" )

        if self.verbose :
            print_flush( "converting rtex" )

        if self.use_py_impl :
            if self.dry_run :
                return

            rtex_doc = xml.dom.minidom.parse( tmp_rtex )
            image = rtex_doc.getElementsByTagName( "image" )[ 0 ]
            format = self.get_format( image.getAttribute( "format" ) )

            width = int( image.getAttribute( "width" ) )
            height = int( image.getAttribute( "height" ) )

            mipmaps = image.getAttribute( "mipmap_level" )
            if mipmaps == "" :
                mipmaps = 0.0
            else :
                mipmaps = float( mipmaps ) - 1.0

            data_unit , unit_code \
                = self.get_unit_spec( image.getAttribute( "data_unit" ) )
            data_size = int( image.getAttribute( "data_size" ) )

            # check for palette
            palette_size = palette_format = 0
            palette_data = None
            palette_name = image.getAttribute( "palette_name" )
            if palette_name != "" :
                palette_size , palette_format , palette_data \
                    = self.load_palette( tmp_rtex , palette_name )

            ofile = open( output_file , "wb" )
            import struct

            # write header, which consists of:
            # magic number (BTEX)
            # platform (wii)
            # pad byte to align on 4 bytes the rest
            # (above comes from BaseConverter.write_base_header)
            # width and height (unsigned short)
            # format (unsigned int)
            # min and max lod (float)
            # size of image data (unsigned int)
            # pad bytes to align texture data on 32B
            # size of palette (if color index texture), 0 if none
            # palette format (irrelevant if no palette)
            write_base_header( ofile , "wii" )
            data = struct.pack( ">HHIffIHH" ,
                                width ,
                                height ,
                                format ,
                                0.0 ,
                                mipmaps ,
                                data_size ,
                                palette_size ,
                                palette_format )
            ofile.write( data )

            # write texture data (processing could be optimized by batching)

            units = get_child_text( image ).split()
            if data_size / data_unit != len( units ) :
                raise ValueError , "wrong image size"

            unit_code = ">" + unit_code
            for u in units :
                data = struct.pack( unit_code , eval( "0x" + u ) )
                ofile.write( data )

            if palette_data :
                for u in palette_data :
                    data = struct.pack( ">H" , u )
                    ofile.write( data )

            ofile.close()

        else : # use external program
            cmd = self.__bcvt_base_cmd + [ tmp_rtex , output_file ]
            self.run_command( cmd )
