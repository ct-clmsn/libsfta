/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with OrderedVector class.
 *
 *****************************************************************************/

#ifndef _SFTA_ORDERED_VECTOR_HH_
#define _SFTA_ORDERED_VECTOR_HH_

// Standard library header files
#include <vector>
#include <algorithm>

// SFTA header files
#include <sfta/convert.hh>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class Key
	>
	class OrderedVector;
}


/**
 * @brief  Implementation of a set using ordered vector
 *
 * This class implements the interface of a set (the same interface as
 * std::set) using ordered vector as the underlying data structure.
 *
 * @tparam  Key  Key type: type of the elements contained in the container.
 *               Each elements in a set is also its key.
 */
template
<
	class Key
>
class SFTA::OrderedVector
{
private:  // Private data types

	typedef SFTA::Private::Convert Convert;

	typedef std::vector<Key> VectorType;

public:   // Public data types

	typedef typename VectorType::iterator iterator;
	typedef typename VectorType::const_iterator const_iterator;
	typedef typename VectorType::const_reference const_reference;

private:  // Private data members

	VectorType vec_;


private:  // Private methods

	bool vectorIsSorted() const
	{
		for (const_iterator itVec = vec_.begin() + 1; itVec < vec_.end(); ++itVec)
		{	// check that the vector is sorted
			if (*(itVec - 1) >= *itVec)
			{	// in case there is an unordered pair (or there is one element twice)
				return false;
			}
		}

		return true;
	}


public:   // Public methods

	OrderedVector()
		: vec_()
	{ }

	explicit OrderedVector(const VectorType& vec)
		: vec_(vec)
	{
		// sort
		std::sort(vec_.begin(), vec_.end());

		// remove duplicates
		typename VectorType::iterator it = std::unique(vec_.begin(), vec_.end());
		vec_.resize(it - vec_.begin());
	}


	inline void insert(const Key& x)
	{
		// Assertions
		assert(vectorIsSorted());

		// perform binary search (cannot use std::binary_search because it is
		// ineffective due to not returning the iterator to the position of the
		// desirable insertion in case the searched element is not present in the
		// range)
		typename VectorType::iterator first = vec_.begin();
		typename VectorType::iterator last = vec_.end();

		while (first < last)
		{	// while the pointers do not overlap
			typename VectorType::iterator middle = first + (last - first) / 2;
			if (*middle == x)
			{	// in case we found x
				return;
			}
			else if (*middle < x)
			{	// in case middle is less than x
				first = middle + 1;
			}
			else
			{	// in case middle is greater than x
				last = middle;
			}
		}

		vec_.push_back(0);

		for (typename VectorType::iterator itVec = vec_.end() - 1;
			itVec != first; --itVec)
		{	// shift successive elements in the vector
			*itVec = *(itVec - 1);
		}

		// insert the new element
		*first = x;

		// Assertions
		assert(vectorIsSorted());
	}


	inline void clear()
	{
		vec_.clear();
	}

	inline size_t size() const
	{
		return vec_.size();
	}

	OrderedVector Union(const OrderedVector& rhs) const
	{
		VectorType newVector;

		typename VectorType::const_iterator lhsIt = vec_.begin();
		typename VectorType::const_iterator rhsIt = rhs.vec_.begin();

		while ((lhsIt != vec_.end()) || (rhsIt != rhs.vec_.end()))
		{	// until we get to the end of both vectors
			if (lhsIt == vec_.end())
			{	// if we are finished with the left-hand side vector
				newVector.push_back(*rhsIt);
				++rhsIt;
			}
			else if (rhsIt == rhs.vec_.end())
			{	// if we are finished with the right-hand side vector
				newVector.push_back(*lhsIt);
				++lhsIt;
			}
			else
			{
				if (*lhsIt < *rhsIt)
				{
					newVector.push_back(*lhsIt);
					++lhsIt;
				}
				else if (*lhsIt > *rhsIt)
				{
					newVector.push_back(*rhsIt);
					++rhsIt;
				}
				else
				{	// in case they are equal
					newVector.push_back(*rhsIt);
					++rhsIt;
					++lhsIt;
				}
			}
		}

		return OrderedVector(newVector);
	}


	template <class InputIterator>
	inline void insert(iterator position, InputIterator first, InputIterator last)
	{
		vec_.insert(position, first, last);
		std::sort(vec_.begin(), vec_.end());
	}


	inline void Erase(size_t position)
	{
		// Assertions
		assert(position < vec_.size());

		vec_.erase(vec_.begin() + position);
	}

	inline iterator begin()
	{
		return vec_.begin();
	}

	inline bool empty() const
	{
		return vec_.empty();
	}

	inline const_iterator begin() const
	{
		return vec_.begin();
	}

	inline iterator end()
	{
		return vec_.end();
	}

	inline const_iterator end() const
	{
		return vec_.end();
	}

	inline const_reference operator[](size_t n) const
	{
		return vec_[n];
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
	friend std::ostream& operator<<(std::ostream& os, const OrderedVector& vec)
	{
		return (os << Convert::ToString(vec.vec_));
	}

	bool operator==(const OrderedVector& rhs) const
	{
		return (vec_ == rhs.vec_);
	}

	bool operator<(const OrderedVector& rhs) const
	{
		return std::lexicographical_compare(vec_.begin(), vec_.end(),
			rhs.vec_.begin(), rhs.vec_.end());
	}

};

#endif
