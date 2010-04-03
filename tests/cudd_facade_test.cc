#include <knihovna/cudd_facade.hh>

#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDFacade
#include <boost/test/unit_test.hpp>


class LogFixture
{

private:
	static bool logInitialized_;

public:

	LogFixture()
	{
		if (!logInitialized_)
		{
			logInitialized_ = true;

			// Create the appender
			log4cpp::Appender* app = new log4cpp::OstreamAppender("ClogAppender", &std::clog);

			// Set the data layout of the appender
			log4cpp::Layout* layout = new log4cpp::BasicLayout();
			app->setLayout(layout);

			log4cpp::Category::getInstance("cudd_facade").setAdditivity(false);
			log4cpp::Category::getInstance("cudd_facade").setAppender(app);
			log4cpp::Category::getInstance("cudd_facade").setPriority(log4cpp::Priority::DEBUG);
		}
	}

	virtual ~LogFixture() { }
};

bool LogFixture::logInitialized_ = false;



BOOST_FIXTURE_TEST_SUITE(suite, LogFixture)

BOOST_AUTO_TEST_CASE(constructors)
{
	log4cpp::Category::getInstance("cudd_facade").debug("Lamo!!!!!!!!!!!");
	Knihovna::Private::CUDDFacade cudd;
}

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
}

BOOST_AUTO_TEST_CASE(serialization)
{
}

BOOST_AUTO_TEST_SUITE_END()