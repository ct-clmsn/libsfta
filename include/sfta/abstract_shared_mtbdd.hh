/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for AbstractSharedMTBDD class.
 *
 *****************************************************************************/

#ifndef _SFTA_ABSTRACT_SHARED_MTBDD_HH_
#define _SFTA_ABSTRACT_SHARED_MTBDD_HH_


// Standard library headers
#include <string>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename Root,
		typename Leaf,
		class VariableAssignment
	>
	class AbstractSharedMTBDD;
}


/**
 * @brief   Abstract class for shared multi-terminal BDD
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Abstract class that defines a high-level interface for classes implementing
 * shared multi-terminal BDD (MTBDD).
 *
 * @tparam  Root                The type of the root of a MTBDD. Is used to
 *                              reference the root.
 * @tparam  Leaf                The type of a leaf of a MTBDD.
 * @tparam  VariableAssignment  The type that is used for representation of
 *                              Boolean variable assignment, i.e.
 *                              representation of a path in the BDD.
 */
template
<
	typename Root,
	typename Leaf,
	class VariableAssignment
>
class SFTA::AbstractSharedMTBDD
{
public:  // Public data types


	/**
	 * @brief  Type of a leaf
	 *
	 * Type of a leaf of MTBDD.
	 */
	typedef Leaf LeafType;


	/**
	 * @brief  Type of a root
	 *
	 * Type of a root of MTBDD.
	 */
	typedef Root RootType;


	/**
	 * @brief  Type of variable assignment
	 *
	 * Type of variable assignment.
	 */
	typedef VariableAssignment VariableAssignmentType;


	/**
	 * @brief  The container type for leaves
	 *
	 * The type that serves as a container of several leaves. This type is used
	 * for example by the GetValue() method.
	 *
	 * @see  GetValue()
	 */
	typedef std::vector<LeafType*> LeafContainer;


	typedef std::map<VariableAssignmentType, LeafType> DescriptionType;


	/**
	 * @brief  The base class for functors that perform @c Apply operations
	 *
	 * Abstract class that defines the interface for functors that carry out
	 * @c Apply operations.
	 */
	class AbstractApplyFunctorType
	{
	public:   // Public methods


		/**
		 * @brief  The operation of the functor
		 *
		 * Abstract method that performs the operation of the functor
		 *
		 * @param[in]  lhs  Left-hand side of the operation
		 * @param[in]  rhs  Right-hand side of the operation
		 *
		 * @returns  Result of the operation
		 */
		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs) = 0;


		/**
		 * @brief  Destructor
		 *
		 * The destructor
		 */
		virtual ~AbstractApplyFunctorType()
		{ }

	};


	/**
	 * @brief  The base class for functors that perform ternary @c Apply operations
	 *
	 * Abstract class that defines the interface for functors that carry out
	 * ternary @c Apply operations.
	 */
	class AbstractTernaryApplyFunctorType
	{
	public:   // Public methods


		/**
		 * @brief  The operation of the functor
		 *
		 * Abstract method that performs the operation of the functor
		 *
		 * @param[in]  lhs  Left-hand side of the operation
		 * @param[in]  mhs  Middle-hand side of the operation
		 * @param[in]  rhs  Right-hand side of the operation
		 *
		 * @returns  Result of the operation
		 */
		virtual LeafType operator()(const LeafType& lhs, const LeafType& mhs,
			const LeafType& rhs) = 0;


		/**
		 * @brief  Destructor
		 *
		 * The destructor
		 */
		virtual ~AbstractTernaryApplyFunctorType()
		{ }

	};


	/**
	 * @brief  The base class for functors that perform monadic @c Apply
	 *         operations
	 *
	 * Abstract class that defines the interface for functors that carry out
	 * monadic @c Apply operations.
	 */
	class AbstractMonadicApplyFunctorType
	{
	public:   // Public methods


		/**
		 * @brief  The operation of the functor
		 *
		 * Abstract method that performs the operation of the functor
		 *
		 * @param[in]  val  Operand of the operation
		 *
		 * @returns  Result of the operation
		 */
		virtual LeafType operator()(const LeafType& val) = 0;


		/**
		 * @brief  Destructor
		 *
		 * The destructor
		 */
		virtual ~AbstractMonadicApplyFunctorType()
		{ }

	};


	/**
	 * Variable type
	 *
	 * Type of variable.
	 */
	typedef unsigned VariableType;


	/**
	 * @brief  Abstract class for renaming functors
	 *
	 * This class serves as the base class for functors that should implement
	 * renaming of MTBDD's variables.
	 */
	class AbstractVariableRenamingFunctorType
	{
	public:

		/**
		 * @brief  Method that performs variable renaming
		 *
		 * Thie method is passed a variable and returns a variable to which it
		 * should be renamed. In case given variable is not to be renamed, the
		 * same name is returned.
		 *
		 * @param[in]  var  Input variable name
		 *
		 * @returns  Output variable name
		 */
		virtual VariableType operator()(const VariableType& var) = 0;


		/**
		 * @brief  Destructor
		 *
		 * The destructor
		 */
		virtual ~AbstractVariableRenamingFunctorType()
		{ }
	};


	/**
	 * @brief  Abstract class for variable predicate functors
	 *
	 * This class serves as the base class for functors that implement
	 * predicates over Boolean variables of MTBDDs.
	 */
	class AbstractVariablePredicateFunctorType
	{
	public:

		/**
		 * @brief  Method that evaluates the predicate
		 *
		 * Thie method is passed a variable and it returns a Boolean value
		 * determinining whether the implemented predicate evaluates for the
		 * variable to either true or false.
		 *
		 * @param[in]  var  Input variable name
		 *
		 * @returns  Boolean value for the predicate
		 */
		virtual bool operator()(const VariableType& var) = 0;


		/**
		 * @brief  Destructor
		 *
		 * The destructor
		 */
		virtual ~AbstractVariablePredicateFunctorType()
		{ }
	};


