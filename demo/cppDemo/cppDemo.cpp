/**
 * File      cppDemo.cpp
 * Brief     Sample C++ program for Cognex barcode decoding libary
 *
 * Details   This is a simple demonstration program for using the Cognex Moblie Barcode SDK core decoder library
 *           (formally the Manatee Works Barcode Scanner Libary, or mwLib). The application takes a JPEG image
 *           file as input, loads it into memory, converting it to grayscale, then decodes it using the
 *           specified configuration (barcode types, multicode features, etc.). This is not a comprehensive demo
 *           of all of the SDK's features, but rather an example of how to use the library to decode barcodes
 *           from a JPEG file.
 *
 * Notice    Copyright (C) Cognex Corporation
 */

#include <stdio.h>
#include <string.h>
#include <jpeglib.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "BarcodeScanner.h"
#include "MWResult.h"
#include "MWParser.h"
#include "imageLib.h"

using namespace std;
using namespace CognexImageLib;


int main(int argc, char* argv[])
{
    struct timeval tval_start, tval_end, tval_diff;
    int effortLevel = 4;
    bool useMultiCode = false;
    bool useTiles = false;
    bool suppressOutput = false;
    int tilesX = 6;
    int tilesY = 6;
    int overlap = 6;
    int maxThreads = 4;

    // Display usage message
    if (argc < 2)
    {
        printf("usage: cppDemo [-En] [-M] [-T] [-Xn] -[Yn] [-On] [-Rn] filename\n"
               "    -En         Effort level (1-5, default %d)\n"
               "    -M          Enable multi-code\n"
               "    -T          Generate tiled regions\n"
               "    -S          Suppress barcode results\n"
               "    -Xn         Tiles X dimension (default %d)\n"
               "    -Yn         Tiles Y dimension (default %d)\n"
               "    -On         Tiles overlap percentage  (default %d)\n"
               "    -Rn         Maximum threads tiles/regions (default %d)\n"
               "    filename    Image file to scan for barcodes\n\n",
               effortLevel, tilesX, tilesY, overlap, maxThreads);
        return 0;
    }

    // Parse the parameters
    //
    //  There are much more elegant and foolproof ways to process command line arguments; but this 
    //  is just a simple sample application.  :)
    //
    for (int x = 1; x < argc - 1; x++)
    {
        switch (toupper(argv[x][1])) {
        // Effort level (assume 3rd character is 1-5)
        case 'E':
            sscanf(argv[x] + 2, "%d", &effortLevel);
            break;

        // Effort level (assume 3rd character is 1-5)
        case 'M':
            useMultiCode = true;
            break;

        // Enable tiles?
        case 'T':
            useTiles = true;
            break;

        // Suppress barcode data ouput?
        case 'S':
            suppressOutput = true;
            break;

        // Set tiles X
        case 'X':
            sscanf(argv[x] + 2, "%d", &tilesX);
            break;

        // Set tiles Y
        case 'Y':
            sscanf(argv[x] + 2, "%d", &tilesY);
            break;

        // Set overlap
        case 'O':
            sscanf(argv[x] + 2, "%d", &overlap);
            break;

        // Set maximum threads
        case 'R':
            sscanf(argv[x] + 2, "%d", &maxThreads);
            break;
        }
    }

    // Last argument must be the image file name
    const char *fileName = argv[argc - 1];

    // Does the file exist?
    struct stat buffer;
    if (stat(fileName, &buffer) != 0)
    {
        printf("Unable to load image %s\n", fileName);
        return 0;
    }

    gettimeofday(&tval_start, NULL);
    printf("Loading image...");

    // The GrayScale class is a simple image loader (see ImageLib.cpp) that converts a JPEG image to the 8-bit grayscale 
    // format used by the decoder library.
    // 
    GrayScale *grayScale = new GrayScale(fileName);
    gettimeofday(&tval_end, NULL);
    timersub(&tval_start, &tval_end, &tval_diff);
    printf(" (%ld ms)\n", (long int)tval_diff.tv_usec / 1000);


    // Initialize the SDK with the license key.
    //
    //   Visit https://cmbdn.cognex.com to obtain a trial license key--without a valid, active license key
    //   barcode scan results will be masked with '*' characters and some features, like multi-code, may 
    //   not be enabled at all. Replace '<your key here>' below with your license key string.
    //
    int status = MWB_registerSDK("<your key here>");
    if (status != MWB_RTREG_OK)
    {
        printf("MWB_registerSDK returned %d; scan results will be masked and some features may not be available\n", status);
    }

    // Active codes
    //
    //  You must specify which symbologies the decoder is to look for; no symbologies are enabled by default. The parameter
    //  to MWBsetActiveCodes is an ORed mask of all the symbologies to be enabled.
    //
    //  For the best performance, only enable symbologies you explicitly need to scan; enabling too many (unnecessary) symbologies
    //  can slow the decoder, especially on low-end devices.
    //
    MWB_setActiveCodes(//MWB_CODE_MASK_QR |
                       //MWB_CODE_MASK_DM |
                       //MWB_CODE_MASK_RSS |
                       //MWB_CODE_MASK_39 |
                       //MWB_CODE_MASK_EANUPC |
                       //MWB_CODE_MASK_128 |
                       MWB_CODE_MASK_PDF |
                       //MWB_CODE_MASK_AZTEC |
                       //MWB_CODE_MASK_25 |
                       //MWB_CODE_MASK_93 |
                       //MWB_CODE_MASK_CODABAR |
                       //MWB_CODE_MASK_DOTCODE |
                       //MWB_CODE_MASK_11 |
                       //MWB_CODE_MASK_MSI |
                       //MWB_CODE_MASK_MAXICODE |
                       //MWB_CODE_MASK_POSTAL |
                       //MWB_CODE_MASK_TELEPEN |
                       0x0);

    // 1D Scan direction
    //
    //  The decoder supports horizontal, vertical, hortizonal & vertical, or omnidirectional scanning for 1D barcodes (2D codes will 
    //  scan in any orientation and are not impacted by this setting). In most cases, using just horizontal & vertical is sufficient
    //  to locate most 1D codes; omnidirectional is much slower.
    //
    MWB_setDirection(MWB_SCANDIRECTION_HORIZONTAL | MWB_SCANDIRECTION_VERTICAL);
    //MWB_setDirection(MWB_SCANDIRECTION_OMNI);

    // Verify 1D location
    //
    //  Due to the weakness of some 1D symbologies, misreads and short reads are possible in cases of poor print quality, blurry images,
    //  extreme skew, etc. To combat this, the decoder provides a verification feature where the decoder works harder to verify the 1D
    //  barcode's edges and value. By default, this feature is on for Code 11, Code 25 (all variants), Code 39, Codabar, Code 128, and 
    //  MSI Plessey. Be aware that this setting can reduce scanning performance, especially if more than a few symoblogies are enabled.
    //
    //MWB_setParam(MWB_CODE_MASK_EANUPC, MWB_PAR_ID_VERIFY_LOCATION, MWB_PAR_VALUE_VERIFY_LOCATION_ON);

    // Quiet zones
    //
    //  Most 1D barcode symbologies define minimum quiet zones for the barcode (whitespace around the barcode's bar/space patterns) to
    //  aid in detection and decoding. By default, the decoder only requires that 50% of this requirement be met for Code 11, Code 25,
    //  Code 39, Code 128, Codabar, and MSI Plessey (this value is not tunable for other supported symbologies). This quiet zone
    //  requirement can be tuned to be more or less strict. Keep in mind though:
    //
    //     * Less strict quiet zones can lead to misreads and short reads (not recommended)
    //     * More strict quiet zones can help reduce misreads and short reads, but may lead to some no reads.
    //
    //  Below is an example of changing Code 128 to require 100% of the specified quiet zone for the code to be read.
    //
    //MWB_setParam(MWB_CODE_MASK_128, MWB_PAR_ID_SAFE_ZONE_SCALE, 100);

    // Decoder effort level (1 - 5)
    //
    //  When scanning from a live video stream, typically use level 2 (use level 1 for very pristine codes), or level 3  
    //  for difficult or dense codes (PDF417). Levels 4 and 5 are intended for offline/batch scanning of an image where
    //  extended decode times are not an issue.
    //
    MWB_setLevel(effortLevel);

    // Scanning rectangle
    //
    //  You can limit the portion of the image that the SDK will search when locating a barcode. This is useful when working
    //  with large images and the general location of the barcode is known (when scanning with a mobile phone, this scenario
    //  is common as the user interface typically leads the user to center the barcode and even within a virtual bounding area
    //  on screen). This may not be the case with offline image processing (that is, the barcode may appear anywhere within
    //  the image).
    //
    //  While a different scanning rectangle can be defined for each symbology, the SDK will use a union of all defined 
    //  scanning rectangles for 1D symbologies when searching for 1D barcodes (including PDF417) and a union of all 
    //  defined scanning rectangles for 2D symbologies when searching for 2D barcodes.
    //
    //  Keep in mind that for 1D codes, a portion of the barocode that is at least the entire width of the barcode (including
    //  quiet zones) must be within the scanning rectangle for it to be decoded (the entire height of the 1D code is not 
    //  required to be within the scanning rectangle), while for 2D codes the entire code including quiet zones, must be
    //  within the scanning rectangle for it to be decoded.
    //
    //  A scanning rectangle for PDF417 is treated as a 1D symbology; however, due to PDF417's error correction capabiliies,
    //  this means that the entire width of a PDF417 may not have to be within the scanning rectangle to be decoded (since 
    //  the error correction can recover erasures). Generally this is only likely to occur when a high ECC level has been
    //  employed: otherwise expect that the entire PDF417 must be within the scanning rectangle for reliable decoding.
    //
    //  NOTE: do not use a scanning rectangle in junction with multi-code and regions as the results may be 
    //        unpredictable.
    //
    //MWB_setScanningRect(MWB_CODE_MASK_PDF, 0, 25, 100, 50);

    // Multi-code
    //
    //  Multi-code can detect as many as 150 barcodes in a single image, though doing so may require significant resources. If the
    //  image(s) being decoded have specific, known regions or very dense codes, consider using tiles or defining custom regions.
    //  Multi-code requires a license key that has this feature enabled (NOTE: a trial license includes multi-code).
    //
    //  Note that when setting global decoder options (code mask == 0), OR the new flag with any existing settings.
    //
    if (useMultiCode)
    {
        MWB_setFlags(0, MWB_getFlags(0) | MWB_CFG_GLOBAL_ENABLE_MULTI);
    }

    // Scan results
    //
    //  Use MWResults for all meta data as well as the scanned barcode data
    //
    MWB_setResultType(MWB_RESULT_TYPE_MW);

    // Parsers
    //
    //  Industry standard parsers for a variety of formats are provided for easier processing of common structured data formats. The 
    //  parsed results can either be a JSON document or formated text (see the use of the parser below). The SDK provides the following
    //  parsers:
    //      GS1 - standard data format for the exchange of global trade information
    //      IUID - Item Unique Identification marking (or UID), mandated by the US Department of Defense
    //      ISBT - global standard for the identification, labeling, and information transfer of medical products of human origin
    //      AAMVA - format used for North American driver's licenses and IDs (PDF417 barcode)
    //      HIBC - similar to IUID/UID, the Health Industry Bar Code addresse the specific safety and security needs of the 
    //             healcare industry
    //      SCM - Structured Carrier message; most commonly used in MaxiCode barcodes for shipping inforamtion 
    //
    //  See mwParser.cs for more details.
    //
    //  Use of the parsers requires a license key that has this feature enabled (NOTE: a trial license includes all parsers).
    //
    int parserMask = MWP_PARSER_MASK_NONE;
    //int parserMask = MWP_PARSER_MASK_GS1;

    // Gets SDK version as a string
    const char *sdkVersion = MWB_getLibVersionText();

    printf("Starting decoder - SDK Version %s\n", sdkVersion);
    printf("  Image %s (%d X %d pixels)\n", fileName, grayScale->getHeight(), grayScale->getWidth());
    printf("  Effort level: %d\n", effortLevel);
    if (useMultiCode)
    {
        printf("  Multi-code enabled\n");
    }
    if (useTiles)
    {
        printf("  Using tiles (%d x %d) overlap %d%%, %d threads\n",
               tilesX, tilesY, overlap, maxThreads);
    }

    // Time how long to decode
    gettimeofday(&tval_start, NULL);

    // Decode the image
    uint8_t *scanResults = NULL;
    int len = 0;
    float *regionData = NULL;
    int regionCount = 0;

    if (useTiles)
    {
        // Tiles/regions
        //
        //  This feature is primarily intended for use with multicode when there exists relatively small codes within a large image, or when 
        //  the layout/locations of the barcodes is known. By dividing the image into smaller "regions", the decoder can process them much
        //  more efficiently, including using multiple threads, if desired. After processing all the regions, the SDK merges the results, 
        //  eliminating duplicates (due to overlapping regions) and retursn the results as a single MWResults list.
        // 
        //  When using tiles, the SDK divides the image into equally sized regions based on the number of columns (tilesX) and rows (tilesY)
        //  specified, with some overlap. Note that the overlap (expressed as a percentage of the image size) needs to be at least as large as
        //  the largest barcode to be scanned--otherwise the possibility exists that a code could be split across adjacent tiles and then
        //  not be detected.
        //
        //  Regions are defined as two pairs of X,Y coordinates as the upper left and lower right corners, in percentages of the image size.
        //
        //  An application can also construct its own region data, explicitly defining the coordinates of each region versus using the tiles
        //  feature. This application only demonstrates using tiles.
        //
        regionData = new float[tilesX * tilesY * 4];

        // Create the region data
        regionCount = MWB_createRegionsFromTiles(tilesX, tilesY, overlap, regionData);

        // Decode using the defined regions (multithreaded)
        len = MWB_scanGrayscaleRegions(grayScale->getGrayScale(), grayScale->getWidth(), grayScale->getHeight(), regionData, regionCount, maxThreads, &scanResults);

        delete regionData;
    }
    else
    {
        len = MWB_scanGrayscaleImage(grayScale->getGrayScale(), grayScale->getWidth(), grayScale->getHeight(), &scanResults);
    }

    gettimeofday(&tval_end, NULL);
    timersub(&tval_start, &tval_end, &tval_diff);

    // The length returned is for the raw results buffer; we will create an MWResults object from this since
    // it's much easier to work with!
    //
    if (len > 0)
    {
        MWResults *mwResults = new MWResults(scanResults);
        free(scanResults);
        if (mwResults != NULL && mwResults->getCount() > 0) 
        {
            printf("Total barcodes detected: %d\n", mwResults->getCount());

            for (int i = 0; !suppressOutput && i < mwResults->getCount(); i++)
            {
                MWResult *mwResult = mwResults->getResult(i);

                printf("%d: (%s) %s\n",
                       i + 1,
                       mwResult->getTypeName().c_str(),
                       mwResult->getText().c_str());

                // do we have any parsers enabled?
                if (parserMask != MWP_PARSER_MASK_NONE)
                {
                    unsigned char *parsedData = NULL;

                    // Envoke the parser
                    //
                    //  The SDK provides two parsers, each returning different results:
                    //    JSON - parsed results are returned as a JSON document
                    //    Text - parsed results are returned as key/value pairs (primarily for demonstration/readability purposes)
                    //
                    double pLen = MWP_getJSON(parserMask, (uint8_t*)mwResult->getParserInput(), (int)strlen(mwResult->getParserInput()), &parsedData);
                    //double pLen = MWP_getFormattedText(parserMask, (uint8_t*)mwResult->getParserInput(), (int)strlen(mwResult->getParserInput()), &parsedData);

                    if (pLen > 0)
                    {
                        printf("  Parsed Result: %s\n", parsedData);
                    }

                    free(parsedData);
                }

            }
            delete mwResults;

        }
        else
        {
            printf("Failed to decode MWResults\n");
        }
    }
    else
    {
        printf("No barcodes found, decoder returned %d\n",len);
    }
 
    printf("Total decoder time %ld ms\n", (long int)tval_diff.tv_usec / 1000);

    return 0;

} // main()
