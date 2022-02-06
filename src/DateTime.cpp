/*******************************************************************
 *                                                                 *
 * DateTime.cpp                                                    *
 * Author: Michael Mason                                           *
 * Date: 2010-12-22                                                *
 *                                                                 *
 * Purpose: Implements the DateTime class, providing a structured  *
 *          interface for manipulating dates and time.             *
 *******************************************************************/

#include <iomanip>
#include <cstring>

#include "DateTime.h"

DateTime::DateTime()
{
	setTime(time(0));
}
DateTime::DateTime(__int64 unixTimestamp)
{
	if(unixTimestamp < 0)
		unixTimestamp = 0;
	setTime(unixTimestamp);
}

int DateTime::getSecond() const
{
	return (int)(_getSecond());
}
int DateTime::getMinute() const
{
	return (int)(_getMinute());
}
int DateTime::getHour() const
{
	return (int)(_getHour());
}
int DateTime::getDay() const
{
	return (int)(_getDay());
}
int DateTime::getMonth() const
{
	return (int)(_getMonth());
}
int DateTime::getYear() const
{
	return ((int)_getYear())+1900;
}
int DateTime::getDayOfYear() const
{
	__int64 unixTimestamp = getTime();
	tm *tt = localtime((time_t*)&unixTimestamp);
	return tt->tm_yday;
}
int DateTime::getDayOfWeek() const
{
	__int64 unixTimestamp = getTime();
	tm *tt = localtime((time_t*)&unixTimestamp);
	return tt->tm_wday;
}
int DateTime::getWeekOfYear() const
{
	__int64 unixTimestamp = getTime();
	tm *tt = localtime((time_t*)&unixTimestamp);
	return tt->tm_yday / 7 + 1;
}
int DateTime::getWeekOfYearM() const
{
	return 0;//TODO: Finish
}

void DateTime::setSecond(int second)
{
	_setSecond((unsigned char)(second));
}
void DateTime::setMinute(int minute)
{
	_setMinute((unsigned char)(minute));
}
void DateTime::setHour(int hour)
{
	_setHour((unsigned char)(hour));
}
void DateTime::setDay(int day)
{
	_setDay((unsigned char)(day));
}
void DateTime::setMonth(int month)
{
	_setMonth((unsigned char)(month-1));
}
void DateTime::setYear(int year)
{
	_setYear((unsigned char)(year-1900));
}

__int64 DateTime::getTime() const
{
	tm t;
	memset(&t, 0, sizeof(tm));
	t.tm_hour = (int)(_getHour());
	t.tm_mday = (int)(_getDay());
	t.tm_year = (int)(_getYear());
	t.tm_mon = (int)(_getMonth());
	t.tm_min = (int)(_getMinute());
	t.tm_sec = (int)(_getSecond());
	t.tm_isdst = -1;

	return (__int64)mktime(&t);
}

//Essentially a clone of: http://www.cppreference.com/wiki/chrono/c/strftime
std::string DateTime::parse(const std::string &format, const DateTime &dateTime)
{
	std::stringstream buffer;

	std::string::size_type pos = 0;

	while(pos < format.size())
	{
		if(format[pos] == '%')
		{
			++pos;
			if(pos >= format.size())
				break;
			
			char symbol = format[pos];
			switch(symbol)
			{
			case 'a':
				buffer << DateTime::getWeekdayName(dateTime.getDayOfWeek()).substr(0,3);
				break;
			case 'A':
				buffer << DateTime::getWeekdayName(dateTime.getDay());
				break;
			case 'b':
				buffer << DateTime::getMonthName(dateTime.getMonth()).substr(0,3);
				break;
			case 'B':
				buffer << DateTime::getMonthName(dateTime.getMonth());
				break;
			case 'c':
				buffer << "";//TODO: Fill this in.
				break;
			case 'd':
				buffer << std::setfill('0') << std::setw(2) << dateTime.getDay();
				break;
			case 'H':
				buffer << std::setfill('0') << std::setw(2) << dateTime.getHour();
				break;
			case 'I':
			{
				int hr = dateTime.getHour() % 12;
				buffer << std::setfill('0') << std::setw(2) << (hr == 0 ? 12 : hr);
				break;
			}
			case 'j':
				buffer << dateTime.getDayOfYear();
				break;
			case 'm':
				buffer << std::setfill('0') << std::setw(2) << (dateTime.getMonth()+1);
				break;
			case 'M':
				buffer << std::setfill('0') << std::setw(2) << dateTime.getMinute();
				break;
			case 'p':
				buffer << (dateTime.getHour() < 12 ? "AM" : "PM");
				break;
			case 'S':
				buffer << std::setfill('0') << std::setw(2) << dateTime.getSecond();
				break;
			case 'U':
				buffer << dateTime.getWeekOfYear();
				break;
			case 'w':
				buffer << DateTime::getWeekdayName(dateTime.getDayOfWeek());
				break;
			case 'W':
				buffer << dateTime.getWeekOfYearM();
				break;
			case 'x'://TODO: Implement
				break;
			case 'X'://TODO: Implement
				break;
			case 'y':
				buffer << std::setfill('0') << std::setw(2) << dateTime.getYear()%100;
				break;
			case 'Y':
				buffer << dateTime.getYear();
				break;
			case 'Z'://TODO: Implement
				break;
			case '%':
				buffer << '%';
				break;
			default://Invalid character.
				buffer << '%' << symbol;
			}
		}
		else
			buffer << format[pos];

		++pos;
	}

	return buffer.str();
}

