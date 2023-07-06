#pragma once

#include "primitives/dictobject.h"

namespace obj {

	struct ClassObject : Object {

		static ObjectType TypeData;
		static void copy(ClassObject* self, const ClassObject* in);
		static void destructor(ClassObject* self);
		static void constructor(ClassObject* self);
		static tp::alni save_size(ClassObject* self);
		static void save(ClassObject* self, tp::File& file_self);
		static void load(tp::File& file_self, ClassObject* self);

		DictObject* members;

		void addMember(Object* obj, tp::string id);
		void createMember(tp::string type, tp::string id);

		template<typename Type>
		Type* createMember(tp::string id) {
			auto out = NDO->create(Type::TypeData.name);
			addMember(out, id);
#ifdef OBJECT_REF_COUNT
			NDO->destroy(out);
#endif // OBJECT_REF_COUNT

			return (Type*) out;
		}

		template<typename Type>
		Type* getMember(const tp::string& id) {
			auto idx = members->presents(id);
			if (idx) {
				return ((Type*)obj::ndo_cast(members->getSlotVal(idx), &Type::TypeData));
			}
			return NULL;
		}

		template<typename Type>
		Type* getMemberAssert(const tp::string& id) {
			auto out = getMember<Type>(id);
			assert(out && "invalid member access");
			return out;
		}
	};


};