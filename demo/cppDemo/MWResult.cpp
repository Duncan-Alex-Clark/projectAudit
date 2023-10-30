/**
 * File    MWResult.cpp
 * Brief   Class to encapsulate the decoder library's results as a C++ class
 *
 * Details This clase provides a convenient wrapper for the Cognex Mobile Barcode
 *         SDK's scan results: it primarily constructs an MWResults object, which is
 *         an array of MWResult objects--each MWResult object is scanned barcode
 *         with all it's available meta data.
 *         
 * Notice  Copyright (C) Cognex Corporation
 *
 */
#include <iostream>
#include <cstring>

#include "MWResult.h"

using namespace Cognex;

PointF::PointF(void) :
    x(0), y(0)
{
}


MWLocation::MWLocation()
{
}

MWLocation::MWLocation(float * _points)
{

    points = new PointF[4];

    for (int i = 0; i < 4; i++)
    {
        points[i].setX(_points[i * 2]);
        points[i].setY(_points[i * 2 + 1]);
    }

    p1 = new PointF();
    p2 = new PointF();
    p3 = new PointF();
    p4 = new PointF();

    p1->setX(_points[0]);
    p1->setY(_points[1]);
    p2->setX(_points[2]);
    p2->setY(_points[3]);
    p3->setX(_points[4]);
    p3->setY(_points[5]);
    p4->setX(_points[6]);
    p4->setY(_points[7]);
}

MWLocation::~MWLocation()
{
    delete p1;
    delete p2;
    delete p3;
    delete p4;
    delete[] points;
}


MWResult::MWResult() :
    text(""), textEncoding(""), typeName(""), bytes(NULL), bytesLength(0), type(0), subtype(0), imageWidth(0), imageHeight(0), isGS1(false), locationPoints(),
	parserInput(NULL), modulesCountX(0), modulesCountY(0), moduleSizeX(0), moduleSizeY(0), skew(0), iskanji(false), barcodeWidth(0), barcodeHeight(0),
	pdfRows(0), pdfColumns(0), pdfIsTruncated(false), pdfECLevel(0), pdfCodewords(NULL)
{
}

MWResult::~MWResult(){
    if (locationPoints != NULL){
        delete locationPoints;
    }
    if (bytes != NULL){
        free(bytes);
    }
    if (parserInput != NULL){
        free(parserInput);
    }
	if (pdfCodewords != NULL) {
		free(pdfCodewords);
	}
    
}

void MWResult::setTypeName(int type) {
	string typeName = "Unknown";
	switch (type) {
	case FOUND_25_INTERLEAVED: typeName = "Code 25 Interleaved"; break;
	case FOUND_25_STANDARD: typeName = "Code 25 Standard"; break;
	case FOUND_128: typeName = "Code 128"; break;
	case FOUND_128_GS1: typeName = "Code 128 GS1"; break;
	case FOUND_39: typeName = "Code 39"; break;
	case FOUND_32: typeName = "Code 32"; break;
	case FOUND_93: typeName = "Code 93"; break;
	case FOUND_AZTEC: typeName = "AZTEC"; break;
	case FOUND_DM: typeName = "Datamatrix"; break;
	case FOUND_QR: typeName = "QR"; break;
	case FOUND_EAN_13: typeName = "EAN 13"; break;
	case FOUND_EAN_8: typeName = "EAN 8"; break;
	case FOUND_NONE: typeName = "None"; break;
	case FOUND_RSS_14: typeName = "Databar 14"; break;
	case FOUND_RSS_14_STACK: typeName = "Databar 14 Stacked"; break;
	case FOUND_RSS_EXP: typeName = "Databar Expanded"; break;
	case FOUND_RSS_LIM: typeName = "Databar Limited"; break;
	case FOUND_UPC_A: typeName = "UPC A"; break;
	case FOUND_UPC_E: typeName = "UPC E"; break;
	case FOUND_PDF: typeName = "PDF417"; break;
	case FOUND_CODABAR: typeName = "Codabar"; break;
	case FOUND_DOTCODE: typeName = "Dotcode"; break;
	case FOUND_11: typeName = "Code 11"; break;
	case FOUND_MSI: typeName = "MSI Plessey"; break;
	case FOUND_25_IATA: typeName = "IATA Code 25"; break;
	case FOUND_ITF14: typeName = "ITF 14"; break;
	case FOUND_25_MATRIX: typeName = "Code 2/5 Matrix"; break;
	case FOUND_25_COOP: typeName = "Code 2/5 COOP"; break;
	case FOUND_25_INVERTED: typeName = "Code 2/5 Inverted"; break;
	case FOUND_MAXICODE: typeName = "Maxicode"; break;
	case FOUND_QR_MICRO: typeName = "Micro QR"; break;
	case FOUND_POSTNET: typeName = "Postnet"; break;
	case FOUND_PLANET: typeName = "Planet"; break;
	case FOUND_IMB: typeName = "Intelligent mail"; break;
	case FOUND_ROYALMAIL: typeName = "Royal mail"; break;
	case FOUND_MICRO_PDF: typeName = "Micro PDF417"; break;
	case FOUND_AUSTRALIAN: typeName = "Australian"; break;
	case FOUND_TELEPEN: typeName = "Telepen"; break;
	}

	this->typeName = typeName;
	return;
}

