#pragma once
//#include "OpenGL/GL.h"
//#include "OpenGL/GL2D.h"
#include "Util/Util.h"

#ifdef _WIN32
#include "DirectX/DX.h"
#endif


namespace Grassland
{
//#define GRLID_IGraphicsEnvironment				GRLUUID("8c7edbfd-fc7b-479d-87ac-069480d65d3e")
//#define GRLID_IGraphicsBuffer					GRLUUID("cb318405-fa08-4ded-8393-67dbd259e63f")
//#define GRLID_IGraphicsTexture					GRLUUID("29433e84-b78d-4a5d-886b-864ccef99acc")
//#define GRLID_IGraphicsDepthMap					GRLUUID("82d47f8c-f737-4584-8e00-7abae3df9804")
//#define GRLID_IGraphicsPipelineState			GRLUUID("18adfb02-d6b5-45e6-a7e6-018cfe1c1a62")

	enum class GRL_FORMAT
	{
		FLOAT = 0,
		FLOAT2 = 1,
		FLOAT3 = 2,
		FLOAT4 = 3,
		INT = 4,
		INT2 = 5,
		INT3 = 6,
		INT4 = 7,
		UINT = 8,
		UINT2 = 9,
		UINT3 = 10,
		UINT4 = 11,
		BYTE = 16,
		BYTE2 = 17,
		BYTE4 = 19,
	};

	uint32_t GRLFormatSizeInByte(GRL_FORMAT format);

	enum class GRL_RENDER_TOPOLOGY
	{
		TRIANGLE = 0,
		LINE = 1
	};

	enum class GRL_GRAPHICS_API
	{
		D3D12 = 0,
		OPENGL = 1,
		VULKAN = 2
	};

	enum class GRL_GRAPHICS_BUFFER_TYPE
	{
		VERTEX = 0,
		INDEX = 1,
		CONSTANT = 2,
	};

	class GRLIGraphicsEnvironment;
	class GRLIGraphicsBuffer;
	class GRLIGraphicsTexture;
	class GRLIGraphicsDepthMap;
	class GRLIGraphicsPipelineState;

	struct GRL_GRAPHICS_PIPELINE_STATE_DESC
	{
		uint32_t numRenderTargets;
		GRL_FORMAT renderTargetFormatsList[8];
		uint32_t numConstantBuffer;
		uint32_t numTexture;
		uint32_t numInputElement;
		GRL_FORMAT *inputElementLayout;
		bool enableDepthTest;
		bool enableBlend;
		bool enableCullFace;
	};

	GRLMakeObjectUUIDAssociate(GRLIGraphicsEnvironment, GRLID_IGraphicsEnvironment, "8c7edbfd-fc7b-479d-87ac-069480d65d3e");
	GRLMakeObjectUUIDAssociate(GRLIGraphicsBuffer, GRLID_IGraphicsBuffer, "cb318405-fa08-4ded-8393-67dbd259e63f");
	GRLMakeObjectUUIDAssociate(GRLIGraphicsTexture, GRLID_IGraphicsTexture, "29433e84-b78d-4a5d-886b-864ccef99acc");
	GRLMakeObjectUUIDAssociate(GRLIGraphicsDepthMap, GRLID_IGraphicsDepthMap, "82d47f8c-f737-4584-8e00-7abae3df9804");
	GRLMakeObjectUUIDAssociate(GRLIGraphicsPipelineState, GRLID_IGraphicsPipelineState, "18adfb02-d6b5-45e6-a7e6-018cfe1c1a62");

	class GRLIGraphicsEnvironment : public GRLIBase
	{
	public:
		virtual void GetSize(uint32_t * width, uint32_t * height) = 0;
		virtual GRL_RESULT PollEvents() = 0;
		virtual GRL_RESULT Resize(uint32_t width, uint32_t height) = 0;
		virtual GRL_RESULT CreateTexture(uint32_t width, uint32_t height, GRL_FORMAT format, GRLIGraphicsTexture** ppTexture) = 0;
		virtual GRL_RESULT CreateDepthMap(uint32_t width, uint32_t height, GRLIGraphicsDepthMap** ppDepthMap) = 0;
		virtual GRL_RESULT CreateBuffer(uint64_t size, GRL_GRAPHICS_BUFFER_TYPE type, GRLIGraphicsBuffer** ppBuffer) = 0;
		virtual GRL_RESULT CreatePipelineState(
			const char* shader_path,
			GRL_GRAPHICS_PIPELINE_STATE_DESC *desc,
			GRLIGraphicsPipelineState** ppPipelineState) = 0;
		virtual GRL_RESULT BeginDraw() = 0;
		virtual GRL_RESULT ApplyPipelineState(GRLIGraphicsPipelineState* pPipelineState) = 0;
		virtual GRL_RESULT SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual GRL_RESULT SetConstantBuffer(uint32_t constantBufferIndex, GRLIGraphicsBuffer* constantBuffer) = 0;
		virtual GRL_RESULT SetTextures(uint32_t textureIndex, GRLIGraphicsTexture* pTexture) = 0;
		virtual GRL_RESULT SetRenderTargets(GRLIGraphicsTexture* pRenderTargetList, GRLIGraphicsDepthMap* pDepthMap) = 0;
		virtual GRL_RESULT SetInternalRenderTarget() = 0;
		virtual GRL_RESULT ClearRenderTargets(GRLColor color) = 0;
		virtual GRL_RESULT ClearDepthMap() = 0;
		virtual GRL_RESULT DrawInstance(
			GRLIGraphicsBuffer* pVertexBuffer,
			uint32_t numInstance,
			GRL_RENDER_TOPOLOGY renderTopology) = 0;
		virtual GRL_RESULT DrawIndexedInstance(
			GRLIGraphicsBuffer* pVertexBuffer, 
			GRLIGraphicsBuffer* pIndexBuffer,
			uint32_t numInstance,
			GRL_RENDER_TOPOLOGY renderTopology) = 0;
		virtual GRL_RESULT EndDraw() = 0;
		virtual void WaitForGpu() = 0;
		virtual void Present(uint32_t enableInterval) = 0;
	};

	class GRLIGraphicsBuffer : public GRLIBase
	{
	public:
		virtual GRL_RESULT WriteData(uint64_t size, uint64_t offset, void* pData) = 0;
	};

	class GRLIGraphicsTexture : public GRLIBase
	{
	public:
		virtual GRL_RESULT WritePixels(void* pData) = 0;
		virtual GRL_RESULT ReadPixels(void* pData) = 0;
		virtual GRL_RESULT GetSize(uint32_t* pWidth, uint32_t* pHeight) = 0;
		virtual GRL_FORMAT GetFormat() = 0;
	};

	class GRLIGraphicsDepthMap : public GRLIBase
	{
	public:
		virtual GRL_RESULT GetSize(uint32_t* pWidth, uint32_t* pHeight) = 0;
	};

	class GRLIGraphicsPipelineState : public GRLIBase
	{
	public:
	};

	GRL_RESULT GRLCreateGraphicsEnvironment(uint32_t width, uint32_t height, const char* window_title, GRL_GRAPHICS_API graphicsAPI, GRLIGraphicsEnvironment** ppEnvironment);
}