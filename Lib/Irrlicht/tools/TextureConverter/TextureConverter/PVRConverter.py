from BaseConverter import BaseConverter
import os
import os.path
from defs import *

class PVRConverter ( BaseConverter ) :

    def __init__( self ,
                  options ) :
        BaseConverter.__init__( self , options )
        self.__cvt_base_cmd = [ os.path.join( bin_path , "PVRTexTool.exe" ) , "-p" ]
        if options.verbose :
            # this is a hack to get PVRTexTool wrapper script on Darwin to print
            # the texturetool command line
            os.environ[ "PVRTEXTOOL_VERBOSE" ] = "1"
        if options.generate_mipmaps :
            self.__cvt_base_cmd.append( "-m" )
        self.format = options.format

    def __set_format( self ,
                      value ) :
        if value :
            self.__format = value
        else :
            self.__format = "PVRTC2"

    format = property( lambda s : s.__format ,
                       __set_format )

    def get_failsafe_format( self ,
                             pixel_format ) :
        if "_failsafe_format_map" not in PVRConverter.__dict__ :
            PVRConverter._failsafe_format_map = { "RGB" : "565" ,
                                                  "RGBA" : "4444" ,
                                                  "RGBX" : "565" ,
                                                  "L" : "L8" ,
                                                  "LA" : "L88" ,
                                                  None : "8888" }
        return PVRConverter._failsafe_format_map[ pixel_format ]

    def process( self ,
                 input_file ,
                 output_file ,
                 image_info ) :
        cmd_extra = []
        if not self.vertical_flip :
            cmd_extra.append( "-yflip0" )
        else :
            cmd_extra.append( "-yflip1" ) 
        if image_info :
            if not is_power_of_two( image_info.width ) \
                    or not is_power_of_two( image_info.height ) :
                self.warn( "non power of two dimensions for %s" \
                               % input_file )
                if self.failsafe_on_warnings :
                    print_flush( "scaling up to nearest power of 2!" )
                    if not is_power_of_two( image_info.width ) :
                        image_info.width = next_power_of_two( image_info.width )
                        cmd_extra.append( "-x%d" % image_info.width )
                    if not is_power_of_two( image_info.height ) :
                        image_info.height \
                            = next_power_of_two( image_info.height )
                        cmd_extra.append( "-y%d" % image_info.height )

        if "PVRTC" in self.format :
            has_warning = False
            if not image_info :
                self.warn( "could not verify %s for PVRTC format" % input_file )
                has_warning = True
            elif image_info.width != image_info.height :
                self.warn( ("%s is not square and will not load on iPhone with "
                            + "requested PVRTC format") % input_file )
                has_warning = True
            if has_warning and self.failsafe_on_warnings :
                prefix = ""
                if self.format.startswith( "OGL" ) :
                    prefix = "OGL"
                pixfmt = None
                if image_info :
                    pixfmt = image_info.pixel_format
                self.format = \
                    prefix + self.get_failsafe_format( pixfmt )
                if self.verbose :
                  print_flush( "using %s instead" % self.format )

        tmp_pvr = None

        if os.path.splitext( input_file )[ 1 ] != ".pvr" :
            cmd = (self.__cvt_base_cmd
                   + self.extra_options
                   + [ "-i" + input_file , "-f" + self.format ])
            tmp_pvr = self.gen_tempname( ".pvr" , input_file , output_file )
            if "PVRTC" not in self.format :
                # disable twiddling
                cmd.append( "-nt" )
            cmd.append( "-o" + tmp_pvr )
            self.run_command( cmd + cmd_extra )
            # %!$%!@#$@#$! iPhone texture tool returns 0 even on failure
            if not os.path.exists( tmp_pvr ) :
                raise RuntimeError , \
                    "failed to run command '%s'" % " ".join( cmd )
        else :
            tmp_pvr = input_file

        if self.verbose :
            print_flush( "adding BTEX header" )

        if self.dry_run :
            return

        if not output_file :
            output_file = change_extension( input_file , ".pvr.btex" )
        ofile = open( output_file , "wb" )

        # write base header
        write_base_header( ofile , "pvr" )

        # then append the original pvr file
        cat_file( tmp_pvr , ofile )
        ofile.close()
