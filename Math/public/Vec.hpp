#pragma once

#include "MathCommon.hpp"

namespace tp {

	template <typename Type, const ualni tSize>
	class Vec {
		typedef SelCopyArg<Type> TypeArg;

	private:
		Type mBuff[tSize];

		inline const Type& get(ualni i) const { return mBuff[i]; }
		inline Type& get(ualni i) { return mBuff[i]; }
		inline Type& set(ualni i, TypeArg arg) { return mBuff[i] = arg; }

	public:

		Vec() {
			MODULE_SANITY_CHECK(gModuleMath)
		}

		Vec(TypeArg val) {
			assign(val);
		}

		Vec(TypeArg val1, TypeArg val2, TypeArg val3, TypeArg val4) {
			static_assert(tSize == 4);
			mBuff[0] = val1;
			mBuff[1] = val2;
			mBuff[2] = val3;
			mBuff[3] = val4;
		}

		Vec(TypeArg val1, TypeArg val2, TypeArg val3, TypeArg val4, TypeArg val5) {
			static_assert(tSize == 5);
			mBuff[0] = val1;
			mBuff[1] = val2;
			mBuff[2] = val3;
			mBuff[3] = val4;
			mBuff[4] = val5;
		}

		Vec(TypeArg val1, TypeArg val2, TypeArg val3, TypeArg val4, TypeArg val5, TypeArg val6) {
			static_assert(tSize == 6);
			mBuff[0] = val1;
			mBuff[1] = val2;
			mBuff[2] = val3;
			mBuff[3] = val4;
			mBuff[4] = val5;
			mBuff[5] = val6;
		}

		Vec(const Vec& in) {
			memcp(mBuff, in.mBuff, sizeof(Type) * tSize);
		}

		Type& operator[](ualni i) {
			DEBUG_ASSERT(i < tSize && i >= 0)
			return mBuff[i];
		}

		const Type& operator[](ualni i) const {
			DEBUG_ASSERT(i < tSize && i >= 0)
			return mBuff[i];
		}

		Vec operator-() {
			Vec out;
			for (halni i = 0; i < tSize; i++) {
				out.get(i) = -get(i);
			}
			return out;
		}

		// write
		Vec& operator+=(const Vec& val) {
			for (ualni i = 0; i < tSize; i++) {
				get(i) += val[i];
			}
			return *this;
		}

		Vec& operator-=(const Vec& val) {
			for (ualni i = 0; i < tSize; i++) {
				get(i) -= val[i];
			}
			return *this;
		}

		Vec& operator+=(Type val) {
			for (ualni i = 0; i < tSize; i++) {
				get(i) += val;
			}
			return *this;
		}

		Vec& operator-=(Type val) {
			for (ualni i = 0; i < tSize; i++) {
				get(i) -= val;
			}
			return *this;
		}

		Vec& operator*=(Type val) {
			for (ualni i = 0; i < tSize; i++) {
				get(i) *= val;
			}
			return *this;
		}

		Vec& operator/=(Type val) {
			for (ualni i = 0; i < tSize; i++) {
				get(i) /= val;
			}
			return *this;
		}

		void assign(Type val) {
			for (ualni i = 0; i < tSize; i++) {
				get(i) = val;
			}
		}

		// create on stack
		Vec operator+(const Vec& in) const {
			Vec out;
			for (ualni i = 0; i < tSize; i++) {
				out[i] = in[i] + get(i);
			}
			return out;
		}

		Vec operator-(const Vec& in) const {
			Vec out;
			for (ualni i = 0; i < tSize; i++) {
				out[i] = in[i] - get(i);
			}
			return out;
		}

		Vec operator+(Type val) const {
			Vec out;
			for (ualni i = 0; i < tSize; i++) {
				out[i] = get(i) + val;
			}
			return out;
		}

		Vec operator-(Type val) const {
			Vec out;
			for (ualni i = 0; i < tSize; i++) {
				out[i] = get(i) - val;
			}
			return out;
		}

		Vec operator*(Type val) const {
			Vec out;
			for (ualni i = 0; i < tSize; i++) {
				out[i] = get(i) * val;
			}
			return out;
		}

		Vec operator/(Type val) const {
			Vec out;
			for (ualni i = 0; i < tSize; i++) {
				out[i] = get(i) / val;
			}
			return out;
		}

		// Vector Properties
		Type dot(const Vec& in) const {
			Type out = 0;
			for (ualni i = 0; i < tSize; i++) {
				out += get(i) * in.get(i);
			}
			return out;
		}

		Type operator*(const Vec& in) const {
			return dot(in);
		}

		alnf length2() const {
			alnf sum = 0;
			for (ualni i = 0; i < tSize; i++) {
				Type val = get(i);
				sum += val * val;
			}
			return sum;
		}

		alnf length() const {
			return sqrt(length2());
		}

		Vec& normalize() {
			operator/=((Type) length());
			return *this;
		}

		Vec unitV() {
			return Vec(*this).normalize();
		}