public:  // Public methods

	/**
	 * @brief  Sets the value of a leaf
	 *
	 * This function sets the value of a leaf at a position determined by the root
	 * of the MTBDD and assignments to Boolean variables.
	 *
	 * @see  GetValue()
	 *
	 * @param[in]  root   The root of the MTBDD in which the method works
	 * @param[in]  asgn   The position of the leaf given by the assignment to
	 *                    Boolean variables of the MTBDD
	 * @param[in]  value  The value of the leaf to be set
	 */
	virtual void SetValue(const RootType& root,
		const VariableAssignmentType& asgn, const LeafType& value) = 0;


	/**
	 * @brief  Gets references to leaves
	 *
	 * This function returns a container with all leaves that are reachable by
	 * given variable assignment. Note that the references in the container may
	 * be made invalid by following operations on the MTBDD. All the items in
	 * the container are guaranteed to be unique.
	 *
	 * @see  LeafContainer
	 * @see  SetValue()
	 *
	 * @param[in]  root   The root of the MTBDD in which the method works
	 * @param[in]  asgn   The position of the leaf given by the assignment to
	 *                    Boolean variables of the MTBDD
	 *
	 * @returns  Container of references to leaves at given position of the MTBDD
	 */
	virtual LeafContainer GetValue(const RootType& root,
		const VariableAssignmentType& asgn) = 0;


	/**
	 * @brief  Apply function for two MTBDDs
	 *
	 * Performs given Apply operation on two MTBDD specified by their roots.
	 *
	 * @see  AbstractSharedMTBDD::AbstractApplyFunctorType
	 *
	 * @param[in]  lhs   Left-hand side MTBDD
	 * @param[in]  rhs   Right-hand side MTBDD
	 * @param[in]  func  The operation to be performed on respective leaves of
	 *                   given MTBDDs
	 *
	 * @returns  Root of the MTBDD with the result of the operation
	 */
	virtual RootType Apply(const RootType& lhs, const RootType& rhs,
		AbstractApplyFunctorType* func) = 0;


	/**
	 * @brief  Ternary Apply function for two MTBDDs
	 *
	 * Performs given ternary Apply operation on two MTBDD specified by their roots.
	 *
	 * @see  AbstractSharedMTBDD::AbstractApplyFunctorType
	 *
	 * @param[in]  lhs   Left-hand side MTBDD
	 * @param[in]  mhs   Middle-hand side MTBDD
	 * @param[in]  rhs   Right-hand side MTBDD
	 * @param[in]  func  The operation to be performed on respective leaves of
	 *                   given MTBDDs
	 *
	 * @returns  Root of the MTBDD with the result of the operation
	 */
	virtual RootType TernaryApply(const RootType& lhs, const RootType& mhs,
		const RootType& rhs, AbstractTernaryApplyFunctorType* func) = 0;


	/**
	 * @brief  Monadic Apply function for an MTBDD
	 *
	 * Performs given monadic Apply operation on an MTBDD specified by its root.
	 *
	 * @see  AbstractSharedMTBDD::AbstractMonadicApplyFunctorType
	 *
	 * @param[in]  root  Root of the MTBDD
	 * @param[in]  func  The operation to be performed on all leaves of
	 *                   given MTBDD
	 *
	 * @returns  Root of the MTBDD with the result of the operation
	 */
	virtual RootType MonadicApply(const RootType& root,
		AbstractMonadicApplyFunctorType* func) = 0;


	/**
	 * @brief  Creates a new root of a MTBDD
	 *
	 * Creates a new MTBDD represented by its root. All paths in the new MTBDD
	 * lead to the background value.
	 *
	 * @returns  A root of a new MTBDD
	 */
	virtual RootType CreateRoot() = 0;


	/**
	 * @brief  Erases a root
	 *
	 * Erases given root and dereferences proper MTBDD
	 *
	 * @param[in]  root  The root of the MTBDD to be erased
	 */
	virtual void EraseRoot(const RootType& root) = 0;


	/**
	 * @brief  Sets the value of the bottom
	 *
	 * This method sets a new value of the leaf for the bottom of the MTBDD.
	 *
	 * @param[in]  bottom  The new value for the bottom of the MTBDD
	 */
	virtual void SetBottomValue(const LeafType& bottom) = 0;


	/**
	 * @brief  Serializes the MTBDD into a string
	 *
	 * This function serializes the MTBDD into a string
	 *
	 * @returns  String that represents the data contained in the MTBDD
	 */
	virtual std::string Serialize() const = 0;


	/**
	 * @brief  Dumps the MTBDD into a Dot file
	 *
	 * Dumps MTBDD into a file in Dot format. This file can then be used to
	 * generate a diagram using GraphViz (http://www.graphviz.org/) Dot tool.
	 *
	 * @param[in]  filename  Name of the output file (preferably with .dot
	 *                       extension)
	 */
	virtual void DumpToDotFile(const std::string& filename) const = 0;


	/**
	 * @brief  Renames variables of an MTBDD
	 *
	 * This method carries out renaming of variables of the MTBDD with given
	 * root node. The function that assigns new variable name needs to be an
	 * injection, otherwise the output is undefined. Also, a new name of
	 * a variable cannot be a name of an existing variable, otherwise the output
	 * is again undefined.
	 *
	 * @param[in]  root  Root of the MTBDD
	 * @param[in]  func  Functor that defines the mapping of old variables'
	 *                   names to new ones. If the variable is not supposed to
	 *                   be touched, the new name needs to be the same as the
	 *                   old name.
	 *
	 * @returns  An MTBDD with renamed variables
	 */
	virtual RootType RenameVariables(const RootType& root,
		AbstractVariableRenamingFunctorType* func) = 0;


	/**
	 * @brief  Trim variables of an MTBDD
	 *
	 * This method trims variables of the MTBDD that satisfy predicate
	 * repesented by passed predicate functor. When a variable node is removed,
	 * an operation given by another functor is performed on its child nodes.
	 *
	 * @param[in]  root    Root of the MTBDD
	 * @param[in]  pred    Predicate functor that for each variable defines
	 *                     whether it should or should not be trimmed
	 * @param[in]  merger  Apply functor that defines the operation that is to
	 *                     be done for child nodes of a removed variable node.
	 *
	 * @returns  An MTBDD with trimmed variables
	 */
	virtual RootType TrimVariables(const RootType& root,
		AbstractVariablePredicateFunctorType* pred,
		AbstractApplyFunctorType* merger) = 0;


	virtual DescriptionType GetMinimumDescription(const RootType& root) const = 0;


	/**
	 * @brief  Destructor
	 *
	 * The destructor of the MTBDD. Releases all allocated resources.
	 */
	virtual ~AbstractSharedMTBDD() { }
};

#endif
