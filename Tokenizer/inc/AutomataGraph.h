
#pragma once

#include "list.h"
#include "array.h"
#include "array2d.h"
#include "mat.h"
#include "map.h"

namespace tp {

	template <typename tAlphabetType, typename tStateType, typename tStateType tNoStateVal, typename tStateType tFailedStateVal>
	class TransitionMatrix;

	template <typename tAlphabetType, typename tStateType, typename tStateType tNoStateVal, typename tStateType tFailedStateVal>
	class DFA;

	// Non-Deterministic Finite-State Automata
	template <typename tAlphabetType, typename tStateType, typename tStateType tNoStateVal, typename tStateType tFailedStateVal>
	class NFA {

		static_assert(TypeTraits<tAlphabetType>::isIntegral, "tAlphabetType must be enumerable.");

	public: struct Vertex;

	private:

		struct Edge {
			Vertex* mVertex = NULL;
			bool mConsumesSymbol = false;
			Range<tAlphabetType> mAcceptingRange;
			bool mAcceptsAll = false;
			bool mExclude = false;

			bool isTransition(const tAlphabetType& symbol) {
				if (symbol == NULL) {
					return false;
				}
				if (!mConsumesSymbol || mAcceptsAll) {
					return true;
				}
				bool const in_range = (symbol >= mAcceptingRange.mBegin && symbol <= mAcceptingRange.mEnd);
				return in_range != mExclude;
			}
		};

	public:

		struct Vertex {
			List<Edge> edges;
			tStateType termination_state = tNoStateVal;

#ifdef ENV_BUILD_DEBUG
			ualni debug_idx = 0;
#endif

			ualni flag = NULL;
		};

	private:

		friend DFA<tAlphabetType, tStateType, tNoStateVal, tFailedStateVal>;

		List<Vertex> mVrticies;
		Vertex* mStart = NULL;

	public:

		NFA() {}

		Vertex* addVertex() {
			auto node = mVrticies.newNode();

#ifdef ENV_BUILD_DEBUG
			node->data.debug_idx = mVrticies.length() + 1;
#endif

			mVrticies.pushBack(node);
			return &node->data;
		}

		void addTransition(Vertex* from, Vertex* to, Range<tAlphabetType> range, bool consumes, bool accepts_all, bool exclude) {
			Edge edge; 
			edge.mVertex = to;
			edge.mConsumesSymbol = consumes;
			edge.mAcceptingRange = range;
			edge.mExclude = exclude;
			edge.mAcceptsAll = accepts_all;
			from->edges.pushBack(edge);
		}

		void setStartVertex(Vertex* start) {
			mStart = start;
		}

		Vertex* getStartVertex() {
			return mStart;
		}

		void setVertexState(Vertex* vertex, tStateType state) {
			vertex->termination_state = state;
		}

		bool isValid() const {
			if (!mStart) {
				return false;
			}
			return true;
		}

		Range<tAlphabetType> alphabetRange() const {
			tAlphabetType start = 0, end = 0;

			Range<tAlphabetType> all_range = { std::numeric_limits<tAlphabetType>::min(), std::numeric_limits<tAlphabetType>::max() };

			bool first = true;
			for (auto vertex : mVrticies) {
				for (auto edge : vertex.data().edges) {
					if (!edge.data().mConsumesSymbol) {
						continue;
					}

					auto const& tran_range = edge.data().mAcceptingRange;

					if (edge.data().mAcceptsAll || edge.data().mExclude) {
						return all_range;
					}
					
					if (tran_range.mBegin < start || first) {
						start = tran_range.mBegin;
					}
					if (tran_range.mEnd > end || first) {
						end = tran_range.mEnd;
					}

					first = false;
				}
			}
			return { start, end + 1 };
		}

		// vertices that are reachable from initial set with no input consumption (E-transitions)
		// does not includes initial set
		void closure(const List<Vertex*>& set, List<Vertex*>& closure, ualni unique_call_id) {
			List<Vertex*> marked;
			marked = set;

			while (marked.length()) {
				auto first = marked.first()->data;

				if (first->flag != unique_call_id) {
					first->flag = unique_call_id;
					closure.pushBack(first);
				}
				
				for (auto edge : first->edges) {
					if (!edge.data().mConsumesSymbol && edge.data().mVertex->flag != unique_call_id) {
						marked.pushBack(edge.data().mVertex);
					}
				}

				marked.popFront();
			}
		}

		// vertices that are reachable from initial set with symbol transition
		void move(const List<Vertex*>& set, List<Vertex*>& reachables, tAlphabetType symbol, ualni unique_call_id) {
			for (auto vertex : set) {
				for (auto edge : vertex->edges) {
					if (!edge.data().mConsumesSymbol) {
						continue;
					}
					bool transition = edge.data().isTransition(symbol);
					if (transition && edge.data().mVertex->flag != unique_call_id) {
						edge.data().mVertex->flag = unique_call_id;
						reachables.pushBack(edge.data().mVertex);
					}
				}
			}
		}
	};

