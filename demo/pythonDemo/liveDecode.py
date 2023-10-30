import sys
import time
import cv2
import numpy as np
from ctypes import *
from PIL import Image, ImageDraw
from os import path

import BarcodeScanner as MWB
import MWParser as MWP
import MWResult as MWR

effortLevel = 4
useMultiCode = False
useTiles = False
suppressOutput = False
writeImage = False
tilesX = 6
tilesY = 6
overlap = 6
maxThreads = 4

argc = len(sys.argv)

# if argc < 2:
#     print("usage: pythonDemo [-En] [-M] [-S] [-T] [-Xn] [-Yn] [-On] [-Rn] [-W] filename\n\n" +
#           "    -En         Effort level (1-5, default "+str(effortLevel)+")\n" +
#           "    -M          Enable multi-code\n" +
#           "    -S          Suppress barcode results\n" +
#           "    -T          Generate tiled regions\n" +
#           "    -Xn         Tiles X dimension (default "+str(tilesX)+")\n" +
#           "    -Yn         Tiles Y dimension (default "+str(tilesY)+")\n" +
#           "    -On         Tiles overlap percentage  (default "+str(overlap)+")\n" +
#           "    -Rn         Maximum threads tiles/regions (default "+str(maxThreads)+")\n" +
#           "    -W          Write output image\n" +
#           "    filename    Image file to scan for barcodes\n\n")
#     exit()

for x in range(argc-2):
    arg = sys.argv[x+1]
    
    # Effort level (assume 3rd character is 1-5)
    if arg[0:2].upper() == "-E":
        effortLevel = int(arg[2:])

    # Enable multicode?        
    elif arg[0:2].upper() == "-M":
        useMultiCode = True
    
    # Suppress output?
    elif arg[0:2].upper() == "-S":
        suppressOutput = True
        
    # Enable tiles (and subsequently, multicode)?
    elif arg[0:2].upper() == "-T":
        useTiles = True

    # Set tiles X
    elif arg[0:2].upper() == "-X":
        tilesX = int(arg[2:])

    # Set tiles Y
    elif arg[0:2].upper() == "-Y":
        tilesY = int(arg[2:])

    # Set overlap
    elif arg[0:2].upper() == "-O":
        overlap = int(arg[2:])

    # Set maximum threads
    elif arg[0:2].upper() == "-R":
        maxThreads = int(arg[2:])

    # Write image with located barcodes?
    elif arg[0:2].upper() == "-W":
        writeImage = True
    
# Last argument must be the image file name
# fileName = sys.argv[argc-1]

