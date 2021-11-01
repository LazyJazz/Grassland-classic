#include "Bitmap.h"

namespace Grassland
{
	namespace Graphics
	{
		BitmapPixel::BitmapPixel()
		{
			r = g = b = 0;
		}

		BitmapPixel::BitmapPixel(uint8_t _r, uint8_t _g, uint8_t _b)
		{
			r = _r;
			g = _g;
			b = _b;
		}

		Bitmap::Bitmap()
		{
			width = height = 0;
			pixels = NULL;
		}

		Bitmap::Bitmap(uint32_t width, uint32_t height)
		{
			this->width = width;
			this->height = height;
			pixels = new BitmapPixel[width * height]();
		}

		Bitmap::~Bitmap()
		{
			if (pixels)
				delete[] pixels,
				pixels = NULL;
		}

		void Bitmap::Resize(uint32_t width, uint32_t height)
		{
			BitmapPixel* newBuffer = new BitmapPixel[width * height]();
			if (pixels)
			{
				for (uint32_t x = 0; x < std::min(width, this->width); x++)
					for (uint32_t y = 0; y < std::min(height, this->height); y++)
						newBuffer[y * width + x] = pixels[y * this->width + x];
				delete[] pixels;
			}
			pixels = newBuffer;
			this->width = width;
			this->height = height;
		}

		uint32_t Bitmap::GetWidth() const { return width; }

		uint32_t Bitmap::GetHeight() const { return height; }

		BitmapPixel* Bitmap::GetColorPtr() const { return pixels; };

		BitmapPixel& Bitmap::operator () (uint32_t x, uint32_t y)
		{
			return pixels[y * width + x];
		}

		BitmapPixel Bitmap::GetColor(uint32_t x, uint32_t y) const
		{
			if (x >= width || y >= height || !pixels) return BitmapPixel(0, 0, 0);
			else return pixels[y * width + x];
		}

		void Bitmap::SetColor(uint32_t x, uint32_t y, BitmapPixel color)
		{
			if (x >= width || y >= height || !pixels) return;
			pixels[y * width + x] = color;
		}

		void Bitmap::Clear(BitmapPixel bkgroundColor)
		{
			for (uint32_t p = 0; p < width * height; p++)
				pixels[p] = bkgroundColor;
		}


		int Bitmap::SaveToFile(const char* _path) const
		{
			FILE* pFile;
#ifdef _WIN32
			fopen_s(&pFile, _path, "w");
#else
			pFile = fopen(_path, "w");
#endif
			if (!pFile)
				return 1, fprintf(stderr, "ERROR: Failed to open file ¡®%s¡¯\n", _path);
			fprintf(pFile, "%u %u\n", width, height);
			for (uint32_t p = 0; p < width * height; p++)
				fprintf(pFile, "%u %u %u\n", pixels[p].r, pixels[p].g, pixels[p].b);
			fclose(pFile);
			return 0;
		}

		int Bitmap::LoadFromFile(const char* _path)
		{
			FILE* pFile;
#ifdef _WIN32
			fopen_s(&pFile, _path, "r");
#else
			pFile = fopen(_path, "r");
#endif
			if (!pFile)
				return 1, fprintf(stderr, "ERROR: Failed to open file ¡®%s¡¯\n", _path);
			uint32_t w, h;
#ifdef _WIN32
			if (fscanf_s(pFile, "%u%u", &w, &h) != 2)
#else
			if (fscanf(pFile, "%u%u", &w, &h) != 2)
#endif
				return 1, fprintf(stderr, "ERROR: Incorrect format\n"), fclose(pFile);
			Resize(w, h);
			uint32_t r, g, b;
			for (uint32_t p = 0; p < width * height; p++)
#ifdef _WIN32
				if (fscanf_s(pFile, "%u%u%u", &r, &g, &b) != 3)
#else
				if (fscanf(pFile, "%u%u%u", &r, &g, &b) != 3)
#endif
					return 1, fprintf(stderr, "ERROR: Incorrect format\n"), fclose(pFile);
				else
					pixels[p] = BitmapPixel(r, g, b);
			fclose(pFile);
			return 0;
		}

		int Bitmap::LoadBitmapFromFile(const char* _path)
		{
			printf("Loading bitmap: '%s'\n", _path);
			FILE* pFile;
#ifdef _WIN32
			fopen_s(&pFile, _path, "rb");
#else
			pFile = fopen(_path, "rb");
#endif
			if (!pFile)
			{
				fprintf(stderr, "ERROR: Failed to open file '%s'\n", _path);
				return 1;
			}
			BITMAPFILEHEADER fileheader = {};
			BITMAPINFOHEADER infoheader = {};
			int res;
			res = fread(&fileheader, sizeof(fileheader), 1, pFile);
			res = fread(&infoheader, sizeof(infoheader), 1, pFile);
			bool wrong_format = false;
			if (fileheader.bfType != 0x4d42) wrong_format = true;
			if (fileheader.bfReserved1 != 0)wrong_format = true;
			if (fileheader.bfReserved2 != 0)wrong_format = true;
			if (fileheader.bfOffBits != 0x36)wrong_format = true;
			if (infoheader.biSize != 0x28)wrong_format = true;
			if (infoheader.biPlanes != 1)wrong_format = true;
			if (infoheader.biBitCount != 0x18)wrong_format = true;
			if (wrong_format)
			{
				fprintf(stderr, "ERROR: Wrong format!!!\n");
				return 1;
			}
			uint32_t rw = infoheader.biWidth, rh = infoheader.biHeight;
			Resize(rw, rh);
			uint32_t widthBytes = ((rw * 3 + 3) / 4) * 4;
			for (int y = rh - 1; y >= 0; y--)
			{
				res = fread(pixels + y * width, sizeof(BitmapPixel), width, pFile);
				for (int i = 0; i < widthBytes - 3 * width; i++)
					fgetc(pFile);
			}
			fclose(pFile);
			return 0;
		}

		void Bitmap::SaveBitmapToFile(const char* _path) const
		{
			printf("Saving image: [%u, %u], Path: '%s'\n", width, height, _path);
			if (!width || !height || !pixels) return;
			FILE* pFile;
#ifdef _WIN32
			fopen_s(&pFile, _path, "wb");
#else
			pFile = fopen(_path, "wb");
#endif
			if (!pFile)
			{
				fprintf(stderr, "ERROR: Failed to open file '%s'\n", _path);
				return;
			}

			BITMAPFILEHEADER fileheader = {};
			BITMAPINFOHEADER infoheader = {};

			uint32_t widthBytes = ((width * 3 + 3) / 4) * 4;

			fileheader.bfType = 0x4d42;
			fileheader.bfSize = height * widthBytes + 54;
			fileheader.bfReserved1 = 0;
			fileheader.bfReserved2 = 0;
			fileheader.bfOffBits = 0x36;
			infoheader.biSize = 0x28;
			infoheader.biWidth = width;
			infoheader.biHeight = height;
			infoheader.biPlanes = 1;
			infoheader.biBitCount = 0x18;
			fwrite(&fileheader, sizeof(fileheader), 1, pFile);
			fwrite(&infoheader, sizeof(infoheader), 1, pFile);
			//for (int y = height - 1; y >= 0; y--)
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++) {
					fputc(pixels[y * width + x].b, pFile);
					fputc(pixels[y * width + x].g, pFile);
					fputc(pixels[y * width + x].r, pFile);
				}//*/
				//fwrite(pixels + y * width, sizeof(BitmapPixel), width, pFile);
				for (int i = 0; i < widthBytes - 3 * width; i++)
					fputc(0, pFile);
			}
			fclose(pFile);
		}
	}
}