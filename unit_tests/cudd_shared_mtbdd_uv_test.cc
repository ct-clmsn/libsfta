/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test suite for CUDDSharedMTBDD class with roots as unsigned integers and
 *    leaves as ordered vectors of unsigned integers.
 *
 *****************************************************************************/

// SFTA headers
#include <sfta/compact_variable_assignment.hh>
#include <sfta/cudd_shared_mtbdd.hh>
#include <sfta/dual_map_leaf_allocator.hh>
#include <sfta/formula_parser.hh>
#include <sfta/map_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>
#include <sfta/ordered_vector.hh>

using SFTA::AbstractSharedMTBDD;
using SFTA::CUDDSharedMTBDD;
using SFTA::Private::Convert;
using SFTA::Private::FormulaParser;


// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDSharedMTBDDCC
#include <boost/test/unit_test.hpp>
#include <boost/random/mersenne_twister.hpp>

// testing headers
#include "log_fixture.hh"


/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/


/**
 * Formulae for standard test cases to be stored in the MTBDD
 */
const char* const STANDARD_TEST_CASES[] =
{
	"~x0 * ~x1 *  x2 *  x3 = { 3, 1, 9}",
	"~x0 *  x1 * ~x2 * ~x3 = { 4, 7, 8}",
	" x0 * ~x1 * ~x2 *  x3 = { 9, 2, 128, 4}",
	" x0 *  x1 *  x2 * ~x3 = {14}",
	" x0 *  x1 *  x2 * ~x3 = {14}",
	" x0 *  x1 *  x2 *  x3 = {15, 78, 54}"
};


/**
 * Number of formulae for standard test cases in the MTBDD
 */
const unsigned STANDARD_TEST_CASES_SIZE =
	sizeof(STANDARD_TEST_CASES) / sizeof(const char* const);


/**
 * Formulae for standard test cases to be found not present in the MTBDD
 */
const char* const STANDARD_FAIL_CASES[] =
{
	"~x0 * ~x1 * ~x2 *  x3 = {1}",
	"~x0 * ~x1 *  x2 * ~x3 = {2}",
	"~x0 *  x1 * ~x2 *  x3 = {5}",
	"~x0 *  x1 *  x2 * ~x3 = {6}",
	"~x0 *  x1 *  x2 *  x3 = {7}",
	" x0 * ~x1 * ~x2 * ~x3 = {8}",
	" x0 * ~x1 *  x2 * ~x3 = {10}",
	" x0 * ~x1 *  x2 *  x3 = {11}",
	" x0 *  x1 * ~x2 * ~x3 = {12}",
	" x0 *  x1 * ~x2 *  x3 = {13}"
};


/**
 * Number of formulae for standard test cases that are to be not found in the
 * MTBDD
 */
const unsigned STANDARD_FAIL_CASES_SIZE =
	sizeof(STANDARD_FAIL_CASES) / sizeof(const char* const);


/**
 * Formulae for standard test cases with trimmed variables
 */
const char* const TRIMMED_STANDARD_TEST_CASES[] =
{
	"~x1 *  x3 = {3, 1, 9, 2, 128, 4}",
	" x1 * ~x3 = {4, 7, 8, 14}",
	" x1 *  x3 = {15, 78, 54}"
};


/**
 * Number of formulae for standard test cases with trimmed variables.
 */
const unsigned TRIMMED_STANDARD_TEST_CASES_SIZE =
	sizeof(TRIMMED_STANDARD_TEST_CASES) / sizeof(const char* const);


/**
 * Number of variables of the MTBDD
 */
const unsigned NUM_VARIABLES = 64;

/**
 * The seed of the pseudorandom number generator
 */
const unsigned PRNG_SEED = 781436;

/**
 * Number of cases for large test formula test
 */
const unsigned LARGE_TEST_FORMULA_CASES = 200;


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

/**
 * @brief  CUDDSharedMTBDD unsigned to vector test fixture
 *
 * Fixture for test of CUDDSharedMTBDD with unsigned integers as roots and
 * ordered vectors of unsigned integers as leaves.
 */
