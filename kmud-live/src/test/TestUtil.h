#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <vector>

class TestCase;

class TestUtil
{
private:
	static TestUtil *self;
	TestUtil();
	~TestUtil();
protected:
	std::vector<TestCase *> testCases;
public:

	static TestUtil *get();
	void destroy();

	void processTestCases();

};

#endif
