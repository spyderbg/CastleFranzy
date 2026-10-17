#ifndef UNITTEST_TESTRESULTS_H
#define UNITTEST_TESTRESULTS_H

namespace UnitTest {

class TestReporter;
class TestDetails;

class TestResults
{
public:
    explicit TestResults(TestReporter* reporter = 0);

	void setReporter(TestReporter* reporter)
	{
		m_testReporter = reporter;
	}

    virtual void OnTestStart(TestDetails const& test);
    virtual void OnTestFailure(TestDetails const& test, char const* failure);
    virtual void OnTestFinish(TestDetails const& test, float secondsElapsed);
	virtual void TestFinishAll();
	
    int GetTotalTestCount() const;
    int GetFailedTestCount() const;
    int GetFailureCount() const;
	
	void ResetTestResult()
	{		
		m_totalTestCount = 0;
		m_failedTestCount = 0;
		m_failureCount = 0;

		m_currentTestFailed = false;
	}

private:
    TestReporter* m_testReporter;
    int m_totalTestCount;
    int m_failedTestCount;
    int m_failureCount;

    bool m_currentTestFailed;

    TestResults(TestResults const&);
    TestResults& operator =(TestResults const&);
};
}

#endif
