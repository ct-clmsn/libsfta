/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with CompactVariableAssignment structure.
 *
 *****************************************************************************/

#ifndef _SFTA_COMPACT_VARIABLE_ASSIGNMENT_HH_
#define _SFTA_COMPACT_VARIABLE_ASSIGNMENT_HH_


// Standard library headers
#include <cassert>
#include <string>
#include <vector>
#include <stdexcept>


// insert class into correct namespace
namespace SFTA
{
	namespace Private
	{
		template
		<
			size_t MaxSize
		>
		struct CompactVariableAssignment;
	}
}


/**
 * @brief   Compact variable assignment
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * A class that represents assignments to Boolean variables in a compact way.
 * Assigned values can be one of '0', '1' and 'X', where 'X' means <em>don't
 * care</em>.
 *
 * @tparam  Variables  The number of Boolean variables.
 */
template
<
	size_t Variables
>
struct SFTA::Private::CompactVariableAssignment
{
public:   // Public data types

	enum
	{
		VariablesCount = Variables
	};


	enum
	{
		ZERO = 0x01,
		ONE = 0x02,
		DONT_CARE = 0x03
	};

	typedef std::vector<CompactVariableAssignment> AssignmentList;

private:  // Private data types

	enum
	{
		BitsPerVariable = 2
	};


	enum
	{
		// needs to be multiple of BitsPerVariable
		BitsInChar = 8
	};


	enum
	{
		NumberOfChars = (VariablesCount*BitsPerVariable - 1) / BitsInChar + 1
	};


	enum
	{
		DefaultMask = 0x03
	};


private:  // Private data members

	/**
	 * @brief  The value of the assignment
	 *
	 * Array of characters representing the value of the assignment.
	 */
	char vars_[NumberOfChars];


	/**
	 * @brief  The name of the Log4cpp category for logging
	 *
	 * The name of the Log4cpp category used for logging messages from this
	 * class.
	 */
	static const char* LOG_CATEGORY_NAME;


private:  // Private methods


	/**
	 * @brief  Gets index of @c char at given variable index
	 *
	 * Returns index of the @c char that holds the value of variable at given
	 * index.
	 *
	 * @see  getIndexInsideChar()
	 *
	 * @param[in]  index  Index of the Boolean variable
	 *
	 * @returns  Index of the @c char in which the variable has value
	 */
	static inline size_t getIndexOfChar(size_t index)
	{
		return (index * BitsPerVariable) / BitsInChar;
	}


	/**
	 * @brief  Gets index of bit inside a @c char for given variable index
	 *
	 * Returns index of a bit in a @c char that starts the block of bits that
	 * hold the value of variable at given index.
	 *
	 * @see  getIndexOfChar()
	 *
	 * @param[in]  index  Index of the Boolean variable
	 *
	 * @returns  Index of the bit that holds the value of the variable
	 */
	static inline size_t getIndexInsideChar(size_t index)
	{
		return (index * BitsPerVariable) % BitsInChar;
	}


public:   // Public methods

	CompactVariableAssignment()
		: vars_()
	{
		for (size_t i = 0; i < Size(); ++i)
		{	// for each variable
			SetIthVariableValue(i, DONT_CARE);
		}
	}

	CompactVariableAssignment(size_t n)
		: vars_()
	{
		for (size_t i = 0; i < Size(); ++i)
		{	// for each variable
			SetIthVariableValue(i, (n & (1 << i) != 0)? ONE : ZERO);
		}
	}

	/**
	 * @brief  Constructor from std::string
	 *
	 * A constructor that creates an instance of the class from a std::string
	 * that desribes the assignments to variable. The string can only hold
	 * values '0', '1' and 'X', where 'X' means <em>don't care</em>.
	 *
	 * @param[in]  value  The string with the value of variables
	 */
	explicit CompactVariableAssignment(const std::string& value)
		: vars_()
	{
		// Assertions
		assert(value.length() == VariablesCount);

		for (size_t i = 0; i < VariablesCount; ++i)
		{	// load the string into the array of variables
			char val = 0;
			switch (value[i])
			{
				case '0': val = ZERO;      break;
				case '1': val = ONE;       break;
				case 'X': val = DONT_CARE; break;
				default: throw std::runtime_error("Invalid input value!");
			}

			// prepare the mask
			char mask = (DefaultMask << getIndexInsideChar(i)) ^ static_cast<char>(-1);

			// mask out bits at given position by the mask
			vars_[getIndexOfChar(i)] &= mask;

			// prepare new value of given bits
			val <<= getIndexInsideChar(i);

			// insert the value of given bits
			vars_[getIndexOfChar(i)] |= val;
		}
	}


