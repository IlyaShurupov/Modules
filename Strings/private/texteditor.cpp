
#include "texteditor.h"

#include "list.h"
#include "array.h"
#include "avltree.h"
#include "addbuff.h"

using namespace tp;
using namespace str;

bool tp::str::PiecesDS::Node::mInsertionOPeration = false;


bool str::isNewLineChar(Char character) {
	return (character == '\n') || (character == '\r');
}

void Input::getLineOfssets(Array<Index>& out) {
	tp::halni lines = 0;
	for (auto idx : Range(mLen)) {
		if (isNewLineChar(mInput[idx])) {
			lines++;
		}
	}
	out.reserve(lines + 2); // plust start and end offsets
	lines = 0;
	for (auto idx : Range(mLen)) {
		if (isNewLineChar(mInput[idx])) {
			out[lines + 1] = Index(idx + 1);
			lines++;
		}
	}
	out[0] = 0;
	out[lines + 1] = mLen;
}

 // ---------------------- Pointer ---------------------- // 

Pointer::Pointer() { 
	mCol = 0; mLine = 0; 
}

Pointer::Pointer(LineId aLine, ColomnId aCol) {
	mLine = aLine;
	mCol = aCol;
}

Pointer::Pointer(const Input& aInput) {
	tp::halni lines = 0;
	tp::halni col = 0;
	for (auto idx : Range(aInput.mLen)) {
		if (isNewLineChar(aInput.mInput[idx])) {
			lines++;
			col = 0;
		}
		else {
			col++;
		}
	}
	mLine = lines;
	mCol = col;
}

void Pointer::operator+=(Char achar) {
	if (isNewLineChar(achar)) {
		mLine += 1;
		mCol = 0;
	}
	else {
		mCol += 1;
	}
}

void Pointer::operator+=(const Pointer& ap) {
	*this = operator+(ap);
}

Pointer Pointer::operator-(const Pointer in) const {
	if (mLine == in.mLine) {
		return { 0, mCol - in.mCol };
	}
	return { mLine - in.mLine, mCol };
}

Pointer Pointer::operator+(const Pointer& ap) const {
	if (ap.mLine == 0) {
		return { mLine, mCol + ap.mCol };
	}
	return { mLine + ap.mLine, ap.mCol };
}

bool Pointer::operator==(const Pointer& in) const {
	return in.mCol == mCol && in.mLine == mLine;
}

bool Pointer::operator>(const Pointer& in) const {
	if (mLine > in.mLine) {
		return true;
	}
	else if (mLine < in.mLine) {
		return false;
	}

	return mCol > in.mCol;
}

bool Pointer::operator>=(const Pointer& in) const {
	return operator>(in) || in == *this;
}

Pointer Piece::diffPtr() const {
	return mEnd - mStart;
}

LineId Piece::nLines() const {
	return { mEnd.mLine - mStart.mLine };
}

// ---------------------- Add Buff ---------------------- // 

AddBuff::AddBuff() {
	Input input = { "", 0 };
	tp::Array<Index> lineoffsets;
	input.getLineOfssets(lineoffsets);
	mLineOffsets.append(lineoffsets.buff(), (Index)lineoffsets.length());
}

void AddBuff::append(Input aInput) {

	// TODO : errors here

	tp::Array<Index> lineoffsets;
	aInput.getLineOfssets(lineoffsets);

	if (lineoffsets.length() == 2) {
		mLineOffsets[mLineOffsets.len() - 1] += lineoffsets[1] - lineoffsets[0];
	}
	else {

		auto last_line_offset = mLineOffsets[mLineOffsets.len() - 1];

		// offset last line ptr
		mLineOffsets[mLineOffsets.len() - 1] += lineoffsets[1];

		// offset all lines by mLineOffsets.mLast ptr
		for (auto offset : lineoffsets) {
			offset.data() += last_line_offset;
		}

		mLineOffsets.append(&lineoffsets[2], (Index)(lineoffsets.length() - 2));
	}

	mBuff.append(aInput.mInput, aInput.mLen);
}

Pointer AddBuff::endPtr() {
	return { mLineOffsets.len() - 2, mLineOffsets[mLineOffsets.len() - 1] - mLineOffsets[mLineOffsets.len() - 2] };
}

// ---------------------- Orig Buff ---------------------- // 

