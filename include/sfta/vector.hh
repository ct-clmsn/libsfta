/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file that contains the Vector class.
 *
 *****************************************************************************/

#ifndef _SFTA_VECTOR_HH_
#define _SFTA_VECTOR_HH_

// SFTA header files
#include <sfta/convert.hh>

// Standard library header files
#include <vector>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename T
	>
	class Vector;
}


template
<
	typename T
>
class SFTA::Vector
	: public std::vector<T>
{
private:   // Private data types

	typedef std::vector<T> ParentClass;

	typedef SFTA::Private::Convert Convert;

public:   // Public methods

	Vector()
		: ParentClass()
	{ }

	explicit Vector(size_t n, const T& value = T())
		: ParentClass(n, value)
	{ }


	/**
	 * @brief  Overloaded << operator
	 *
	 * Overloaded << operator for output stream.
	 *
	 * @see  ToString()
	 *
	 * @param[in]  os   The output stream
	 * @param[in]  vec  The vector
	 *
	 * @returns  Modified output stream
	 */
	friend std::ostream& operator<<(std::ostream& os, const Vector<T>& vec)
	{
		return (os << Convert::ToString(static_cast<std::vector<T> >(vec)));
	}
};

#endif