#include "GLTexture.h"

#include "GraphicsOpenGL.h"

namespace Grassland {
namespace Graphics {
namespace OpenGL {

uint32_t __OpenGLFormatTranslate(GRL_OPENGL_TEXTURE_FORMAT format) {
  switch (format) {
    case GRL_OPENGL_TEXTURE_FORMAT_DEPTH:
      return GL_DEPTH_COMPONENT;
    case GRL_OPENGL_TEXTURE_FORMAT_RGB:
      return GL_RGB;
    case GRL_OPENGL_TEXTURE_FORMAT_RGBA:
      return GL_RGBA;
    case GRL_OPENGL_TEXTURE_FORMAT_R:
      return GL_RED;
    default:
      return 0;
  }
}

uint32_t __OpenGLInternalFormatTranslate(GRL_OPENGL_TEXTURE_FORMAT format) {
  switch (format) {
    case GRL_OPENGL_TEXTURE_FORMAT_DEPTH:
      return GL_DEPTH_COMPONENT32F;
    case GRL_OPENGL_TEXTURE_FORMAT_RGB:
      return GL_RGB32F;
    case GRL_OPENGL_TEXTURE_FORMAT_RGBA:
      return GL_RGBA32F;
    case GRL_OPENGL_TEXTURE_FORMAT_R:
      return GL_R32F;
    default:
      return 0;
  }
}

Texture::Texture(int32_t width,
                 int32_t height,
                 GRL_OPENGL_TEXTURE_FORMAT format,
                 void *data) {
  __texture_format = format;
  glGenTextures(1, &__texture_handle);
  glBindTexture(GL_TEXTURE_2D, __texture_handle);
  glTexImage2D(GL_TEXTURE_2D, 0, __OpenGLInternalFormatTranslate(format), width,
               height, 0, __OpenGLFormatTranslate(format), GL_FLOAT, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
  _width = width;
  _height = height;
}
Texture::~Texture() {
  glDeleteTextures(1, &__texture_handle);
}
int32_t Texture::GetWidth() const {
  return _width;
}
int32_t Texture::GetHeight() const {
  return _height;
}
uint32_t Texture::GetHandle() const {
  return __texture_handle;
}
GRL_OPENGL_TEXTURE_FORMAT Texture::GetFormat() const {
  return __texture_handle;
}
GRL_RESULT Texture::GetImage(Grassland::Graphics::Util::Image *img) const {
  img->Resize(_width, _height);
  if (__texture_format == GRL_OPENGL_TEXTURE_FORMAT_DEPTH) {
    float *depth_buffer = new float[_width * _height];
    GRLGLCall(glGetTextureImage(__texture_handle, 0, GL_DEPTH_COMPONENT,
                                GL_FLOAT, sizeof(float) * _width * _height,
                                depth_buffer));
    for (int i = 0; i < _width * _height; i++)
      img->GetImageBuffer()[i] = Grassland::Graphics::Util::Color(
          depth_buffer[i], depth_buffer[i], depth_buffer[i], 1.0f);
    delete[] depth_buffer;
  } else {
    GRLGLCall(glGetTextureImage(
        __texture_handle, 0, GL_RGBA, GL_FLOAT,
        sizeof(Grassland::Graphics::Util::Color) * _width * _height,
        img->GetImageBuffer()));
  }
  return GRL_FALSE;
}
void Texture::Resize(int32_t width, int32_t height) {
  glBindTexture(GL_TEXTURE_2D, __texture_handle);
  glTexImage2D(GL_TEXTURE_2D, 0,
               __OpenGLInternalFormatTranslate(__texture_format), width, height,
               0, __OpenGLFormatTranslate(__texture_format), GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);
  _width = width;
  _height = height;
}
}  // namespace OpenGL
}  // namespace Graphics

class GRLCOpenGLTexture : public GRLIOpenGLTexture {
 public:
  GRLCOpenGLTexture(Graphics::OpenGL::Texture *pTexture);
  ~GRLCOpenGLTexture();
  virtual int32_t GetWidth() const;
  virtual int32_t GetHeight() const;
  virtual uint32_t GetHandle() const;
  virtual GRL_OPENGL_TEXTURE_FORMAT GetFormat() const;
  virtual GRL_RESULT GetImage(GRLIImage *pImg) const;
  virtual GRL_RESULT BindTexture(int32_t slot) const;
  virtual GRL_RESULT Resize(int32_t width, int32_t height) const;
  virtual GRL_RESULT AddRef();
  virtual GRL_RESULT Release();