OrigBuff::OrigBuff(Input aOriginal) {
	mOriginal = aOriginal;
	mOriginal.getLineOfssets(mLineOffsets);

	mPtrStart = { 0, 0 };
	mPtrEnd = mOriginal;
}

void OrigBuff::makeOwnOriginalInput() {
	if (mOwnsInput) {
		return;
	}

	auto input_copy = new Char[mOriginal.mLen + 1];
	assert(sizeof(Char) == sizeof(uint1));
	memcp(input_copy, mOriginal.mInput, mOriginal.mLen);
	input_copy[mOriginal.mLen] = '\0';
	mOriginal.mInput = input_copy;
	mOwnsInput = true;
}

OrigBuff::~OrigBuff() {
	if (mOwnsInput) {
		delete[] mOriginal.mInput;
	}
}

// ---------------------- Editor ---------------------- // 

PieceCrs TextEditor::findPiece(Pointer ptr) const {
	PieceCrs out;
	if (!mPieces.nPieces()) { 
		return out; 
	}

	if (mPieces.ptrSum() == ptr) {
		out.piece = mPieces.maxNode(mPieces.head());
	} else {
		out.piece = mPieces.find(ptr);
	}

	DBG_BREAK(!out.piece);
	out.piece_ptr = out.piece->getFindKey();
	out.ptr = ptr;
	out.offset = out.ptr - out.piece_ptr;
	return out;
}

void TextEditor::clampPtr(Pointer& ptr) const {
	if (!mPieces.nPieces()) {
		ptr = { 0, 0 };
		return;
	}

	CLAMP(ptr.mLine, 0, mNLines());

	auto crs = findPiece(Pointer{ ptr.mLine, 0 });
	Index line_len = getPieceLineLen(crs.piece->mPiece, crs.offset.mLine);

	auto next_piece_ptr = nextPiecePtr(crs.piece, crs.piece_ptr);
	auto piece = crs.piece;
	while (piece->mNext && ptr.mLine == next_piece_ptr.mLine) {
		piece = piece->mNext;
		next_piece_ptr = nextPiecePtr(piece, next_piece_ptr);
		line_len += getPieceLineLen(piece->mPiece, 0);
	}

	if (ptr.mLine != mNLines()) {
		line_len -= 1;
	}

	CLAMP(ptr.mCol, 0, line_len);
}

void TextEditor::setCursor(Pointer ptr) {
	clampPtr(ptr);
	mCursor = ptr;
}

void TextEditor::setCursorEnd(Pointer ptr) {
	clampPtr(ptr);
	mCursorEnd = ptr;
	mCursor = mCursorEnd;
}

const Char* TextEditor::getPieceLine(const Piece* piece, LineId offset) const {
	const Char* buff;
	if (piece->mAddedBuff) {
		if (offset == 0) {
			buff = &mAddBuff.mBuff[mAddBuff.mLineOffsets[piece->mStart.mLine]] + piece->mStart.mCol;
		}
		else if (offset > piece->nLines()) {
			buff = &mAddBuff.mBuff[mAddBuff.mLineOffsets[piece->mEnd.mLine]] + piece->mEnd.mCol;
		}
		else {
			buff = &mAddBuff.mBuff[mAddBuff.mLineOffsets[piece->mStart.mLine + offset]];
		}
	}
	else {
		if (offset == 0) {
			buff = &mOrigBuff.mOriginal.mInput[mOrigBuff.mLineOffsets[piece->mStart.mLine]] + piece->mStart.mCol;
		}
		else if (offset > piece->nLines()) {
			buff = &mOrigBuff.mOriginal.mInput[mOrigBuff.mLineOffsets[piece->mEnd.mLine]] + piece->mEnd.mCol;
		}
		else {
			buff = &mOrigBuff.mOriginal.mInput[mOrigBuff.mLineOffsets[piece->mStart.mLine + offset]];
		}
	}
	return buff;
}

const Char* TextEditor::getBuffLine(const Piece* piece, LineId offset) const {
	const Char* buff;
	if (piece->mAddedBuff) {
		buff = &mAddBuff.mBuff[mAddBuff.mLineOffsets[piece->mStart.mLine + offset]];
	}
	else {
		buff = &mOrigBuff.mOriginal.mInput[mOrigBuff.mLineOffsets[piece->mStart.mLine + offset]];
	}
	return buff;
}

