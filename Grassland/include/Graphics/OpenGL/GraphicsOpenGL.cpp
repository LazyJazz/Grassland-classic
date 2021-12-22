#include "GraphicsOpenGL.h"

namespace Grassland
{

	bool g_glfwInited = false;
	bool g_gladInited = false;

	void GRLOpenGLClearError()
	{
		while (glGetError() != GL_NO_ERROR);
	}

	bool GRLOpenGLLogCall(const char* function, const char* file, int line)
	{
		while (GLenum error = glGetError())
		{
			std::cout << "[ OpenGL Error ] (" << error << ") " << function <<
				" " << file << ":" << line << std::endl;
			return false;
		}
		return true;
	}

	uint32_t GRLOpenGLCompileShaderFromFile(std::string shader_path, uint32_t shader_type)
	{
		uint32_t shader = 0;
		FILE* file = nullptr;
#ifdef _WIN32
		fopen_s(&file, shader_path.c_str(), "rb");
#else
		file = fopen(shader_path.c_str(), "rb");
#endif
		if (!file)
		{
			GRLSetErrorInfo("CompileShaderFromFile Failed. [Open file failed.]");
			return 0;
		}
		fseek(file, 0, SEEK_END);
		int32_t length = ftell(file);
		//std::cout << "Code length:" << length << std::endl;
		char* source_code = new char[length + 1];
		memset(source_code, 0, length + 1);
		fseek(file, 0, SEEK_SET);
		fread(source_code, 1, length, file);
		//std::cout << "Shader Path:" << shader_file_path << std::endl;
		//std::cout << "Shader Code:" << std::endl << source_code << std::endl;
		//std::cout << source_code << std::endl;
		fclose(file);

		shader = glCreateShader(
			shader_type
		);
		GRLOpenGLCall(glShaderSource, shader, 1, &source_code, nullptr);
		GRLOpenGLCall(glCompileShader, shader);
		delete[] source_code;

		int32_t success;
		GRLOpenGLCall(glGetShaderiv, shader, GL_COMPILE_STATUS, &success);
		if (success != GL_TRUE)
		{
			int32_t log_length = 0;
			char message[1024];
			GRLOpenGLCall(glGetShaderInfoLog, shader, 1000, &log_length, message);
			GRLSetErrorInfo("OpenGL Shader Compilation Failed.");
			std::cout << "Reason [" << ((shader_type == GL_FRAGMENT_SHADER) ? "fragment shader" : "vertex shader") << "]\n" << message << std::endl;
			GRLOpenGLCall(glDeleteShader, shader);
			return 0;
		}
		return shader;
	}

	uint32_t GRLFormatToOpenGLElementType(GRL_FORMAT format)
	{
		switch (format)
		{
		case GRL_FORMAT::BYTE:
		case GRL_FORMAT::BYTE2:
		case GRL_FORMAT::BYTE4:
			return GL_UNSIGNED_BYTE;
		case GRL_FORMAT::FLOAT:
		case GRL_FORMAT::FLOAT2:
		case GRL_FORMAT::FLOAT3:
		case GRL_FORMAT::FLOAT4:
			return GL_FLOAT;
		case GRL_FORMAT::INT:
		case GRL_FORMAT::INT2:
		case GRL_FORMAT::INT3:
		case GRL_FORMAT::INT4:
			return GL_INT;
		case GRL_FORMAT::UINT:
		case GRL_FORMAT::UINT2:
		case GRL_FORMAT::UINT3:
		case GRL_FORMAT::UINT4:
			return GL_UNSIGNED_INT;
		}
		return GL_NONE;
	}

	uint32_t GRLFormatElementNumber(GRL_FORMAT format)
	{
		switch (format)
		{
		case GRL_FORMAT::BYTE:
		case GRL_FORMAT::FLOAT:
		case GRL_FORMAT::INT:
		case GRL_FORMAT::UINT:
			return 1;
		case GRL_FORMAT::BYTE2:
		case GRL_FORMAT::FLOAT2:
		case GRL_FORMAT::INT2:
		case GRL_FORMAT::UINT2:
			return 2;
		case GRL_FORMAT::FLOAT3:
		case GRL_FORMAT::INT3:
		case GRL_FORMAT::UINT3:
			return 3;
		case GRL_FORMAT::BYTE4:
		case GRL_FORMAT::FLOAT4:
		case GRL_FORMAT::INT4:
		case GRL_FORMAT::UINT4:
			return 4;
		}
		return 0;
	}