	/**
	 * @brief  Returns value of variable at given index
	 *
	 * Returns the value of variable at given index.
	 *
	 * @param[in]  i  Index of the variable
	 *
	 * @returns  Value of the variable
	 */
	inline char GetIthVariableValue(size_t i) const
	{
		// Assertions
		assert(i < VariablesCount);

		return (vars_[getIndexOfChar(i)] >> getIndexInsideChar(i)) & DefaultMask;
	}


	inline void SetIthVariableValue(size_t i, char value)
	{
		// Assertions
		assert(i < VariablesCount);

		switch (value)
		{
			case ZERO:      break;
			case ONE:       break;
			case DONT_CARE: break;
			default:        throw std::runtime_error("Invalid input value!");
		}

		// prepare the mask
		char mask = (DefaultMask << getIndexInsideChar(i)) ^ static_cast<char>(-1);

		// mask out bits at given position by the mask
		vars_[getIndexOfChar(i)] &= mask;

		// prepare new value of given bits
		value <<= getIndexInsideChar(i);

		// insert the value of given bits
		vars_[getIndexOfChar(i)] |= value;
	}


	/**
	 * @brief  The number of variables
	 *
	 * Returns the number of variables of the variable assignment.
	 *
	 * @returns  The number of variables of the variable assignment
	 */
	inline size_t Size() const
	{
		return VariablesCount;
	}


	/**
	 * @brief  Returns string representation
	 *
	 * Transforms the variable assignment to its string representation.
	 *
	 * @see  operator<<()
	 *
	 * @returns  The string representation of the variable assignment
	 */
	std::string ToString() const
	{
		std::string result;

		for (size_t i = 0; i < VariablesCount; ++i)
		{	// append all variables to the string
			switch (GetIthVariableValue(i))
			{
				case ZERO:      result += '0'; break;
				case ONE:       result += '1'; break;
				case DONT_CARE: result += 'X'; break;
				default: throw std::runtime_error("Invalid bit value!");
			}
		}

		return result;
	}


	static AssignmentList GetAllAssignments()
	{
		std::string str;
		for (size_t i = 0; i < VariablesCount; ++i)
		{	// for all variables
			str += 'X';
		}

		AssignmentList lst;
		lst.push_back(CompactVariableAssignment(str));
		return lst;
	}


	CompactVariableAssignment& operator++()
	{
		for (size_t i = 0; i < Size(); ++i)
		{	// for each variable
			char value = GetIthVariableValue(i);
			if (value == ZERO)
			{	// in case we can stop here
				SetIthVariableValue(i, ONE);
				return *this;
			}
			else if (value == ONE)
			{	// we change to zero and continue to search zero
				SetIthVariableValue(i, ZERO);
			}
			else
			{	// otherwise
				throw std::runtime_error(
					"An attempt to increment assignment with invalid states");
			}
		}

		return *this;
	}

	/**
	 * @brief  Overloaded << operator
	 *
	 * Overloaded << operator for output stream.
	 *
	 * @see  ToString()
	 *
	 * @param[in]  os    The output stream
	 * @param[in]  asgn  Assignment to the variables
	 *
	 * @returns  Modified output stream
	 */
	friend std::ostream& operator<<(std::ostream& os,
		const CompactVariableAssignment& asgn)
	{
		return (os << asgn.ToString());
	}

	friend bool operator<(const CompactVariableAssignment& lhs,
		const CompactVariableAssignment& rhs)
	{
		// Assertions
		assert(lhs.Size() == rhs.Size());

		for (size_t i = 0; i < lhs.Size(); ++i)
		{
			char lhsIthValue = lhs.GetIthVariableValue(lhs.Size() - i - 1);
			char rhsIthValue = rhs.GetIthVariableValue(rhs.Size() - i - 1);

			switch (lhsIthValue)
			{
				case ZERO:
					switch (rhsIthValue)
					{
						case ZERO: continue; break;
						case ONE: return true; break;
						case DONT_CARE: return true; break;
						default: throw std::runtime_error("Invalid variable assignment value");
							break;
					}
					break;

				case ONE:
					switch (rhsIthValue)
					{
						case ZERO: return false; break;
						case ONE: continue; break;
						case DONT_CARE: return false; break;
						default: throw std::runtime_error("Invalid variable assignment value");
							break;
					}
					break;

				case DONT_CARE:
					switch (rhsIthValue)
					{
						case ZERO: return false; break;
						case ONE: return true; break;
						case DONT_CARE: continue; break;
						default: throw std::runtime_error("Invalid variable assignment value");
							break;
					}
					break;

				default: throw std::runtime_error("Invalid variable assignment value");
					break;
			}
		}

		return false;
	}

public:   // Public static methods

	static CompactVariableAssignment GetUniversalSymbol()
	{
		return CompactVariableAssignment();
	}
};

#endif
