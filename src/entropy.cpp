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
#include <math.h>
#include <map>

using namespace zlibcomplete;

extern "C"
double ComputeShannonEntropy (FILE* f, long long flen, int useCompression)
{
	std::map<unsigned char, int> dict;
	unsigned char buffer[4 * 4096];
	size_t count, i;
	double freq, ret = 0.0;
	GZipCompressor compressor(9, auto_flush);
	long long compressedLen = 0;

	if (!useCompression)
	{
		for (i = 0; i < 256; i++)
			dict[(unsigned char) i] = 0;
	}

	do
	{
		count = fread (buffer, 1, sizeof (buffer), f);
		if (count == 0)
			break;

		if (useCompression)
		{
			std::string input((char*) buffer, count);
			std::string output = compressor.compress(input);
			compressedLen += (long long) output.size();
		}
		else
		{
			for (i = 0; i < count; i++)
				dict[buffer[i]] += 1;
		}
	}
	while (true);

	if (useCompression)
	{
		std::string output = compressor.finish();
		compressedLen += (long long) output.size();

		//remove GZip header (1à bytes) and footer (8 bytes) size
		compressedLen -= 18;

		if (compressedLen > flen)
			compressedLen = flen;

		ret = ((double) compressedLen / (double) flen) * 8.0;
	}
	else
	{
		for (i = 0; i < 256; i++)
		{
			if (dict[(unsigned char) i] == 0)
				continue;
			freq = (double) dict[(unsigned char) i] / (double) (flen);
			ret -= freq * (log(freq) / log(2.0));
		}
	}

	return ret;
}
