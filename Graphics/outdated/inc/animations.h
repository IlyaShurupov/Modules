#pragma once

#include "rect.h"
#include "color.h"

namespace tp {
	namespace glw {

		extern bool gInTransition;

		struct AnimValue {
			halnf mValPrev = 0;
			halnf mVal = 0;
			time_ms mTimeStart = 0;
			halni mTimeAnim = 250;

			halnf interpolate() const;

			AnimValue();
			AnimValue(halnf val);

			halnf prev() { return mValPrev; }
			void set(halnf val);
			void setNoTransition(halnf);
			void setAnimTime(halni time);
			halnf get() const;
			halnf getTarget() const;
			bool inTransition() const;
			explicit operator halnf() const;
			void operator=(halnf val);
		};

		struct AnimRect : rect<AnimValue> {

			AnimRect() {
				set({ 0, 0, 0, 0 });
				setAnimTime(450);
			}

			rectf get() const;
			rectf getTarget() const;
			void setNoTransition(rectf);
			void set(const rectf&);
			void setAnimTime(halni time_ms);
		};

		struct AnimColor {
			AnimRect mColor;
			rgba get() const;
			void set(const rgba&);
		};
	};
};