	uint32_t GRLFormatToOpenGLSizedFormat(GRL_FORMAT format)
	{
		switch (format)
		{
		case GRL_FORMAT::BYTE: return GL_R8;
		case GRL_FORMAT::BYTE2: return GL_RG8;
		case GRL_FORMAT::BYTE4: return GL_RGBA8;
		case GRL_FORMAT::FLOAT: return GL_R32F;
		case GRL_FORMAT::FLOAT2: return GL_RG32F;
		case GRL_FORMAT::FLOAT3: return GL_RGB32F;
		case GRL_FORMAT::FLOAT4: return GL_RGBA32F;
		case GRL_FORMAT::INT: return GL_R32I;
		case GRL_FORMAT::INT2: return GL_RG32I;
		case GRL_FORMAT::INT3: return GL_RGB32I;
		case GRL_FORMAT::INT4: return GL_RGBA32I;
		case GRL_FORMAT::UINT: return GL_R32UI;
		case GRL_FORMAT::UINT2: return GL_RG32UI;
		case GRL_FORMAT::UINT3: return GL_RGB32UI;
		case GRL_FORMAT::UINT4: return GL_RGBA32UI;
		}
		return GL_NONE;
	}

	uint32_t GRLFormatToOpenGLBaseFormat(GRL_FORMAT format)
	{
		switch (format)
		{
		case GRL_FORMAT::BYTE:
		case GRL_FORMAT::FLOAT:
		case GRL_FORMAT::INT:
		case GRL_FORMAT::UINT:
			return GL_RED;
		case GRL_FORMAT::BYTE2:
		case GRL_FORMAT::FLOAT2:
		case GRL_FORMAT::INT2:
		case GRL_FORMAT::UINT2:
			return GL_RG;
		case GRL_FORMAT::FLOAT3:
		case GRL_FORMAT::INT3:
		case GRL_FORMAT::UINT3:
			return GL_RGB;
		case GRL_FORMAT::BYTE4:
		case GRL_FORMAT::FLOAT4:
		case GRL_FORMAT::INT4:
		case GRL_FORMAT::UINT4:
			return GL_RGBA;
		}
		return 0;
	}

	uint32_t GRLRenderTopologyToOpenGLTopology(GRL_RENDER_TOPOLOGY topology)
	{
		switch (topology)
		{
		case GRL_RENDER_TOPOLOGY::TRIANGLE: return GL_TRIANGLES;
		case GRL_RENDER_TOPOLOGY::LINE: return GL_LINES;
		}
		return GL_NONE;
	}

	uint32_t GRLFilterToOpenGLFilter(GRL_GRAPHICS_SAMPLER_FILTER filter)
	{
		switch (filter)
		{
		case GRL_GRAPHICS_SAMPLER_FILTER::LINEAR: return GL_LINEAR;
		case GRL_GRAPHICS_SAMPLER_FILTER::MIN_MAG_LINEAR: return GL_LINEAR;
		case GRL_GRAPHICS_SAMPLER_FILTER::MIP_LINEAR: return GL_NEAREST;
		}
		return GL_NEAREST;
	}

	uint32_t GRLExtensionModeToOpenGLWrapMode(GRL_GRAPHICS_TEXTURE_EXTENSION_MODE extension)
	{
		switch (extension)
		{
		case GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::BLACK: return GL_CLAMP_TO_BORDER;
		case GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::CLAMP: return GL_CLAMP_TO_EDGE;
		case GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::MIRROR: return GL_MIRRORED_REPEAT;
		case GRL_GRAPHICS_TEXTURE_EXTENSION_MODE::REPEAT: return GL_REPEAT;
		}
		return GL_CLAMP_TO_BORDER;
	}
	
	GRL_RESULT GRLCreateOpenGLEnvironment(uint32_t width, uint32_t height, const char* window_title, GRLIGraphicsEnvironment** ppEnvironment)
	{
		*ppEnvironment = new GRLCOpenGLEnvironment(width, height, window_title);
		return GRL_FALSE;
	}

