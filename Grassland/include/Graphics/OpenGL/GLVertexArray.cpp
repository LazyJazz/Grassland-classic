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
				__vertex_array_object = __vertex_buffer_object = __element_buffer_object = 0;
				puts("Initializing");
				
				GRLGLCall(glGenVertexArrays(1, &__vertex_array_object));
				GRLGLCall(glGenBuffers(1, &__vertex_buffer_object));
				GRLGLCall(glGenBuffers(1, &__element_buffer_object));
				GRLGLCall(glBindVertexArray(__vertex_array_object));
				GRLGLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, __element_buffer_object));
				GRLGLCall(glBindVertexArray(0));//*/
				__indices_cnt = 0;
			}
			VertexArray::~VertexArray()
			{
				glDeleteVertexArrays(1, &__vertex_array_object);
				glDeleteBuffers(1, &__vertex_buffer_object);
				glDeleteBuffers(1, &__element_buffer_object);
			}
			void VertexArray::BindVerticesData(void* vertices_data, int32_t data_count, GRL_OPENGL_BUFFER_USAGE usage)
			{
				//GRLGLCall(glBindVertexArray(__vertex_array_object));
				GRLGLCall(glBindBuffer(GL_ARRAY_BUFFER, __vertex_buffer_object));
				GRLGLCall(glBufferData(
					GL_ARRAY_BUFFER,
					sizeof(float) * data_count,
					vertices_data,
					__OpenGLLifecycleEnumToMacro(usage)
					));
				//GRLGLCall(glBindVertexArray(0));
			}
			void VertexArray::BindIndicesData(void* indices_data, int32_t data_count, GRL_OPENGL_BUFFER_USAGE usage)
			{
				GRLGLCall(glBindVertexArray(__vertex_array_object));
				GRLGLCall(glBufferData(
					GL_ELEMENT_ARRAY_BUFFER,
					sizeof(uint32_t) * data_count,
					indices_data,
					__OpenGLLifecycleEnumToMacro(usage)
				));
				GRLGLCall(glBindVertexArray(0));
				__indices_cnt = data_count;
			}
			void VertexArray::ActiveVerticesLayout(int32_t slot, int32_t bundle_size, int32_t stride, int32_t offset)
			{
				GRLGLCall(glBindBuffer(GL_ARRAY_BUFFER, __vertex_buffer_object));
				GRLGLCall(glBindVertexArray(__vertex_array_object));
				GRLGLCall(glVertexAttribPointer(slot, bundle_size, GL_FLOAT, GL_FALSE, stride*sizeof(float), (void*)(offset * sizeof(float))));
				GRLGLCall(glBindVertexArray(0));
			}
			void VertexArray::EnableSlot(int32_t slot)
			{
				GRLGLCall(glBindVertexArray(__vertex_array_object));
				GRLGLCall(glEnableVertexAttribArray(slot));
				GRLGLCall(glBindVertexArray(0));
			}
			void VertexArray::DisableSlot(int32_t slot)
			{
				GRLGLCall(glBindVertexArray(__vertex_array_object));
				GRLGLCall(glDisableVertexAttribArray(slot));
				GRLGLCall(glBindVertexArray(0));
			}
			void VertexArray::Render()
			{
				GRLGLCall(glBindVertexArray(__vertex_array_object));
				GRLGLCall(glDrawElements(GL_TRIANGLES, __indices_cnt, GL_UNSIGNED_INT, nullptr));
				GRLGLCall(glBindVertexArray(0));
			}
		}
	}
}