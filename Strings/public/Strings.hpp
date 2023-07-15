#pragma once

#include "PoolAllocator.hpp"
#include "StringLogic.hpp"
#include "Buffer.hpp"

namespace tp {

	// Static data
	extern ModuleManifest gModuleStrings;

	template<typename tChar>
	class StringTemplate;
	using String = StringTemplate<int1>;

	template<typename tChar = int1>
	class StringTemplate {
	public:
		typedef StringLogic<tChar> Logic;
		typedef Logic::Index Index;

	private:
		// Hidden slave class
		class StringData {
		public:
			uint2 mIsConst; // source is non-modifiable
			uint2 mEditedIdx; // editor id
			uint4 mReferenceCount; // number of users of this
			tChar* mBuff; // actual string data

			StringData() {
				MODULE_SANITY_CHECK(gModuleStrings)
				mBuff = (tChar*) "*";
				mReferenceCount = 0;
				mIsConst = true;
				mEditedIdx = 0;
			}

			StringData(const tChar* aBuff, bool aIsConst) {
				MODULE_SANITY_CHECK(gModuleStrings)
				DEBUG_ASSERT(aBuff)
				mBuff = (tChar*) aBuff;
				mReferenceCount = 0;
				mIsConst = aIsConst;
				mEditedIdx = 0;
			}

			explicit StringData(const tChar* aBuff) {
				MODULE_SANITY_CHECK(gModuleStrings)
				DEBUG_ASSERT(aBuff)
				auto const len = Logic::calcLength(aBuff);
				resize(len);
				memCopy(mBuff, aBuff, len);
				mReferenceCount = 0;
				mIsConst = false;
				mEditedIdx = 0;
			}

			[[nodiscard]] tChar* getBuffer() {
				return mBuff;
			}

			~StringData() {
				if (!mIsConst) delete[] mBuff;
			}

			void resize(ualni size) {
				DEBUG_ASSERT(mReferenceCount == 1 && !mIsConst)
				delete[] mBuff;
				mBuff = new tChar[size + 1];
				mBuff[size] = Logic::getEndChar();
			}
		};

	private:
		class StringData* mStringData;

	private:
		enum { STRINGS_POOL_SIZE = 1024, };
		static PoolAlloc<StringData, STRINGS_POOL_SIZE>* sStringPool;
		static StringData sNullString;

		//  --------------------------------- Main Interface ---------------------------------//
	public:
		// Creates null string reference
		StringTemplate() {
			mStringData = &sNullString;
			incReference(mStringData);
		}

		// References existing string data
		StringTemplate(const StringTemplate& in) {
			mStringData = in.mStringData;
			incReference(mStringData);
		}

		// Creates new string data from raw data pointer.
		// Does not own string buffer - string buffer will not be freed. Initializes as const memory.
		explicit StringTemplate(const tChar* raw) {
			mStringData = new (sStringPool->allocate(0)) StringData(raw, true);
			incReference(mStringData);
		}

		// Copies raw data and claims ownership over it
		explicit StringTemplate(tChar* raw) {
			mStringData = new (sStringPool->allocate(0)) StringData(raw);
			incReference(mStringData);
		}

		~StringTemplate() {
			decReference(mStringData);
		}

	private:
		static void incReference(StringData* dp) {
			dp->mReferenceCount++;
		}

		void decReference(StringData* dp) {
			dp->mReferenceCount--;
			if (!dp->mReferenceCount) {
				sStringPool->deallocate(mStringData);
				mStringData->~StringData();
			}
		}

	public: // Access
		// used to read data
		[[nodiscard]] const tChar* read() const {
			return mStringData->mBuff;
		}

		// used to write data
		// output will be null if in TextEditing mode
		[[nodiscard]] tChar* write() {
			makeModifiable();
			return mStringData->getBuffer();
		}

		// output will be null if in TextEditing mode
		[[nodiscard]] tChar* resize() {
			if (!mStringData->getEditor()) mStringData->resize();
			return mStringData->getBuffer();
		}

	public:
		void makeModifiable() {
			// We have no rights to modify if someone references that memory too
			// So we need to create new copy of StringData
			if (mStringData->mReferenceCount > 1) {
				mStringData = new (sStringPool->allocate(0)) StringData(mStringData->mBuff);
				incReference(mStringData);
				return;
			}

			// Also if initial raw data was marked as const - create copy of raw data
			// Note - raw data will be lost at this point if no other record of such is stored
			if (mStringData->mIsConst) {
				mStringData = new (sStringPool->allocate(0)) StringData(mStringData->mBuff);
				incReference(mStringData);
			}
		}

	public: // Syntax sugars
		Index size() const {
			return Logic::calcLength(read());
		}

		void calcLineOffsets(Buffer<Index>& aOut) const {
			Logic::calcLineOffsets(read(), size(), aOut);
		}

		StringTemplate& operator=(const tChar* in) {
			this->~StringTemplate();
			new (this) StringTemplate(in);
			return *this;
		}

		StringTemplate& operator=(const StringTemplate& in) {
			if (&in == this) return *this;
			~StringTemplate();
			new (this) StringTemplate(in);
			return *this;
		}
	};
}