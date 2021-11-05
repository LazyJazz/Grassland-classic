#pragma once
#include "GLHeader.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace OpenGL
		{
			class VertexArray
			{
			public:
				VertexArray();
				~VertexArray();
			private:
				uint32_t __vertex_array_object;
				uint32_t __vertex_buffer_object;
				uint32_t __element_buffer_object;
			};
		}
	}
}