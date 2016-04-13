#ifndef TEST_CASE_H
#define TEST_CASE_H

class TestCase
{
private:
protected:
public:

	virtual void setup() {}
	virtual void process() = 0;
	virtual void cleanup() {}
};

#endif
