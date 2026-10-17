#ifndef UNITTEST_TESTRUNNER_H
#define UNITTEST_TESTRUNNER_H

#include "Test.h"
#include "TestList.h"
#include "CurrentTest.h"
#include "TestResults.h"
namespace UnitTest {

class TestReporter;
class TestResults;
class Timer;
class TestDetails;

int RunAllTests();

struct True
{
	bool operator()(const Test* const) const
	{
		return true;	
	}
};

class TestRunner
{
public:
	explicit TestRunner(TestReporter& reporter);
	~TestRunner();

	template <class Predicate>
	int RunTestsIf(TestList const& list, char const* suiteName, 
				   const Predicate& predicate, int maxTestTimeInMs) const
	{
	    Test* curTest = list.GetHead();

	    while (curTest != 0)
	    {
		    if (IsTestInSuite(curTest,suiteName) && predicate(curTest))
			{
				RunTest(m_result, curTest, maxTestTimeInMs);
			}

			curTest = curTest->next;
	    }

	    return Finish();
	}	

	TestResults* getTestResult()
	{
		return m_result;
	}

	void setResult(TestResults *result)
	{
		m_result = result;
		m_result->setReporter(m_reporter);
	}

	int Finish() const;
	bool IsTestInSuite(const Test* const curTest, char const* suiteName) const;
	void RunTest(TestResults* const result, Test* const curTest, int const maxTestTimeInMs) const;

private:
	TestReporter* m_reporter;
	TestResults* m_result;
	Timer* m_timer;		
};

}

#endif