		// Comparisons
		bool operator>(const Vec& in) const { return this->length2() > in.length2(); }
		bool operator<(const Vec& in) const { return this->length2() < in.length2(); }
		bool operator>=(const Vec& in) const { return this->length2() >= in.length2(); }
		bool operator<=(const Vec& in) const { return this->length2() <= in.length2(); }

		bool operator==(const Vec& in) const {
			for (ualni i = 0; i < tSize; i++) {
				if (get(i) != in.get(i)) {
					return false;
				}
			}
			return true;
		}

		bool operator!=(const Vec& in) const {
			return !operator==(in);
		}
	};

	template <typename Type>
	using Vec2 = Vec<Type, 2>;

	using Vec2F = Vec<halnf, 2>;
	using Vec2I = Vec<halni, 2>;

	template <typename Type>
	class Vec<Type, 2> {

	public:
		Type x;
		Type y;

		Vec() : x(0), y(0) {}

		// Initialization
		template <typename TypeIn>
		Vec(TypeIn aX, TypeIn aY) {
			x = (Type) aX;
			y = (Type) aY;
		}

		template <typename TypeIn>
		Vec(TypeIn Vec[2]) {
			x = (Type) Vec[0];
			y = (Type) Vec[1];
		}

		template <typename TypeIn>
		Vec(TypeIn val) {
			x = y = (Type) val;
		}

		template <typename TypeIn>
		Vec(Vec<TypeIn, 2>& Vec) {
			x = (Type) Vec.x;
			y = (Type) Vec.y;
		}

		template <typename TypeIn>
		void assign(TypeIn aX, TypeIn aY) {
			x = (Type) aX;
			y = (Type) aY;
		}

		template <typename TypeIn>
		void assign(const Vec<TypeIn, 2>& in) {
			x = (Type) in.x;
			y = (Type) in.y;
		}

		template <typename TypeIn>
		void assign(TypeIn in[2]) {
			x = (Type) in[0];
			y = (Type) in[1];
		}

		template <typename TypeIn>
		Vec& operator=(TypeIn val) {
			x = y = (Type) val;
			return *this;
		}

		// Conversion
		template <typename TypeIn>
		Vec& operator=(const Vec<TypeIn, 2>& in) {
			x = (Type) in.x;
			y = (Type) in.y;
			return *this;
		}

		//  create on stack
		Vec2<Type> operator+(const Vec2<Type>& in) const { return Vec2<Type>(x + in.x, y + in.y); }
		Vec2<Type> operator-(const Vec2<Type>& in) const { return Vec2<Type>(x - in.x, y - in.y); }
		Vec2<Type> operator+(Type val) const { return Vec2<Type>(x + val, y + val); }
		Vec2<Type> operator-(Type val) const { return Vec2<Type>(x - val, y - val); }
		Vec2<Type> operator*(Type val) const { return Vec2<Type>(x * val, y * val); }
		Vec2<Type> operator/(Type val) const { return Vec2<Type>(x / val, y / val); }

		//  write
		void operator-=(Type val) {
			x -= val;
			y -= val;
		}

		void operator+=(Type val) {
			x += val;
			y += val;
		}

		void operator-=(const Vec2<Type>& Vec) {
			x -= Vec.x;
			y -= Vec.y;
		}

		void operator+=(const Vec2<Type>& Vec) {
			x += Vec.x;
			y += Vec.y;
		}

		void operator*=(Type val) {
			x *= val;
			y *= val;
		}

		void operator/=(Type val) {
			x /= val;
			y /= val;
		}

		void operator/=(const Vec2<Type>& in) {
			x /= in.x;
			y /= in.y;
		}

		void operator*=(const Vec2<Type>& in) {
			x *= in.x;
			y *= in.y;
		}

		bool compareEach(const Vec& in) const { return (x > in.x && y > in.y); }

		bool operator>(const Vec& in) const { return length2() > in.length2(); }
		bool operator<(const Vec& in) const { return length2() < in.length2(); }
		bool operator>=(const Vec& in) const { return length2() >= in.length2(); }
		bool operator<=(const Vec& in) const { return length2() <= in.length2(); }
		bool operator==(const Vec& in) const { return (x == in.x && y == in.y); }
		bool operator!=(const Vec& in) const { return (x != in.x || y != in.y); }

		Type& operator[](bool axes) { return (&x)[axes]; }

		const Type& operator[](bool axes) const { return (&x)[axes]; }

		// Vector Properties
		alnf operator*(const Vec& in) const {
			return dot(in);
		}

		alnf dot(const Vec& in) const {
			return (x * in.x + y * in.y);
		}

		Vec unitV() const {
			return Vec(*this / (Type) this->length());
		}

		void normalize() {
			*this /= (Type) length();
		}

		Vec normal() {
			return { -y, x };
		}

		alnf length2() const {
			return (x * x + y * y);
		}

		alnf length() const {
			Type const tmp = (Type) (x * x + y * y);
			return sqrt(tmp);
		}

		// Vector Transformation
		void Rot(float cosA, float sinA) {
			Type tmp = x;
			x = x * cosA - y * sinA;
			y = tmp * sinA + y * cosA;
		}

		void clamp(const Vec& min, const Vec& max) {
			tp::clamp(x, min.x, max.x);
			tp::clamp(y, min.y, max.y);
		}
	};

