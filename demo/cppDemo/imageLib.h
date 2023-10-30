#pragma once
/**
 * File    imageLib.h
 * Brief   Wrapper/helper functions to load images and convert to grayscale
 *
 * Notice  Copyright (C) Cognex Corporation
 *
 */

#include "MWResult.h"

using namespace Cognex;

namespace CognexImageLib
{
	class GrayScale
	{

	public:
		int getHeight(void);
		int getWidth(void);
        int getBitPlanes(void);
		int getSize(void);
		uint8_t *getGrayScale(void);

		GrayScale(const char *fileName);
		~GrayScale(void);

	private:
		int height;
		int width;
        int bitPlanes;
		uint8_t *grayScale;
	};

}

