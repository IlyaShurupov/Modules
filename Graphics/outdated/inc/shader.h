
#pragma once

namespace tp {
	namespace glw {
		class shader {

			uint4 programm;
			uint4 VertexShaderID;
			uint4 FragmentShaderID;
			uint4 GeometryShaderID;

		public:

			bool compile_shader(const char* ShaderCode, uint4 ShaderID);
			void load(const char* vert, const char* geom, const char* frag, bool paths);

			shader();

			void vert_bind_source(const char* vert_src);
			void frag_bind_source(const char* frag_src);
			void geom_bind_source(const char* geom_src);

			void compile();

			shader(const char* vertid, const char* geomid, const char* fragid, bool paths = true);
			void bind();
			uint4 getu(const char* uid);
			void unbind();
			~shader();

			alni sizeAllocatedMem();
			alni sizeUsedMem();
		};
	};
};