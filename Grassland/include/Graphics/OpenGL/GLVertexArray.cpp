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
				GRLGLCall(glEnableVertexAttribArray(slot));
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

	class GRLCVertexArray : public GRLIVertexArray
	{
	public:
		GRLCVertexArray(Graphics::OpenGL::VertexArray* pVertexArray);
		~GRLCVertexArray();
		virtual void BindVerticesData(void* vertices_data, int32_t data_count, GRL_OPENGL_BUFFER_USAGE usage);
		virtual void BindIndicesData(void* indices_data, int32_t data_count, GRL_OPENGL_BUFFER_USAGE usage);
		virtual void ActiveVerticesLayout(int32_t slot, int32_t bundle_size, int32_t stride, int32_t offset);
		virtual void EnableSlot(int32_t slot);
		virtual void DisableSlot(int32_t slot);
		virtual void Render();
		virtual GRL_RESULT AddRef();
		virtual GRL_RESULT Release();
	private:
		Graphics::OpenGL::VertexArray* __vertex_array;
		int32_t __Ref_Cnt;
	};

	GRL_RESULT GRLCreateVertexArray(GRLIVertexArray** ppVertexArray)
	{
		if (ppVertexArray)
			*ppVertexArray = new GRLCVertexArray(
				new Graphics::OpenGL::VertexArray()
			);
		else
			return GRL_TRUE;
		return GRL_FALSE;
	}
	GRLCVertexArray::GRLCVertexArray(Graphics::OpenGL::VertexArray* pVertexArray)
	{
		__Ref_Cnt = 1;
		__vertex_array = pVertexArray;
	}
	GRLCVertexArray::~GRLCVertexArray()
	{
		delete __vertex_array;
	}
	void GRLCVertexArray::BindVerticesData(void* vertices_data, int32_t data_count, GRL_OPENGL_BUFFER_USAGE usage)
	{
		__vertex_array->BindVerticesData(vertices_data, data_count, usage);
	}
	void GRLCVertexArray::BindIndicesData(void* indices_data, int32_t data_count, GRL_OPENGL_BUFFER_USAGE usage)
	{
		__vertex_array->BindIndicesData(indices_data, data_count, usage);
	}
	void GRLCVertexArray::ActiveVerticesLayout(int32_t slot, int32_t bundle_size, int32_t stride, int32_t offset)
	{
		__vertex_array->ActiveVerticesLayout(slot, bundle_size, stride, offset);
	}
	void GRLCVertexArray::EnableSlot(int32_t slot)
	{
		__vertex_array->EnableSlot(slot);
	}
	void GRLCVertexArray::DisableSlot(int32_t slot)
	{
		__vertex_array->DisableSlot(slot);
	}
	void GRLCVertexArray::Render()
	{
		__vertex_array->Render();
	}
	GRL_RESULT GRLCVertexArray::AddRef()
	{
		__Ref_Cnt++;
		return GRL_FALSE;
	}
	GRL_RESULT GRLCVertexArray::Release()
	{
		__Ref_Cnt--;
		if (!__Ref_Cnt)
			delete this;
		return GRL_FALSE;
	}
}