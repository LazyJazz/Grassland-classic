#include "GL2DImage.h"

namespace Grassland
{
	GRLPtr<GRLIOpenGLProgram> g_img_program;
	GRLPtr<GRLIOpenGLVertexArray> g_img_vertex_array;

	GRL_RESULT GRLOpenGL2DImageInit()
	{
		if (GRLCreateOpenGLProgramFromSourceFile(
			"shaders/OpenGL/gl2dimagevs.glsl",
			"shaders/OpenGL/gl2dimagefs.glsl",
			nullptr,
			&g_img_program
		)) return TRUE;

		if (GRLCreateOpenGLVertexArray(&g_img_vertex_array))
			return TRUE;

		float vbuffer[] = {
			0.0, 0.0,
			0.0, 1.0,
			1.0, 0.0,
			1.0, 1.0
		};

		uint32_t ibuffer[] = {
			0, 1, 2,
			1, 3, 2
		};

		g_img_vertex_array->ActiveVerticesLayout(0, 2, GRL_TYPE_FLOAT, sizeof(float) * 2, 0);
		g_img_vertex_array->EnableSlot(0);
		g_img_vertex_array->BindIndicesData(ibuffer, 6, GRL_OPENGL_BUFFER_USAGE_STATIC);
		g_img_vertex_array->BindVerticesData(vbuffer, 8, GRL_OPENGL_BUFFER_USAGE_STATIC);

		return GRL_FALSE;
	}

	GRL_RESULT GRLOpenGL2DPutImage_s(float x, float y, float width, float height, GRLIOpenGLTexture* pTexture)
	{
		y = 1.0 - (y + height);
		x = x * 2.0 - 1.0;
		y = y * 2.0 - 1.0;
		width *= 2.0;
		height *= 2.0;

		g_img_program->SetVec2("vOrigin", GRLVec2(x, y));
		g_img_program->SetVec2("vSize", GRLVec2(width, height));
		g_img_program->SetInt("texture0", 0);
		g_img_program->Use();
		pTexture->BindTexture(0);
		g_img_vertex_array->Render();
		return GRL_FALSE;
	}

	GRL_RESULT GRLOpenGL2DPutImage(float x, float y, float width, float height, GRLIOpenGLTexture* pTexture)
	{
		int scr_width, scr_height;
		GRLOpenGLGetFrameBufferSize(&scr_width, &scr_height);
		float coe_width = 1.0f / (float)scr_width, coe_height = 1.0f / (float)scr_height;
		//std::cout << scr_width << " " << scr_height << std::endl;
		return GRLOpenGL2DPutImage_s(x * coe_width, y * coe_height, width * coe_width, height * coe_height, pTexture);
	}

	GRL_RESULT GRLOpenGL2DPutImage(float x, float y, GRLIOpenGLTexture* pTexture)
	{
		return GRLOpenGL2DPutImage(x, y, pTexture->GetWidth(), pTexture->GetHeight(), pTexture);
	}
}


