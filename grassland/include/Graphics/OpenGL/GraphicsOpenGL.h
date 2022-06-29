#pragma once

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "../../GrasslandDecl.h"
#include "../../Math/Math.h"
#include "../Graphics.h"
#include "../Util/Util.h"

namespace Grassland {
void GRLOpenGLClearError();

bool GRLOpenGLLogCall(const char *function, const char *file, int line);

template <class glFunc, class... glArgs>
GRL_RESULT GRLOpenGLFunctionCall(const char *code,
                                 const char *file,
                                 int line,
                                 glFunc &&func,
                                 glArgs &&...args) {
  while (glGetError() != GL_NO_ERROR)
    ;
  func(args...);
  while (GLenum error = glGetError()) {
    std::cout << "[ OpenGL Error ] (" << std::hex << error << ") " << code
              << " " << file << ":" << line << std::endl;
    return error;
  }
  return GRL_FALSE;
}

#define GRLASSERT(x) \
  if (!(x))          \
    __debugbreak();
#define GRLGLCall(x)     \
  GRLOpenGLClearError(); \
  x;                     \
  GRLASSERT(GRLOpenGLLogCall(#x, __FILE__, __LINE__))

#define GRLOpenGLCall(GLFUNC, ...) \
  GRLOpenGLFunctionCall(#GLFUNC, __FILE__, __LINE__, GLFUNC, __VA_ARGS__)

uint32_t GRLOpenGLCompileShaderFromFile(std::string shader_path,
                                        uint32_t shader_type);

uint32_t GRLFormatToOpenGLElementType(GRL_FORMAT format);
uint32_t GRLFormatElementNumber(GRL_FORMAT format);
uint32_t GRLFormatToOpenGLSizedFormat(GRL_FORMAT format);
uint32_t GRLFormatToOpenGLBaseFormat(GRL_FORMAT format);
uint32_t GRLRenderTopologyToOpenGLTopology(GRL_RENDER_TOPOLOGY topology);
uint32_t GRLFilterToOpenGLFilter(GRL_GRAPHICS_SAMPLER_FILTER filter);
uint32_t GRLExtensionModeToOpenGLWrapMode(
    GRL_GRAPHICS_TEXTURE_EXTENSION_MODE extension);

class GRLCOpenGLEnvironment;
class GRLCOpenGLBuffer;
class GRLCOpenGLPipelineState;
class GRLCOpenGLDepthMap;
class GRLCOpenGLTexture;

GRLMakeObjectUUIDAssociate(GRLCOpenGLEnvironment,
                           GRLID_COpenGLEnvironment,
                           "698eb0b1-9767-5076-a06b-050d53290d11");
GRLMakeObjectUUIDAssociate(GRLCOpenGLBuffer,
                           GRLID_COpenGLBuffer,
                           "03058c9e-40d3-5b33-b0f9-907842e56e49");
GRLMakeObjectUUIDAssociate(GRLCOpenGLPipelineState,
                           GRLID_COpenGLPipelineState,
                           "432dcf2b-a630-5299-bb66-85c113bb15ae");
GRLMakeObjectUUIDAssociate(GRLCOpenGLDepthMap,
                           GRLID_COpenGLDepthMap,
                           "e07c64e0-188e-5335-ba2b-944234531214");
GRLMakeObjectUUIDAssociate(GRLCOpenGLTexture,
                           GRLID_COpenGLTexture,
                           "668c8ca1-18c2-54e4-a49d-f832ade20d79");

class GRLCOpenGLEnvironment : public GRLIGraphicsEnvironment {
 public:
  GRLCOpenGLEnvironment(uint32_t width,
                        uint32_t height,
                        const char *window_title);
  virtual void GetSize(uint32_t *width, uint32_t *height) override;
  virtual GRL_RESULT PollEvents() override;
  virtual GRL_RESULT Resize(uint32_t width, uint32_t height) override;
  virtual GRL_RESULT CreateTexture(uint32_t width,
                                   uint32_t height,
                                   GRL_FORMAT format,
                                   GRLIGraphicsTexture **ppTexture) override;
  virtual GRL_RESULT CreateDepthMap(uint32_t width,
                                    uint32_t height,
                                    GRLIGraphicsDepthMap **ppDepthMap) override;
  virtual GRL_RESULT CreateBuffer(uint64_t size,
                                  GRL_GRAPHICS_BUFFER_TYPE type,
                                  GRL_GRAPHICS_BUFFER_USAGE usage,
                                  void *pData,
                                  GRLIGraphicsBuffer **ppBuffer) override;
  virtual GRL_RESULT CreatePipelineState(
      const char *shader_path,
      GRL_GRAPHICS_PIPELINE_STATE_DESC *desc,
      GRLIGraphicsPipelineState **ppPipelineState) override;
  virtual GRL_RESULT BeginDraw() override;
  virtual GRL_RESULT ApplyPipelineState(
      GRLIGraphicsPipelineState *pPipelineState) override;
  virtual GRL_RESULT SetViewport(uint32_t x,
                                 uint32_t y,
                                 uint32_t width,
                                 uint32_t height) override;
  virtual GRL_RESULT SetConstantBuffer(
      uint32_t constantBufferIndex,
      GRLIGraphicsBuffer *constantBuffer) override;
  virtual GRL_RESULT SetTextures(
      uint32_t numTexture,
      GRLIGraphicsTexture *const *pTextures) override;
  virtual GRL_RESULT SetRenderTargets(
      uint32_t numRenderTarget,
      GRLIGraphicsTexture *const *pRenderTargets,
      GRLIGraphicsDepthMap *pDepthMap) override;
  virtual GRL_RESULT SetInternalRenderTarget() override;
  virtual GRL_RESULT ClearRenderTargets(GRLColor color) override;
  virtual GRL_RESULT ClearDepthMap() override;
  virtual GRL_RESULT DrawInstance(GRLIGraphicsBuffer *pVertexBuffer,
                                  uint32_t numVertices,
                                  GRL_RENDER_TOPOLOGY renderTopology) override;
  virtual GRL_RESULT DrawIndexedInstance(
      GRLIGraphicsBuffer *pVertexBuffer,
      GRLIGraphicsBuffer *pIndexBuffer,
      uint32_t numVertices,
      GRL_RENDER_TOPOLOGY renderTopology) override;
  virtual GRL_RESULT EndDraw() override;
  virtual void WaitForGpu() override;
  virtual void Present(uint32_t enableInterval) override;
  virtual GRL_RESULT AddRef() override;
  virtual GRL_RESULT Release() override;
  virtual GRL_RESULT QueryInterface(GRLUUID Uuid, void **ppObject) override;
  void MakeThisContextCurrent();

 private:
  GLFWwindow *m_window;
  int32_t m_swapInterval;
  GRLPtr<GRLCOpenGLPipelineState> m_workingPipelineState;
  bool m_duringDraw;
  uint32_t m_lastActiveAttrib;
  uint32_t __Ref_Cnt;
};

GRL_RESULT GRLCreateOpenGLEnvironment(uint32_t width,
                                      uint32_t height,
                                      const char *window_title,
                                      GRLIGraphicsEnvironment **ppEnvironment);

uint32_t GRLBufferTypeToOpenGLBufferType(GRL_GRAPHICS_BUFFER_TYPE type);

class GRLCOpenGLBuffer : public GRLIGraphicsBuffer {
 public:
  GRLCOpenGLBuffer(uint64_t size,
                   GRL_GRAPHICS_BUFFER_TYPE type,
                   GRL_GRAPHICS_BUFFER_USAGE usage,
                   void *pData,
                   GRLCOpenGLEnvironment *pEnvironment);
  ~GRLCOpenGLBuffer();
  virtual GRL_RESULT WriteData(uint64_t size,
                               uint64_t offset,
                               void *pData) override;
  virtual GRL_RESULT AddRef() override;
  virtual GRL_RESULT Release() override;
  virtual GRL_RESULT QueryInterface(GRLUUID Uuid, void **ppObject) override;

 private:
  friend class GRLCOpenGLEnvironment;
  uint32_t m_buffer;
  GRL_GRAPHICS_BUFFER_TYPE m_type;
  GRL_GRAPHICS_BUFFER_USAGE m_usage;
  GRLPtr<GRLCOpenGLEnvironment> m_environment;
  uint32_t m_glType;
  uint32_t m_glUsage;
  uint64_t m_size;
  uint32_t __Ref_Cnt;
};

class GRLCOpenGLPipelineState : public GRLIGraphicsPipelineState {
 public:
  GRLCOpenGLPipelineState(const char *shader_path,
                          GRL_GRAPHICS_PIPELINE_STATE_DESC *desc,
                          GRLCOpenGLEnvironment *pEnvironment);
  ~GRLCOpenGLPipelineState();
  virtual GRL_RESULT AddRef() override;
  virtual GRL_RESULT Release() override;
  virtual GRL_RESULT QueryInterface(GRLUUID Uuid, void **ppObject) override;
  int GetTextureLocation(int textureIndex);

 private:
  friend class GRLCOpenGLEnvironment;
  std::map<int32_t, int32_t> m_mapTextureLocation;
  int32_t m_cullFace;
  uint32_t m_enableBlend;
  uint32_t m_enableDepthTest;
  uint32_t __Ref_Cnt;
  uint32_t m_numTexture;
  uint32_t m_numRenderTargets;
  uint32_t m_numConstantBuffer;
  uint32_t m_numInputElement;
  uint32_t m_shaderProgram;
  uint32_t m_frameBuffer;
  GRLPtr<GRLCOpenGLEnvironment> m_environment;
  std::vector<GRL_FORMAT> m_inputLayout;
  std::vector<GRL_GRAPHICS_SAMPLER_DESC> m_samplerDesc;
  std::vector<GRLCOpenGLTexture *> m_workingTextures;
  uint32_t m_strideSizeInBytes;
};

class GRLCOpenGLDepthMap : public GRLIGraphicsDepthMap {
 public:
  GRLCOpenGLDepthMap(uint32_t width,
                     uint32_t height,
                     GRLCOpenGLEnvironment *pEnvironment);
  ~GRLCOpenGLDepthMap();
  virtual GRL_RESULT GetSize(uint32_t *pWidth, uint32_t *pHeight) override;
  virtual GRL_RESULT AddRef() override;
  virtual GRL_RESULT Release() override;
  virtual GRL_RESULT QueryInterface(GRLUUID Uuid, void **ppObject) override;

 private:
  friend class GRLCOpenGLEnvironment;
  uint32_t m_depthMap;
  GRLPtr<GRLCOpenGLEnvironment> m_environment;
  uint32_t m_width;
  uint32_t m_height;
  uint32_t __Ref_Cnt;
};

class GRLCOpenGLTexture : public GRLIGraphicsTexture {
 public:
  GRLCOpenGLTexture(uint32_t width,
                    uint32_t height,
                    GRL_FORMAT format,
                    GRLCOpenGLEnvironment *pEnvironment);
  ~GRLCOpenGLTexture();
  virtual GRL_RESULT WritePixels(void *pData) override;
  virtual GRL_RESULT ReadPixels(void *pData) override;
  virtual GRL_RESULT GetSize(uint32_t *pWidth, uint32_t *pHeight) override;
  virtual GRL_FORMAT GetFormat() override;
  virtual GRL_RESULT AddRef() override;
  virtual GRL_RESULT Release() override;
  virtual GRL_RESULT QueryInterface(GRLUUID Uuid, void **ppObject) override;

 private:
  friend class GRLCOpenGLEnvironment;
  uint32_t m_texture;
  uint32_t m_width, m_height;
  GRL_FORMAT m_format;
  uint32_t m_internalFormat;
  uint32_t __Ref_Cnt;
  GRLPtr<GRLCOpenGLEnvironment> m_environment;
};

}  // namespace Grassland