Pointer TextEditor::prevPiecePtr(PiecesDS::Node* piece_node, Pointer aPtr) const {
	if (!piece_node->mPrev) {
		return { 0, 0 };
	}

	return piece_node->mPrev->getFindKey();
}

Pointer TextEditor::nextPiecePtr(PiecesDS::Node* aPiece, Pointer aPtr) const {
	if (!aPiece) return { -1, -1 };

	if (aPiece->mNext) {
		return aPiece->mNext->getFindKey();
	}
	return aPiece->getFindKey() + aPiece->mDiff;
}

LineId TextEditor::getPieceLineLen(Piece* aPiece, LineId line) const {
	return LineId(getPieceLine(aPiece, line + 1) - getPieceLine(aPiece, line));
}

TextEditor::TextEditor(Input original) : mOrigBuff(original) {
	assert(original.mLen);
	auto new_node = mPieces.insertAfter(0, { false, mOrigBuff.mPtrStart, mOrigBuff.mPtrEnd });
	mPieces.pushUndoPoint();
}

TextEditor::TextEditor() : mOrigBuff({ 0, 0 }) {
}

Index TextEditor::mNLines() const {
	return mPieces.ptrSum().mLine;
}

Pointer TextEditor::getCursor() const {
	return mCursor;
}

Pointer TextEditor::getCursorEnd() const {
	return mCursorEnd;
}

void TextEditor::insert(Input aInput) {
	// handle selectiom
	remove(true);

	// save input to addbuff
	auto start_ref = mAddBuff.endPtr();
	mAddBuff.append(aInput);
	auto end_ref = mAddBuff.endPtr();
	auto input_diff = end_ref - start_ref;

	PiecesDS::Node* prev_node = NULL;
	PiecesDS::Node* new_node = NULL;

	auto crs = findPiece(mCursor);

	if (crs.piece && crs.piece->mPrev && crs.offset == Pointer{ 0, 0 }) {
		crs.piece_ptr = prevPiecePtr(crs.piece, crs.piece_ptr);
		crs.offset = crs.ptr - crs.piece_ptr;
		crs.piece = crs.piece->mPrev;
	}

	if (crs.piece) {
		if (crs.ptr == crs.piece_ptr) { // start
			prev_node = crs.piece->mPrev;
		}
		else if (crs.offset == crs.piece->mPiece->diffPtr()) { // end
			if (mLastInsertionCrs == crs.ptr && crs.piece->mPiece->mAddedBuff && crs.piece->mPiece->mEnd == start_ref) {
				crs.piece->mPiece->mEnd += input_diff;
				mPieces.update_cache(crs.piece);
				goto END;
			}
			else {
				prev_node = crs.piece;
			}
		}
		else { // middle
			prev_node = splitPiece(crs.piece, crs.offset);
		}
	}

	// create the new node
	new_node = mPieces.insertAfter(prev_node, { true, start_ref, end_ref });

	END:
	mCursor = mCursor + input_diff;
	mCursorEnd = mCursor;
	mLastInsertionCrs = mCursor;
	mPieces.pushUndoPoint();
}

PiecesDS::Node* TextEditor::splitPiece(PiecesDS::Node* piece, Pointer split_ptr) {
	auto span = piece->mPiece->diffPtr();

	if (span == split_ptr || split_ptr == Pointer{ 0, 0 }) {
		return NULL;
	}

	// save temp data
	auto end_ptr = piece->mPiece->mEnd;
	auto left_piece = Piece{ piece->mPiece->mAddedBuff, piece->mPiece->mStart, piece->mPiece->mStart + split_ptr };
	auto left_node = mPieces.insertAfter(piece->mPrev, left_piece);

	// create right half
	auto right_node = mPieces.insertAfter(left_node, { left_node->mPiece->mAddedBuff, left_node->mPiece->mEnd, end_ptr });

	auto line_len = tp::halni(getBuffLine(right_node->mPiece, 1) - getBuffLine(right_node->mPiece, 0));
	if (line_len == right_node->mPiece->mStart.mCol) {
		DBG_BREAK(1);
		right_node->mPiece->mStart = { right_node->mPiece->mStart.mLine + 1, 0 };
	}

	// remove current node
	mPieces.remove(right_node->mNext);

	return left_node;
}

