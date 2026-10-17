import os
import os.path
from defs import *
import subprocess

class BaseConverter ( object ) :

    def __init__( self ,
                  options ) :
        self.warning_behavior = options.warning_behavior
        self.verbose = options.verbose
        self.cleanup = options.cleanup
        self.__temporaries = set( [] )
        self.extra_options = options.extra_options
        self.generate_mipmaps = options.generate_mipmaps
        self.dry_run = options.dry_run
        self.use_py_impl = options.py_impl
        self.raw_format = options.raw_format
        self.vertical_flip = options.vertical_flip

    def _quiet( self , msg ) :
        pass

    def _warn( self , msg ) :
        print_flush( "warning: " + msg )

    def _error( self , msg ) :
        raise RuntimeError , msg

    warning_behaviors = { "quiet" : _quiet ,
                          "warn" : _warn ,
                          "error" : _error ,
                          "failsafe" : _warn }

    warn = _warn

    def __set_warning_behavior( self ,
                                value ) :
        BaseConverter.warn = BaseConverter.warning_behaviors[ value ]
        self.__warning_behavior = value

    warning_behavior = property( lambda s : s.__warning_behavior ,
                                 __set_warning_behavior )

    failsafe_on_warnings = property( lambda s : \
                                         s.__warning_behavior == "failsafe" )

    def __set_verbose( self ,
                       value ) :
        self.__verbose = bool( value )

    verbose = property( lambda s : s.__verbose ,
                        __set_verbose )

    def __set_cleanup( self ,
                       value ) :
        self.__cleanup = bool( value )

    cleanup = property( lambda s : s.__cleanup ,
                        __set_cleanup )

    def __set_extra_options( self ,
                             value ) :
        self.__extra_options = value.split()

    extra_options = property( lambda s : s.__extra_options ,
                              __set_extra_options )

    def __set_generate_mipmaps( self ,
                                value ) :
        self.__generate_mipmaps = bool( value )

    generate_mipmaps = property( lambda s : s.__generate_mipmaps ,
                                 __set_generate_mipmaps )

    def __set_dry_run( self ,
                       value ) :
        self.__dry_run = bool( value )

    dry_run = property( lambda s : s.__dry_run ,
                        __set_dry_run )

    def __set_use_py_impl( self ,
                           value ) :
        self.__use_py_impl = bool( value )

    use_py_impl = property( lambda s : s.__use_py_impl ,
                            __set_use_py_impl )

    def __set_raw_format( self ,
                          value ) :
        self.__raw_format = bool( value )

    raw_format = property( lambda s : s.__raw_format ,
                           __set_raw_format )

    def __set_vertical_flip( self ,
                             value ) :
        self.__vertical_flip = bool( value )

    vertical_flip = property( lambda s : s.__vertical_flip ,
                              __set_vertical_flip )

    def gen_tempname( self ,
                      ext ,
                      input_file = None ,
                      output_file = None) :
        base = None
        if not self.cleanup :
            if input_file :
                base = os.path.splitext( input_file )[ 0 ]
            elif output_file :
                base = os.path.splitext( output_file )[ 0 ]
            if base :
                tmp_file = base + ext
                self.__temporaries.add( ext )
                return tmp_file
        for i in xrange( 0 , 10000 ) :
            tmp_file = "texcvt_tmp_%d_%04d%s" % ( os.getpid() , i , ext )
            try :
                # create the actual file to ensure we get it
                fd = os.open( tmp_file , os.O_CREAT | os.O_EXCL )
                os.close( fd )
                self.__temporaries.add( tmp_file )
                return tmp_file
            except :
                pass
        raise RuntimeException , "failed to create a temporary file"

    def add_temporary( self ,
                       name ) :
        self.__temporaries.add( name )

    def run_command( self ,
                     cmd ) :
        if self.verbose or self.dry_run :
            print_command( cmd )
            if self.dry_run :
                return
        ret = subprocess.call( cmd )
        if ret != 0 :
            raise RuntimeError , \
                "failed to run command '%s'" % " ".join( cmd )

    class ImageInfo :
        def __init__( self ,
                      pil_image ) :
            self.width , self.height = pil_image.size
            self.format = pil_image.format
            self.pixel_format = pil_image.mode

    def __call__( self ,
                  input_file ,
                  output_file = None ) :
        # Try to get some image stats (lazy implementation with PIL)
        image_info = None
        try :
            import PIL
            import PIL.ImageFile
            ifile = open( input_file , "rb" )
            parser = PIL.ImageFile.Parser()
            parser.feed( ifile.read( 1024 ) ) # 1024 should be enough to get the header
            if parser.image :
                image_info = BaseConverter.ImageInfo( parser.image )
            try :
                p.close()
            except :
                # skip image incomplete error
                pass
            ifile.close()
            #del parser
            #del ifile
            #del PIL.ImageFile
            #del PIL
        except ImportError :
            pass

        if self.verbose :
            if image_info :
                print_flush( "image info: %dx%d %s %s" \
                                 % ( image_info.width ,
                                     image_info.height ,
                                     image_info.format ,
                                     image_info.pixel_format ) )
            else :
                print_flush( "image info: None" )

        cheat_backup = None
        try :
            if output_file == "__cheat__" :
                base , ext = os.path.splitext( input_file )
                if not self.dry_run :
                    cheat_backup = base + "_orig" + ext
                    self.__temporaries.add( cheat_backup )
                    cat_file( input_file , cheat_backup )
                output_file = input_file
            elif output_file == "__brutal_cheat__" :
                output_file = input_file

            self.process( input_file , output_file , image_info )

            if cheat_backup :
                self.__temporaries.remove( cheat_backup )
        except :
            if cheat_backup and os.path.exists( cheat_backup ) :
                if os.path.exists( input_file ) :
                    os.remove( input_file )
                os.rename( cheat_backup , input_file )
            self.clean()
            raise
        self.clean()

    def process( self ,
                 input_file ,
                 output_file ) :
        "Has to be implemented by subclasses."
        raise NotImplementedError

    def clean( self ) :
        remove_files( self.__temporaries )
        self.__temporaries.clear()
