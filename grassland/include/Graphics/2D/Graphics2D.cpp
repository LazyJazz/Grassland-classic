#include "Graphics2D.h"

namespace Grassland
{
	GRLCGraphics2DEnvironment::GRLCGraphics2DEnvironment(GRLIGraphicsEnvironment* pEnvironment)
	{
		__Ref_Cnt = 1;

		GRL_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc;
		GRL_FORMAT inputElementLayout[] = {GRL_FORMAT::FLOAT2, GRL_FORMAT::FLOAT4};
		GRL_GRAPHICS_SAMPLER_DESC samplerDesc[] = {
			{GRL_GRAPHICS_SAMPLER_FILTER::POINT, GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::BLACK, GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::BLACK, GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::BLACK}
		};
		pipelineDesc.cullFace = 0;
		pipelineDesc.enableBlend = 1;
		pipelineDesc.enableDepthTest = 0;
		pipelineDesc.inputElementLayout = inputElementLayout;
		pipelineDesc.numConstantBuffer = 1;
		pipelineDesc.numInputElement = 2;
		pipelineDesc.numRenderTargets = 1;
		pipelineDesc.numTexture = 1;
		pipelineDesc.renderTargetFormatsList[0] = GRL_FORMAT::BYTE4;
		pipelineDesc.samplerDesc = samplerDesc;

		m_constantContent.mode = 0;
		m_constantContent.transform = GRLMat4(1.0f);

		m_environment->CreatePipelineState("shaders/2d_shader", &pipelineDesc, &m_pipelineState);
		m_environment->CreateBuffer(sizeof(m_constantContent), GRL_GRAPHICS_BUFFER_TYPE::CONSTANT, GRL_GRAPHICS_BUFFER_USAGE::DYNAMIC, &m_constantContent, &m_constantBuffer);
		m_environment->CreateBuffer(6*sizeof(float)*64, GRL_GRAPHICS_BUFFER_TYPE::VERTEX, GRL_GRAPHICS_BUFFER_USAGE::DYNAMIC, nullptr, &m_vertexBuffer);
		m_environment->CreateBuffer(3*sizeof(uint32_t)*64, GRL_GRAPHICS_BUFFER_TYPE::INDEX, GRL_GRAPHICS_BUFFER_USAGE::DYNAMIC, nullptr, &m_indexBuffer);
	}
	GRL_RESULT GRLCGraphics2DEnvironment::BeginDraw()
	{
		return m_environment->BeginDraw();
	}

	GRL_RESULT GRLCGraphics2DEnvironment::EndDraw()
	{
		return m_environment->EndDraw();
	}
	void GRLCGraphics2DEnvironment::SetTransformIdentity()
	{
		m_constantContent.transform = GRLMat4(1.0f);
	}
	void GRLCGraphics2DEnvironment::SetTransformScreenAspect()
	{
		uint32_t width, height;
		m_environment->GetSize(&width, &height);
		m_constantContent.transform = GRLMat4(
			(float)height / (float)width, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
	void GRLCGraphics2DEnvironment::SetTransform(GRLMat4 mat)
	{
		m_constantContent.transform = mat;
	}
	GRL_RESULT GRLCGraphics2DEnvironment::DrawRectangle(float x0, float y0, float x1, float y1, float x2, float y2, GRLColor color)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCGraphics2DEnvironment::DrawTriangle(float x0, float y0, float x1, float y1, GRLColor color)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCGraphics2DEnvironment::PutTexture(float x0, float y0, float width, float height, GRLIGraphicsTexture* texture, float alpha)
	{
		return GRL_RESULT();
	}
	GRL_RESULT GRLCGraphics2DEnvironment::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCGraphics2DEnvironment::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt)
			delete this;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCGraphics2DEnvironment::QueryInterface(GRLUUID Uuid, void** ppObject)
	{
		if (Uuid == GRLID_CGraphics2DEnvironment) *ppObject = reinterpret_cast<GRLCGraphics2DEnvironment*>(this);
		else if (Uuid == GRLID_IGraphics2DEnvironment) *ppObject = reinterpret_cast<GRLIGraphics2DEnvironment*>(this);
		else if (Uuid == GRLID_IBase) *ppObject = reinterpret_cast<GRLIBase*>(this);
		else return GRL_TRUE;
		AddRef();
		return GRL_FALSE;
	}
}