int DateTime::compareTo(const DateTime& otherDate) const
{
	__int64 myTime = getTime();
	__int64 otherTime = otherDate.getTime();

	if(myTime > otherTime)
		return 1;
	if(myTime < otherTime)
		return -1;
	return 0;
}

bool DateTime::operator == (const DateTime &otherDate)
{
	return compareTo(otherDate)==0;
}

std::string DateTime::toString() const
{
	return DateTime::parse("%a %b %d %H:%M:%S %Y", *this);
}


void DateTime::moveDayForward()
{
	_setDay(_getDay()+1);
	if( _getDay() > DateTime::getDaysInMonth(_getMonth(),_getYear()) ) {
		moveMonthForward();
		_setDay(1);
	}
}
void DateTime::moveDayBackward()
{
	if(day <= 1) {
		moveMonthBackward();
		_setDay(getDaysInMonth(_getMonth(),_getYear()));
	}
	else {
		_setDay(_getDay()-1);
	}
}

void DateTime::moveMonthForward()
{
	if(++month > 11) {
		++year;
		month = 0;
	}
}

void DateTime::moveMonthBackward()
{
	//Month is unsigned. That means we must check case 0 before decreasing. Otherwise it will roll over.
	if(month == 0) {
		_setMonth(11);
		_setYear(_getYear()-1);
	}
	else {
		_setMonth(_getMonth()-1);
	}
}

std::string DateTime::getWeekdayName(int weekDay)
{
	switch(weekDay)
	{
	case 0:	return "Sunday";
	case 1: return "Monday";
	case 2: return "Tuesday";
	case 3: return "Wednesday";
	case 4: return "Thursday";
	case 5: return "Friday";
	case 6: return "Saturday";
	default:return "";
	}
}
std::string DateTime::getMonthName(int month)
{
	switch(month)
	{
	case 0: return "January";
	case 1: return "February";
	case 2: return "March";
	case 3: return "April";
	case 4: return "May";
	case 5: return "June";
	case 6: return "July";
	case 7: return "August";
	case 8: return "September";
	case 9: return "October";
	case 10:return "November";
	case 11:return "December";
	default:return "";
	}
}

int DateTime::getDaysInMonth(int month, int year)
{
	switch(month)
	{
	case 0: return 31;
	case 1: return ((year%4)?28:29);
	case 2: return 31;
	case 3: return 30;
	case 4: return 31;
	case 5: return 30;
	case 6: return 31;
	case 7: return 31;
	case 8: return 30;
	case 9: return 31;
	case 10: return 30;
	case 11: return 31;
	}
	return 0;
}

void DateTime::setTime(__int64 unixTimestamp)
{
	tm *t = localtime((time_t*)&unixTimestamp);
	_setYear((unsigned char)(t->tm_year));
	_setMonth((unsigned char)(t->tm_mon));
	_setDay((unsigned char)(t->tm_mday));
	_setHour((unsigned char)(t->tm_hour));
	_setMinute((unsigned char)(t->tm_min));
	_setSecond((unsigned char)(t->tm_sec));
//	this->unixTimestamp = unixTimestamp;
}

bool DateTime::before(const DateTime &otherDate)
{
	return compareTo(otherDate) < 0;
}
bool DateTime::after(const DateTime &otherDate)
{
	return compareTo(otherDate) > 0;
}

void DateTime::operator =(const DateTime &otherDate)
{
	this->year = (unsigned char)(otherDate.getYear()-1900);
	this->month = (unsigned char)(otherDate.getMonth());
	this->day = (unsigned char)(otherDate.getDay());
	this->hour = (unsigned char)(otherDate.getHour());
	this->minute = (unsigned char)(otherDate.getMinute());
	this->second = (unsigned char)(otherDate.getSecond());
}

unsigned char DateTime::_getYear() const
{
	return year;
}
unsigned char DateTime::_getMonth() const
{
	return month;
}
unsigned char DateTime::_getDay() const
{
	return day;
}
unsigned char DateTime::_getHour() const
{
	return hour;
}
unsigned char DateTime::_getMinute() const
{
	return minute;
}
unsigned char DateTime::_getSecond() const
{
	return second;
}

void DateTime::_setYear(const unsigned char year)
{
	this->year = year;
}
void DateTime::_setMonth(const unsigned char month)
{
	this->month = month;
}
void DateTime::_setDay(const unsigned char day)
{
	this->day = day;
}
void DateTime::_setHour(const unsigned char hour)
{
	this->hour = hour;
}
void DateTime::_setMinute(const unsigned char minute)
{
	this->minute = minute;
}
void DateTime::_setSecond(const unsigned char second)
{
	this->second = second;
}

bool DateTime::sameDay(const DateTime &datetime) const
{
	return datetime.getYear() == getYear() && datetime.getMonth() == getMonth() && datetime.getDay() == getDay();
}
