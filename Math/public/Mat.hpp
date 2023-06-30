#pragma once

#include "Vec.hpp"

namespace tp {

	template <typename Type, const halni tNRows, const halni tNColoumns>
	class Mat {
		typedef Vec<Type, tNRows> MVec;
		
	private:
		MVec mCol[tNColoumns];

	public:
		Mat() = default;

		explicit Mat(const Type& val) {
			operator=(val);
		}

		Mat(const Mat& in) {
			operator=(in);
		}

		MVec& operator[](alni i) {
			DEBUG_ASSERT(i < tNColoumns && i >= 0)
			return mCol[i];
		}

		const MVec& operator[](alni i) const {
			DEBUG_ASSERT(i < tNColoumns && i >= 0)
			return mCol[i];
		}

		Mat& operator=(const Mat& in) {
			if (&in == this) return *this;
			memcp(this, &in, sizeof(Mat<Type, tNRows, tNColoumns>));
			return *this;
		}

		Mat& operator=(const Type& val) {
			clear(val);
			return *this;
		}

		void clear(const Type& val) {
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					(*this)[i][j] = val;
				}
			}
		}

		void setDiagonal(const Type& val) {
			halni len = min(tNColoumns, tNRows);
			for (halni i = 0; i < len; i++) {
				(*this)[i][i] = val;
			}
		}

		Mat& fillRandom() {
			DEBUG_ASSERT(0)
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					(*this)[i][j] = (Type) 0;
				}
			}
			return *this;
		}

		Mat operator-() {
			Mat out;
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					out[i][j] = -(*this)[i][j];
				}
			}
			return out;
		}

		Mat& operator+=(const Mat& in) {
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					(*this)[i][j] += in[i][j];
				}
			}
			return *this;
		}

		Mat& operator-=(const Mat& in) {
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					(*this)[i][j] -= in[i][j];
				}
			}
			return *this;
		}

		Mat& operator+=(const Type& val) {
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					(*this)[i][j] += val;
				}
			}
			return *this;
		}

		Mat& operator-=(const Type& val) {
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					(*this)[i][j] -= val;
				}
			}
			return *this;
		}

		Mat& operator/=(const Type& val) {
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					(*this)[i][j] /= val;
				}
			}
			return *this;
		}

		Mat& operator*=(const Type& val) {
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					(*this)[i][j] *= val;
				}
			}
			return *this;
		}

		Mat operator+(const Mat& in) {
			Mat out;
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					out[i][j] = (*this)[i][j] + in[i];
				}
			}
			return out;
		}

		Mat operator-(const Mat& in) {
			Mat out;
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					out[i][j] = (*this)[i][j] - in[i];
				}
			}
			return out;
		}

		Mat operator+(const Type& val) {
			Mat out;
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					out[i][j] = (*this)[i][j] + val;
				}
			}
			return out;
		}

		Mat operator-(const Type& val) {
			Mat out;
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					out[i][j] = (*this)[i][j] - val;
				}
			}
			return out;
		}

		Mat operator*(const Type& val) {
			Mat out;
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					out[i][j] = (*this)[i][j] * val;
				}
			}
			return out;
		}

		Mat operator/(const Type& val) {
			Mat out;
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = 0; j < tNRows; j++) {
					out[i][j] = (*this)[i][j] / val;
				}
			}
			return out;
		}

		// Matrix Properties
		MVec transform(const MVec& in) const {
			static_assert(tNRows == tNColoumns);
			MVec out;
			for (halni i = 0; i < tNRows; i++) {
				Type tmp = 0;
				for (halni j = 0; j < tNColoumns; j++) {
					tmp += (*this)[i][j] * in[j];
				}
				out[i] = tmp;
			}
			return out;
		}

		Mat transform(const Mat& in) const {
			Mat out;
			out.clear(0);
			for (halni i = 0; i < tNRows; i++) {
				for (halni j = 0; j < tNRows; j++) {
					for (halni u = 0; u < tNRows; u++) {
						out[i][j] += (*this)[i][u] * in[u][j];
					}
				}
			}
			return out;
		}

		Mat& transpose() {
			static_assert( tNRows == tNColoumns);
			for (halni i = 0; i < tNColoumns; i++) {
				for (halni j = i + 1; j < tNColoumns; j++) {
					swap((*this)[i][j], (*this)[j][i]);
				}
			}
			return *this;
		}

		Mat operator*(const Mat& in) const {
			return transform(in);
		}

		MVec operator*(const MVec& in) const {
			return transform(in);
		}

		Mat<Type, tNRows - 1, tNRows - 1> minor(halni p, halni q) {
			Mat<Type, tNRows - 1, tNRows - 1> out;
			halni i = 0, j = 0;
			for (halni row = 0; row < tNRows; row++) {
				for (halni col = 0; col < tNRows; col++) {
					if (row != p && col != q) {
						out[i][j++] = (*this)[row][col];
						if (j == tNRows - 1) {
							j = 0;
							i++;
						}
					}
				}
			}
			return out;
		}

		Type det() {
			static_assert( tNRows == tNColoumns);

			Type out = 0;

			if (tNRows == 1) {
				return (*this)[0][0];
			}
			if (tNRows == 2) {
				return ((*this)[0][0] * (*this)[1][1]) - ((*this)[1][0] * (*this)[0][1]);
			}

			halni sign = 1;
			for (int i = 0; i < tNRows; i++) {
				out += sign * (*this)[0][i] * minor(0, i).det();
				sign = -sign;
			}
			return out;
		}

		Mat cofactors() {
			static_assert( tNRows == tNColoumns);
			Mat out;
			for (int i = 0; i < tNRows; i++) {
				for (int j = 0; j < tNRows; j++) {
					Type sign = (Type) ((i + j + 2) & 1 ? -1 : 1);
					out[i][j] = minor(j, i).det() * sign;
				}
			}
			return out;
		}

		Mat inv() {
			Type detV = det();
			DEBUG_ASSERT(detV)
			return cofactors() /= detV;
		}
	};

	template <typename Type>
	using Mat2 = Mat<Type, 2, 2>;
	using Mat2F = Mat2<halnf>;

	template <typename Type>
	class Mat<Type, 2, 2> {
		typedef Vec<Type, 2> MVec;

		MVec i;
		MVec j;

	public:

		Mat() = default;

		explicit Mat(const Type& val) {
			operator=(val);
		}

		Mat(const MVec& pi, const MVec& pj) {
			i.x = pi.x;
			i.y = pi.y;
			j.x = pj.x;
			j.y = pj.y;
		}

		Mat(Type ix, Type iy, Type jx, Type jy) {
			i.x = ix;
			i.y = iy;
			j.x = jx;
			j.y = jy;
		}

		Mat(const Mat& in) {
			operator=(in);
		}

		MVec& operator[](alni idx) {
			DEBUG_ASSERT(idx < 2 && idx >= 0)
			return (&i)[idx];
		}

		const MVec& operator[](alni idx) const {
			DEBUG_ASSERT(idx < 2 && idx >= 0)
			return (&i)[idx];
		}

		Mat& operator=(const Mat& in) {
			memcp(this, &in, sizeof(Mat2<Type>));
			return *this;
		}

		Mat& operator=(const Type& val) {
			clear(val);
			return *this;
		}

		void clear(const Type& val) {
			i.x = val;
			i.y = val;
			j.x = val;
			j.y = val;
		}

		void setDiagonal(const Type& val) {
			i.x = val;
			j.y = val;
		}

		Mat& fillRandom() {
			DEBUG_ASSERT(0)
			i.x = (Type) 0;
			i.y = (Type) 0;
			j.x = (Type) 0;
			j.y = (Type) 0;
			return *this;
		}

		Mat operator-() {
			return Mat(-i.x, -i.y, -j.x, -j.y);
		}

		Mat& operator+=(const Mat& in) {
			i.x += in.i.x;
			i.y += in.i.y;
			j.x += in.j.x;
			j.y += in.j.y;
			return *this;
		}

		Mat& operator-=(const Mat& in) {
			i.x -= in.i.x;
			i.y -= in.i.y;
			j.x -= in.j.x;
			j.y -= in.j.y;
			return *this;
		}

		Mat& operator+=(Type val) {
			i.x += val;
			i.y += val;
			j.x += val;
			j.y += val;
			return *this;
		}

		Mat& operator-=(Type val) {
			i.x -= val;
			i.y -= val;
			j.x -= val;
			j.y -= val;
			return *this;
		}

		Mat& operator/=(Type val) {
			i.x /= val;
			i.y /= val;
			j.x /= val;
			j.y /= val;
			return *this;
		}

		Mat& operator*=(Type val) {
			i.x *= val;
			i.y *= val;
			j.x *= val;
			j.y *= val;
			return *this;
		}

		Mat operator+(const Mat& in) {
			Mat out;
			out.i.x = i.x + in.i.x;
			out.i.y = i.y + in.i.y;
			out.j.x = j.x + in.j.x;
			out.j.y = j.y + in.j.y;
			return out;
		}

		Mat operator-(const Mat& in) {
			Mat out;
			out.i.x = i.x - in.i.x;
			out.i.y = i.y - in.i.y;
			out.j.x = j.x - in.j.x;
			out.j.y = j.y - in.j.y;
			return out;
		}

		Mat operator+(const Type& val) {
			Mat out;
			out.i.x = i.x + val;
			out.i.y = i.y + val;
			out.j.x = j.x + val;
			out.j.y = j.y + val;
			return out;
		}

		Mat operator-(const Type& val) {
			Mat out;
			out.i.x = i.x - val;
			out.i.y = i.y - val;
			out.j.x = j.x - val;
			out.j.y = j.y - val;
			return out;
		}

		Mat operator*(const Type& val) {
			Mat out;
			out.i.x = i.x * val;
			out.i.y = i.y * val;
			out.j.x = j.x * val;
			out.j.y = j.y * val;
			return out;
		}

		Mat operator/(const Type& val) {
			Mat out;
			out.i.x = i.x / val;
			out.i.y = i.y / val;
			out.j.x = j.x / val;
			out.j.y = j.y / val;
			return out;
		}

		// Matrix Properties
		MVec transform(const MVec& in) const {
			return MVec(i.x * in.x + i.y * in.y, j.x * in.y + j.y * in.x);
		}

		Mat transform(const Mat& in) const {
			Mat out;
			out.i.x = i.x * in.i.x + j.x * in.i.y;
			out.i.y = i.y * in.i.x + j.y * in.i.y;
			out.j.x = i.x * in.j.x + j.x * in.j.y;
			out.j.y = i.y * in.j.x + j.y * in.j.y;
			return out;
		}

		Mat operator*(const Mat& in) {
			return transform(in);
		}

		Mat& transpose() {
			swap(j.x, i.y);
			return *this;
		}

		Type det() {
			return i.x * j.y - i.y * j.x;
		}

		Mat cofactors() {
			return Mat(j.y, -i.y, -j.x, i.x);
		}

		Mat inv() {
			Type detV = det();
			DEBUG_ASSERT(detV != 0)
			return (cofactors() /= detV);
		}
	};

	template <typename Type>
	using mat3 = Mat<Type, 3, 3>;
	using mat3f = mat3<halnf>;

	template <typename Type>
	class Mat<Type, 3, 3> {
		typedef Vec3<Type> vec;

	public:
		vec I;
		vec J;
		vec K;

	public:
		Mat() = default;

		explicit Mat(Type val) {
			I.assign(val, 0.f, 0.f);
			J.assign(0.f, val, 0.f);
			K.assign(0.f, 0.f, val);
		}

		Mat(const vec& i, const vec& j, const vec& k) {
			I = i;
			J = j;
			K = k;
		}

		Mat(const Mat& in) {
			this->I = in.I;
			this->J = in.J;
			this->K = in.K;
		}

		void assign(const vec& i, const vec& j, const vec& k) {
			I = i;
			J = j;
			K = k;
		}

		Mat& fillRandom() {
			I.randf();
			J.randf();
			K.randf();
			return *this;
		}

		vec& operator[](alni i) {
			DEBUG_ASSERT(i < 3 && i >= 0)
			return (&I)[i];
		}

		const vec& operator[](alni i) const {
			DEBUG_ASSERT(i < 3 && i >= 0)
			return (&I)[i];
		}

		// create on stack
		Mat operator+(const Mat& in) { return Mat(I + in.I, J + in.J, K + in.K); }
		Mat operator-(const Mat& in) { return Mat(I - in.I, J - in.J, K - in.K); }
		Mat operator+(Type val) { return Mat(I + val, J + val, K + val); }
		Mat operator-(Type val) { return Mat(I - val, J - val, K - val); }
		Mat operator*(Type val) { return Mat(I * val, J * val, K * val); }
		Mat operator/(Type val) { return Mat(I / val, J / val, K / val); }

		// write
		Mat& operator=(const Mat& in) {
			I = in.I;
			J = in.J;
			K = in.K;
			return *this;
		}

		Mat& operator+=(const Mat& in) {
			I += in.I;
			J += in.J;
			K += in.K;
			return *this;
		}

		Mat& operator-=(const Mat& in) {
			I -= in.I;
			J -= in.J;
			K -= in.K;
			return *this;
		}

		Mat& operator+=(Type val) {
			I += val;
			J += val;
			K += val;
			return *this;
		}

		Mat& operator-=(Type val) {
			I -= val;
			J -= val;
			K -= val;
			return *this;
		}

		Mat& operator*=(Type val) {
			I *= val;
			J *= val;
			K *= val;
			return *this;
		}

		Mat& operator/=(Type val) {
			I /= val;
			J /= val;
			K /= val;
			return *this;
		}

		// Matrix transformation
		vec transform(const vec& in) {
			return vec(
				I.x * in.x + I.y * in.y + I.z * in.z,
				J.x * in.x + J.y * in.y + J.z * in.z,
				K.x * in.x + K.y * in.y + K.z * in.z
			);
		}

		Mat transform(const Mat& in) {
			return Mat(
				{(in.I.x * I.x + in.I.y * J.x + in.I.z * K.x), (in.I.x * I.y + in.I.y * J.y + in.I.z * K.y), (in.I.x * I.z + in.I.y * J.z + in.I.z * K.z)},
				{(in.J.x * I.x + in.J.y * J.x + in.J.z * K.x), (in.J.x * I.y + in.J.y * J.y + in.J.z * K.y), (in.J.x * I.z + in.J.y * J.z + in.J.z * K.z)},
				{(in.K.x * I.x + in.K.y * J.x + in.K.z * K.x), (in.K.x * I.y + in.K.y * J.y + in.K.z * K.y), (in.K.x * I.z + in.K.y * J.z + in.K.z * K.z)}
			);
		}

		vec operator*(const vec& in) {
			return transform(in);
		}

		Mat operator*(const Mat& in) {
			return transform(in);
		}

		Mat<Type, 2, 2> minor(halni, halni) {
			Mat<Type, 2, 2> out;
			return out;
		}

		Mat& transpose() {
			swap(I.y, J.x);
			swap(I.z, K.x);
			swap(J.z, K.y);
			return *this;
		}

		Type det() {
			return (
				+I.x * (J.y * K.z - J.z * K.y)
				- I.y * (J.x * K.z - J.z * K.x)
				+ I.z * (J.x * K.y - J.y * K.x)
			);
		}

		Mat cofactors() {
			return Mat(
				vec(+(J.y * K.z - J.z * K.y), -(I.y * K.z - I.z * K.y), +(I.y * J.z - I.z * J.y)),
				vec(-(J.x * K.z - J.z * K.x), +(I.x * K.z - I.z * K.x), -(I.x * J.z - I.z * J.x)),
				vec(+(J.x * K.y - J.y * K.x), -(I.x * K.y - I.y * K.x), +(I.x * J.y - I.y * J.x))
			);
		}

		Mat inv() {
			return cofactors() /= det();
		}

		Mat rotatorX(alnf angle) {
			alnf cosA = (alnf) cos(angle);
			alnf sinA = (alnf) sin(angle);
			return {
				{1, 0, 0},
				{0, cosA, -sinA},
				{0, sinA, cosA}
			};
		}

		Mat rotatorY(alnf angle) {
			alnf cosA = (alnf) cos(angle);
			alnf sinA = (alnf) sin(angle);
			return {
				{cosA, 0, sinA},
				{0, 1, 0},
				{-sinA, 0, cosA}
			};
		}

		Mat rotatorZ(alnf angle) {
			alnf cosA = (alnf) cos(angle);
			alnf sinA = (alnf) sin(angle);
			return {
				{cosA, -sinA, 0},
				{sinA, cosA, 0},
				{0, 0, 1}
			};
		}

		static Mat rotatorDir(vec dir, alnf angle) {
			dir.normalize();

			Mat out;
			alnf cosA = (alnf) cos(angle);
			alnf sinA = (alnf) sin(angle);
			alnf tmp = 1 - cosA;

			out.I.x = (Type) (cosA + dir.x * dir.x * tmp);
			out.I.y = (Type) (dir.x * dir.y * tmp - dir.z * sinA);
			out.I.z = (Type) (dir.x * dir.z * tmp + dir.y * sinA);

			out.J.x = (Type) (dir.y * dir.x * tmp + dir.z * sinA);
			out.J.y = (Type) (cosA + dir.y * dir.y * tmp);
			out.J.z = (Type) (dir.y * dir.z * tmp - dir.x * sinA);

			out.K.x = (Type) (dir.z * dir.x * tmp - dir.y * sinA);
			out.K.y = (Type) (dir.z * dir.y * tmp + dir.x * sinA);
			out.K.z = (Type) (cosA + dir.z * dir.z * tmp);

			return out;
		}
	};

	template <typename Type >
	using Mat4 = Mat<Type, 4, 4>;

	using Mat4F = Mat4<halnf>;
	using Mat4I = Mat4<halni>;

	template <typename Type >
	class Mat< Type, 0, 0 > {
		typedef Vec<Type, 1> MVec;
		MVec dummy;

	public:
		MVec& operator[](alni) {
			return dummy;
		}

		const MVec& operator[](alni) const {
			return dummy;
		}

		Type det() {
			return Type();
		}
	};
}