class CUDDSharedMTBDDUnsignedVectorFixture : public LogFixture
{
private:  // private constants

public:   // public types

	/**
	 * @brief  Root type
	 *
	 * The type of MTBDD root
	 */
	typedef unsigned RootType;

	/**
	 * @brief  Leaf container inner type
	 *
	 * The type that is in the leaf container.
	 */
	typedef unsigned Containee;


	/**
	 * @brief  Leaf type
	 *
	 * The type of MTBDD leaf
	 */
	typedef SFTA::OrderedVector<Containee> LeafType;


	/**
	 * @brief  List of test cases
	 *
	 * List of strings with formulae for test cases.
	 */
	typedef std::vector<std::string> ListOfTestCasesType;

	/**
	 * @brief  Variable assignment
	 *
	 * Type for variable assignment used in the test.
	 */
	typedef SFTA::Private::CompactVariableAssignment MyVariableAssignment;

	/**
	 * @brief  Abstract MTBDD type
	 *
	 * Type for abstract MTBDD with fully bound template parameters.
	 */
	typedef AbstractSharedMTBDD<RootType, LeafType, MyVariableAssignment> ASMTBDDUV;

	/**
	 * @brief  CUDD MTBDD type
	 *
	 * Type for MTBDD that uses CUDD with fully bound template paramaters.
	 */
	typedef CUDDSharedMTBDD<RootType, LeafType, MyVariableAssignment,
			SFTA::Private::DualMapLeafAllocator, SFTA::Private::MapRootAllocator>
			CuddMTBDDUV;

	/**
	 * @brief  Dictionary of variables
	 *
	 * Type for dictionary of variables mapping variable name to index.
	 */
	typedef std::map<std::string, unsigned> VariableNameDictionary;

private:  // private data members


	/**
	 * Counter of known variables.
	 */
	unsigned varCounter_;


	/**
	 * Dictionary mapping variables' names to variable indices.
	 */
	VariableNameDictionary varDict_;

private:  // private methods

	/**
	 * @brief  Copy constructor
	 *
	 * Copy constructor.
	 *
	 * @param[in]  fixture  Input fixture
	 */
	CUDDSharedMTBDDUnsignedVectorFixture(
		const CUDDSharedMTBDDUnsignedVectorFixture& fixture);


	/**
	 * @brief  Assignment operator
	 *
	 * Assignment operator.
	 *
	 * @param[in]  rhs  Assigned value
	 *
	 * @returns  The new value
	 */
	CUDDSharedMTBDDUnsignedVectorFixture& operator=(
		const CUDDSharedMTBDDUnsignedVectorFixture& rhs);

public:   // public methods

	/**
	 * @brief  Default constructor
	 *
	 * Default constructor.
	 */
	CUDDSharedMTBDDUnsignedVectorFixture()
		: varCounter_(0), varDict_()
	{ }

protected:// protected methods

	/**
	 * @brief  Loads standard tests
	 *
	 * A routine that loads passed structures with standard tests.
	 *
	 * @param[out]  testCases    Reference to list of test cases that is to be
	 *                           filled
	 * @param[out]  failedCases  Reference to list of failing test cases that is
	 *                           to be filled
	 */
	static void loadStandardTests(ListOfTestCasesType& testCases,
		ListOfTestCasesType& failedCases)
	{
		// formulae that we wish to store in the BDD
		for (size_t i = 0; i < STANDARD_TEST_CASES_SIZE; ++i)
		{	// load test cases
			testCases.push_back(STANDARD_TEST_CASES[i]);
		}

		// formulae that we want to check that are not in the BDD
		for (unsigned i = 0; i < STANDARD_FAIL_CASES_SIZE; ++i)
		{	// load test cases
			failedCases.push_back(STANDARD_FAIL_CASES[i]);
		}
	}


