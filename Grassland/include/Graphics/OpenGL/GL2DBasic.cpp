#include "GL2DBasic.h"

namespace Grassland
{
	GRLPtr<GRLIOpenGLProgram> g_ellipse_program;
	GRLPtr<GRLIOpenGLVertexArray> g_ellipse_vertex_array;
	GRLColor g_2d_draw_color;

	GRL_RESULT GRLOpenGL2DBasicInit()
	{
		if (GRLCreateOpenGLProgramFromSourceFile(
			"shaders/OpenGL/gl2dellipsevs.glsl",
			"shaders/OpenGL/gl2dellipsefs.glsl",
			nullptr,
			&g_ellipse_program
		)) return GRL_TRUE;

		if (GRLCreateOpenGLVertexArray(
			&g_ellipse_vertex_array
		)) return GRL_TRUE;
		float vbuffer[] = {
			-1.0, -1.0,
			-1.0,  1.0,
			 1.0, -1.0,
			 1.0,  1.0
		};
		uint32_t ibuffer[] = {
			0, 1, 2,
			1, 3, 2
		};
		g_ellipse_vertex_array->BindIndicesData(ibuffer, 6, GRL_OPENGL_BUFFER_USAGE_STATIC);
		g_ellipse_vertex_array->BindVerticesData(vbuffer, 8, GRL_OPENGL_BUFFER_USAGE_STATIC);
		g_ellipse_vertex_array->ActiveVerticesLayout(0, 2, GRL_TYPE_FLOAT, sizeof(float) * 2, 0);
		return GRL_FALSE;
	}
	GRL_RESULT GRLOpenGL2DSetDrawColor(GRLColor color)
	{
		g_2d_draw_color = color;
		return GRL_FALSE;
	}
	GRLColor GRLOpenGL2DGetDrawColor()
	{
		return g_2d_draw_color;
	}

	GRL_RESULT GRLOpenGL2DDrawEllipse(float originx, float originy, float x_axis, float y_axis)
	{
		originy = 1.0 - originy;
		originx = originx * 2.0 - 1.0;
		originy = originy * 2.0 - 1.0;
		x_axis *= 2.0;
		y_axis *= 2.0;
		g_ellipse_program->SetMat3("transformMat", 
			GRLMat3(
				x_axis, 0.0, originx,
				0.0, y_axis, originy,
				0.0, 0.0, 1.0
			));
		g_ellipse_program->SetVec4("drawColor", GRLVec4(
			g_2d_draw_color.r,
			g_2d_draw_color.g,
			g_2d_draw_color.b,
			g_2d_draw_color.a
		));
		GRLOpenGLSetBlendState(g_2d_draw_color.a < 1.0);
		g_ellipse_program->Use();
		g_ellipse_vertex_array->Render();
		return GRL_FALSE;
	}
}