/*******************************************************************
 *                                                                 *
 * DateTime.h                                                      *
 * Author: Michael Mason                                           *
 * Date: 2010-12-22                                                *
 *                                                                 *
 * Purpose: Defines the DateTime class skeleton.                   *
 *******************************************************************/

#ifndef DATE_TIME_H
#define DATE_TIME_H

#include <string>

#ifndef WIN32
typedef long long __int64;
#endif

class DateTime {

public:
	DateTime();
	DateTime(__int64 unixTimestamp);

	int getSecond() const;
	int getMinute() const;
	int getHour() const;
	int getDay() const;
	int getMonth() const;
	int getYear() const;
	int getDayOfYear() const;
	int getDayOfWeek() const;
	int getWeekOfYear() const;
	int getWeekOfYearM() const;

	void setSecond(int second);
	void setMinute(int minute);
	void setHour(int hour);
	void setDay(int day);
	void setMonth(int month);
	void setYear(int year);

	bool sameDay(const DateTime &datetime) const;

	void moveDayForward();
	void moveDayBackward();
	void moveMonthForward();
	void moveMonthBackward();

	bool before(const DateTime &otherDate);
	bool after(const DateTime &otherDate);

	__int64 getTime() const;

	static std::string parse(const std::string &format, const DateTime &dateTime);

	int compareTo(const DateTime& otherDate) const;

	std::string toString() const;

	void setTime(__int64 unixTimestamp);

	bool operator == (const DateTime &otherDate);
	void operator =(const DateTime &otherDate);

private:

	static std::string getWeekdayName(int weekDay);
	static std::string getMonthName(int month);
	static int getDaysInMonth(int month, int year);

	unsigned char _getYear() const;
	unsigned char _getMonth() const;
	unsigned char _getDay() const;
	unsigned char _getHour() const;
	unsigned char _getMinute() const;
	unsigned char _getSecond() const;

	void _setYear(const unsigned char year);
	void _setMonth(const unsigned char month);
	void _setDay(const unsigned char day);
	void _setHour(const unsigned char hour);
	void _setMinute(const unsigned char minute);
	void _setSecond(const unsigned char second);

	//__int64 unixTimestamp;
	unsigned char year;
	unsigned char month;
	unsigned char day;
	unsigned char hour;
	unsigned char minute;
	unsigned char second;
};

#endif
