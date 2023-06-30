#include "Camera.hpp"

using namespace tp;

Camera::Camera() {
	lookAtPoint({ 0, 0, 0 }, { 2, 0, 0 }, { 0, 0, 1 });
}

Vec3F Camera::getTarget() const { return mTarget; }

void Camera::offset_target(halnf val) { mTarget += (mPos - mTarget).normalize() * val; }

Vec3F Camera::getForward() const { return (mTarget - mPos).normalize(); }

Vec3F Camera::getUp() const { return mUp; }

Vec3F& Camera::getPos() { return mPos; }

halnf Camera::getFar() const { return mFar; }

halnf Camera::getNear() const { return mNear; }

halnf Camera::getRatio() const { return mRatio; }

void Camera::setRatio(halnf in) { mRatio = in; }

void Camera::setFOV(halnf in) { mFOV = in; }

halnf Camera::getFOV() const { return mFOV; }

Mat4F Camera::calculateTransformationMatrix() {
	return calculateProjectionMatrix() * calculateViewMatrix();
}

Mat4F Camera::calculateViewMatrix() {
	const Vec3F& F = (mPos - mTarget).unitV();
	const Vec3F& S = mUp * F;
	const Vec3F& U = F * S;
	const Vec3F& P = mPos;

	Mat4F out;
	out[0] = Vec4F(S.x, S.y, S.z, -P.dot(S));
	out[1] = Vec4F(U.x, U.y, U.z, -P.dot(U));
	out[2] = Vec4F(F.x, F.y, F.z, -P.dot(F));
	out[3] = Vec4F(0, 0, 0, 1);
	return out;
}

Mat4F Camera::calculateProjectionMatrix() const {
	auto r = (halnf) sqrt(mRatio);
	halnf c = 1 / r;
	auto s = halnf(1.f / tan(mFOV / 2.f));

	Mat4F out;
	out[0] = Vec4F(s * r, 0, 0, 0);
	out[1] = Vec4F(0, s * c, 0, 0);
	out[2] = Vec4F(0, 0, -2.f / (mFar - mNear), -(mFar + mNear) / (mFar - mNear));
	out[3] = Vec4F(0, 0, -1, 0);
	return out;
}

Vec3F Camera::project(Vec2F normalized) {
	auto camMat = calculateTransformationMatrix();
	auto inv = camMat.inv();

	halnf z = halnf((((mTarget - mPos).length() - mNear) / (mFar - mNear) - 1.f / 2) * 2.f);
	halnf w = halnf((mTarget - mPos).length());
	Vec4<halnf> world_pos4(normalized.x * w, normalized.y * w, z, w);

	return inv * world_pos4;
}

Vec2F Camera::project(const Vec3F& world) {
	Vec4F world_pos4(world.x, world.y, world.z, 1);
	Vec4F transformed = calculateViewMatrix() * world_pos4;
	transformed = calculateProjectionMatrix() * transformed;
	return { transformed[0] / transformed[3], transformed[1] / transformed[3] };
}

Vec2F Camera::project(const tp::Vec3F& world, const tp::Mat4F& viewMat, const tp::Mat4F& projMat) {
	Vec4F world_pos4(world.x, world.y, world.z, 1);
	Vec4F transformed = viewMat * world_pos4;
	transformed = projMat * transformed;
	return { transformed[0] / transformed[3], transformed[1] / transformed[3] };
}

void Camera::lookAtPoint(const Vec3F& aTarget, const Vec3F& aPos, Vec3F aUp) {
	if (aTarget == aPos) return;
	mPos = aPos;
	mTarget = aTarget;
	Vec3F f = (mPos - mTarget).normalize();
	mUp = f * (aUp.normalize() * f);
}

void Camera::zoom(halnf ratio) {
	ratio = abs(ratio);
	if (ratio < 0.1f) {
		return;
	}
	if (abs((mPos - mTarget).length2()) < 0.05f && ratio < 1.f) {
		return;
	}
	mPos = mTarget + (mPos - mTarget) * ratio;
	lookAtPoint(mTarget, mPos, mUp);
}

void Camera::move(Vec2F aPos, Vec2F aPrevPos) {
	Vec3F p1 = project(aPrevPos);
	Vec3F p2 = project(aPos);
	Vec3F move = p1 - p2;
	mPos += move;
	mTarget += move;
	lookAtPoint(mTarget, mPos, mUp);
}

void Camera::rotate(halnf angleX, halnf angleY) {
	Vec3F wup(0, 0, 1);
	mPos -= mTarget;

	mat3f rotZ = mat3f::rotatorDir(wup, angleX);
	mPos = rotZ * mPos;
	mUp = rotZ * mUp;

	Vec3F f = mPos.unitV();
	Vec3F s = mUp * f;

	mPos = mat3f::rotatorDir(s, -angleY) * mPos;

	mPos += mTarget;

	lookAtPoint(mTarget, mPos, mUp);
}