	/**
	 * @brief  Translates variable name to variable index
	 *
	 * This method translates variable name to corresponding variable index
	 * using variable dictionary. In case the variable name is not in the
	 * dictionary, it is assigned new free index and the pair is inserted in the
	 * dictionary.
	 *
	 * @param[in]  varName  Variable name
	 *
	 * @returns  Variable index
	 */
	size_t translateVarNameToIndex(const std::string& varName)
	{
		VariableNameDictionary::const_iterator itDict;
		if ((itDict = varDict_.find(varName)) == varDict_.end())
		{	// in case the variable was not found in the dictionary
			itDict = varDict_.insert(std::make_pair(varName, varCounter_++)).first;
		}

		return itDict->second;
	}


	/**
	 * @brief  Translates list of variables to variable assignment
	 *
	 * This method takes a list of variables and their values (either normal or
	 * complemented) and transforms it into the variable assignment structure.
	 *
	 * @param[in]  varList  List of variables and their values
	 *
	 * @returns  Variable assignment data structure
	 */
	MyVariableAssignment varListToAsgn(
		const FormulaParser::VariableListType& varList)
	{
		MyVariableAssignment asgn(NUM_VARIABLES);

		for (FormulaParser::VariableListType::const_iterator itVar =
			varList.begin(); itVar != varList.end(); ++itVar)
		{	// for each variable in the list, change the corresponding assignment
			size_t index = translateVarNameToIndex(itVar->first);
			asgn.SetIthVariableValue(index, (itVar->second)?
				MyVariableAssignment::ONE : MyVariableAssignment::ZERO);
		}

		return asgn;
	}


	/**
	 * @brief  Converts container with leaf values to string
	 *
	 * This static method returns string representation of a container of leaf
	 * values.
	 *
	 * @param[in]  leafCont  Container with leaves
	 *
	 * @returns  String representation of the container
	 */
	static std::string leafContainerToString(const ASMTBDDUV::LeafContainer& leafCont)
	{
		std::string result;

		for (ASMTBDDUV::LeafContainer::const_iterator itLeaf = leafCont.begin();
			itLeaf != leafCont.end(); ++itLeaf)
		{	// append all leaves
			result += ((itLeaf == leafCont.begin())? " " : ", ") +
				Convert::ToString(**itLeaf);
		}

		return result;
	}


	/**
	 * @brief  Equality comparer of leaves
	 *
	 * This static method compares two leaves and determines if they are equal.
	 *
	 * @param[in]  lhs  Left-hand side leaf
	 * @param[in]  rhs  Right-hand side leaf
	 *
	 * @returns  Boolean value determining whether the leaves are equal
	 */
	inline static bool compareLeafValues(const LeafType* lhs, const LeafType* rhs)
	{
		return *lhs == *rhs;
	}


	/**
	 * @brief  Equality comparer of leaf containers
	 *
	 * This static method compares two containers with leaf values and
	 * determines if they are equal.
	 *
	 * @param[in]  lhs  Left-hand side container
	 * @param[in]  rhs  Right-hand side container
	 *
	 * @returns  Boolean value determining whether the leaf containers are equal
	 */
	static bool compareTwoLeafContainers(const ASMTBDDUV::LeafContainer& lhs,
		const ASMTBDDUV::LeafContainer& rhs)
	{
		const ASMTBDDUV::LeafContainer* left = &lhs;
		const ASMTBDDUV::LeafContainer* right = &rhs;

		if (left->size() != right->size())
		{	// in case the sizes differ
			return false;
		}

		return std::equal(left->begin(), left->end(), right->begin(), compareLeafValues);
	}


	/**
	 * @brief  Creates MTBDDs for test cases
	 *
	 * This method creates in given MTBDD new root with leaves according to the
	 * set of test cases.
	 *
	 * @param[in]  bdd        An MTBDD
	 * @param[in]  testCases  Set of test cases to be inserted in the MTBDD
	 *
	 * @returns  New root with leaves according to the test cases
	 */
	RootType createMTBDDForTestCases(ASMTBDDUV* bdd,
		const ListOfTestCasesType& testCases)
	{
		RootType root = bdd->CreateRoot();

		for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
			itTests != testCases.end(); ++itTests)
		{
			FormulaParser::ParserResultUnsignedVecType prsRes =
				FormulaParser::ParseExpressionUnsignedVec(*itTests);
			LeafType leafValue = static_cast<LeafType>(prsRes.first);
			MyVariableAssignment asgn = varListToAsgn(prsRes.second);
			bdd->SetValue(root, asgn, leafValue);
		}

