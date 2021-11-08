#include "GLVertexArray.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace OpenGL
		{
			uint32_t __OpenGLLifecycleEnumToMacro(GRL_OPENGL_BUFFER_USAGE usage)
			{
				switch (usage)
				{
				case GRL_OPENGL_BUFFER_USAGE_DYNAMIC:
					return GL_DYNAMIC_DRAW;
				case GRL_OPENGL_BUFFER_USAGE_STATIC:
					return GL_STATIC_DRAW;
				case GRL_OPENGL_BUFFER_USAGE_STREAM:
					return GL_STREAM_DRAW;
				}
				return 0;
				;
			}

			VertexArray::VertexArray()
			{
				glGenVertexArrays(1, &__vertex_array_object);
				glGenBuffers(1, &__vertex_buffer_object);
				glGenBuffers(1, &__element_buffer_object);
				glBindVertexArray(__vertex_array_object);
				glBindBuffer(GL_ARRAY_BUFFER, __vertex_buffer_object);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, __element_buffer_object);
				glBindVertexArray(0);
				__indices_cnt = 0;
			}
			VertexArray::~VertexArray()
			{
				glDeleteVertexArrays(1, &__vertex_array_object);
				glDeleteBuffers(1, &__vertex_buffer_object);
				glDeleteBuffers(1, &__element_buffer_object);
			}
			void VertexArray::BindVerticesData(float* vertices_data, int32_t data_count, GRL_OPENGL_BUFFER_USAGE usage)
			{
				glBindVertexArray(__vertex_array_object);
				glBufferData(
					GL_ARRAY_BUFFER,
					sizeof(float) * data_count,
					vertices_data,
					__OpenGLLifecycleEnumToMacro(usage)
					);
				glBindVertexArray(0);
			}
			void VertexArray::BindIndicesData(uint32_t* indices_data, int32_t data_count, GRL_OPENGL_BUFFER_USAGE usage)
			{
				glBindVertexArray(__vertex_array_object);
				glBufferData(
					GL_ELEMENT_ARRAY_BUFFER,
					sizeof(uint32_t) * data_count,
					indices_data,
					__OpenGLLifecycleEnumToMacro(usage)
				);
				glBindVertexArray(0);
				__indices_cnt = data_count;
			}
			void VertexArray::ActiveVerticesLayout(int32_t slot, int32_t bundle_size, int32_t stride, int32_t offset)
			{
				glBindVertexArray(__vertex_array_object);
				glVertexAttribPointer(slot, bundle_size, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)(offset * sizeof(float)));
				glEnableVertexAttribArray(slot);
				glBindVertexArray(0);
			}
			void VertexArray::DisableSlot(int32_t slot)
			{
				glDisableVertexAttribArray(slot);
			}
			void VertexArray::Render()
			{
				glBindVertexArray(__vertex_array_object);
				glDrawElements(GL_TRIANGLES, __indices_cnt, GL_UNSIGNED_INT, nullptr);
				glBindVertexArray(0);
			}
		}
	}
}