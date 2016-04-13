#ifndef GENERAL_TEST_CASE_H
#define GENERAL_TEST_CASE_H

#include "../TestCase.h"

class GeneralTestCase : public TestCase
{
private:
protected:
public:

	virtual void setup();
	virtual void process();
	virtual void cleanup();
};

#endif
