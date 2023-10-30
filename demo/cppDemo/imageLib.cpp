/**
 * File    imageLib.cpp
 * Brief   Wrapper/helper functions to load images and convert to grayscale
 *
 * Details This class implements converting images files (BMP, PNG, JPG, and TIFF) of various color types
 *         to grayscale for processing by the decoder library. Optimized methods for RBG and aRGB images
 *         have been provided, with the slower, default case handling most other color formats.
 *
 * Notice  Copyright (C) Cognex Corporation
 *
 */

#include <stdio.h>
#include <jpeglib.h>

#include "Barcodescanner.h"
#include "imageLib.h"

namespace CognexImageLib {

	GrayScale::GrayScale(const char *fileName)
		: height(0), width(0), bitPlanes(0), grayScale(NULL)
	{
        unsigned char *raw_image = NULL;

        /* these are standard libjpeg structures for reading(decompression) */
        struct jpeg_decompress_struct cinfo;
        struct jpeg_error_mgr jerr;

        /* libjpeg data structure for storing one row, that is, scanline of an image */
        JSAMPROW row_pointer[1];

        FILE *infile = fopen(fileName, "rb");
        unsigned long location = 0;

        if (!infile)
        {
            return;
        }

        /* here we set up the standard libjpeg error handler */
        cinfo.err = jpeg_std_error(&jerr);

        /* setup decompression process and source, then read JPEG header */
        jpeg_create_decompress(&cinfo);

        /* this makes the library read from infile */
        jpeg_stdio_src(&cinfo, infile);

        /* reading the image header which contains image information */
        jpeg_read_header(&cinfo, TRUE);

        /* Start decompression jpeg here */
        jpeg_start_decompress(&cinfo);

        /* allocate memory to hold the uncompressed image */
        raw_image = (unsigned char*)malloc(cinfo.output_width*cinfo.output_height*cinfo.num_components);

        /* allocate memory for a decompressed scan line */
        row_pointer[0] = (unsigned char *)malloc(cinfo.output_width*cinfo.num_components);

        width = cinfo.output_width;
        height = cinfo.image_height;
        bitPlanes = cinfo.num_components;

        /* read one scan line at a time */
        int line = 0;
        while (cinfo.output_scanline < cinfo.image_height)
        {
            jpeg_read_scanlines(&cinfo, row_pointer, 1);
            for (unsigned int i = 0; i < cinfo.image_width*cinfo.num_components; i++)
                raw_image[location++] = row_pointer[0][i];
            line++;
        }

        /* wrap up decompression, destroy objects, free pointers and close open files */
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);

        free(row_pointer[0]);
        fclose(infile);

        /* Now convert to grayscale */
        grayScale = (uint8_t *)malloc(height * width * sizeof(uint8_t));

        bool useSingleBitplane = true;
        int x, y;
        for (y = 0; y < height; y++) {
            int srcOffset = y * width;
            int dstOffset = (y * width) * bitPlanes;
            for (x = 0; x < width; x++) {
                if (useSingleBitplane)
                {
                    grayScale[srcOffset] = raw_image[dstOffset];
                }
                else
                {
                    //use all bitplanes
                    int bitplaneSum = 0;
                    for (int bitplane = 0; bitplane < bitPlanes; bitplane++)
                        bitplaneSum += raw_image[dstOffset + bitplane];
                    grayScale[srcOffset] = bitplaneSum / bitPlanes;
                }
                srcOffset++;
                dstOffset += bitPlanes;
            }
        }


        /*  //scaled down version
            width /= 2;
            height /= 2;
            bool useSingleBitplane = true;
            int x, y;
            for (y = 0; y < height; y++) {
                int srcOffset = y * width;
                int dstOffset = (y * width * 4) * bitPlanes;
                for (x = 0; x < width; x++) {
                    if (useSingleBitplane)
                    {
                        grayScale[srcOffset] = raw_image[dstOffset];
                    }
                    else
                    {
                        //use all bitplanes
                        int bitplaneSum = 0;
                        for (int bitplane = 0; bitplane < bitPlanes; bitplane++)
                            bitplaneSum += raw_image[dstOffset + bitplane];
                        grayScale[srcOffset] = bitplaneSum / bitPlanes;
                    }
                    srcOffset++;
                    dstOffset += (bitPlanes * 2);
                }
            }
        */

        free(raw_image);

	}


	GrayScale::~GrayScale()
	{
		height = 0;
		width = 0;
		if (grayScale != NULL)
		{
			free(grayScale);
			grayScale = NULL;
		}
	}

	uint8_t *GrayScale::getGrayScale()
	{
		return grayScale;
	}

	int GrayScale::getHeight()
	{
		return height;
	}

	int GrayScale::getWidth()
	{
		return width;
	}

    int GrayScale::getBitPlanes()
    {
        return bitPlanes;
    }

    int GrayScale::getSize()
	{
		return width * height;
	}

} // namespace CognexImageLib