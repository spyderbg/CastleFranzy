#include "CPS3ImageLoaderPNG.h"
#include "CImage.h"
#include "CReadFile.h"
#include "irros.h"

namespace irr
{
namespace video
{

bool CPS3ImageLoaderPng::isALoadableFileExtension(const c8* fileName) const
{
    const c8* ext = strrchr(fileName, '.');
    if (ext == 0)
        return false;
    return (strcmp(ext, ".PNG") == 0) || (strcmp(ext, ".png") == 0);
}
static int png_sig_cmp(char* sig, int start, int num_to_check)
{
   char png_signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};
   if (num_to_check > 8)
      num_to_check = 8;
   else if (num_to_check < 1)
      return (-1);

   if (start > 7)
      return (-1);

   if (start + num_to_check > 8)
      num_to_check = 8 - start;

   return ((int)(memcmp(&sig[start], &png_signature[start], num_to_check)));
}

bool CPS3ImageLoaderPng::isALoadableFileFormat(io::IReadFile* file) const
{
    if (!file) return false;
	char buffer[8];
	// Read the first few bytes of the PNG file
	if (file->read(buffer, 8) != 8)
		return false;

	// Check if it really is a PNG file
	return !png_sig_cmp(buffer, 0, 8);
}
/*E Set the argument for malloc callback function. */
typedef struct CtrlMallocArg{
	uint32_t mallocCallCounts;
} CtrlMallocArg;

/*E Set the argument for free callback function. */
typedef struct CtrlFreeArg{
	uint32_t freeCallCounts;
} CtrlFreeArg;
/*E Set the malloc callback function. */
static void *pngDecCbControlMalloc(uint32_t size, void *cbCtrlMallocArg){
	//CtrlMallocArg *arg;
	//arg = (CtrlMallocArg *)cbCtrlMallocArg;
	//arg->mallocCallCounts++;
	return memalign(128, size);
	//return MALLOC(size);
}

/*E Set the free callback function. */
static int32_t pngDecCbControlFree(void *ptr, void *cbCtrlFreeArg){
	//CtrlFreeArg *arg;
	//arg = (CtrlFreeArg *)cbCtrlFreeArg;
	//arg->freeCallCounts++;
	free(ptr);
	return 0;
}
static CtrlMallocArg ctrlMallocArg;
static CtrlFreeArg ctrlFreeArg;
int32_t PngDecCreate(CellPngDecMainHandle * mainHandle)
{
	/*E PNG Decoder needs those parameters. */
	CellPngDecThreadInParam 	threadInParam;
	CellPngDecThreadOutParam 	threadOutParam;


	/*E Parameter to contorl File. */
	//char                       fileName[NUMFILE][MAX_FILE_NAME_SIZE];
	//uint32_t                   fileNum;

	/*E Parameter to get return code. */
	int32_t                    ret;

	/*E Initialize display & allocate g_disp_buffs.element[].buffer */
	//displayInit();

	/*E Set the File Name */
	//sprintf( fileName[0], "%s%s%s", SYS_APP_HOME, "/", "Beach.png" );
	//sprintf( fileName[1], "%s%s%s", SYS_APP_HOME, "/", "Valley.png" );
	//sprintf( fileName[2], "%s%s%s", SYS_APP_HOME, "/", "Palm.png" );
	//sprintf( fileName[3], "%s%s%s", SYS_APP_HOME, "/", "CherryBlossom.png" );

	/*E Set the spu thread disable/enable. */
	ctrlMallocArg.mallocCallCounts = 0;
	ctrlFreeArg.freeCallCounts     = 0;
	threadInParam.spuThreadEnable   = CELL_PNGDEC_SPU_THREAD_ENABLE;//CELL_PNGDEC_SPU_THREAD_DISABLE;//
	threadInParam.ppuThreadPriority = 512;
	threadInParam.spuThreadPriority = 200;
	threadInParam.cbCtrlMallocFunc  = pngDecCbControlMalloc;
	threadInParam.cbCtrlMallocArg   = &ctrlMallocArg;
	threadInParam.cbCtrlFreeFunc    = pngDecCbControlFree;
	threadInParam.cbCtrlFreeArg     = &ctrlFreeArg;


	/*E Create the PNG Decoder. */
	ret = cellPngDecCreate(mainHandle, &threadInParam, &threadOutParam);
	//__glutReportInfo("cellPngDecCreate() returned 0x%x\n", ret);

	/*E Display png codec version */
	//__glutReportInfo("PNG codec version = %06x\n", threadOutParam.pngCodecVersion);

	return ret;
}
int32_t PngDecDestroy(CellPngDecMainHandle * mainHandle)
{
	/*E Destroy the PNG Decoder.   */
	int32_t                    ret;
	ret = cellPngDecDestroy(*mainHandle);
	//__glutReportInfo("cellPngDecDestroy() returned 0x%x\n", ret);

	//__glutReportInfo("Call Malloc Function = %d\n", ctrlMallocArg.mallocCallCounts);
	//__glutReportInfo("Call Free Function = %d\n",   ctrlFreeArg.freeCallCounts);

	//__glutReportInfo("Finished decoding all file.\n"); 
	return ret;
}
CPS3ImageLoaderPng::CPS3ImageLoaderPng()
{
    
}
CPS3ImageLoaderPng::~CPS3ImageLoaderPng()
{
}
IImage* CPS3ImageLoaderPng::loadImage(io::IReadFile* file) const
{
    int32_t                    ret;

    CellPngDecSubHandle         subHandle;
    CellPngDecSrc 		        src; 
    CellPngDecOpnInfo 	        openInfo;
    CellPngDecInfo 		        info;
    CellPngDecInParam 	        inParam;
    CellPngDecOutParam 	        outParam;
    CellPngDecDataOutInfo 	    dataOutInfo;
    CellPngDecDataCtrlParam     dataCtrlParam;
    video::IImage* image = 0;
    /*E PNG decode data */
    uint8_t                     *data = NULL;
    CellPngDecMainHandle mainHandle = 0;
    PngDecCreate(&mainHandle);
    /*E Decode PNG Files. */
    if(mainHandle != 0)
    {
        size_t dataSize   = file->getSize();
        char* buf         = irrnew c8[dataSize];
        if(file->read(buf,dataSize) != dataSize)
        {
            delete buf;
            buf = 0;
            os::Printer::log("Error Read file",ELL_ERROR);
            return NULL;
        }
        /*E Set the stream source. */
        src.srcSelect     = CELL_PNGDEC_BUFFER; //CELL_PNGDEC_FILE;
        src.fileName      = NULL;//fileName[fileNum];
        src.fileOffset    = 0;		
        src.fileSize      = 0;
        src.streamPtr     = buf;
        src.streamSize    = dataSize;

        /*E Set the spu thread disable/enable. */
        src.spuThreadEnable  = CELL_PNGDEC_SPU_THREAD_ENABLE;//CELL_PNGDEC_SPU_THREAD_DISABLE;//

        /*E Open the stream. */
        ret = cellPngDecOpen(mainHandle, &subHandle, &src, &openInfo);
        if(ret == CELL_OK)
        {
            /*E Read the png header by PNG Decoder. */
            ret = cellPngDecReadHeader(mainHandle, subHandle, &info);
        }
        if (ret == CELL_PNGDEC_ERROR_SPU_UNSUPPORT && src.spuThreadEnable == CELL_PNGDEC_SPU_THREAD_ENABLE)
        {
            // Adam7 interlace data and data with more than 10240 bytes or 2560 pixels in a line cannot be decoded 
            // on SPUs. It is possible to specify CELL_PNGDEC_SPU_THREAD_ENABLE and open such data, but the error 
            // CELL_PNGDEC_ERROR_SPU_UNSUPPORT will occur when the image information is obtained with cellPngDecReadHeader() 
            // or cellPngDecExtReadHeader(). When this happens, call cellPngDecClose() to close the stream and then 
            // reopen the stream with CELL_PNGDEC_SPU_THREAD_DISABLE specified.
            ret = cellPngDecClose(mainHandle, subHandle);
            src.spuThreadEnable  = CELL_PNGDEC_SPU_THREAD_DISABLE;
            ret = cellPngDecOpen(mainHandle, &subHandle, &src, &openInfo);
            ret = cellPngDecReadHeader(mainHandle, subHandle, &info);
        }
        if(ret == CELL_OK)
        {
            image = irrnew CImage(ECF_R8G8B8A8, core::dimension2d<s32>(info.imageWidth, info.imageHeight));
            /*E Display the info */
            /*
             *texWidth	= info.imageWidth;
             *texHeight	= info.imageHeight;
             *format     = GL_RGBA;
             *type		= GL_UNSIGNED_INT_8_8_8_8;
             */
            /*E Set the parameter for PNG Decoder. */
            inParam.commandPtr        = NULL;
            inParam.outputMode        = CELL_PNGDEC_TOP_TO_BOTTOM;
            inParam.outputColorSpace  = CELL_PNGDEC_RGBA;
            inParam.outputBitDepth    = 8;
            inParam.outputPackFlag    = CELL_PNGDEC_1BYTE_PER_1PIXEL;

            if((info.colorSpace == CELL_PNGDEC_GRAYSCALE_ALPHA) ||
                    (info.colorSpace == CELL_PNGDEC_RGBA) ||
                    (info.chunkInformation & 0x10))
            {
                inParam.outputAlphaSelect = CELL_PNGDEC_STREAM_ALPHA;
            }
            else
            {
                inParam.outputAlphaSelect = CELL_PNGDEC_FIX_ALPHA;
            }

            inParam.outputColorAlpha  = 0xff;

            ret = cellPngDecSetParameter(mainHandle, subHandle, &inParam, &outParam);
        }
        else
        {
            os::Printer::log("Error: cellPngDecReadHeader", ELL_ERROR);
            return NULL;
        }
        if(ret == CELL_OK)
        {
            dataCtrlParam.outputBytesPerLine = info.imageWidth * 4;
            data = (uint8_t*)image->lock();

            ret = cellPngDecDecodeData(mainHandle, subHandle, data, &dataCtrlParam, &dataOutInfo);
        }
        /*E Check the return code. */
        switch(ret)
        {
            case CELL_OK:
                break;
            case CELL_PNGDEC_ERROR_HEADER:
                os::Printer::log("Not a PNG stream", ELL_ERROR);
                break;
            case CELL_PNGDEC_ERROR_STREAM_FORMAT:
                os::Printer::log("Not completely compliant with the PNG format.", ELL_ERROR);
                break;
            case CELL_PNGDEC_ERROR_ARG:
                os::Printer::log("The argument of function is a wrong.",ELL_ERROR); 
                break;
            case CELL_PNGDEC_ERROR_SEQ:      
                os::Printer::log("The execution sequence of function is a wrong.",ELL_ERROR); 
                break;
            case CELL_PNGDEC_ERROR_BUSY:      
                os::Printer::log("The PNG Decoder is busy.",ELL_ERROR); 
                break;
            case CELL_PNGDEC_ERROR_FATAL:     
                os::Printer::log("The PNG Decoder is fatal error.",ELL_ERROR);
                break;
            case CELL_PNGDEC_ERROR_OPEN_FILE:    
                os::Printer::log("Can't open", ELL_ERROR);
                break;
            case CELL_PNGDEC_ERROR_SPU_UNSUPPORT:
                os::Printer::log("Can't decode with SPU.",ELL_ERROR);
                break;
            case CELL_PNGDEC_ERROR_SPU_ERROR:
                os::Printer::log("Error occurs while decoding with SPU.",ELL_ERROR);
                break;
            default:
                os::Printer::log("Error of Lv2/libc = ",ELL_ERROR);
                break;
        }
        if(ret != 0)
        {
            delete image;
            image = 0;
            os::Printer::log("decode PNG ERROR",ELL_ERROR);
        }
        /*E Close stream */
        ret = cellPngDecClose(mainHandle, subHandle);
        delete buf;
        buf = 0;
    }
    if(mainHandle)PngDecDestroy(&mainHandle);
    mainHandle = 0;
    return image;
}

IImageLoader* createPS3ImageLoaderPNG()
{
	return irrnew CPS3ImageLoaderPng();
}
}
}