def main():
    # Open live view of the camera
    cap = cv2.VideoCapture(0)
    if not cap.isOpened():
        print("Cannot open camera")
        exit()
    
    # Setup the SDK
    status = MWB.MWBregisterSDK(b'<your key here>')
    if status != MWB.MWB_RTREG_OK:
        print("MWBregisterSDK returned " + str(status) + "; scan results will be masked and some features may not be available")
    MWB.MWBsetActiveCodes(  # MWB.MWB_CODE_MASK_QR  |
                            MWB.MWB_CODE_MASK_DM |
                            # MWB.MWB_CODE_MASK_RSS |
                            # MWB.MWB_CODE_MASK_39 |
                            # MWB.MWB_CODE_MASK_EANUPC |
                            # MWB.MWB_CODE_MASK_128  |
                            # MWB.MWB_CODE_MASK_PDF  |
                            # MWB.MWB_CODE_MASK_AZTEC |
                            # MWB.MWB_CODE_MASK_25 |
                            # MWB.MWB_CODE_MASK_93 |
                            # MWB.MWB_CODE_MASK_CODABAR |
                            # MWB.MWB_CODE_MASK_DOTCODE |
                            # MWB.MWB_CODE_MASK_11 |
                            # MWB.MWB_CODE_MASK_MSI |
                            # MWB.MWB_CODE_MASK_MAXICODE |
                            # MWB.MWB_CODE_MASK_POSTAL |
                            # MWB.MWB_CODE_MASK_TELEPEN |
                            0x0)
    MWB.MWBsetDirection(MWB.MWB_SCANDIRECTION_HORIZONTAL | MWB.MWB_SCANDIRECTION_VERTICAL)
    MWB.MWBsetLevel(effortLevel)
    if useMultiCode:
        MWB.MWBsetFlags(0, MWB.MWBgetFlags(0) | MWB.MWB_CFG_GLOBAL_ENABLE_MULTI)
    MWB.MWBsetResultType(MWB.MWB_RESULT_TYPE_MW)
    parserMask = MWP.MWP_PARSER_MASK_NONE
    parserMask = MWP.MWP_PARSER_MASK_AAMVA
    sdkVersion = MWB.MWBgetLibVersionText()
    print("Starting decoder - SDK Version " + str(sdkVersion))
    print("  Effort level: " + str(effortLevel))
    if useMultiCode:
        print("  Multi-code enabled")
    if useTiles:
        print("  Using tiles (" + str(tilesX) + " x " + str(tilesY) + ") overlap " + str(overlap) + "%, " + str(maxThreads) + " threads");

    resultLen = int(0)
    regionData = []
    regionCount = int(0)

    # Go through the main loop
    while True:
        ret, frame = cap.read()
        # frame = cv2.imread('download.jpeg')
        # Pass the frame to the cognex sdk
        '''
        1. Convert cv2 image to PIL image
        2. Continue with SDK
        '''
        img = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        frame = Image.fromarray(frame)
        grayScale = frame.convert('L')
        pixels = grayScale.tobytes()
        if useTiles:
            # Tiles/regions
            #
            #  This feature is primarily intended for use with multicode when there exists relatively small codes within a large image, or when 
            #  the layout/locations of the barcodes is known. By dividing the image into smaller "regions", the decoder can process them much
            #  more efficiently, including using multiple threads, if desired. After processing all the regions, the SDK merges the results, 
            #  eliminating duplicates (due to overlapping regions) and retursn the results as a single MWResults list.
            # 
            #  When using tiles, the SDK divides the image into equally sized regions based on the number of columns (tilesX) and rows (tilesY)
            #  specified, with some overlap. Note that the overlap (expressed as a percentage of the image size) needs to be at least as large as
            #  the largest barcode to be scanned--otherwise the possibility exists that a code could be split across adjacent tiles and then
            #  not be detected.
            #
            #  Regions are defined as two pairs of X,Y coordinates as the upper left and lower right corners, in percentages of the image size.
            #
            #  An application can also construct its own region data, explicitly defining the coordinates of each region versus using the tiles
            #  feature. This application only demonstrates using tiles.
            #
            
            # Create the region data
            regionCount, regionData = MWB.MWBcreateRegionsFromTiles(tilesX, tilesY, overlap)
            
            # Decode using the defined regions (multithreaded)
            resultLen, scanResults = MWB.MWBscanGrayscaleRegions(pixels, grayScale.width, grayScale.height, regionData, regionCount, maxThreads)
        
        else:
            # Decode using the entire image (and any defined scanning rectangle)
            resultLen, scanResults = MWB.MWBscanGrayscaleImage(pixels, grayScale.width, grayScale.height)

        # Make sure the results did not overrun our buffer (very bad)
        if resultLen > sizeof(scanResults):
            print("Critical error: scanResults buffer is too small")
            exit()

        # The length returned is for the raw results buffer; we will create an MWResults object from this since
        # it's much easier to work with!
        #
        if resultLen > 0:
            # Display MWResults
            results = MWR.MWResults(scanResults)
            if results.count > 0:
                print("Total barcodes detected: ", results.count)

                if not(suppressOutput):
                    for i in range(results.count):
                        result = results.results[i]
                        print(str(i+1) + ": (" + result.typeName+ ") " + str(result.text))
                        if parserMask != MWP.MWP_PARSER_MASK_NONE:
                            
                            parsedData = create_string_buffer(10000)
                            
                            # Envoke the parser
                            #
                            #  The SDK provides two parsers, each returning different results:
                            #    JSON - parsed results are returned as a JSON document
                            #    Text - parsed results are returned as key/value pairs (primarily for demonstration/readability purposes)
                            #
                            #pLen, parsedData = MWB.MWPgetJSON(parserMask, result.parserInput, len(result.parserInput))
                            pLen, parsedData = MWB.MWPgetFormattedText(parserMask, result.parserInput, len(result.parserInput))
                            if pLen > sizeof(parsedData):
                                print("Critical error: parsedData buffer is too small")
                                exit()
            
                            if pLen > 0:
                                print("  Parsed Result: " + parsedData.raw[0:int(pLen)].decode("utf-8"))

                                
                # Reload the orignal image
                im = frame
                draw = ImageDraw.Draw(im)
                
                # Draw a red box around each barcode found
                for i in range(results.count):
                    result = results.results[i]
                    draw.line([(result.locationPoints.p1.x,result.locationPoints.p1.y),
                            (result.locationPoints.p2.x,result.locationPoints.p2.y),
                            (result.locationPoints.p3.x,result.locationPoints.p3.y),
                            (result.locationPoints.p4.x,result.locationPoints.p4.y),
                            (result.locationPoints.p1.x,result.locationPoints.p1.y)],fill=(255,0,0),width=4)
                        
                img = np.array(im)
                img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
                cv2.imshow('image', img)

                if cv2.waitKey(1) == ord('q'):
                    cap.release()
                    cv2.destroyAllWindows()
                    exit()
        
        

        else:
            print("No barcodes found: decoder returned " + str(resultLen))
            ret, frame = cap.read()
            cv2.imshow('image', frame)

            if cv2.waitKey(1) == ord('q'):
                cap.release()
                cv2.destroyAllWindows()
                exit()



        # Display the live view
        


if __name__ == '__main__':
    main()