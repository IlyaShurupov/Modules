
#pragma once

#include "rect.h"
#include "color.h"

namespace tp {
	namespace glw {
		class fbuffer {

			uint4 mFrameBufferID = 0; // regroups 0, 1, or more textures, and 0 or 1 depth buffer.
			uint4 mTextureId = 0; // texture we're going to render to ( colour attachement #0 ) 
			uint4 mDepthBufferID = 0;
			uint4 mDrawBuffers[1];

			vec2f mSize;

		public:
			
			rgba mClearCol = 0.f;

			fbuffer(const vec2f& size);
			fbuffer(const vec2f& size, tp::uint1 samples);

			void beginDraw();
			void setViewport(const rectf& viewport);
			void clear();
			void endDraw();

			uint4 texId() const;
			uint4 buffId() const;

			const vec2f& getSize() const;
			uhalni sizeAllocatedMem() const;
			uhalni sizeUsedMem() const;

			~fbuffer();
		};
	};
};