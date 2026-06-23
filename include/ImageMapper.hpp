#pragma once
#include <iostream>
#include "Agent.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool loadTerrainMap(const char *filepath, uint8_t *terrainGrid, int expectedWidth, int expectedHeight)
{
    int imgWidth, imgHeight, channels;

    // Force the image to load as 1 channel (grayscale)
    unsigned char *data = stbi_load(filepath, &imgWidth, &imgHeight, &channels, 1);

    if (!data || imgWidth != expectedWidth || imgHeight != expectedHeight)
    {
        std::cerr << "Failed to load map or dimensions mismatch!" << std::endl;
        return false;
    }

    for (int y = 0; y < expectedHeight; ++y)
    {
        for (int x = 0; x < expectedWidth; ++x)
        {
            int pixelValue = data[(expectedHeight - 1 - y) * expectedWidth + x];

            if (pixelValue < 50)
                terrainGrid[y * expectedWidth + x] = OBSTACLE;
            else
                terrainGrid[y * expectedWidth + x] = AIR;
        }
    }

    stbi_image_free(data);
    return true;
}