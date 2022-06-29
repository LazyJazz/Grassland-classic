#include "GL2DBasic.h"

namespace Grassland
{
	GRLPtr<GRLIOpenGLProgram> g_ellipse_program;
	GRLPtr<GRLIOpenGLVertexArray> g_ellipse_vertex_array;
	GRLPtr<GRLIOpenGLProgram> g_triangle_program;
	GRLPtr<GRLIOpenGLVertexArray> g_triangle_vertex_array;
	GRLPtr<GRLIOpenGLVertexArray> g_rectangle_vertex_array;
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

		if (GRLCreateOpenGLProgramFromSourceFile(
			"shaders/OpenGL/gl2dtrianglevs.glsl",
			"shaders/OpenGL/gl2dtrianglefs.glsl",
			nullptr,
			&g_triangle_program
		)) return GRL_TRUE;

		if (GRLCreateOpenGLVertexArray(
			&g_triangle_vertex_array
		)) return GRL_TRUE;

		if (GRLCreateOpenGLVertexArray(
			&g_rectangle_vertex_array
		)) return GRL_TRUE;
		g_triangle_vertex_array->BindIndicesData(ibuffer, 3, GRL_OPENGL_BUFFER_USAGE_STATIC);
		g_triangle_vertex_array->ActiveVerticesLayout(0, 2, GRL_TYPE_FLOAT, sizeof(float) * 2, 0);

		g_rectangle_vertex_array->BindIndicesData(ibuffer, 6, GRL_OPENGL_BUFFER_USAGE_STATIC);
		g_rectangle_vertex_array->ActiveVerticesLayout(0, 2, GRL_TYPE_FLOAT, sizeof(float) * 2, 0);

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

	GRL_RESULT GRLOpenGL2DDrawEllipse_s(float originx, float originy, float x_axis, float y_axis)
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
	GRL_RESULT GRLOpenGL2DDrawEllipse(float originx, float originy, float x_axis, float y_axis)
	{
		int32_t width, height;
		GRLOpenGLGetFrameBufferSize(&width, &height);
		float xcoe = 1.0f / (float) width, ycoe = 1.0f / (float)height;
		return GRLOpenGL2DDrawEllipse_s(originx * xcoe, originy * ycoe, x_axis * xcoe, y_axis * ycoe);
	}

	GRL_RESULT GRLOpenGL2DDrawCircle(float originx, float originy, float radius)
	{
		return GRLOpenGL2DDrawEllipse(originx, originy, radius, radius);
	}
	
	GRL_RESULT GRLOpenGL2DDrawTriangle_s(float x0, float y0, float x1, float y1, float x2, float y2)
	{
		y0 = 1.0 - y0;
		y1 = 1.0 - y1;
		y2 = 1.0 - y2;
		x0 = x0 * 2.0 - 1.0;
		x1 = x1 * 2.0 - 1.0;
		x2 = x2 * 2.0 - 1.0;
		y0 = y0 * 2.0 - 1.0;
		y1 = y1 * 2.0 - 1.0;
		y2 = y2 * 2.0 - 1.0;
		g_triangle_program->SetVec4("drawColor", GRLVec4(
			g_2d_draw_color.r,
			g_2d_draw_color.g,
			g_2d_draw_color.b,
			g_2d_draw_color.a
		));
		g_triangle_program->Use();
		float vbuffer[] = {
			x0, y0,
			x1, y1,
			x2, y2
		};
		GRLOpenGLSetBlendState(g_2d_draw_color.a < 1.0);
		g_triangle_vertex_array->BindVerticesData(vbuffer, 6, GRL_OPENGL_BUFFER_USAGE_DYNAMIC);
		g_triangle_vertex_array->Render();
		return GRL_FALSE;
	}
	GRL_RESULT GRLOpenGL2DDrawTriangle(float x0, float y0, float x1, float y1, float x2, float y2)
	{
		int32_t width, height;
		GRLOpenGLGetFrameBufferSize(&width, &height);
		float xcoe = 1.0f / (float)width, ycoe = 1.0f / (float)height;
		return GRLOpenGL2DDrawTriangle_s(
			x0 * xcoe, y0 * ycoe,
			x1 * xcoe, y1 * ycoe,
			x2 * xcoe, y2 * ycoe
		);
	}
	
	GRL_RESULT GRLOpenGL2DDrawRectangle_s(float x0, float y0, float x1, float y1)
	{
		y0 = 1.0 - y0;
		y1 = 1.0 - y1;
		x0 = x0 * 2.0 - 1.0;
		y0 = y0 * 2.0 - 1.0;
		x1 = x1 * 2.0 - 1.0;
		y1 = y1 * 2.0 - 1.0;
		g_triangle_program->Use();
		g_triangle_program->SetVec4("drawColor", GRLVec4(
			g_2d_draw_color.r,
			g_2d_draw_color.g,
			g_2d_draw_color.b,
			g_2d_draw_color.a
		));
		float vbuffer[] = {
			x0, y0,
			x0, y1,
			x1, y0,
			x1, y1
		};
		GRLOpenGLSetBlendState(g_2d_draw_color.a < 1.0);
		g_rectangle_vertex_array->BindVerticesData(vbuffer, 8, GRL_OPENGL_BUFFER_USAGE_DYNAMIC);
		g_rectangle_vertex_array->Render();
		return GRL_FALSE;
	}
	GRL_RESULT GRLOpenGL2DDrawRectangle(float x0, float y0, float x1, float y1)
	{
		int32_t width, height;
		GRLOpenGLGetFrameBufferSize(&width, &height);
		float xcoe = 1.0f / (float)width, ycoe = 1.0f / (float)height;
		return GRLOpenGL2DDrawRectangle_s(x0 * xcoe, y0 * ycoe, x1 * xcoe, y1 * ycoe);
	}
}