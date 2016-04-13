#include "../conf.h"
#include "../sysdep.h"

#include "TestUtil.h"
#include "TestCase.h"

#include "general/GeneralTestCase.h"

TestUtil *TestUtil::self;

TestUtil::TestUtil()
{
	testCases.push_back(new GeneralTestCase());
}

TestUtil::~TestUtil()
{
	for(auto testCase : testCases)
		delete testCase;
}

TestUtil *TestUtil::get()
{
	return self == nullptr ? (self = new TestUtil()) : self;
}

void TestUtil::destroy()
{
	delete self;
}

void TestUtil::processTestCases()
{
	for(auto testCase : testCases)
	{
		testCase->setup();
		testCase->process();
		testCase->cleanup();
	}
}