void MWResult::setBytes(char * array)
{
	int bytesLen = getBytesLength();
	locationPoints = NULL;

	bytes = (char *)malloc(bytesLen);
	if (!bytes)
	{
		cout << "Can not allocate memory!!!" << endl;
		return;
	}
	memcpy(bytes, array, bytesLen);
}

void MWResult::setParserInput(char *array, int len)
{
	parserInput = (char *)malloc(len + 1);
	if (!parserInput)
	{
		cout << "Not enought memory!!" << endl;
		return;
	}
	memcpy(parserInput, array, len);
	parserInput[len] = '\0';
}

int MWResult::setPdfCodewords(uint8_t *buffer_pdfCodewords_p)
{
	const int byteShift = 8;
	int i = 0;

	//Little-Endian order of bytes: Last byte (4th) is the MSB, first byte (lowest address) is the LSB
	int first_int_value = (buffer_pdfCodewords_p[3] << (3 * byteShift)) |
		(buffer_pdfCodewords_p[2] << (2 * byteShift)) |
		(buffer_pdfCodewords_p[1] << (1 * byteShift)) |
		(buffer_pdfCodewords_p[0]);

	int array_length = first_int_value;

	if (array_length > 0)
	{
		pdfCodewords = (int*)calloc(array_length, sizeof(int));

		int buffer_length = array_length * sizeof(int); //for passing the bytes containing the int pdfCodewords[] data

		int pdfCodewords_count = 0;
		pdfCodewords[0] = first_int_value;
		pdfCodewords_count++;

		const int int_size = sizeof(int);
		i = sizeof(first_int_value);
		for (; i < buffer_length; i += int_size)
		{
			int value = (buffer_pdfCodewords_p[i + 3] << (3 * byteShift)) |
				(buffer_pdfCodewords_p[i + 2] << (2 * byteShift)) |
				(buffer_pdfCodewords_p[i + 1] << (1 * byteShift)) |
				(buffer_pdfCodewords_p[i]);

			pdfCodewords[pdfCodewords_count++] = value;
		}

	}

	return i;

}


MWResults::MWResults() :
	version(0), count(0), results()
{
}

MWResults::~MWResults()
{
	for (int i = 0; i < count; i++)
	{
		MWResult *result = getResult(i);
		delete result;
    }
}