 private:
  Graphics::OpenGL::Texture *__texture;
  int32_t __Ref_Cnt;
};

GRL_RESULT GRLCreateOpenGLTexture(int32_t width,
                                  int32_t height,
                                  GRL_OPENGL_TEXTURE_FORMAT format,
                                  void *init_data,
                                  GRLIOpenGLTexture **ppTexture) {
  *ppTexture = new GRLCOpenGLTexture(
      new Graphics::OpenGL::Texture(width, height, format, init_data));
  return GRL_FALSE;
}

GRL_RESULT GRLCreateOpenGLTextureFromImage(GRLIImage *pImg,
                                           GRLIOpenGLTexture **ppTexture) {
  GRLColor *revColorBuffer = new GRLColor[pImg->GetWidth() * pImg->GetHeight()];
  GRLColor *ColorBuffer;
  pImg->GetImageBuffer(&ColorBuffer);
  for (int y = 0; y < pImg->GetHeight(); y++)
    memcpy(revColorBuffer + y * pImg->GetWidth(),
           ColorBuffer + (pImg->GetHeight() - 1 - y) * pImg->GetWidth(),
           pImg->GetWidth() * sizeof(GRLColor));
  GRL_RESULT res = GRLCreateOpenGLTexture(pImg->GetWidth(), pImg->GetHeight(),
                                          GRL_OPENGL_TEXTURE_FORMAT_RGBA,
                                          revColorBuffer, ppTexture);
  delete[] revColorBuffer;
  return res;
}

GRLCOpenGLTexture::GRLCOpenGLTexture(Graphics::OpenGL::Texture *pTexture) {
  __texture = pTexture;
  __Ref_Cnt = 1;
}

GRLCOpenGLTexture::~GRLCOpenGLTexture() {
  delete __texture;
}

int32_t GRLCOpenGLTexture::GetWidth() const {
  return __texture->GetWidth();
}

int32_t GRLCOpenGLTexture::GetHeight() const {
  return __texture->GetHeight();
}

uint32_t GRLCOpenGLTexture::GetHandle() const {
  return __texture->GetHandle();
}

GRL_OPENGL_TEXTURE_FORMAT GRLCOpenGLTexture::GetFormat() const {
  return __texture->GetFormat();
}

GRL_RESULT GRLCOpenGLTexture::GetImage(GRLIImage *_pImg) const {
  GRLPtr<GRLIImage> pImg(_pImg);
  if (pImg->GetWidth() != GetWidth() || pImg->GetHeight() != GetHeight())
    pImg->Resize(GetWidth(), GetHeight());
  uint64_t size = GetWidth() * GetHeight();
  GRLColor *pBuffer;
  pImg->GetImageBuffer(&pBuffer);
  if (GetFormat() == GRL_OPENGL_TEXTURE_FORMAT_DEPTH) {
    float *depth_buffer = new float[size];
    GRLGLCall(glGetTextureImage(GetHandle(), 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                                sizeof(float) * GetWidth() * GetHeight(),
                                depth_buffer));
    for (int y = 0; y < GetHeight(); y++)
      for (int x = 0; x < GetWidth(); x++)
        pBuffer[(GetHeight() - 1 - y) * GetWidth() + x] =
            Grassland::Graphics::Util::Color(depth_buffer[y * GetWidth() + x],
                                             depth_buffer[y * GetWidth() + x],
                                             depth_buffer[y * GetWidth() + x],
                                             1.0f);
    delete[] depth_buffer;
  } else {
    GRLColor *depth_buffer = new GRLColor[size];
    GRLGLCall(glGetTextureImage(GetHandle(), 0, GL_RGBA, GL_FLOAT,
                                sizeof(Grassland::Graphics::Util::Color) * size,
                                depth_buffer));
    for (int y = 0; y < GetHeight(); y++)
      memcpy(pBuffer + (GetHeight() - 1 - y) * GetWidth(),
             depth_buffer + y * GetWidth(), GetWidth() * sizeof(GRLColor));
    /*for (int x = 0; x < GetWidth(); x++)
            pBuffer[i] = Grassland::Graphics::Util::Color(
                    depth_buffer[i],
                    depth_buffer[i],
                    depth_buffer[i],
                    1.0f
            );//*/

    delete[] depth_buffer;
  }
  return GRL_FALSE;
}

GRL_RESULT GRLCOpenGLTexture::BindTexture(int32_t slot) const {
  glActiveTexture(GL_TEXTURE0 + slot);
  glBindTexture(GL_TEXTURE_2D, GetHandle());
  return GRL_FALSE;
}

GRL_RESULT GRLCOpenGLTexture::Resize(int32_t width, int32_t height) const {
  __texture->Resize(width, height);
  return GRL_FALSE;
}

GRL_RESULT GRLCOpenGLTexture::AddRef() {
  __Ref_Cnt++;
  return GRL_FALSE;
}

GRL_RESULT GRLCOpenGLTexture::Release() {
  __Ref_Cnt--;
  if (!__Ref_Cnt)
    delete this;
  return GRL_FALSE;
}

}  // namespace Grassland
