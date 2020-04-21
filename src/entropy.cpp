/** 

EntropyCal - A tool to compute an estimation of a file entropy

Copyright (c) 2020 Mounir IDRASSI (mounir@idrix.fr)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

**/

#include "entropy.h"
#include <zlc/zlibcomplete.hpp>
#include <LzmaEnc.h>
#include <math.h>
#include <stdlib.h>
#include <map>
#include <vector>
#include <algorithm>

using namespace zlibcomplete;

static void* AllocForLzma(ISzAllocPtr p, size_t size) { return malloc(size); }
static void FreeForLzma(ISzAllocPtr p, void* address) { free(address); }
static ISzAlloc SzAllocForLzma = { &AllocForLzma, &FreeForLzma };

typedef struct
{
	ISeqInStream SeqInStream;
	FILE* f;
	long long* processedBytes;
} VectorInStream;

SRes VectorInStream_Read(const ISeqInStream* p, void* buf, size_t* size)
{
	VectorInStream* ctx = (VectorInStream*)p;
	*size = fread(buf, 1, *size, ctx->f);
	*(ctx->processedBytes) += *size;
	return SZ_OK;
}

typedef struct
{
	ISeqOutStream SeqOutStream;
	long long* compressedLen;
} VectorOutStream;

size_t VectorOutStream_Write(const ISeqOutStream* p, const void* buf, size_t size)
{
	VectorOutStream* ctx = (VectorOutStream*)p;
	*(ctx->compressedLen) += size;
	return size;
}

static long long LzmaCompressedSize(
	FILE *f,
	long long& processedBytes)
{
	CLzmaEncHandle enc = LzmaEnc_Create(&SzAllocForLzma);
	CLzmaEncProps props;
	LzmaEncProps_Init(&props);
	props.writeEndMark = 0; // 0 or 1
	props.level = 9;

	long long compressedLen = 0;
	processedBytes = 0;

	SRes res = LzmaEnc_SetProps(enc, &props);

	unsigned propsSize = LZMA_PROPS_SIZE;

	VectorInStream inStream = { &VectorInStream_Read, f, &processedBytes };
	VectorOutStream outStream = { &VectorOutStream_Write, &compressedLen };

	res = LzmaEnc_Encode(enc,
		(ISeqOutStream*)&outStream, (ISeqInStream*)&inStream,
		0, &SzAllocForLzma, &SzAllocForLzma);

	LzmaEnc_Destroy(enc, &SzAllocForLzma, &SzAllocForLzma);

	return compressedLen;
}

extern "C"
double ComputeShannonEntropy(FILE * f, eEntropyMethod meth)
{
	long long curPos = _ftelli64(f),processedBytes = 0;
	if (curPos == -1)
		return 0;

	if (meth == ENT_METH_MIN_ALL)
	{
		double e1 = ComputeShannonEntropy(f, ENT_METH_FREQ);
		_fseeki64(f, curPos, SEEK_SET);
		double e2 = ComputeShannonEntropy(f, ENT_METH_GZIP);
		_fseeki64(f, curPos, SEEK_SET);
		double e3 = ComputeShannonEntropy(f, ENT_METH_LZMA);

		return std::min(std::min(e1, e2), e3);
	}
	else if (meth == ENT_METH_LZMA)
	{
		long long compressedLen = LzmaCompressedSize(f, processedBytes);
		if (compressedLen > processedBytes)
			compressedLen = processedBytes;

		return ((double)compressedLen / (double)processedBytes) * 8.0;
	}
	else
	{
		std::map<unsigned char, int> dict;
		unsigned char buffer[4 * 4096];
		size_t count, i;
		double freq, ret = 0.0;
		GZipCompressor compressor(9, auto_flush);
		long long compressedLen = 0;

		if (meth == ENT_METH_FREQ)
		{
			for (i = 0; i < 256; i++)
				dict[(unsigned char)i] = 0;
		}


		do
		{
			count = fread(buffer, 1, sizeof(buffer), f);
			if (count == 0)
				break;

			processedBytes += count;

			if (meth == ENT_METH_GZIP)
			{
				std::string input((char*)buffer, count);
				std::string output = compressor.compress(input);
				compressedLen += (long long)output.size();
			}
			else
			{
				for (i = 0; i < count; i++)
					dict[buffer[i]] += 1;
			}
		} while (true);

		if (meth == ENT_METH_FREQ)
		{
			for (i = 0; i < 256; i++)
			{
				if (dict[(unsigned char)i] == 0)
					continue;
				freq = (double)dict[(unsigned char)i] / (double)(processedBytes);
				ret -= freq * (log(freq) / log(2.0));
			}
		}
		else
		{
			std::string output = compressor.finish();
			compressedLen += (long long)output.size();

			//remove GZip header (10 bytes) and footer (8 bytes) size
			compressedLen -= 18;

			if (compressedLen > processedBytes)
				compressedLen = processedBytes;

			ret = ((double)compressedLen / (double)processedBytes) * 8.0;
		}

		return ret;
	}	
}
