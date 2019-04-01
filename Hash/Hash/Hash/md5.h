#ifndef MD5_H
#define MD5_H

#define _CRT_SECURE_NO_WARNINGS

#include <string>
#include <cstring>

typedef unsigned int MD5_u32;

typedef struct {
	MD5_u32 lo, hi;
	MD5_u32 a, b, c, d;
	unsigned char buffer[64];
	MD5_u32 block[16];
} MD5_CTX;

void MD5_Init(MD5_CTX *ctx);
void MD5_Update(MD5_CTX *ctx, const void *data, unsigned long size);
void MD5_Final(unsigned char *result, MD5_CTX *ctx);

std::string md5(std::string dat);
std::string md5(const void* dat, size_t len);
std::string md5file(const char* filename);
std::string md5file(std::FILE* file);
std::string md5sum6(std::string dat);
std::string md5sum6(const void* dat, size_t len);

#endif // end of MD5_H