		return root;
	}
};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/


BOOST_FIXTURE_TEST_SUITE(suite, CUDDSharedMTBDDUnsignedVectorFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	ASMTBDDUV* bdd = new CuddMTBDDUV();
	bdd->SetBottomValue(LeafType());

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	for (unsigned i = 0; i < testCases.size(); ++i)
	{	// test that the test cases have been stored properly
		FormulaParser::ParserResultUnsignedVecType prsRes =
			FormulaParser::ParseExpressionUnsignedVec(testCases[i]);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDUV::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root, asgn), res),
			testCases[i] + " != " + leafContainerToString(bdd->GetValue(root, asgn)));

		for (ListOfTestCasesType::const_iterator itFailed = failedCases.begin();
			itFailed != failedCases.end(); ++itFailed)
		{	// for every test case that should fail
			FormulaParser::ParserResultUnsignedVecType prsFailedRes =
				FormulaParser::ParseExpressionUnsignedVec(*itFailed);
			MyVariableAssignment failedAsgn = varListToAsgn(prsFailedRes.second);

			ASMTBDDUV::LeafContainer resFailed;

			BOOST_CHECK_MESSAGE(
				compareTwoLeafContainers(bdd->GetValue(root, failedAsgn), resFailed),
				testCases[i] + " == " + leafContainerToString(bdd->GetValue(root, failedAsgn)));
		}
	}

	delete bdd;
}

BOOST_AUTO_TEST_CASE(large_diagram_test)
{
	ASMTBDDUV* bdd = new CuddMTBDDUV();
	bdd->SetBottomValue(LeafType());

	boost::mt19937 prnGen(PRNG_SEED);

	// formulae that we wish to store in the BDD
	ListOfTestCasesType testCases;

	for (unsigned i = 0; i < LARGE_TEST_FORMULA_CASES; ++i)
	{	// generate test cases
		std::string formula;

		for (unsigned j = 0; j < NUM_VARIABLES; ++j)
		{
			if (prnGen() % 4 != 0)
			{
				formula += (formula.empty()? "" : " * ") +
					Convert::ToString((prnGen() % 2 == 0)? " " : "~")
					+ "x" + Convert::ToString(j);
			}
		}

		formula += " = {";

		bool first = true;
		while ((prnGen() % 8 != 0) || first)
		{	// generate a set of variables
			formula += (first? " " : ", ") + Convert::ToString(static_cast<Containee>(prnGen()));

			first = false;
		}

		formula += "}";

		testCases.push_back(formula);
	}

	// formulae that we want to check that are not in the BDD
	ListOfTestCasesType failedCases;

	for (unsigned i = 0; i < LARGE_TEST_FORMULA_CASES; ++i)
	{	// generate failed test cases
		std::string formula;

		for (unsigned j = 0; j < NUM_VARIABLES; ++j)
		{
			if (prnGen() % 31 != 0)
			{
				formula += (formula.empty()? "" : " * ") +
					Convert::ToString((prnGen() % 2 == 0)? " " : "~")
					+ "x" + Convert::ToString(j);
			}
		}

		formula += " = {1}";

		failedCases.push_back(formula);
	}

	RootType root = createMTBDDForTestCases(bdd, testCases);

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedVecType prsRes =
			FormulaParser::ParseExpressionUnsignedVec(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDUV::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root, asgn), res),
			*itTests + " != " + leafContainerToString(bdd->GetValue(root, asgn)));
	}

	for (ListOfTestCasesType::const_iterator itFailed = failedCases.begin();
		itFailed != failedCases.end(); ++itFailed)
	{	// for every test case that should fail
#if DEBUG
		BOOST_TEST_MESSAGE("Finding failed " + *itFailed);
#endif
		FormulaParser::ParserResultUnsignedVecType prsFailedRes =
			FormulaParser::ParseExpressionUnsignedVec(*itFailed);
		MyVariableAssignment asgn = varListToAsgn(prsFailedRes.second);

		ASMTBDDUV::LeafContainer res;

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root, asgn), res),
			*itFailed + " == " + leafContainerToString(bdd->GetValue(root, asgn)));
	}

	delete bdd;
}


