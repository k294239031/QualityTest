#pragma once
#include <stdio.h>
#include <tchar.h>
#include <SDKDDKVer.h>
#include <iostream>

#define MAKEUS(a, b)    ((unsigned short) ( ((unsigned short)(a))<<8 | ((unsigned short)(b)) ))
#define MAKEUI(a,b,c,d) ((unsigned int) ( ((unsigned int)(a)) << 24 | ((unsigned int)(b)) << 16 | ((unsigned int)(c)) << 8 | ((unsigned int)(d)) ))

#define M_DATA  0x00
#define M_SOF0  0xc0
#define M_DHT   0xc4
#define M_SOI   0xd8
#define M_EOI   0xd9
#define M_SOS   0xda
#define M_DQT   0xdb
#define M_DNL   0xdc
#define M_DRI   0xdd
#define M_APP0  0xe0
#define M_APPF  0xef
#define M_COM   0xfe

#include <stdlib.h>
#include <string.h>

int GetPNGWidthHeight(const char* path, unsigned int* punWidth, unsigned int* punHeight)
{
    int Finished = 0;
    unsigned char uc[4];
    FILE* pfRead;

    *punWidth = 0;
    *punHeight = 0;

    if (fopen_s(&pfRead, path, "rb") != 0)
    {
        printf("[GetPNGWidthHeight]:can't open file:%s\n", path);
        return -1;
    }

    for (int i = 0; i < 4; i++)
        fread(&uc[i], sizeof(unsigned char), 1, pfRead);
    if (MAKEUI(uc[0], uc[1], uc[2], uc[3]) != 0x89504e47)
        printf("[GetPNGWidthHeight]:png format error\n", path);
    for (int i = 0; i < 4; i++)
        fread(&uc[i], sizeof(unsigned char), 1, pfRead);
    if (MAKEUI(uc[0], uc[1], uc[2], uc[3]) != 0x0d0a1a0a)
        printf("[GetPNGWidthHeight]:png format error\n", path);

    fseek(pfRead, 16, SEEK_SET);
    for (int i = 0; i < 4; i++)
        fread(&uc[i], sizeof(unsigned char), 1, pfRead);
    *punWidth = MAKEUI(uc[0], uc[1], uc[2], uc[3]);
    for (int i = 0; i < 4; i++)
        fread(&uc[i], sizeof(unsigned char), 1, pfRead);
    *punHeight = MAKEUI(uc[0], uc[1], uc[2], uc[3]);

}

int IsJPEGDamage(const char* path)
{
	unsigned char id;
	FILE* pfRead;
    int len = strlen(path);
    if (strncmp(path + len - 3, "jpg", 3))
    {
        printf("[IsJPEGDamage]:is not JPEG file:%s\n", path);
        return 1;
    }

	if (fopen_s(&pfRead, path, "rb") != 0)
	{
		printf("[IsJPEGDamage]:can't open file:%s\n", path);
		return 2;
	}

	if (fread(&id, sizeof(char), 1, pfRead) && id == 0xff 
        && fread(&id, sizeof(char), 1, pfRead) && id == 0xd8)
	{
        fseek(pfRead, -2*sizeof(char), 2);
        if (fread(&id, sizeof(char), 1, pfRead) && id == 0xff 
            && fread(&id, sizeof(char), 1, pfRead) && id == 0xd9)
            return 0;
        else
        {
            printf("[IsJPEGDamage]:EOI can't found:%s\n", path);
            return 4;
        }
	}
    else
    {
        printf("[IsJPEGDamage]:SOI can't found:%s\n", path);
        return 3;
    }
}

int main(int argc, char* argv[])
{
    const char* path = "1.jpg";
    int a;
    a=IsJPEGDamage(path);
    std::cout << "a=" << a << std::endl;
    
    system("pause");
    return 0;
}
