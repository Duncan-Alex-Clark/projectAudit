/**
 * File    MWResult.h
 * Brief   Class to encapsulate the decoder library's results as a C++ class
 *
 * Notice  Copyright (C) Cognex Corporation
 *
 */
#ifndef MWRESULT_H
#define MWRESULT_H

#include <vector>
#include <string>
#include <string.h>

#include "BarcodeScanner.h"

using namespace std;

namespace Cognex
{
	class PointF
	{
	public:
		PointF();
		float getX(void) { return x; }
		void setX(float x) { this->x = x; }
		float getY(void) { return y; }
		void setY(float y) { this->y = y; }

	private:
		float x;
		float y;

	}; // class PointF

	class MWLocation
	{

	public:
		MWLocation(void);
		~MWLocation(void);

		MWLocation(float *points);

		PointF * getPoint1(void) { return p1; }
		PointF * getPoint2(void) { return p2; }
		PointF * getPoint3(void) { return p3; }
		PointF * getPoint4(void) { return p4; }

		PointF * getPoints(void) { return points; }

	private:
		PointF *p1;
		PointF *p2;
		PointF *p3;
		PointF *p4;

		PointF *points;

	}; // class MWLocation


	class MWResult
	{
	public:
		MWResult();
		~MWResult();
		//    MWResult(uint8_t *);
		string getText(void) { return text; }
		void setText(string txt) { text = txt; }

		string getTextEncoding(void) { return textEncoding; }
		void setTextEncoding(string _textEncoding) { textEncoding = _textEncoding; }

		char * getBytes(void) { return bytes; }
		void setBytes(char * array);

		int getBytesLength(void) { return bytesLength; }
		void setBytesLength(int len) { bytesLength = len; }

		int getType(void) { return type; }
		void setType(int type) { this->type = type; }

		string getTypeName(void) { return typeName; }
		void setTypeName(int type);

		int getSubType(void) { return subtype; }
		void setSybType(int sybtype) { this->subtype = subtype; }

		int getImageWidth(void) { return imageWidth; }
		void setImageWidth(int width) { imageWidth = width; }

		int getImageHeight(void) { return imageHeight; }
		void setImageHeight(int height) { imageHeight = height; }

		bool isGs1(void) { return isGS1; }
		void setGs1(bool t) { isGS1 = t; }

		void setLocationPoints(MWLocation *points) { locationPoints = points; }
		MWLocation *getLocationPoints(void) { return locationPoints; }

		char * getParserInput(void) { return parserInput; }
		void setParserInput(char *array, int len);

		int getModulesCountX(void) { return modulesCountX; }
		void setModulesCountX(int _modulesCountX) { modulesCountX = _modulesCountX; }

		int getModulesCountY(void) { return modulesCountY; }
		void setModulesCountY(int _modulesCountY) { modulesCountY = _modulesCountY; }

		float getModuleSizeX(void) { return moduleSizeX; }
		void setModuleSizeX(float _moduleSizeX) { moduleSizeX = _moduleSizeX; }

		float getModuleSizeY(void) { return moduleSizeY; }
		void setModuleSizeY(float _moduleSizeY) { moduleSizeY = _moduleSizeY; }

		float getSkew(void) { return skew; }
		void setSkew(float _skew) { skew = _skew; }

		bool isKanji(void) { return iskanji; }
		void setKanji(bool _iskanji) { iskanji = _iskanji; }

		float getBarcodeWidth(void) { return barcodeWidth; }
		void setBarcodeWidth(float _barcodeWidth) { barcodeWidth = _barcodeWidth; }

		float getBarcodeHeight(void) { return barcodeHeight; }
		void setBarcodeHeight(float _barcodeHeight) { barcodeHeight = _barcodeHeight; }

		int getPdfRows(void) { return pdfRows; }
		void setPdfRows(int _pdfRows) { pdfRows = _pdfRows; }

		int getPdfColumns(void) { return pdfColumns; }
		void setPdfColumns(int _pdfColumns) { pdfColumns = _pdfColumns; }

		bool PdfIsTruncated(void) { return pdfIsTruncated; }
		void setPdfTruncated(bool _pdfIsTruncated) { pdfIsTruncated = _pdfIsTruncated; }

		int getPdfECLevel(void) { return pdfECLevel; }
		void setPdfECLevel(int _pdfECLevel) { pdfECLevel = _pdfECLevel; }

		int *getPdfCodewords(void) { return pdfCodewords; }
		int setPdfCodewords(uint8_t *pdfCodewords);

	private:
		string text;
		string textEncoding;
		string typeName;
		char *bytes;
		int bytesLength;
		int type;
		int subtype;
		int imageWidth;
		int imageHeight;
		bool isGS1;
		MWLocation *locationPoints;
		char *parserInput;

		int modulesCountX;
		int modulesCountY;
		float moduleSizeX;
		float moduleSizeY;
		float skew;
		bool iskanji;

		float barcodeWidth;
		float barcodeHeight;

		int pdfRows;
		int pdfColumns;
		bool pdfIsTruncated;
		int pdfECLevel;
		int *pdfCodewords;

	}; // clase MWResult

	class MWResults
	{
	public:
		MWResults();
		~MWResults();
		MWResults(uint8_t *buffer);
		int getVersion(void) { return version; }
		void setVersion(int version) { this->version = version; }

		int getCount(void) { return count; }
		void setCount(int count) { this->count = count; }

		MWResult *getResult(int index) { return results.at(index); }

	private:
		int version;
		int count;
		vector<MWResult *> results;

	}; // class MWResults
}


#endif // MWRESULT_H
