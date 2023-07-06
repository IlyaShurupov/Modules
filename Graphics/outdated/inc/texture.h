
#pragma once

#include "array2d.h"
#include "color.h"
#include "vec.h"

namespace tp {
	namespace glw {
		class texture {
			uint4 id;
			public:

			texture();
			~texture();

			uint4 getid();
			void update(const Array2D<rgba>& buff);
			void draw(const uint4& out = 0);

			alni sizeAllocatedMem();
			alni sizeUsedMem();

			static void init();
			static void deinit();
			static void draw_texture(uint4 out, uint4 in);
			static uint4 get_tex(const char* TexId);
			static void drawCurcle(vec2f pos, double radius, rgba col);
		};
	};
};