BOOST_AUTO_TEST_CASE(no_variables_formula)
{
	const char* const TEST_VALUE = " = {42}";

	ASMTBDDUV* bdd = new CuddMTBDDUV();
	bdd->SetBottomValue(LeafType());
	RootType root = bdd->CreateRoot();

	FormulaParser::ParserResultUnsignedVecType prsRes =
		FormulaParser::ParseExpressionUnsignedVec(TEST_VALUE);
	LeafType leafValue = static_cast<LeafType>(prsRes.first);
	MyVariableAssignment asgn = varListToAsgn(prsRes.second);
	bdd->SetValue(root, asgn, leafValue);

	ASMTBDDUV::LeafContainer res;
	res.push_back(&leafValue);

	BOOST_CHECK_MESSAGE(compareTwoLeafContainers(bdd->GetValue(root, asgn), res),
		Convert::ToString(TEST_VALUE) + " != " +
		leafContainerToString(bdd->GetValue(root, asgn)));

	delete bdd;
}


BOOST_AUTO_TEST_CASE(multiple_independent_bdds)
{
	ASMTBDDUV* bdd = new CuddMTBDDUV();
	bdd->SetBottomValue(LeafType());

	// load test cases for the first BDD
	ListOfTestCasesType testCases1;
	ListOfTestCasesType failedCases1;
	loadStandardTests(testCases1, failedCases1);

	// load test cases for the second BDD
	ListOfTestCasesType testCases2;
	ListOfTestCasesType failedCases2;
	loadStandardTests(failedCases2, testCases2);

	RootType root1 = createMTBDDForTestCases(bdd, testCases1);
	RootType root2 = createMTBDDForTestCases(bdd, testCases2);

	for (ListOfTestCasesType::const_iterator itTests = testCases1.begin();
		itTests != testCases1.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedVecType prsRes =
			FormulaParser::ParseExpressionUnsignedVec(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDUV::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root1, asgn), res),
			*itTests + " != " + leafContainerToString(bdd->GetValue(root1, asgn)));
	}

	for (ListOfTestCasesType::const_iterator itTests = testCases2.begin();
		itTests != testCases2.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedVecType prsRes =
			FormulaParser::ParseExpressionUnsignedVec(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDUV::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root2, asgn), res),
			*itTests + " != " + leafContainerToString(bdd->GetValue(root2, asgn)));
	}

	for (ListOfTestCasesType::const_iterator itFailed = failedCases1.begin();
		itFailed != failedCases1.end(); ++itFailed)
	{	// for every test case that should fail
#if DEBUG
		BOOST_TEST_MESSAGE("Finding failed " + *itFailed);
#endif
		FormulaParser::ParserResultUnsignedVecType prsFailedRes =
			FormulaParser::ParseExpressionUnsignedVec(*itFailed);
		MyVariableAssignment asgn = varListToAsgn(prsFailedRes.second);

		ASMTBDDUV::LeafContainer res;

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root1, asgn), res),
			*itFailed + " == " + leafContainerToString(bdd->GetValue(root1, asgn)));
	}

	for (ListOfTestCasesType::const_iterator itFailed = failedCases2.begin();
		itFailed != failedCases2.end(); ++itFailed)
	{	// for every test case that should fail
#if DEBUG
		BOOST_TEST_MESSAGE("Finding failed " + *itFailed);
#endif
		FormulaParser::ParserResultUnsignedVecType prsFailedRes =
			FormulaParser::ParseExpressionUnsignedVec(*itFailed);
		MyVariableAssignment asgn = varListToAsgn(prsFailedRes.second);

		ASMTBDDUV::LeafContainer res;

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root2, asgn), res),
			*itFailed + " == " + leafContainerToString(bdd->GetValue(root2, asgn)));
	}

	delete bdd;
}


