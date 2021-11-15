#include "GLText.h"

namespace Grassland
{
	struct __grl_char_index
	{
	public:
		uint32_t __set_pixel_width;
		uint32_t __set_pixel_height;
		wchar_t __char;
		bool operator < (const __grl_char_index& index2) const
		{
			return (__set_pixel_width < index2.__set_pixel_width) ?
				true :
				(
					(__set_pixel_width == index2.__set_pixel_width) ?
					((__set_pixel_height < index2.__set_pixel_height) ? true : (
						(__set_pixel_height == index2.__set_pixel_height) ?
						(__char < index2.__char) :
						false
						)) :
					false
					);
		}
	};

	struct __grl_char_asset
	{
	public:
		uint32_t __char_tex;
		uint32_t __char_width;
		uint32_t __char_height;
		int32_t __char_bearingX;
		int32_t __char_bearingY;
		int32_t __char_advance;
	};

	FT_Library g_ft_library;

	uint32_t g_pixel_width, g_pixel_height;
	

	struct __grl_char_factory
	{
		FT_Face g_ft_face;
		std::map<std::string, FT_Face> g_map_font_face;
		std::map<std::string, std::map<__grl_char_index, __grl_char_asset>*> g_map_font_char_map;
		std::map<__grl_char_index, __grl_char_asset>* g_map_char_asset;

		GRL_RESULT __grl_set_font(const char* font_path)
		{
			if (g_map_font_face.count(font_path))
			{
				g_ft_face = g_map_font_face[font_path];
			}
			else
			{
				FT_Face newface;
				if (FT_New_Face(g_ft_library, font_path, 0, &newface))
				{
					GRLSetErrorInfo("Grassland_FREETYPE: Failed to load font");
					return GRL_TRUE;
				}
				g_map_font_face[font_path] = g_ft_face = newface;
			}
			if (!g_map_font_char_map.count(font_path))
				g_map_font_char_map[font_path] = new std::map<__grl_char_index, __grl_char_asset>;
			g_map_char_asset = g_map_font_char_map[font_path];
			FT_Set_Pixel_Sizes(g_ft_face, g_pixel_width, g_pixel_height);
#ifdef _DEBUG
			std::cout << "Using Font:" << g_ft_face->family_name << " " << g_ft_face->style_name << std::endl;
#endif
			return GRL_FALSE;
		}

		GRL_RESULT __grl_set_size(uint32_t pixel_width, uint32_t pixel_height)
		{
			FT_Set_Pixel_Sizes(g_ft_face, g_pixel_width, g_pixel_height);
			return GRL_FALSE;
		}

		__grl_char_asset __grl_opengl_text_get_char_asset(wchar_t __Char)
		{
			//puts("Enter-------------------------");
			__grl_char_index __char_idx;
			__char_idx.__set_pixel_width = g_pixel_width;
			__char_idx.__set_pixel_height = g_pixel_height;
			__char_idx.__char = __Char;
			if (!g_map_char_asset)
			{
				GRLSetErrorInfo("char asset map is nullptr.");
				return __grl_char_asset();
			}
			if (g_map_char_asset->count(__char_idx))
				return (*g_map_char_asset)[__char_idx];
			__grl_char_asset __char_asset = {};
			if (FT_Load_Char(g_ft_face, __Char, FT_LOAD_RENDER))
				return __char_asset;

			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glGenTextures(1, &__char_asset.__char_tex);
			glBindTexture(GL_TEXTURE_2D, __char_asset.__char_tex);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				g_ft_face->glyph->bitmap.width,
				g_ft_face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				g_ft_face->glyph->bitmap.buffer
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			__char_asset.__char_width = g_ft_face->glyph->bitmap.width;
			__char_asset.__char_height = g_ft_face->glyph->bitmap.rows;
			__char_asset.__char_bearingX = g_ft_face->glyph->bitmap_left;
			__char_asset.__char_bearingY = g_ft_face->glyph->bitmap_top;
			__char_asset.__char_advance = g_ft_face->glyph->advance.x;
			glBindTexture(GL_TEXTURE_2D, 0);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
			(*g_map_char_asset)[__char_idx] = __char_asset;
			return __char_asset;
		}
	}grl_text_unicode_factory, grl_text_ascii_factory;

	GRLPtr<GRLIOpenGLVertexArray> g_text_vertex_array;
	GRLPtr<GRLIOpenGLProgram> g_text_program;

	GRLColor g_text_color;


	GRL_RESULT GRLOpenGLTextInit()
	{
		if (FT_Init_FreeType(&g_ft_library)) {
			GRLSetErrorInfo("Grassland_FREETYPE: Could not init FreeType Library");
			return GRL_TRUE;
		}

		if (GRLCreateOpenGLProgramFromSourceFile(
			"shaders/OpenGL/gl2dtextvs.glsl",
			"shaders/OpenGL/gl2dtextfs.glsl",
			nullptr,
			&g_text_program
		)) return GRL_TRUE;

		if (GRLCreateOpenGLVertexArray(
			&g_text_vertex_array
		)) return GRL_TRUE;

		uint32_t ibuffer[] = {
			0, 1, 2,
			3 ,2, 1
		};

		float vbuffer[] = {
			0.0, 0.0,
			0.0, 1.0,
			1.0, 0.0,
			1.0, 1.0
		};

		g_text_vertex_array->BindIndicesData(ibuffer, 6, GRL_OPENGL_BUFFER_USAGE_STATIC);
		g_text_vertex_array->BindVerticesData(vbuffer, 8, GRL_OPENGL_BUFFER_USAGE_STATIC);
		g_text_vertex_array->ActiveVerticesLayout(0, 2, GRL_TYPE_FLOAT, sizeof(float) * 2, 0);
		g_text_program->SetInt("texture0", 0);
		g_text_program->SetMat3("transMat", GRLMat3(1.0));
		g_text_program->SetVec4("texColor", GRLVec4(1.0, 1.0, 1.0, 1.0));
		g_text_color = GRLColor(1.0, 1.0, 1.0, 1.0);
		g_pixel_width = 0;
		g_pixel_height = 32;
		return GRL_FALSE;
	}