MWResults::MWResults(uint8_t *buffer) :
    version(0), count(0), results()
{
    setCount(0);

	// First 3 characters of the buffer must be 'MWR'
    if (buffer[0] != 'M' || buffer[1] != 'W' || buffer[2] != 'R')
    {
        return;
    }

	// 3rd character is the version (not meaningful)
	setVersion(buffer[3]);

	// 4th character is the number of results (scanned codes)
    setCount(buffer[4]);
    
    for (int currentPos = 5,i = 0; i < count; i++)
    {
        MWResult *result = new MWResult();

        int fieldsCount = buffer[currentPos];
        currentPos++;
        for (int f = 0; f < fieldsCount; f++) {
			// Each field consists of 5 values:
			//    fieldType - 1 byte
			//    fieldNameLength - 1 byte
			//    fieldContentLength - 2 bytes
			//    fieldName - fieldNameLength bytes
			//    content - fieldContentLenght bytes
			int fieldType = buffer[currentPos];
            int fieldNameLength = buffer[currentPos + 1];
            int fieldContentLength = 256 * (buffer[currentPos + 3 + fieldNameLength] & 0xFF) + (buffer[currentPos + 2 + fieldNameLength] & 0xFF);
            string fieldName = "";

            if (fieldNameLength > 0)
			{
				// Not actually used/stored in the results class
				fieldName.assign((char *)buffer + currentPos + 2, fieldNameLength);
			}

            int contentPos = currentPos + fieldNameLength + 4;

            switch (fieldType)
            {
			// Barcode contents (as UTF8 string)
			case MWB_RESULT_FT_TEXT:
			{
				string textStr;
				textStr.assign((char*)buffer + contentPos, fieldContentLength);
				result->setText(textStr);
				break;
			}

			// Text Encoding (currenlty only used by QR codes)
			case MWB_RESULT_FT_TEXT_ENCODING:
			{
				string encStr;
				encStr.assign((char*)buffer + contentPos, fieldContentLength);
				result->setText(encStr);
				break;
			}

			// Barcode Type (symbology)
			case MWB_RESULT_FT_TYPE:
                result->setType(*(int*)&buffer[contentPos]);
				result->setTypeName(result->getType());
                break;

			// Barcode Subtype (symbology)
			case MWB_RESULT_FT_SUBTYPE:
                result->setSybType(*(int*)&buffer[contentPos]);
                break;

			// IsGs1 Code flag
			case MWB_RESULT_FT_ISGS1:
                result->setGs1((*(int*)&buffer[contentPos]) == 1);
                break;

			// Image width (pixels)
			case MWB_RESULT_FT_IMAGE_WIDTH:
                result->setImageWidth(*(int*)&buffer[contentPos]);
                break;

			// Image height (pixels)
			case MWB_RESULT_FT_IMAGE_HEIGHT:
                result->setImageHeight(*(int*)&buffer[contentPos]);
                break;

			// Barcode location (4 points as floating point percentages)
			case MWB_RESULT_FT_LOCATION:
            {
                float locations[8 * sizeof(float)];
                for (int l = 0; l < 8; l++)
                {
                    locations[l] = *(float *)&buffer[contentPos + l * 4];
                }

                MWLocation *l = new MWLocation(locations);
                result->setLocationPoints(l);
                break;
            }

			// Barcode contents (raw bytes)
			case MWB_RESULT_FT_BYTES:
                result->setBytesLength(fieldContentLength);
                result->setBytes((char*)buffer + contentPos);
                break;

			// Parser bytes (hex encoded string)
			case MWB_RESULT_FT_PARSER_BYTES:
                result->setParserInput((char *)buffer + contentPos, fieldContentLength);
                break;

			// Module count (X axis)
			case MWB_RESULT_FT_MODULES_COUNT_X:
                result->setModulesCountX(*(int*)&buffer[contentPos]);
                break;

			// Module count (y axis)
			case MWB_RESULT_FT_MODULES_COUNT_Y:
                result->setModulesCountY(*(int*)&buffer[contentPos]);
                break;

			// Module size in pixels (x axis)
			case MWB_RESULT_FT_MODULE_SIZE_X:
                result->setModuleSizeX(*(float*)&buffer[contentPos]);
                break;

			// Module size in pixels (y axis)
			case MWB_RESULT_FT_MODULE_SIZE_Y:
                result->setModuleSizeY(*(float*)&buffer[contentPos]);
                break;

			// Skew
            case MWB_RESULT_FT_SKEW:
                result->setSkew(*(float*)&buffer[contentPos]);
                break;

			// Is Kanji Encoded flag (QR only)
			case MWB_RESULT_FT_KANJI:
                result->setKanji((*(int*)&buffer[contentPos]) == 1);
                break;

			// Barcode width in pixels
			case MWB_RESULT_FT_BARCODE_WIDTH:
                result->setBarcodeWidth(*(float*)&buffer[contentPos]);
                break;

			// Barcode height in pixels
			case MWB_RESULT_FT_BARCODE_HEIGHT:
                result->setBarcodeHeight(*(float*)&buffer[contentPos]);
                break;

			// PDF417 only: number of data rows
			case MWB_RESULT_FT_PDF_ROWS:
                result->setPdfRows(*(int*)&buffer[contentPos]);
                break;

			// PDF417 only: number of data columns
			case MWB_RESULT_FT_PDF_COLUMNS:
                result->setPdfColumns(*(int*)&buffer[contentPos]);
                break;

			// PDF417 only: Is Truncated flag
			case MWB_RESULT_FT_PDF_TRUNCATED:
                result->setPdfTruncated((*(int*)&buffer[contentPos]) > 0);
                break;

			// PDF417 only: ECC Level
			case MWB_RESULT_FT_PDF_ECLEVEL:
                result->setPdfECLevel(*(int*)&buffer[contentPos]);
                break;

			// PDF417 only: Codewords
			case MWB_RESULT_FT_PDF_CODEWORDS:
                result->setPdfCodewords(&buffer[contentPos]);
                break;

            default:
				cout << "Unknown results field |" << fieldName << "|\n";
                break;
            }
            currentPos += (fieldNameLength + fieldContentLength + 4);
        }
        results.push_back(result);
    }
} // MWResults::MWResults