void TextEditor::remove(bool only_selection, Dir dir, DirUnit unit) {
	if (!mPieces.nPieces()) {
		return;
	}

	if (mCursor == mCursorEnd) {
		if (only_selection) {
			return;
		}

		if (dir == LEFT) {
			if (mCursor == Pointer(0, 0)) {
				return;
			}

			PieceCrs crs;
			if (mCursor.mCol == NULL) {
				auto new_ptr = Pointer{ mCursor.mLine - 1, ENV_HALNI_MAX };
				clampPtr(new_ptr);
				crs = findPiece(new_ptr);
				//crs = findPiece({ crs.ptr.mLine, crs.ptr.mCol - 1 });
			}
			else {
				crs = findPiece({ mCursor.mLine, mCursor.mCol - 1 });
			}

			mCursorEnd = mCursor;
			mCursor = crs.ptr;
		}
		else if (dir == RIGHT) {
			auto crs = findPiece(Pointer{ mCursor.mLine, mCursor.mCol + 1 });
			mCursorEnd = mCursor;
			mCursor = crs.ptr;

			if (mCursor == mCursorEnd) {
				return;
			}
		}
		else {
			return;
		}
	}

	if (mCursor > mCursorEnd) {
		swap(mCursor, mCursorEnd);
	}

	auto crs = findPiece(mCursor);
		
	// first and last pieces to be deleted
	PiecesDS::Node* del_start = NULL, *del_end = NULL;

	// find left
	if (crs.offset == Pointer{ 0, 0 }) {
		del_start = crs.piece;
	}
	else if (crs.piece->mPiece->diffPtr() == crs.offset) {
		del_start = crs.piece->mNext;
	}
	else {
		del_start = splitPiece(crs.piece, crs.offset)->mNext;
	}
	
	auto crs_end = findPiece(mCursorEnd);

	// find right
	if (crs_end.offset == Pointer{ 0, 0 }) {
		del_end = crs_end.piece->mPrev;
	}
	else if (crs_end.piece->mPiece->diffPtr() == crs_end.offset) {
		del_end = crs_end.piece;
	}
	else {
		bool intersect = crs_end.piece == del_start;
		del_end = splitPiece(crs_end.piece, crs_end.offset);
		if (intersect) {
			del_start = del_end;
		}
	}

	assert(del_start && del_end);
	auto end_piece = del_end->mNext ? del_end->mNext->mPiece : NULL;

	while (del_start && del_start->mPiece != end_piece) {
		auto del = del_start;

		// del_start will be reused by the avl-tree after deletion otherwise
		// works as we go from left to right
		if (!del_start->mNext || del_start->mHeight <= del_start->mNext->mHeight) {
			del_start = del_start->mNext;
		}

		mPieces.remove(del);
	}
	mCursorEnd = mCursor;

	mPieces.pushUndoPoint();
}

void TextEditor::undo() {
	mPieces.undo();
}

void TextEditor::redo() {
	mPieces.redo();
}

TextEditor::~TextEditor() {
}

// ------------------------------ ITERATOR ------------------------------ //

Iterator TextEditor::begin() { return Iterator(this); }
alni TextEditor::end() { return NULL; }

Iterator::Iterator(const TextEditor* aSelf) {
	mSelf = aSelf;
	mNode = mSelf->mPieces.first();
	mVal = mSelf->getPieceLine(mNode->mPiece, 0)[0];
}

bool Iterator::inputLeft() {
	return mNode;
}

Char Iterator::character() {
	return mVal;
}

void Iterator::operator++() {
	auto nl = isNewLineChar(mVal);
	auto diff = Pointer{ nl, !nl };
	mPtr += diff;
	mOffset += diff;

	if (mOffset == mNode->mPiece->diffPtr()) {
		mNode = mNode->mNext;
		mOffset = { 0, 0 };
		if (!mNode) {
			return;
		}
	}

	mVal = mSelf->getPieceLine(mNode->mPiece, mOffset.mLine)[mOffset.mCol];
}

bool Iterator::operator!=(alni const& iter) {
	return inputLeft();
}

bool Iterator::operator==(alni const& iter) {
	return !inputLeft();
}

Char Iterator::operator->() { return mVal; }
const Iterator& Iterator::operator*() { return *this; }