	GRL_RESULT GRLOpenGLTextInit(const char* ascii_font, const char* non_ascii_font, int32_t pixel_width, int32_t pixel_height)
	{
		if (GRLOpenGLTextInit()) return GRL_TRUE;
		if (GRLOpenGLTextSetASCIIFont(ascii_font)) return GRL_TRUE;
		if (!non_ascii_font) non_ascii_font = ascii_font;
		if (GRLOpenGLTextSetNonASCIIFont(non_ascii_font)) return GRL_TRUE;
		if (GRLOpenGLTextSetSize(pixel_width, pixel_height)) return GRL_TRUE;
		return GRL_FALSE;
	}

	GRL_RESULT GRLOpenGLTextSetASCIIFont(const char* font_path)
	{
		return grl_text_ascii_factory.__grl_set_font(font_path);
	}

	GRL_RESULT GRLOpenGLTextSetNonASCIIFont(const char* font_path)
	{
		return grl_text_unicode_factory.__grl_set_font(font_path);
	}

	GRL_RESULT GRLOpenGLTextSetFont(const char* font_path)
	{
		if (GRLOpenGLTextSetASCIIFont(font_path))
			return GRL_TRUE;
		if (GRLOpenGLTextSetNonASCIIFont(font_path))
			return GRL_TRUE;
		return GRL_FALSE;
	}

	GRL_RESULT GRLOpenGLTextSetSize(uint32_t pixel_width, uint32_t pixel_height)
	{
		g_pixel_width = pixel_width;
		g_pixel_height = pixel_height;
		if (grl_text_unicode_factory.__grl_set_size(pixel_width, pixel_height))
			return GRL_TRUE;
		if (grl_text_ascii_factory.__grl_set_size(pixel_width, pixel_height))
			return GRL_TRUE;
		return GRL_FALSE;
	}

	GRL_RESULT GRLOpenGLTextRender(float x, float y, const wchar_t* wtext, int32_t length)
	{
		GRLOpenGLSetBlendState(true);
		int32_t scr_width, scr_height;
		GRLOpenGLGetFrameBufferSize(&scr_width, &scr_height);
		float scalex = 1.0f / (float)scr_width, scaley = 1.0f / (float)scr_height;
		float base_x = x, base_y = y;

		g_text_program->Use();

		int32_t uniform_idx = g_text_program->GetUniformLocation("transMat");

		glActiveTexture(GL_TEXTURE0);
		if (length < 0)
			for (length = 0; wtext[length]; length++);
		
		for (int i = 0; i < length; i++)
		{
			wchar_t __Char = wtext[i];
			__grl_char_asset char_asset;
			if (__Char < 128u)
				char_asset = grl_text_ascii_factory.__grl_opengl_text_get_char_asset(__Char);
			else
				char_asset = grl_text_unicode_factory.__grl_opengl_text_get_char_asset(__Char);
			float 
				x0 = base_x + char_asset.__char_bearingX,
				y0 = base_y - char_asset.__char_bearingY,
				x1 = x0 + char_asset.__char_width,
				y1 = y0 + char_asset.__char_height;
			x0 *= scalex; x0 = x0 * 2.0 - 1.0;
			y0 *= scaley; y0 = y0 * -2.0 + 1.0;
			x1 *= scalex; x1 = x1 * 2.0 - 1.0;
			y1 *= scaley; y1 = y1 * -2.0 + 1.0;
			g_text_program->SetMat3(uniform_idx, 
				GRLMat3(
					x1 - x0, 0.0, x0,
					0.0, y1 - y0, y0,
					0.0, 0.0, 1.0
				));
			glBindTexture(GL_TEXTURE_2D, char_asset.__char_tex);
			g_text_vertex_array->Render();
			base_x += char_asset.__char_advance / 64.0f;
		}
		return GRL_FALSE;
	}

	GRL_RESULT Grassland::GRLOpenGLTextRender(float x, float y, std::wstring wtext)
	{
		return GRLOpenGLTextRender(x, y, wtext.c_str(), wtext.length());
	}

	GRL_RESULT Grassland::GRLOpenGLTextRender(float x, float y, std::string text)
	{
		return GRLOpenGLTextRender(x, y, GRLStringUTF8toUnicode(text));
	}

	GRL_RESULT Grassland::GRLOpenGLTextRender(float x, float y, const char * text)
	{
		return GRLOpenGLTextRender(x, y, GRLStringUTF8toUnicode(text));
	}

	void GRLOpenGLTextSetColor(GRLColor color)
	{
		g_text_color = color;
		g_text_program->SetVec4("texColor", GRLVec4(color.r, color.g, color.b, color.a));
	}

	GRLColor GRLOpenGLTextGetColor()
	{
		return g_text_color;
	}
}