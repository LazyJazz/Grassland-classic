#include "Image.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
#ifndef _WINDOWS_
#pragma pack (1)
			struct BITMAPFILEHEADER {
				uint16_t    bfType;
				uint32_t   bfSize;
				uint16_t    bfReserved1;
				uint16_t    bfReserved2;
				uint32_t   bfOffBits;
			};

			struct BITMAPINFOHEADER {
				uint32_t      biSize;
				int32_t       biWidth;
				int32_t       biHeight;
				uint16_t       biPlanes;
				uint16_t       biBitCount;
				uint32_t      biCompression;
				uint32_t      biSizeImage;
				int32_t       biXPelsPerMeter;
				int32_t       biYPelsPerMeter;
				uint32_t      biClrUsed;
				uint32_t      biClrImportant;
			};
#pragma pack ()
#endif


			Image::Image()
			{
				width = height = 0;
				pColorBuffer = nullptr;
			}
			Image::Image(int32_t width, int32_t height)
			{
				this->width = width;
				this->height = height;
				this->pColorBuffer = new Color[width * height];
			}
			void Image::Resize(int32_t width, int32_t height)
			{
				if (this->width == width && this->height == height)
					return;
				int32_t
					copy_width = std::min(this->width, width), 
					copy_height = std::min(this->height, height);
				Color* pColorBuffer_new = new Color[width * height];
				for (int y = 0; y < copy_height; y++)
					memcpy(
						pColorBuffer_new + (y * width),
						pColorBuffer + (y * this->width),
						sizeof(Color) * copy_width
					);
				delete[] this->pColorBuffer;
				this->pColorBuffer = pColorBuffer_new;
				this->width = width;
				this->height = height;
			}
			int32_t Image::GetHeight() const
			{
				return height;
			}
			int32_t Image::GetWidth() const
			{
				return width;
			}
			Color& Image::operator()(int32_t x, int32_t y)
			{
				return pColorBuffer[y * width + x];
			}
			const Color& Image::operator()(int32_t x, int32_t y) const
			{
				return pColorBuffer[y * width + x];
			}
			uint32_t Image::LoadBMP(const char* _path)
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
					return GRL_TRUE;
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
					return GRL_TRUE;
				}
				int32_t rw = infoheader.biWidth, rh = infoheader.biHeight;
				Resize(rw, rh);
				uint32_t widthBytes = ((rw * 3 + 3) / 4) * 4;
				uint8_t *__ReadBuf = new uint8_t [width*3];
				for (int y = rh - 1; y >= 0; y--)
				{
					res = fread(__ReadBuf, 3, width, pFile);
					for (int x = 0; x < width; x++)
						pColorBuffer[y * width + x] = 
						Color(
							__ReadBuf[x * 3 + 2] * (1.0f / 255.0f),
							__ReadBuf[x * 3 + 1] * (1.0f / 255.0f),
							__ReadBuf[x * 3] * (1.0f / 255.0f)
						);
					for (int i = 0; i < widthBytes - 3 * width; i++)
						fgetc(pFile);
				}
				delete[] __ReadBuf;
				fclose(pFile);
				return GRL_FALSE;
			}

			uint32_t Image::StoreBMP(const char* _path) const
			{
				printf("Saving image: [%u, %u], Path: '%s'\n", width, height, _path);
				if (!width || !height || !pColorBuffer) return GRL_TRUE;
				FILE* pFile;
#ifdef _WIN32
				fopen_s(&pFile, _path, "wb");
#else
				pFile = fopen(_path, "wb");
#endif
				if (!pFile)
				{
					fprintf(stderr, "ERROR: Failed to open file '%s'\n", _path);
					return GRL_TRUE;
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
				for (int y = height - 1; y >= 0; y--)
				{
					for (int x = 0; x < width; x++) {
						Color c = pColorBuffer[y * width + x];
						fputc(Color_Float_to_Int(c.b), pFile);
						fputc(Color_Float_to_Int(c.g), pFile);
						fputc(Color_Float_to_Int(c.r), pFile);
					}//*/
					//fwrite(pixels + y * width, sizeof(BitmapPixel), width, pFile);
					for (int i = 0; i < widthBytes - 3 * width; i++)
						fputc(0, pFile);
				}
				fclose(pFile);
				return GRL_FALSE;
			}

			void Image::Clear(Color clear_color)
			{
				for (int i = 0; i < width * height; i++)
					pColorBuffer[i] = clear_color;
			}

			void Image::Release()
			{
				if (pColorBuffer) delete[] pColorBuffer;
				pColorBuffer = nullptr;
				width = height = 0;
			}
			Color* Image::GetImageBuffer()
			{
				return pColorBuffer;
			}

			const Color* Image::GetImageBuffer() const
			{
				return pColorBuffer;
			}
		}
	}

	class GRLCImage: public GRLIImage
	{
	public:
		GRLCImage();
		GRLCImage(int32_t width, int32_t height);
		~GRLCImage();

		virtual GRL_RESULT Resize(int32_t width, int32_t height) const override;
		virtual int32_t GetHeight() const override;
		virtual int32_t GetWidth() const override;
		virtual void Clear(GRLColor color) const override;
		virtual GRL_RESULT GetImageBuffer(GRLColor ** ppBuffer) const override;
		virtual GRL_RESULT LoadBMP(const char* __bmp_file_path) const override;
		virtual GRL_RESULT StoreBMP(const char* __bmp_file_path) const override;
		virtual GRL_RESULT AddRef() override;
		virtual GRL_RESULT Release() override;
		virtual GRL_RESULT QueryInterface(GRLUUID uuid, void** ppObject) override;
	private:
		Graphics::Util::Image * pImage;
		int32_t __Ref_Cnter;
	};

	GRL_RESULT GRLCreateImage(int32_t width, int32_t height, GRLIImage** ppImage)
	{
		if (width == 0 && height == 0)
		{
			*ppImage = new GRLCImage;
			return GRL_FALSE;
		}
		if (width <= 0 || height <= 0)
		{
			*ppImage = nullptr;
			return GRL_TRUE;
		}
		*ppImage = new GRLCImage(width, height);
		return GRL_FALSE;
	}

	GRLCImage::GRLCImage()
	{
		pImage = new Graphics::Util::Image;
		__Ref_Cnter = 1;
	}

	GRLCImage::GRLCImage(int32_t width, int32_t height)
	{
		pImage = new Graphics::Util::Image(width, height);
		__Ref_Cnter = 1;
	}
	GRLCImage::~GRLCImage()
	{
		delete pImage;
	}
	GRL_RESULT GRLCImage::AddRef()
	{
		__Ref_Cnter++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCImage::Release()
	{
		__Ref_Cnter--;
		if (!__Ref_Cnter)
		{
			delete this;
		}
		return GRL_FALSE;
	}
	GRL_RESULT GRLCImage::QueryInterface(GRLUUID uuid, void** ppObject)
	{
		if (uuid == GRLID_Image) *ppObject = (GRLIImage*)this;
		else if (uuid == GRLID_IBase) *ppObject = (GRLIBase*)this;
		else return GRL_TRUE;
		AddRef();
		return GRL_FALSE;
	}
	GRL_RESULT GRLCImage::Resize(int32_t width, int32_t height) const
	{
		if (width <= 0 || height <= 0)
		{
			return GRL_TRUE;
		}
		pImage->Resize(width, height);
		return GRL_FALSE;
	}
	int32_t GRLCImage::GetHeight() const
	{
		return pImage->GetHeight();
	}
	int32_t GRLCImage::GetWidth() const
	{
		return pImage->GetWidth();
	}
	void GRLCImage::Clear(GRLColor color) const
	{
		pImage->Clear(color);
	}
	GRL_RESULT GRLCImage::GetImageBuffer(GRLColor** ppBuffer) const
	{
		*ppBuffer = pImage->GetImageBuffer();
		if (*ppBuffer)
		{
			return GRL_FALSE;
		}
		return GRL_TRUE;
	}

	GRL_RESULT GRLCImage::LoadBMP(const char* __bmp_file_path) const
	{
		return pImage->LoadBMP(__bmp_file_path);
	}
	GRL_RESULT GRLCImage::StoreBMP(const char* __bmp_file_path) const
	{
		return pImage->StoreBMP(__bmp_file_path);
	}
}