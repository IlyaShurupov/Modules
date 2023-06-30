#pragma once

#include "Camera.hpp"
#include "Buffer.hpp"

namespace tp {

	class TrigCache {
		static Vec3F gHitPos;

	public:
		ualni mP1, mP2, mP3;
		Vec3F mEdgeP1P2, mEdgeP1P3;
		Vec3F mNormal;
		Vec3F mOrigin;

	public:
		TrigCache();
		TrigCache(const TrigCache& in);
		TrigCache(ualni v1, ualni v2, ualni v3);

	public:
		static const Vec3F& getHitPos() ;
		[[nodiscard]] const Vec3F& getNormal() const;

	public:
		void updateCache(const Buffer<Vec3F>& points);
		[[nodiscard]] bool castRay(const Ray& ray) const;
	};

	class Topology {
		mat3f mBasis;
		Vec3F mOrigin;

		Buffer<Vec3F> mPoints;
		Buffer<Vec3F> mPointsTransformed;
		Buffer<TrigCache> mTrigCaches;

	public:
		Topology() = default;
		~Topology() = default;

	public:
		void addTrig(const Vec3F& v1, const Vec3F& v2, const Vec3F& v3);
		void transformPoint(Vec3F& vert);
		void updateTransformed();
	};
}