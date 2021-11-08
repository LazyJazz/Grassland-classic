#include "GLShaderProgram.h"

namespace Grassland
{
	namespace Graphics
	{
		namespace OpenGL
		{
			Program::Program()
			{
				__program_handle = NULL;
			}

			Program::~Program()
			{
				Clear();
			}

			void Program::Clear()
			{
				if (__program_handle) {
					glDeleteProgram(__program_handle);
					__program_handle = 0;
				}
				__uniform_location_map.clear();
			}
		}
	}
}