	uint32_t GRLBufferTypeToOpenGLBufferType(GRL_GRAPHICS_BUFFER_TYPE type)
	{
		switch (type)
		{
		case GRL_GRAPHICS_BUFFER_TYPE::CONSTANT: return GL_UNIFORM_BUFFER;
		case GRL_GRAPHICS_BUFFER_TYPE::INDEX: return GL_ELEMENT_ARRAY_BUFFER;
		case GRL_GRAPHICS_BUFFER_TYPE::VERTEX: return GL_ARRAY_BUFFER;
		}
		return GL_NONE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::CreateDepthMap(uint32_t width, uint32_t height, GRLIGraphicsDepthMap** ppDepthMap)
	{
		MakeThisContextCurrent();
		*ppDepthMap = new GRLCOpenGLDepthMap(width, height, this);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::CreateBuffer(uint64_t size, GRL_GRAPHICS_BUFFER_TYPE type, GRL_GRAPHICS_BUFFER_USAGE usage, void* pData, GRLIGraphicsBuffer** ppBuffer)
	{
		MakeThisContextCurrent();
		*ppBuffer = new GRLCOpenGLBuffer(size, type, usage, pData, this);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::CreatePipelineState(const char* shader_path, GRL_GRAPHICS_PIPELINE_STATE_DESC* desc, GRLIGraphicsPipelineState** ppPipelineState)
	{
		MakeThisContextCurrent();
		*ppPipelineState = new GRLCOpenGLPipelineState(shader_path, desc, this);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::BeginDraw()
	{
		if (m_duringDraw) return GRL_TRUE;
		MakeThisContextCurrent();
		m_workingPipelineState.Reset();
		m_duringDraw = true;
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::ApplyPipelineState(GRLIGraphicsPipelineState* pPipelineState)
	{
		if (!m_duringDraw) return GRL_TRUE;
		MakeThisContextCurrent();
		if (pPipelineState->QueryInterface(GRLID_PPV_ARGS(&m_workingPipelineState)))
			return GRL_TRUE;
		GRLOpenGLCall(glUseProgram, m_workingPipelineState->m_shaderProgram);
		if (m_workingPipelineState->m_enableDepthTest) GRLOpenGLCall(glEnable, GL_DEPTH_TEST);
		else GRLOpenGLCall(glDisable, GL_DEPTH_TEST);
		if (m_workingPipelineState->m_enableBlend) GRLOpenGLCall(glEnable, GL_BLEND);
		else GRLOpenGLCall(glDisable, GL_BLEND);
		if (m_workingPipelineState->m_cullFace) GRLOpenGLCall(glEnable, GL_CULL_FACE);
		else GRLOpenGLCall(glDisable, GL_CULL_FACE);
		if (m_workingPipelineState->m_cullFace == 1) GRLOpenGLCall(glCullFace, GL_BACK);
		else GRLOpenGLCall(glCullFace, GL_FRONT);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		if (!m_duringDraw) return GRL_TRUE;
		MakeThisContextCurrent();
		GRLOpenGLCall(glViewport, x, y, width, height);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::SetConstantBuffer(uint32_t constantBufferIndex, GRLIGraphicsBuffer* constantBuffer)
	{
		if (!m_duringDraw) return GRL_TRUE;
		MakeThisContextCurrent();
		GRLPtr<GRLCOpenGLBuffer> pConstantBuffer;
		if (constantBuffer->QueryInterface(GRLID_PPV_ARGS(&pConstantBuffer)))
			return GRL_TRUE;
		GRLOpenGLCall(glBindBufferBase, GL_UNIFORM_BUFFER, constantBufferIndex, pConstantBuffer->m_buffer);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::SetTextures(uint32_t numTexture, GRLIGraphicsTexture* const* pTextures)
	{
		if (!m_duringDraw) return GRL_TRUE;
		MakeThisContextCurrent();
		GRLPtr<GRLCOpenGLTexture>* textures = new GRLPtr<GRLCOpenGLTexture>[numTexture];
		for (int i = 0; i < numTexture; i++)
		{
			if (pTextures[i]->QueryInterface(GRLID_PPV_ARGS(&textures[i])))
				return GRL_TRUE;
		}
		for (int i = 0; i < numTexture; i++)
		{
			GRLOpenGLCall(glBindTexture, GL_TEXTURE_2D, textures[i]->m_texture);
			GRLOpenGLCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GRLFilterToOpenGLFilter(m_workingPipelineState->m_samplerDesc[i].filter));
			GRLOpenGLCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GRLFilterToOpenGLFilter(m_workingPipelineState->m_samplerDesc[i].filter));
			GRLOpenGLCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GRLExtensionModeToOpenGLWrapMode(m_workingPipelineState->m_samplerDesc[i].extensionU));
			GRLOpenGLCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GRLExtensionModeToOpenGLWrapMode(m_workingPipelineState->m_samplerDesc[i].extensionV));
			GRLOpenGLCall(glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GRLExtensionModeToOpenGLWrapMode(m_workingPipelineState->m_samplerDesc[i].extensionW));
			GRLOpenGLCall(glBindTexture, GL_TEXTURE_2D, 0);
		}
		for (int i = 0; i < numTexture; i++)
		{
			GRLOpenGLCall(glActiveTexture, GL_TEXTURE0 + i);
			GRLOpenGLCall(glBindTexture, GL_TEXTURE_2D, textures[i]->m_texture);
			GRLOpenGLCall(glUniform1i, m_workingPipelineState->GetTextureLocation(i), i);
		}

		for (int i = 0; i < numTexture; i++)
			textures[i].Reset();
		delete[] textures;
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::SetRenderTargets(uint32_t numRenderTarget, GRLIGraphicsTexture* const* pRenderTargets, GRLIGraphicsDepthMap* pDepthMap)
	{
		if (!m_duringDraw) return GRL_TRUE;
		MakeThisContextCurrent();
		GRLPtr<GRLCOpenGLTexture> renderTargets[8];
		GRLPtr<GRLCOpenGLDepthMap> depthMap;
		for (int i = 0; i < numRenderTarget; i++)
		{
			if (pRenderTargets[i]->QueryInterface(GRLID_PPV_ARGS(&renderTargets[i])))
				return GRL_TRUE;
		}
		if (pDepthMap)
			if (pDepthMap->QueryInterface(GRLID_PPV_ARGS(&depthMap)))
				return GRL_TRUE;

		GRLOpenGLCall(glBindFramebuffer, GL_FRAMEBUFFER, m_workingPipelineState->m_frameBuffer);
		if (pDepthMap)
		{
			GRLOpenGLCall(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap->m_depthMap, 0);
		}
		uint32_t colorAttachments[32] = {};
		for (int i = 0; i < numRenderTarget; i++)
		{
			GRLOpenGLCall(glFramebufferTexture2D, GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, renderTargets[i]->m_texture, 0);
			colorAttachments[i] = GL_COLOR_ATTACHMENT0 + i;
		}
		GRLOpenGLCall(glDrawBuffers, numRenderTarget, colorAttachments);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::SetInternalRenderTarget()
	{
		if (!m_duringDraw) return GRL_TRUE;
		MakeThisContextCurrent();
		GRLOpenGLCall(glBindFramebuffer, GL_FRAMEBUFFER, 0);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::ClearRenderTargets(GRLColor color)
	{
		if (!m_duringDraw) return GRL_TRUE;
		MakeThisContextCurrent();
		GRLOpenGLCall(glClearColor, color.r, color.g, color.b, color.a);
		GRLOpenGLCall(glClear, GL_COLOR_BUFFER_BIT);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::ClearDepthMap()
	{
		if (!m_duringDraw) return GRL_TRUE;
		MakeThisContextCurrent();
		GRLOpenGLCall(glClearDepth, 1.0);
		GRLOpenGLCall(glClear, GL_DEPTH_BUFFER_BIT);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::DrawInstance(GRLIGraphicsBuffer* pVertexBuffer, uint32_t numVertices, GRL_RENDER_TOPOLOGY renderTopology)
	{
		if (!m_duringDraw) return GRL_TRUE;
		if (!m_workingPipelineState.Get()) return GRL_TRUE;
		MakeThisContextCurrent();
		GRLPtr<GRLCOpenGLBuffer> vertexBuffer;
		if (pVertexBuffer->QueryInterface(GRLID_PPV_ARGS(&vertexBuffer)))
			return GRL_TRUE;
		GRLOpenGLCall(glBindBuffer, GL_ARRAY_BUFFER, vertexBuffer->m_buffer);

		size_t offset = 0;
		for (int index = 0; index < m_workingPipelineState->m_numInputElement; index++)
		{
			//std::cout << "input index: " << index << std::endl;
			GRLOpenGLCall(glVertexAttribPointer,
				index,
				GRLFormatElementNumber(m_workingPipelineState->m_inputLayout[index]),
				GRLFormatToOpenGLElementType(m_workingPipelineState->m_inputLayout[index]),
				FALSE,
				m_workingPipelineState->m_strideSizeInBytes,
				(void*)offset
				);
			offset += GRLFormatSizeInBytes(m_workingPipelineState->m_inputLayout[index]);
			GRLOpenGLCall(glEnableVertexAttribArray,index);
		}
		for (int index = m_workingPipelineState->m_numInputElement; index < m_lastActiveAttrib; index++)
		{
			GRLOpenGLCall(glDisableVertexAttribArray, index);
		}
		m_lastActiveAttrib = m_workingPipelineState->m_numInputElement;
		GRLOpenGLCall(glDrawArrays, GRLRenderTopologyToOpenGLTopology(renderTopology), 0, numVertices);

		GRLOpenGLCall(glBindBuffer, GL_ARRAY_BUFFER, 0);
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::DrawIndexedInstance(GRLIGraphicsBuffer* pVertexBuffer, GRLIGraphicsBuffer* pIndexBuffer, uint32_t numVertices, GRL_RENDER_TOPOLOGY renderTopology)
	{
		if (!m_duringDraw) return GRL_TRUE;
		if (!m_workingPipelineState.Get()) return GRL_TRUE;
		MakeThisContextCurrent();
		GRLPtr<GRLCOpenGLBuffer> vertexBuffer;
		GRLPtr<GRLCOpenGLBuffer> indexBuffer;
		if (pVertexBuffer->QueryInterface(GRLID_PPV_ARGS(&vertexBuffer)))
			return GRL_TRUE;
		if (pIndexBuffer->QueryInterface(GRLID_PPV_ARGS(&indexBuffer)))
			return GRL_TRUE;
		GRLOpenGLCall(glBindBuffer, GL_ARRAY_BUFFER, vertexBuffer->m_buffer);
		GRLOpenGLCall(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, indexBuffer->m_buffer);

		size_t offset = 0;
		for (int index = 0; index < m_workingPipelineState->m_numInputElement; index++)
		{
			//std::cout << "input index: " << index << std::endl;
			GRLOpenGLCall(glVertexAttribPointer,
				index,
				GRLFormatElementNumber(m_workingPipelineState->m_inputLayout[index]),
				GRLFormatToOpenGLElementType(m_workingPipelineState->m_inputLayout[index]),
				FALSE,
				m_workingPipelineState->m_strideSizeInBytes,
				(void*)offset
			);
			offset += GRLFormatSizeInBytes(m_workingPipelineState->m_inputLayout[index]);
			GRLOpenGLCall(glEnableVertexAttribArray, index);
		}
		for (int index = m_workingPipelineState->m_numInputElement; index < m_lastActiveAttrib; index++)
		{
			GRLOpenGLCall(glDisableVertexAttribArray, index);
		}
		m_lastActiveAttrib = m_workingPipelineState->m_numInputElement;
		GRLOpenGLCall(glDrawElements, GRLRenderTopologyToOpenGLTopology(renderTopology), numVertices, GL_UNSIGNED_INT, nullptr);
		//glDrawElements();

		GRLOpenGLCall(glBindBuffer, GL_ARRAY_BUFFER, 0);
		GRLOpenGLCall(glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);

		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLEnvironment::EndDraw()
	{
		if (!m_duringDraw) return GRL_TRUE;
		MakeThisContextCurrent();
		m_duringDraw = false;
		return GRL_FALSE;
	}

	void GRLCOpenGLEnvironment::WaitForGpu()
	{
		MakeThisContextCurrent();
	}

	void GRLCOpenGLEnvironment::Present(uint32_t enableInterval)
	{
		MakeThisContextCurrent();

		if (m_swapInterval != enableInterval)
		{
			glfwSwapInterval(enableInterval);
			m_swapInterval = enableInterval;
		}
		glfwSwapBuffers(m_window);
	}

	GRL_RESULT GRLCOpenGLEnvironment::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLEnvironment::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt)
			delete this;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLEnvironment::QueryInterface(GRLUUID Uuid, void** ppObject)
	{
		if (Uuid == GRLID_IBase) *ppObject = reinterpret_cast<GRLIBase*>(this);
		else if (Uuid == GRLID_IGraphicsEnvironment) *ppObject = reinterpret_cast<GRLIGraphicsEnvironment*>(this);
		else if (Uuid == GRLID_COpenGLEnvironment) *ppObject = reinterpret_cast<GRLCOpenGLEnvironment*>(this);
		else GRL_TRUE;
		AddRef();
		return GRL_FALSE;
	}

	void GRLCOpenGLEnvironment::MakeThisContextCurrent()
	{
		if (glfwGetCurrentContext() != m_window)
		{
			glfwMakeContextCurrent(m_window);
		}
	}

	GRLCOpenGLEnvironment::GRLCOpenGLEnvironment(uint32_t width, uint32_t height, const char* window_title)
	{
		__Ref_Cnt = 1;
		m_swapInterval = -1;
		m_window = nullptr;
		m_duringDraw = false;
		if (!g_glfwInited)
		{
			if (!glfwInit())
			{
				GRLSetErrorInfo("glfwInit Failed.");
				return;
			}
			g_glfwInited = true;
		}

		std::string strtitle = window_title;
#ifdef _DEBUG
		strtitle = strtitle + " [OpenGL]";
#endif
		m_window = glfwCreateWindow(width, height, strtitle.c_str(), nullptr, nullptr);
		if (!m_window)
		{
			GRLSetErrorInfo("glfwCreateWindow Failed.");
			return;
		}

		this->MakeThisContextCurrent();

		if (!g_gladInited)
		{
			if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				glfwTerminate();
				GRLSetErrorInfo("glfwCreateWindow Failed.");
				return;
			}
			g_gladInited = true;
		}
		m_lastActiveAttrib = 0;
		return;
	}
	void GRLCOpenGLEnvironment::GetSize(uint32_t* width, uint32_t* height)
	{
		int swidth, sheight;
		glfwGetWindowSize(m_window, &swidth, &sheight);
		if (width) *width = swidth;
		if (height) *height = sheight;
	}
	GRL_RESULT GRLCOpenGLEnvironment::PollEvents()
	{
		glfwPollEvents();
		auto res = glfwWindowShouldClose(m_window);
		return res;
	}
	GRL_RESULT GRLCOpenGLEnvironment::Resize(uint32_t width, uint32_t height)
	{
		glfwSetWindowSize(m_window, width, height);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLEnvironment::CreateTexture(uint32_t width, uint32_t height, GRL_FORMAT format, GRLIGraphicsTexture** ppTexture)
	{
		MakeThisContextCurrent();
		*ppTexture = new GRLCOpenGLTexture(width, height, format, this);
		return GRL_FALSE;
	}
	GRLCOpenGLDepthMap::GRLCOpenGLDepthMap(uint32_t width, uint32_t height, GRLCOpenGLEnvironment* pEnvironment)
	{
		__Ref_Cnt = 1;
		m_width = width;
		m_height = height;
		m_environment.Set(pEnvironment);

		GRLOpenGLCall(glGenTextures, 1, &m_depthMap);
		GRLOpenGLCall(glBindTexture, GL_TEXTURE_2D, m_depthMap);
		GRLOpenGLCall(glTexImage2D,GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		GRLOpenGLCall(glBindTexture, GL_TEXTURE_2D, 0);
	}
	GRLCOpenGLDepthMap::~GRLCOpenGLDepthMap()
	{
		m_environment->MakeThisContextCurrent();
		GRLOpenGLCall(glDeleteTextures, 1, &m_depthMap);
	}
	GRL_RESULT GRLCOpenGLDepthMap::GetSize(uint32_t* pWidth, uint32_t* pHeight)
	{
		if (pWidth) *pWidth = m_width;
		if (pHeight) *pHeight = m_height;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLDepthMap::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLDepthMap::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt) delete this;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLDepthMap::QueryInterface(GRLUUID Uuid, void** ppObject)
	{
		if (Uuid == GRLID_IBase) *ppObject = reinterpret_cast<GRLIBase*>(this);
		else if (Uuid == GRLID_IGraphicsDepthMap) *ppObject = reinterpret_cast<GRLIGraphicsDepthMap*>(this);
		else if (Uuid == GRLID_COpenGLDepthMap) *ppObject = reinterpret_cast<GRLCOpenGLDepthMap*>(this);
		else return GRL_TRUE;
		AddRef();
		return GRL_FALSE;
	}
	
	Grassland::GRLCOpenGLTexture::GRLCOpenGLTexture(uint32_t width, uint32_t height, GRL_FORMAT format, GRLCOpenGLEnvironment* pEnvironment)
	{
		__Ref_Cnt = 1;
		GRLOpenGLCall(glGenTextures, 1, &m_texture);
		m_width = width;
		m_height = height;
		m_format = format;
		m_internalFormat = GRLFormatToOpenGLSizedFormat(format);
		GRLOpenGLCall(glBindTexture, GL_TEXTURE_2D, m_texture);
		GRLOpenGLCall(glTexImage2D, GL_TEXTURE_2D, 0, m_internalFormat, width, height, 0, GRLFormatToOpenGLBaseFormat(format), GRLFormatToOpenGLElementType(format), nullptr);
		GRLOpenGLCall(glBindTexture, GL_TEXTURE_2D, 0);
		m_environment.Set(pEnvironment);
	}

	GRL_RESULT Grassland::GRLCOpenGLTexture::WritePixels(void* pData)
	{
		m_environment->MakeThisContextCurrent();
		uint32_t pixelSize = GRLFormatSizeInBytes(m_format);
		uint8_t *pReversedData = new uint8_t [m_width * m_height * pixelSize];
		uint8_t* pRealData = reinterpret_cast<uint8_t*>(pData);
		for (int i = 0; i < m_height; i++)
			memcpy(pReversedData + i * (m_width * pixelSize), pRealData + (m_height - 1 - i) * (m_width * pixelSize), pixelSize * m_width);
		GRLOpenGLCall(glTextureSubImage2D, m_texture,
			0,
			0,
			0,
			m_width,
			m_height,
			GRLFormatToOpenGLBaseFormat(m_format),
			GRLFormatToOpenGLElementType(m_format),
			pReversedData);
		delete[] pReversedData;
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLTexture::ReadPixels(void* pData)
	{
		m_environment->MakeThisContextCurrent();
		uint32_t pixelSize = GRLFormatSizeInBytes(m_format);
		uint8_t* pReversedData = new uint8_t[m_width * m_height * pixelSize];
		uint8_t* pRealData = reinterpret_cast<uint8_t*>(pData);
		GRLOpenGLCall(glGetTextureImage, m_texture, 0,
			GRLFormatToOpenGLBaseFormat(m_format),
			GRLFormatToOpenGLElementType(m_format),
			m_width * m_height * GRLFormatSizeInBytes(m_format),
			pReversedData);
		for (int i = 0; i < m_height; i++)
			memcpy(pRealData + i * (m_width * pixelSize), pReversedData + (m_height - 1 - i) * (m_width * pixelSize), pixelSize * m_width);
		delete[] pReversedData;
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLTexture::GetSize(uint32_t* pWidth, uint32_t* pHeight)
	{
		if (pWidth) *pWidth = m_width;
		if (pHeight) *pHeight = m_height;
		return GRL_FALSE;
	}

	GRL_FORMAT GRLCOpenGLTexture::GetFormat()
	{
		return m_format;
	}

	Grassland::GRLCOpenGLTexture::~GRLCOpenGLTexture()
	{
		m_environment->MakeThisContextCurrent();
		GRLOpenGLCall(glDeleteTextures, 1, &m_texture);
	}

	GRL_RESULT GRLCOpenGLTexture::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLTexture::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt) delete this;
		return GRL_FALSE;
	}

	GRL_RESULT GRLCOpenGLTexture::QueryInterface(GRLUUID Uuid, void** ppObject)
	{
		if (Uuid == GRLID_IBase) *ppObject = reinterpret_cast<GRLIBase*>(this);
		else if (Uuid == GRLID_IGraphicsTexture) *ppObject = reinterpret_cast<GRLIGraphicsTexture*>(this);
		else if (Uuid == GRLID_COpenGLTexture) *ppObject = reinterpret_cast<GRLCOpenGLTexture*>(this);
		else return GRL_TRUE;
		AddRef();
		return GRL_FALSE;
	}
	GRLCOpenGLPipelineState::GRLCOpenGLPipelineState(const char* shader_path, GRL_GRAPHICS_PIPELINE_STATE_DESC* desc, GRLCOpenGLEnvironment* pEnvironment)
	{
		__Ref_Cnt = 1;
		m_cullFace = desc->cullFace;
		m_enableBlend = desc->enableBlend;
		m_enableDepthTest = desc->enableDepthTest;
		m_environment = pEnvironment;
		std::string str = shader_path, fragment_shader_path, vertex_shader_path;
		fragment_shader_path = str + "/fragment_shader.glsl";
		vertex_shader_path = str + "/vertex_shader.glsl";

		uint32_t
			fragmentShader = GRLOpenGLCompileShaderFromFile(fragment_shader_path.c_str(), GL_FRAGMENT_SHADER),
			vertexShader = GRLOpenGLCompileShaderFromFile(vertex_shader_path.c_str(), GL_VERTEX_SHADER);

		if (!fragmentShader || !vertexShader) return;

		m_shaderProgram = glCreateProgram();
		GRLOpenGLCall(glAttachShader, m_shaderProgram, fragmentShader);
		GRLOpenGLCall(glAttachShader, m_shaderProgram, vertexShader);

		GRLOpenGLCall(glLinkProgram, m_shaderProgram);
		int32_t success;
		GRLOpenGLCall(glGetProgramiv, m_shaderProgram, GL_LINK_STATUS, &success);
		if (success != GL_TRUE)
		{
			int32_t log_length = 0;
			char message[1024];
			GRLOpenGLCall(glGetProgramInfoLog, m_shaderProgram, 1000, &log_length, message);
			GRLSetErrorInfo("Program Link Failed.");
			std::cout << "Reason : " << message << std::endl;
			return;
		}

		GRLOpenGLCall(glDeleteShader, fragmentShader);
		GRLOpenGLCall(glDeleteShader, vertexShader);

		m_numTexture = desc->numTexture;
		m_numRenderTargets = desc->numRenderTargets;
		m_numConstantBuffer = desc->numConstantBuffer;
		m_numInputElement = desc->numInputElement;

		m_strideSizeInBytes = 0;
		m_inputLayout.clear();
		for (int index = 0; index < desc->numInputElement; index++)
		{
			m_strideSizeInBytes += GRLFormatSizeInBytes(desc->inputElementLayout[index]);
			m_inputLayout.push_back(desc->inputElementLayout[index]);
		}

		GRLOpenGLCall(glGenFramebuffers, 1, &m_frameBuffer);
		m_workingTextures.clear();
		m_mapTextureLocation.clear();
		m_samplerDesc.clear();
		for (int index = 0; index < desc->numTexture; index++)
			m_samplerDesc.push_back(desc->samplerDesc[index]);
	}
	GRLCOpenGLPipelineState::~GRLCOpenGLPipelineState()
	{
		m_environment->MakeThisContextCurrent();
		GRLOpenGLCall(glDeleteProgram, m_shaderProgram);
		GRLOpenGLCall(glDeleteFramebuffers, 1, &m_frameBuffer);
	}
	GRL_RESULT GRLCOpenGLPipelineState::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLPipelineState::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt) delete this;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLPipelineState::QueryInterface(GRLUUID Uuid, void** ppObject)
	{
		if (Uuid == GRLID_IBase) *ppObject = reinterpret_cast<GRLIBase*>(this);
		else if (Uuid == GRLID_IGraphicsPipelineState) *ppObject = reinterpret_cast<GRLIGraphicsPipelineState*>(this);
		else if (Uuid == GRLID_COpenGLPipelineState) *ppObject = reinterpret_cast<GRLCOpenGLPipelineState*>(this);
		else return GRL_TRUE;
		AddRef();
		return GRL_FALSE;
	}
	int GRLCOpenGLPipelineState::GetTextureLocation(int textureIndex)
	{
		if (!m_mapTextureLocation.count(textureIndex))
		{
			std::string textureName = "texture" + std::to_string(textureIndex);
			m_mapTextureLocation[textureIndex] = glGetUniformLocation(m_shaderProgram, textureName.c_str());
		}
		return m_mapTextureLocation[textureIndex];
	}
	GRLCOpenGLBuffer::GRLCOpenGLBuffer(uint64_t size, GRL_GRAPHICS_BUFFER_TYPE type, GRL_GRAPHICS_BUFFER_USAGE usage, void * pData, GRLCOpenGLEnvironment* pEnvironment)
	{
		__Ref_Cnt = 1;
		m_type = type;
		m_size = size;
		m_buffer = 0;
		GRLOpenGLCall(glGenBuffers, 1, &m_buffer);
		m_glType = GRLBufferTypeToOpenGLBufferType(type);
		GRLOpenGLCall(glBindBuffer, m_glType, m_buffer);

		if (usage == GRL_GRAPHICS_BUFFER_USAGE::DEFAULT)
			if (type == GRL_GRAPHICS_BUFFER_TYPE::CONSTANT)
				usage = GRL_GRAPHICS_BUFFER_USAGE::DYNAMIC;
			else
				usage = GRL_GRAPHICS_BUFFER_USAGE::STATIC;
		m_usage = usage;
		if (usage == GRL_GRAPHICS_BUFFER_USAGE::DYNAMIC)
			m_glUsage = GL_DYNAMIC_DRAW;
		else
			m_glUsage = GL_STATIC_DRAW;
		GRLOpenGLCall(glBufferData, m_glType, size, pData, m_glUsage);
		GRLOpenGLCall(glBindBuffer, m_glType, 0);
		m_environment = pEnvironment;
	}
	GRLCOpenGLBuffer::~GRLCOpenGLBuffer()
	{
		m_environment->MakeThisContextCurrent();
		GRLOpenGLCall(glDeleteBuffers, 1, &m_buffer);
	}
	GRL_RESULT GRLCOpenGLBuffer::WriteData(uint64_t size, uint64_t offset, void* pData)
	{
		m_environment->MakeThisContextCurrent();
		GRLOpenGLCall(glNamedBufferSubData, m_buffer, offset, size, pData);
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLBuffer::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLBuffer::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt) delete this;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCOpenGLBuffer::QueryInterface(GRLUUID Uuid, void** ppObject)
	{
		if (Uuid == GRLID_IBase) *ppObject = reinterpret_cast<GRLIBase*>(this);
		else if (Uuid == GRLID_IGraphicsBuffer) *ppObject = reinterpret_cast<GRLIGraphicsBuffer*>(this);
		else if (Uuid == GRLID_COpenGLBuffer) *ppObject = reinterpret_cast<GRLCOpenGLBuffer*>(this);
		else return GRL_TRUE;
		AddRef();
		return GRL_FALSE;
	}
}