#include <iostream>
#include <cstdio>
#include <utility>
#include "TgaLoader.h"
#include <cstring>

constexpr int SIGNATURE_SIZE = 12;
constexpr int BITS_PER_BYTE = 8;

bool BGRtoRGB(unsigned char *&data, const unsigned int dataLength)
{
    constexpr int RGB_LENGTH = 3;

    for (unsigned int i = 0; i < dataLength; i += RGB_LENGTH)
    {
        std::swap(data[i], data[i + 2]);
    }
    return true;
}

bool BGRtoBGRA(unsigned char *&data, const unsigned int dataLength)
{
    constexpr int RGB_LENGTH = 3;

    for (unsigned int i = 0; i < dataLength; i += RGB_LENGTH)
    {
        std::swap(data[i], data[i + 2]);
    }
    return true;
}

bool LoadTga(const char *filename, unsigned char *&data, unsigned short &width, unsigned short &height)
{
    FILE *file = fopen(filename, "rb");

    if (!file)
    {
        std::cerr << "Could not open file" << filename << "." << std::endl;
        return false;
    }

    constexpr unsigned char CORRECT_SIGNATURE[] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    unsigned char signature[SIGNATURE_SIZE];

    fread(signature, 1, SIGNATURE_SIZE, file);

    if (memcmp(signature, CORRECT_SIGNATURE, SIGNATURE_SIZE) != 0)
    {
        std::cerr << filename << " is an invalid TGA file." << std::endl;
        fclose(file);
        return false;
    }

    unsigned int dataLength;
    unsigned short bpp; // bytes per pixel

    fread(&width, 1, sizeof(short), file);
    fread(&height, 1, sizeof(short), file);
    fread(&bpp, 1, sizeof(short), file);

    bpp /= BITS_PER_BYTE;

    dataLength = width * height * bpp;
    data = (unsigned char *)malloc(dataLength);
    if (!data)
    {
        std::cerr << "Could not allocate memory for the TGA image." << std::endl;
        fclose(file);
        return false;
    }
    fread(data, 1, dataLength, file);
    BGRtoRGB(data, dataLength);

    fclose(file);
    return true;
}
