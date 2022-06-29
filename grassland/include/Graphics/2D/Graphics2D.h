#pragma once

#include "../Graphics.h"

namespace Grassland {

GRLDeclareObject(CGraphics2DEnvironment,
                 "d8342bdc-ed73-42a6-82c5-42849b9f1685");
class GRLCGraphics2DEnvironment : public GRLIGraphics2DEnvironment {
  struct ConstantContent {
    GRLMat4 transform;
    int mode;
  } m_constantContent;

 public:
  GRLCGraphics2DEnvironment(GRLIGraphicsEnvironment *pEnvironment);
  virtual GRL_RESULT BeginDraw() override;
  virtual GRL_RESULT EndDraw() override;
  virtual void SetTransformIdentity() override;
  virtual void SetTransformScreenAspect() override;
  virtual void SetTransform(GRLMat4 mat) override;
  virtual GRL_RESULT DrawRectangle(float x0,
                                   float y0,
                                   float x1,
                                   float y1,
                                   float x2,
                                   float y2,
                                   GRLColor color) override;
  virtual GRL_RESULT DrawTriangle(float x0,
                                  float y0,
                                  float x1,
                                  float y1,
                                  GRLColor color) override;
  virtual GRL_RESULT PutTexture(float x0,
                                float y0,
                                float width,
                                float height,
                                GRLIGraphicsTexture *texture,
                                float alpha = 1.0) override;
  virtual GRL_RESULT AddRef() override;
  virtual GRL_RESULT Release() override;
  virtual GRL_RESULT QueryInterface(GRLUUID Uuid, void **ppObject) override;

 private:
  GRLPtr<GRLIGraphicsEnvironment> m_environment;
  GRLPtr<GRLIGraphicsPipelineState> m_pipelineState;
  GRLPtr<GRLIGraphicsBuffer> m_constantBuffer;
  GRLPtr<GRLIGraphicsBuffer> m_vertexBuffer;
  GRLPtr<GRLIGraphicsBuffer> m_indexBuffer;
  uint32_t __Ref_Cnt;
};
}  // namespace Grassland