	// Deterministic Finite-State Automata
	template <typename tAlphabetType, typename tStateType, typename tStateType tNoStateVal, typename tStateType tFailedStateVal>
	class DFA {

		static_assert(TypeTraits<tAlphabetType>::isIntegral, "tAlphabetType must be enumerable.");
		friend TransitionMatrix<tAlphabetType, tStateType, tNoStateVal, tFailedStateVal>;

		struct Vertex {

			struct Edge {
				Vertex* vertex = NULL;
				tAlphabetType transition_code = NULL;
			};

			List<Edge> edges;
			tStateType termination_state = tNoStateVal;
			bool marked;
		};

		List<Vertex> mVrticies;
		Vertex* mStart = NULL;
		const Vertex* mIter = NULL;
		Range<tAlphabetType> mAlphabetRange;
		bool mTrapState = false;

		typedef typename NFA<tAlphabetType, tStateType, tNoStateVal, tFailedStateVal>::Vertex NState;

		struct DstateKey {
			const List<NState*>* nstates;
			bool operator==(const DstateKey& in) {
				if (nstates->length() != in.nstates->length()) {
					return false;
				}
				// FIXME : make linear time
				for (auto state : *nstates) {
					bool found = false;
					for (auto in_state : *in.nstates) {
						if (state.data() == in_state.data()) {
							found = true;
							break;
						}
					}

					if (!found) {
						return false;
					}
				}

				return true;
			}
		};

		static ualni dstate_hashfunc(DstateKey key) {
			alni out = 0;
			for (auto state : *key.nstates) {
				out += alni(state.data());
			}
			return out;
		};

	public:

		DFA(NFA<tAlphabetType, tStateType, tNoStateVal, tFailedStateVal>& nfa) {
			if (!nfa.isValid()) {
				return;
			}
			
			mAlphabetRange = nfa.alphabetRange();

			// all NFA states that are reachable from initial DFA State for specific symbol in alphabet
			struct DState {

				struct DTransition {
					DState* state = NULL;
					tAlphabetType accepting_code;
				};

				List<NState*> nstates;
				List<DTransition> transitions;

				Vertex* dvertex = NULL; // relevant DFA vertex

#ifdef ENV_BUILD_DEBUG
				ualni debug_idx = 0;
#endif
			};

			// includes closure of NFA start state by definition
			auto start_state = new DState();
			nfa.closure({ nfa.getStartVertex() }, start_state->nstates, ualni(start_state));

			HashMap<DState*, DstateKey, dstate_hashfunc, 256> dstates;

			dstates.put({ &start_state->nstates }, start_state);

			List<DState*> working_set = { start_state };

			// while there is items to work with
			auto current_dstate = working_set.first();
			while (current_dstate) {

				// check all possible transitions for any symbol
				for (auto symbol : mAlphabetRange) {

					List<NState*> reachable_nstates;

					nfa.move(current_dstate->data->nstates, reachable_nstates, symbol, ualni(current_dstate + symbol));
					nfa.closure(reachable_nstates, reachable_nstates, ualni(current_dstate + symbol));

					if (!reachable_nstates.length()) {
						continue;
					}

					DState* target_dstate = NULL;

					// check if set of all reachable NFA states already forms existing DFA state
					auto idx = dstates.presents({ &reachable_nstates });
					if (idx) {
						target_dstate = dstates.getSlotVal(idx);
					}
					
					if (!target_dstate) {
						// register new DFA state
						target_dstate = new DState();

#ifdef ENV_BUILD_DEBUG
						target_dstate->debug_idx = dstates.size();
#endif

						target_dstate->nstates = reachable_nstates;

						// append to working stack
						working_set.pushBack(target_dstate);
						dstates.put({ &target_dstate->nstates }, target_dstate);
					}

					// add transition to DFA state
					current_dstate->data->transitions.pushBack({ target_dstate, symbol });
				}

				working_set.popFront();
				current_dstate = working_set.first();
			}

			// create own vertices
			for (auto node : dstates) {
				tStateType state = tNoStateVal;
				for (auto iter : node->val->nstates) {
					if (iter->termination_state != tNoStateVal) {
						state = iter->termination_state;
						break;
					}
				}
				node->val->dvertex = addVertex(state);
			}

			// connect all vertices
			for (auto node : dstates) {
				for (auto edge : node->val->transitions) {
					addTransition(node->val->dvertex, edge.data().state->dvertex, edge.data().accepting_code);
				}
			}

			// set the starting vertex
			mStart = start_state->dvertex;

			// cleanup
			for (auto node : dstates) {
				delete node->val;
			}

			collapseEquivalentVertices();
			mAlphabetRange = alphabetRange();
		}

