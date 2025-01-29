#pragma once


bool BGRtoRGB(unsigned char *&data, const unsigned int dataLength);
bool BGRtoBGRA(unsigned char *&data, const unsigned int dataLength);

bool LoadTga(const char *filename, unsigned char *&data, unsigned short &width, unsigned short &height);