/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file with SymbolicBUTreeAutomaton class.
 *
 *****************************************************************************/

#ifndef _SYMBOLIC_BU_TREE_AUTOMATON_HH_
#define _SYMBOLIC_BU_TREE_AUTOMATON_HH_

// SFTA headers
#include <sfta/abstract_bu_tree_automaton.hh>
#include <sfta/ordered_vector.hh>
#include <sfta/vector_map.hh>

// Loki headers
#include <loki/SmartPtr.h>

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class MTBDDTransitionTableWrapper,
		typename State,
		typename Symbol,
		class RightHandSide
	>
	class SymbolicBUTreeAutomaton;
}


/**
 * @brief   Symbolically represented bottom-up tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Bottom-up tree automaton with symbolically represented transition function.
 */
template
<
	class MTBDDTransitionTableWrapper,
	typename State,
	typename Symbol,
	class RightHandSide
>
class SFTA::SymbolicBUTreeAutomaton
	: public SFTA::AbstractBUTreeAutomaton
		<
			State,
			Symbol,
			RightHandSide
		>
{
public:   // Public data types

	typedef SymbolicBUTreeAutomaton
		<
			MTBDDTransitionTableWrapper,
			State,
			Symbol,
			RightHandSide
		> Type;

	typedef State StateType;
	typedef Symbol SymbolType;

	typedef SFTA::AbstractBUTreeAutomaton
		<
			StateType,
			SymbolType,
			RightHandSide
		> ParentClass;

	typedef typename ParentClass::HierarchyRoot HierarchyRoot;

	typedef MTBDDTransitionTableWrapper MTBDDTTWrapperType;
	typedef Loki::SmartPtr<MTBDDTTWrapperType> TTWrapperPtrType;

	typedef typename MTBDDTTWrapperType::SharedMTBDDType::RootType
		RootType;

	typedef typename ParentClass::LeftHandSideType LeftHandSideType;

	typedef typename ParentClass::RightHandSideType RightHandSideType;

	typedef typename SFTA::OrderedVector
		<
			StateType
		> StateSetType;

	typedef typename SFTA::VectorMap
		<
			StateType,
			RootType
		> LHSRootContainerType;


	/**
	 * @brief  Data type for transition rule of an automaton
	 *
	 * This type is used to store elements of transition rule of an automaton.
	 */
	struct Transition
	{
		LeftHandSideType lhs;
		SymbolType symbol;
		RightHandSideType rhs;

		Transition(const LeftHandSideType& inLhs, const SymbolType& inSymbol,
			const RightHandSideType& inRhs)
			: lhs(inLhs),
				symbol(inSymbol),
				rhs(inRhs)
		{ }
	};

	typedef Transition TransitionType;


	/**
	 * @brief  @copybrief SFTA::AbstractBUTreeAutomaton::Operation
	 *
	 * @copydetails SFTA::AbstractBUTreeAutomaton::Operation
	 */
	class Operation
		: public ParentClass::Operation
	{
	};

private:  // Private data types

	typedef SFTA::Private::Convert Convert;
	typedef typename MTBDDTTWrapperType::SharedMTBDDType SharedMTBDDType;
	typedef typename SharedMTBDDType::DescriptionType TransitionMapType;


private:  // Private data members

	StateSetType states_;

	StateSetType finalStates_;

	TTWrapperPtrType ttWrapper_;

	RootType sinkSuperState_;

	LHSRootContainerType rootMap_;

private:  // Private methods

	SymbolicBUTreeAutomaton& operator=(const SymbolicBUTreeAutomaton& aut);

protected:// Protected methods


	virtual Operation* createOperation() const = 0;

	inline RootType getRoot(const LeftHandSideType& lhs) const
	{
		return rootMap_.GetValue(lhs);
	}

	inline void setRoot(const LeftHandSideType& lhs, RootType root)
	{
		rootMap_.SetValue(lhs, root);
	}

	void copyStates(const Type& aut)
	{
		states_.insert(aut.states_);
		finalStates_.insert(aut.finalStates_);

		// also copy superstates
		rootMap_.insert(aut.rootMap_);
	}

	void copyStates(const HierarchyRoot& aut)
	{
		const Type* autSym = static_cast<Type*>(0);

		if ((autSym = dynamic_cast<const Type*>(&aut)) !=
			static_cast<const Type*>(0))
		{
			return copyStates(*autSym);
		}

		throw std::runtime_error(__func__ + std::string(": Invalid types"));
	}

	inline RootType getSinkSuperState() const
	{
		return sinkSuperState_;
	}

	inline const LHSRootContainerType& getRootMap() const
	{
		return rootMap_;
	}

	inline const StateSetType& getStates() const
	{
		return states_;
	}

	inline bool isStateLocal(const StateType& state) const
	{
		return (states_.find(state) != states_.end());
	}

	bool vectorContainsLocalStates(const LeftHandSideType& vec) const
	{
		for (typename LeftHandSideType::const_iterator it = vec.begin();
			it != vec.end(); ++it)
		{
			if (!isStateLocal(*it))
			{
				return false;
			}
		}

		return true;
	}

public:   // Public methods

	/**
	 * @brief  Default constructor
	 *
	 * Default constructor.
	 */
	SymbolicBUTreeAutomaton()
		: states_(),
			finalStates_(),
			ttWrapper_(new MTBDDTTWrapperType()),
			sinkSuperState_(GetTTWrapper()->GetMTBDD()->CreateRoot()),
			rootMap_(sinkSuperState_)
	{
		// Assertions
		assert(ttWrapper_ != static_cast<TTWrapperPtrType>(0));

		GetTTWrapper()->GetMTBDD()->SetBottomValue(RightHandSideType());
	}

	/**
	 * @brief  Copy constructor
	 *
	 * Copy constructor of the class.
	 *
	 * @param[in]  aut  The automaton to be copied
	 */
	SymbolicBUTreeAutomaton(const SymbolicBUTreeAutomaton& aut)
		: ParentClass(aut),
			states_(aut.states_),
			finalStates_(aut.finalStates_),
			ttWrapper_(aut.ttWrapper_),
			sinkSuperState_(aut.sinkSuperState_),
			rootMap_(aut.rootMap_)
	{
		// Assertions
		assert(ttWrapper_ != static_cast<TTWrapperPtrType>(0));
	}

	explicit SymbolicBUTreeAutomaton(TTWrapperPtrType ttWrapper)
		: states_(),
			finalStates_(),
			ttWrapper_(ttWrapper),
			sinkSuperState_(GetTTWrapper()->GetMTBDD()->CreateRoot()),
			rootMap_(sinkSuperState_)
	{
		// Assertions
		assert(ttWrapper_ != static_cast<TTWrapperPtrType>(0));
	}

	virtual void CopyStates(const HierarchyRoot& aut)
	{
		copyStates(aut);
	}

	virtual StateType AddState()
	{
		StateType newState = GetTTWrapper()->CreateState();
		states_.insert(newState);

		return newState;
	}

	virtual void SetStateFinal(const StateType& state)
	{
		// Assertions
		assert(isStateLocal(state));

		finalStates_.insert(state);
	}

	virtual bool IsStateFinal(const StateType& state) const
	{
		// Assertions
		assert(isStateLocal(state));

		return finalStates_.find(state) != finalStates_.end();
	}

	virtual void AddTransition(const LeftHandSideType& lhs,
		const SymbolType& symbol, const RightHandSideType& rhs)
	{
		// Assertions
		assert(vectorContainsLocalStates(lhs));

		RootType root = rootMap_.GetValue(lhs);
		if (root == sinkSuperState_)
		{	// in case there is not any transition from this super-state
			root = GetTTWrapper()->GetMTBDD()->CreateRoot();
			rootMap_.SetValue(lhs, root);
		}

		RightHandSideType outRhs;
		for (typename RightHandSideType::const_iterator it = rhs.begin();
			it != rhs.end(); ++it)
		{
			outRhs.insert(*it);
		}

		GetTTWrapper()->GetMTBDD()->SetValue(root, symbol, outRhs);
	}


	virtual RightHandSideType GetTransition(const LeftHandSideType& lhs,
		const SymbolType& symbol)
	{
		// Assertions
		assert(vectorContainsLocalStates(lhs));

		RightHandSide rhs;

		RootType root = rootMap_.GetValue(lhs);
		if (root == sinkSuperState_)
		{	// in case there is not any transition from this super-state
			return rhs;
		}
		else
		{
			typename SharedMTBDDType::LeafContainer output =
				GetTTWrapper()->GetMTBDD()->GetValue(root, symbol);

			for (typename SharedMTBDDType::LeafContainer::const_iterator itCont =
				output.begin(); itCont != output.end(); ++itCont)
			{
				rhs.insert(**itCont);
			}

			return rhs;
		}
	}


	/**
	 * @brief  Returns a transition table wrapper
	 *
	 * This method returns given transition table wrapper of the automaton.
	 *
	 * @returns  Transition table wrapper
	 */
	virtual TTWrapperPtrType GetTTWrapper() const
	{
		// Assertions
		assert(ttWrapper_ != static_cast<TTWrapperPtrType>(0));

		return ttWrapper_;
	}

	virtual std::vector<TransitionType> GetVectorOfTransitions() const
	{
		typedef std::vector<TransitionType> TransitionVector;

		TransitionVector result;

		for (typename LHSRootContainerType::const_iterator itRoot = rootMap_.begin();
			itRoot != rootMap_.end(); ++itRoot)
		{
			TransitionMapType transMap =
				ttWrapper_->GetMTBDD()->GetMinimumDescription(itRoot->second);

			for (typename TransitionMapType::const_iterator itTrans = transMap.begin();
				itTrans != transMap.end(); ++itTrans)
			{
				if (!(itTrans->second.empty()))
				{
					result.push_back(
						TransitionType(itRoot->first, itTrans->first, itTrans->second));
				}
			}
		}

		return result;
	}

	virtual std::string ToString() const
	{
		std::string result;
		result += "Automaton\n";
		result += "States: " + Convert::ToString(states_) + "\n";
		result += "Final states: " + Convert::ToString(finalStates_) + "\n";
		result += "Transitions: \n";

		typedef std::vector<TransitionType> TransitionVector;
		TransitionVector trans = GetVectorOfTransitions();
		for (typename TransitionVector::const_iterator itTrans = trans.begin();
			itTrans != trans.end(); ++itTrans)
		{
			result += Convert::ToString(itTrans->symbol) + " ";
			result += Convert::ToString(itTrans->lhs);
			result += " -> ";
			result += Convert::ToString(itTrans->rhs);
			result += "\n";
		}

		return result;
	}

	virtual std::vector<StateType> GetVectorOfStates() const
	{
		std::vector<StateType> result;

		for (typename StateSetType::const_iterator itStates = states_.begin();
			itStates != states_.end(); ++itStates)
		{
			result.push_back(*itStates);
		}

		return result;
	}


	virtual std::vector<StateType> GetVectorOfFinalStates() const
	{
		std::vector<StateType> result;

		for (typename StateSetType::const_iterator itStates = finalStates_.begin();
			itStates != finalStates_.end(); ++itStates)
		{
			result.push_back(*itStates);
		}

		return result;
	}
};

#endif
