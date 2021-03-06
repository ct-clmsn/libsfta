/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with TimbukTDTABuilder class.
 *
 *****************************************************************************/

#ifndef _SFTA_TIMBUK_TD_TA_BUILDER_HH_
#define _SFTA_TIMBUK_TD_TA_BUILDER_HH_

// Standard library header files
#include <vector>

// SFTA header files
#include <sfta/abstract_ta_builder.hh>
#include <sfta/convert.hh>

// Boost header files
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class TDTreeAutomaton
	>
	class TimbukTDTABuilder;
}

/**
 * @brief   Top-down tree automata builder from Timbuk format
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This class is used to build top-down tree automata from files that have
 * Timbuk format.
 *
 * @tparam  TDTreeAutomaton   Top-down tree automaton type.
 */
template
<
	class TDTreeAutomaton
>
class SFTA::TimbukTDTABuilder
	: public AbstractTABuilder
		<
			TDTreeAutomaton
		>
{
public:   // Public data types

	typedef TDTreeAutomaton TDTreeAutomatonType;

private:  // Private data types

	typedef typename TDTreeAutomatonType::LeftHandSideType LeftHandSideType;
	typedef typename TDTreeAutomatonType::RightHandSideType RightHandSideType;

	typedef SFTA::Private::Convert Convert;

public:   // Public methods 

	virtual void Build(std::istream& is, TDTreeAutomatonType* automaton) const
	{
		bool readingTransitions = false;
		std::string str;
		while (std::getline(is, str))
		{	// until we get to the end of the file
			boost::trim(str);

			boost::algorithm::replace_all(str, "  ", " ");
			boost::algorithm::replace_all(str, ", ", ",");

			std::vector<std::string> spl;
			boost::algorithm::split(spl, str, isspace,
				boost::algorithm::token_compress_on);

			if ((spl.size() == 0) || ((spl.size() == 1) && spl[0] == ""))
			{	// if we read an empty line
				continue;
			}
			else if (readingTransitions)
			{	// in case we are reading transitions
				if (spl.size() != 3)
				{	// if the format is wrong
					throw std::runtime_error(__func__ +
						std::string("Unknown token in input stream: ") + Convert::ToString(str));
				}

				LeftHandSideType lhs = spl[2];

				std::string leftSide = spl[0];
				size_t pos = leftSide.find('(');
				if (pos == leftSide.npos)
				{	// in case we are dealing with nullary symbol
					SFTA_LOGGER_DEBUG("Adding transition: " + spl[0] + " -> " + spl[2]);

					RightHandSideType rhs;
					rhs.insert(typename RightHandSideType::value_type());

					automaton->AddTransition(lhs, spl[0], rhs);
				}
				else
				{	// in case we are not dealing with nullary symbol
					std::string symbol = leftSide.substr(0, pos);

					size_t endPos = leftSide.find(')');
					std::string stateStr = leftSide.substr(pos + 1, endPos - pos - 1);

					std::vector<std::string> states;

					boost::algorithm::split(states, stateStr, ispunct,
						boost::algorithm::token_compress_on);

					typename RightHandSideType::value_type rhsElem;
					for (size_t i = 0; i < states.size(); ++i)
					{	// for each state
						rhsElem.push_back(states[i]);
					}

					RightHandSideType rhs;
					rhs.insert(rhsElem);

					SFTA_LOGGER_DEBUG("Adding transition: " + spl[0] + " -> " + spl[2]);

					automaton->AddTransition(lhs, symbol, rhs);
				}

				continue;
			}
			else if (spl[0] == "Ops")
			{	// we dispose of definition of arity for operations
				continue;
			}
			else if (spl[0] == "Automaton")
			{	// we are not interested in the name of the automaton
				continue;
			}
			else if (spl[0] == "States")
			{	// we are reading states
				for (size_t i = 1; i < spl.size(); ++i)
				{	// for each state in the list
					std::string stateName = spl[i];
					size_t pos = stateName.find(':');
					stateName.erase(pos);
					SFTA_LOGGER_DEBUG("Adding state: " + stateName);

					automaton->AddState(stateName);
				}

				continue;
			}
			else if ((spl.size() >= 2) &&
				((spl[0] + " " + spl[1]) == "Final States"))
			{	// if we are reading final states
				for (size_t i = 2; i < spl.size(); ++i)
				{	// for each final state in the list
					SFTA_LOGGER_DEBUG("Setting state as final: " + spl[i]);

					automaton->SetStateInitial(spl[i]);
				}

				continue;
			}
			else if (spl[0] == "Transitions")
			{	// if we are reading transitions
				readingTransitions = true;

				continue;
			}
			else
			{	// unknown token
				throw std::runtime_error("Unknown token in input stream");
			}
		}
	}
};

#endif