	template <typename Type>
	using Vec3 = Vec<Type, 3>;

	using Vec3F = Vec3<halnf>;
	using Vec3I = Vec3<halni>;

	template <typename Type>
	class Vec<Type, 3> {

		public:

		Type x;
		Type y;
		Type z;

		// Initialization
		Vec() {}

		Vec(const Vec<Type, 4>& in) {
			x = in[0];
			y = in[1];
			z = in[2];
		}

		Vec(Type aX, Type aY, Type aZ) {
			x = aX;
			y = aY;
			z = aZ;
		}

		Vec(Type Vec[3]) {
			x = Vec[0];
			y = Vec[1];
			z = Vec[2];
		}

		Vec(Type x) {
			assign(x);
		}

		Vec(const Vec& Vec) {
			x = Vec.x;
			y = Vec.y;
			z = Vec.z;
		}

		void assign(Type aX, Type aY, Type aZ) {
			x = aX;
			y = aY;
			z = aZ;
		}

		void assign(const Vec& in) {
			x = in.x;
			y = in.y;
			z = in.z;
		}

		void assign(Type val) {
			x = val;
			y = val;
			z = val;
		}

		Vec& operator=(const Vec& in) {
			x = in.x;
			y = in.y;
			z = in.z;
			return *this;
		}

		Vec& operator=(const Type* in) {
			x = in[0];
			y = in[1];
			z = in[2];
			return *this;
		}

		Type& operator[](alni i) {
			DEBUG_ASSERT(i < 3 && i >= 0)
			return (&x)[i];
		}

		const Type& operator[](alni i) const {
			DEBUG_ASSERT(i < 3 && i >= 0)
			return (&x)[i];
		}

		//  create on stack
		Vec operator+(const Vec& in) const {
			return Vec(x + in.x, y + in.y, z + in.z);
		}

		Vec operator-(const Vec& in) const {
			return Vec(x - in.x, y - in.y, z - in.z);
		}

		Vec operator+(Type val) const {
			return Vec(x + val, y + val, z + val);
		}

		Vec operator-(Type val) const {
			return Vec(x - val, y - val, z - val);
		}

		Vec operator*(Type val) const {
			return Vec(x * val, y * val, z * val);
		}

		Vec operator/(Type val) const {
			return Vec(x / val, y / val, z / val);
		}


	 // compare
		bool operator>(const Vec& Vec) const {
			return length2() > Vec.length2();
		}

		bool operator<(const Vec& Vec) const {
			return length2() < Vec.length2();
		}

		bool operator>=(const Vec& Vec) const {
			return length2() >= Vec.length2();
		}

		bool operator<=(const Vec& Vec) const {
			return length2() <= Vec.length2();
		}

		bool operator==(const Vec& Vec) const {
			return (x == Vec.x && y == Vec.y && z == Vec.z);
		}

		//  write
		Vec operator-() {
			return Vec(-x, -y, -z);
		}

		void operator -= (Type val) {
			x -= val;
			y -= val;
			z -= val;
		}

		void operator += (Type val) {
			x += val;
			y += val;
			z += val;
		}

		void operator -= (const Vec& in) {
			x -= in.x;
			y -= in.y;
			z -= in.z;
		}

		void operator += (const Vec& in) {
			x += in.x;
			y += in.y;
			z += in.z;
		}

		void operator *= (Type val) {
			x *= val;
			y *= val;
			z *= val;
		}

		void operator /= (Type val) {
			x /= val;
			y /= val;
			z /= val;
		}

		// Vector Properties
		Type dot(const Vec& in) const {
			return (x * in.x + y * in.y + z * in.z);
		}

		alnf length() const {
			return sqrt((halnf) (x * x + y * y + z * z));
		}

		Type length2() const {
			return (x * x + y * y + z * z);
		}

		Vec unitV() const {
			return *this / (Type) this->length();
		}

		Vec& normalize() {
			*this /= (Type) this->length();
			return *this;
		}

		Vec cross(const Vec& in) const {
			return Vec(
				y * in.z - z * in.y,
				z * in.x - x * in.z,
				x * in.y - y * in.x
			);
		}

		Vec operator*(const Vec& in) const {
			return cross(in);
		}

		// Vector Transformation
		void rotZ(alnf cosA, alnf sinA) {
			Type tmp = x;
			x = (Type) (x * cosA - y * sinA);
			y = (Type) (tmp * sinA + y * cosA);
		}

		void rotY(float cosA, float sinA) {
			Type tmp = x;
			x = (Type) (x * cosA - z * sinA);
			z = (Type) (tmp * sinA + z * cosA);
		}

		void rotX(float cosA, float sinA) {
			Type tmp = y;
			y = (Type) (y * cosA - z * sinA);
			z = (Type) (tmp * sinA + z * cosA);
		}

		halnf angelX() const {
			return (halnf) atan2(y, z);
		}

		halnf angelY() const {
			return (halnf) atan2(x, z);
		}
	};

	template <typename Type>
	using Vec4 = Vec<Type, 4>;

	using Vec4F = Vec4<halnf>;
	using Vec4I = Vec4<halni>;
}