		tStateType move(tAlphabetType symbol) {
			if (mTrapState || !mIter) {
				return tNoStateVal;
			}

			for (auto edge : mIter->edges) {
				if (edge.data().transition_code == symbol) {
					mIter = edge.data().vertex;
					return mIter->termination_state;
				}
			}

			mTrapState = true;
			return tNoStateVal;
		}

		void start() {
			mIter = mStart;
			mTrapState = false;
		}

		uhalni nVerts() const  {
			return (uhalni) mVrticies.length();
		}
		
		Range<tAlphabetType> getRange() const{
			return mAlphabetRange;
		}

	private:

		Range<tAlphabetType> alphabetRange() const {
			Range<tAlphabetType> out;
			for (auto vertex : mVrticies) {
				vertex.data().marked = false;
			}

			bool first = true;
			alphabetRangeUtil(mStart, &out, first);
			out.mEnd++;
			return out;
		}

		void alphabetRangeUtil(Vertex* vert, Range<tAlphabetType>* out, bool& first) const {
			vert->marked = true;
			for (auto edge : vert->edges) {
				auto const code = edge.data().transition_code;

				if (first) {
					*out = { code, code };
					first = false;
				}

				if (code < out->mBegin) {
					out->mBegin = code;
				}
				if (code > out->mEnd) {
					out->mEnd = code;
				}

				if (!edge.data().vertex->marked) {
					alphabetRangeUtil(edge.data().vertex, out, first);
				}
			}
		}

		void collapseEquivalentVertices() {

		}

		Vertex* addVertex(tStateType state) {
			auto node = mVrticies.addNodeBack();
			node->data.termination_state = state;
			return &node->data;
		}

		void addTransition(Vertex* from, Vertex* to, tAlphabetType transition_symbol) {
			from->edges.pushBack({ to, transition_symbol });
		}
	};

	template <typename tAlphabetType, typename tStateType, typename tStateType tNoStateVal, typename tStateType tFailedStateVal>
	class TransitionMatrix {

		static_assert(TypeTraits<tAlphabetType>::isIntegral, "tAlphabetType must be enumerable.");

		tp::Array2D<ualni> mTransitions;
		tp::Array<tStateType> mStates;
		Range<tAlphabetType> mSymbolRange = NULL;
		
		ualni mIter = 0;
		ualni mIterPrev = 0;
		ualni mStart = 0;

	public:

		TransitionMatrix() {}

		void construct(const DFA<tAlphabetType, tStateType, tNoStateVal, tFailedStateVal>& dfa) {
			mSymbolRange = dfa.getRange();
			auto range_len = uhalni(mSymbolRange.mEnd - mSymbolRange.mBegin);
			vec2<uhalni> dim = { range_len ? range_len : 1, (uhalni) (dfa.nVerts() + 1) };

			mTransitions.reserve(dim);
			mTransitions.assign(dfa.nVerts());
			mStates.reserve(dim.y);

			for (auto vertex : dfa.mVrticies) {
				auto state = vertex.data().termination_state;
				mStates[vertex.idx()] = state;
			}

			mStates[dfa.nVerts()] = tFailedStateVal;

			for (auto vertex : dfa.mVrticies) {
				if (&vertex.data() == dfa.mStart) {
					mStart = mIter = mIterPrev = vertex.idx();
				}
			}

			for (auto vertex : dfa.mVrticies) {
				for (auto edge : vertex.data().edges) {
					ualni idx = 0;
					for (auto vertex : dfa.mVrticies) {
						if (edge.data().vertex == &vertex.data()) {
							idx = vertex.idx();
							break;
						}
					}
					auto const code = edge.data().transition_code;
					mTransitions.set(code - mSymbolRange.mBegin, (uhalni) vertex.idx(), idx);
				}
			}
		}

		bool isTraped() {
			return mStates[mIter] == tFailedStateVal;
		}

		tStateType move(tAlphabetType symbol) {
			if (symbol >= mSymbolRange.mBegin && symbol < mSymbolRange.mEnd) {
				mIter = mTransitions.get((uhalni) (symbol - mSymbolRange.mBegin), (uhalni) mIter);
			}
			else {
				mIter = mStates.length() - 1;
			}
			
			if (mIterPrev == mStart) {
				if (mStates[mIter] == tFailedStateVal) {
					reset();
					return tFailedStateVal;
				}
				else {
					mIterPrev = mIter;
					return tNoStateVal;
				}
			}
			else {
				if (mStates[mIter] == tFailedStateVal) {
					if (mStates[mIterPrev] != tNoStateVal) {
						auto out = mStates[mIterPrev];
						reset();
						return out;
					}
					else {
						reset();
						return tFailedStateVal;
					}
				}
				else {
					mIterPrev = mIter;
					return tNoStateVal;
				}
			}

			mIterPrev = mIter;
			return mStates[mIter];
		}

		void reset() {
			mIter = mStart;
			mIterPrev = mStart;
		}
	};
};