BOOST_AUTO_TEST_CASE(monadic_apply)
{
	ASMTBDDUV* bdd = new CuddMTBDDUV();
	bdd->SetBottomValue(LeafType());

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	// apply functor that squares values in leaves
	class SquareMonadicApplyFunctor
		: public ASMTBDDUV::AbstractMonadicApplyFunctorType
	{
	public:

		virtual LeafType operator()(const LeafType& val)
		{
			LeafType result;
			for (LeafType::const_iterator itLeaf = val.begin();
				itLeaf != val.end(); ++itLeaf)
			{	// square each element of the vector
				result.insert((*itLeaf) * (*itLeaf));
			}

			return result;
		}
	};

	SquareMonadicApplyFunctor func;

	RootType squaredRoot = bdd->MonadicApply(root, &func);

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedVecType prsRes =
			FormulaParser::ParseExpressionUnsignedVec(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		LeafType squaredSet;
		for (LeafType::const_iterator itLeaf = leafValue.begin();
			itLeaf != leafValue.end(); ++itLeaf)
		{	// square each element of the vector
			squaredSet.insert((*itLeaf) * (*itLeaf));
		}
		leafValue = squaredSet;

		ASMTBDDUV::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(squaredRoot, asgn), res),
			*itTests + " != " + leafContainerToString(bdd->GetValue(squaredRoot, asgn)));
	}


	delete bdd;
}


BOOST_AUTO_TEST_CASE(apply)
{
	ASMTBDDUV* bdd = new CuddMTBDDUV();
	bdd->SetBottomValue(LeafType());

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	// apply functor that squares values in leaves
	class TimesApplyFunctor
		: public ASMTBDDUV::AbstractApplyFunctorType
	{
	public:

		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
		{
			LeafType result;
			for (LeafType::const_iterator itLhs = lhs.begin(), itRhs = rhs.begin();
				(itLhs != lhs.end()) && (itRhs != rhs.end()) ; ++itLhs, ++itRhs)
			{
				result.insert((*itLhs) * (*itRhs));
			}

			return result;
		}
	};

	TimesApplyFunctor func;

	RootType timesRoot = bdd->Apply(root, root, &func);

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedVecType prsRes =
			FormulaParser::ParseExpressionUnsignedVec(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		LeafType squaredSet;
		for (LeafType::const_iterator itLeaf = leafValue.begin();
			itLeaf != leafValue.end(); ++itLeaf)
		{	// square each element of the vector
			squaredSet.insert((*itLeaf) * (*itLeaf));
		}
		leafValue = squaredSet;

		ASMTBDDUV::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(timesRoot, asgn), res),
			*itTests + " != " + leafContainerToString(bdd->GetValue(timesRoot, asgn)));
	}

	delete bdd;
}


