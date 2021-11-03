#pragma once
#include "Color.h"
#include "../../GrasslandDecl.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
			enum GRL_COLOR_FORMAT
			{
				GRL_COLOR_FORMAT_R = 0,
				GRL_COLOR_FORMAT_RGB = 0,
				GRL_COLOR_FORMAT_RGBA = 0
			};

			class Image
			{
			public:
				Image();
				Image(int32_t width, int32_t height);
				void Resize(int32_t width, int32_t height);
				int32_t GetHeight() const;
				int32_t GetWidth() const;
				Color& operator () (int32_t x, int32_t y);
				const Color& operator () (int32_t x, int32_t y) const;
				uint32_t LoadBMP(const char * _path);
				uint32_t StoreBMP(const char * _path) const;
				void Clear(Color clear_color);
				void Release();
				Color* GetImageBuffer();
				const Color* GetImageBuffer() const;
			private:
				Color* pColorBuffer;
				int32_t width, height;
			};
		}
	}

	typedef ::Grassland::Graphics::Util::Color GRLColor;

	class GRLIImage : public GRLIBase
	{
	public:
		virtual GRL_RESULT Resize(int32_t width, int32_t height) const = 0;
		virtual int32_t GetHeight() const = 0;
		virtual int32_t GetWidth() const = 0;
		virtual void Clear() const = 0;
		virtual GRL_RESULT Map(GRLColor * pfBuffer) const = 0;
		virtual GRL_RESULT Unmap() const = 0;
		virtual GRL_RESULT LoadBitmap(const char * __bmp_file_path) const = 0;
		virtual GRL_RESULT StoreBMP(const char* __bmp_file_path) const = 0;
	};
	/*
	* @param width
	* Image width.
	* If width and height are both zero, image won't initialize.
	* 
	* @param height
	* Image Height.
	* If width and height are both zero, image won't initialize.
	*
	* @param ppImage
	* To receive the image object that created.
	* 
	* @return If succeed, return GRL_FALSE, Otherwise, return GRL_TRUE. 
	*/
	GRL_RESULT GRLCreateImage(int32_t width, int32_t height, GRLIImage** ppImage);
}