BOOST_AUTO_TEST_CASE(variable_renaming)
{
	ASMTBDDUV* bdd = new CuddMTBDDUV();
	bdd->SetBottomValue(LeafType());

	for (unsigned i = 0; i < NUM_VARIABLES; ++i)
	{	// fill the table of variables
		translateVarNameToIndex("x" + Convert::ToString(i));
	}

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	class MovingUpVariableRenamingFunctor
		: public ASMTBDDUV::AbstractVariableRenamingFunctorType
	{
	public:

		virtual ASMTBDDUV::VariableType operator()(const ASMTBDDUV::VariableType& var)
		{
			if (var < NUM_VARIABLES/2)
			{
				return var + NUM_VARIABLES/2;
			}
			else
			{
				return var;
			}
		}
	};

	MovingUpVariableRenamingFunctor funcMovingUp;
	RootType renamedRoot = bdd->RenameVariables(root, &funcMovingUp);

	class MovingDownVariableRenamingFunctor
		: public ASMTBDDUV::AbstractVariableRenamingFunctorType
	{
	public:

		virtual ASMTBDDUV::VariableType operator()(const ASMTBDDUV::VariableType& var)
		{
			if ((var >= NUM_VARIABLES/2) && (var < NUM_VARIABLES))
			{
				return 2 * (var - NUM_VARIABLES/2) + 1;
			}
			else
			{
				return var;
			}
		}
	};

	MovingDownVariableRenamingFunctor funcMovingDown;
	renamedRoot = bdd->RenameVariables(renamedRoot, &funcMovingDown);


	ListOfTestCasesType renamedTestCases;
	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// for each test case create new with properly renamed variables
		std::string renamedCase = *itTests;

		for (unsigned i = 0; i < NUM_VARIABLES/2; ++i)
		{	// double the name of each variable, 
			boost::algorithm::replace_all(renamedCase, "x" + Convert::ToString(i) + " ",
				"x" + Convert::ToString(i+NUM_VARIABLES/2) + " ");
		}

		for (unsigned i = NUM_VARIABLES/2; i < NUM_VARIABLES; ++i)
		{	// for each variable
			boost::algorithm::replace_all(renamedCase, "x" + Convert::ToString(i) + " ",
				"x" + Convert::ToString(2*(i-static_cast<int>(NUM_VARIABLES)/2)+1) + " ");
		}

		renamedTestCases.push_back(renamedCase);
	}

	for (ListOfTestCasesType::const_iterator itTests = renamedTestCases.begin();
		itTests != renamedTestCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedVecType prsRes =
			FormulaParser::ParseExpressionUnsignedVec(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDUV::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(renamedRoot, asgn), res), *itTests
			+ " != " + leafContainerToString(bdd->GetValue(renamedRoot, asgn)));
	}


	delete bdd;
}


BOOST_AUTO_TEST_CASE(variable_trimming)
{
	ASMTBDDUV* bdd = new CuddMTBDDUV();
	bdd->SetBottomValue(LeafType());

	for (unsigned i = 0; i < NUM_VARIABLES; ++i)
	{	// fill the table of variables
		translateVarNameToIndex("x" + Convert::ToString(i));
	}

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	class OddVariablePredicateFunctorType
		: public ASMTBDDUV::AbstractVariablePredicateFunctorType
	{
	public:

		virtual bool operator()(const ASMTBDDUV::VariableType& var)
		{
			return var % 2 == 0;
		}
	};

	OddVariablePredicateFunctorType oddPred;

	class AdditionApplyFunctorType: public ASMTBDDUV::AbstractApplyFunctorType
	{
	public:
		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
		{
			return lhs.Union(rhs);
		}
	};

	AdditionApplyFunctorType addApply;

	RootType trimmedRoot = bdd->TrimVariables(root, &oddPred, &addApply);

	ListOfTestCasesType trimmedTestCases;
	// formulae that we wish to check
	for (size_t i = 0; i < TRIMMED_STANDARD_TEST_CASES_SIZE; ++i)
	{	// load test cases
		trimmedTestCases.push_back(TRIMMED_STANDARD_TEST_CASES[i]);
	}

	for (ListOfTestCasesType::const_iterator itTests = trimmedTestCases.begin();
		itTests != trimmedTestCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedVecType prsRes =
			FormulaParser::ParseExpressionUnsignedVec(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDUV::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(trimmedRoot, asgn), res), *itTests
			+ " != " + leafContainerToString(bdd->GetValue(trimmedRoot, asgn)));
	}
	delete bdd;
}

#if 0
BOOST_AUTO_TEST_CASE(serialization)
{
	ASMTBDDUV* bdd = new CuddMTBDDUV();

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	std::string str = bdd->Serialize();

	BOOST_TEST_MESSAGE("BDD:\n" + str);

	delete bdd;
}

#endif

BOOST_AUTO_TEST_SUITE_END()
