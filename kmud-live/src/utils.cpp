/* ************************************************************************
*   File: utils.c                                       Part of CircleMUD *
*  Usage: various internal functions of a utility nature                  *
*                                                                         *
*  All rights reserved.  See license.doc for complete information.        *
*                                                                         *
*  Copyright (C) 1993, 94 by the Trustees of the Johns Hopkins University *
*  CircleMUD is based on DikuMUD, Copyright (C) 1990, 1991.               *
************************************************************************ */

#include <boost/thread/mutex.hpp>
#include "conf.h"
#include "sysdep.h"

#include "structs.h"
#include "spells.h"
#include "utils.h"
#include "db.h"
#include "comm.h"
#include "screen.h"
#include "handler.h"
#include "shop.h"

#include "StringUtil.h"
#include "Descriptor.h"

extern Descriptor *descriptor_list;
extern struct TimeData time_info;
extern std::string subroutine;

/* local functions */
struct GameTime *real_time_passed(time_t t2, time_t t1);
struct GameTime *mud_time_passed(time_t t2, time_t t1);

#ifndef WIN32
std::string Execute( const char *command )
{
	std::stringstream strCommandBuffer;
	strCommandBuffer << command << " > output";
	system(strCommandBuffer.str().c_str());

	FILE* fp = fopen( "output", "r" );
	std::string cppBuffer;
	char result[ 1024 ];
	while( !feof( fp ) ) {
		fread(result,1,1024,fp);
		cppBuffer += result;
	}
	fclose (fp);
	return cppBuffer;
/********
	FILE* fp;
	std::string cppBuffer;
	char result[ 1024 ];

	fp = popen(command,"r");
	while( !feof( fp ) ) {
		fread(result,1,1024,fp);
		cppBuffer += result;
	}
	fclose (fp);
	return cppBuffer;
********/
}
#endif

#ifndef WIN32
__int64 AvailableSystemMemory()
{
	char buffer[1024];
	FILE *processPipe = popen("/usr/bin/free", "r");
	if(processPipe == NULL)
	{
		return -1;
	}

	std::string str;
	while(!feof(processPipe))
	{
		fgets(buffer, 1023, processPipe);

		str += std::string(buffer);
	}

	fclose(processPipe);

	std::vector<std::string> lineVector = StringUtil::SplitToVector(str, '\n');
	for(int index = 0;index < lineVector.size();++index)
	{
		std::cout << "Line: " << lineVector[index] << std::endl;
		if(StringUtil::startsWith(lineVector[index], "-/+"))
		{
			std::vector<std::string> wordVector = StringUtil::SplitToVector(lineVector[index], ' ');
			if(wordVector.size() > 0)
				return MiscUtil::Convert< __int64 >( wordVector[ wordVector.size() - 1 ] );
		}
	}

	return -1;
}
#endif

std::string StripFilePath( const std::string &sFilePath )
{
	std::string::size_type iPos = sFilePath.find_last_of("/");
	if( iPos != std::string::npos )
		return sFilePath.substr( iPos+1 );
	return sFilePath;
}

/************ Time Utilities *****************/
Time::Time(time_t tt)
{
	this->t = tt;
}

Time::Time()
{
	this->t = time(0);
}

time_t Time::dSeconds()
{
	return this->t % 60;
}
time_t Time::dMinutes()
{
	return (this->t / 60) % 60;
}
time_t Time::dHours()
{
	return (this->t / 60 / 60) % 24;
}
time_t Time::dDays()
{
	return (this->t / 60 / 60 / 24);
}

time_t Time::dtSeconds()
{
	return (this->t);
}
time_t Time::dtMinutes()
{
	return (dtSeconds() / 60);
}
time_t Time::dtHours()
{
	return (dtMinutes() / 60 / 60);
}
time_t Time::dtDays()
{
	return (dtHours() / 60 / 60 / 24);
}


time_t Time::Seconds()
{
	return time(0) - this->t;
}

time_t Time::Minutes()
{
	return this->Seconds() / 60;
}

time_t Time::Hours()
{
	return this->Minutes() / 60;
}

time_t Time::Days()
{
	return this->Hours() / 24;
}
void Time::SetTime( time_t newTime )
{
	this->t = newTime;
}


//Return true if both tm blocks refer to the same calendar day. False otherwise.
bool Time::SameDay( const tm t1, const tm t2 )
{
	return( t1.tm_year == t2.tm_year && t1.tm_mon == t2.tm_mon && t1.tm_mday == t2.tm_mday );
}
bool Time::SameDay( const time_t t1, const time_t t2 )
{
	return Time::SameDay( Time::TimeToDate(t1), Time::TimeToDate(t2) );
}

//Convert a numeric epoch time representation to the C-time tm structure
tm Time::TimeToDate(time_t t)
{
	return (*(localtime(&t)));
}

//Convert a dated std::string formatted MONTH-DAY-YEAR to a time_t integer.
time_t Time::DateToTime(const std::string &date)
{
	std::string::size_type loc1 = 0, loc2 = 0 ;
	std::string day, month, year;
	time_t t = 0;
	struct tm time_info;

	if(!(loc1 = date.find_first_of("-", 0)))
		return 0;
	if(!(loc2 = date.find_first_of("-", loc1 + 1)))
		return 0;

	month.append(date, 0, loc1);
	day.append(date, loc1 + 1, loc2 - loc1 - 1);
	year.append(date, loc2 + 1, date.size() - loc2 - 1);
	//Now we have the numerical values of month, date, and year... Now to translate them.

	memset(&time_info, 0, sizeof(tm));
	t = time(0);
	time_info = *localtime(&t);

	time_info.tm_mon = atoi(month.c_str()) - 1;
	time_info.tm_year = atoi(year.c_str()) - 1900;
	time_info.tm_mday = atoi(day.c_str());
	time_info.tm_hour = 0;
	time_info.tm_min = 0;
	time_info.tm_sec = 0;

	t = mktime(&time_info);

	return t;
}
int Time::DaysPerMonth( const int month, const int year )
{
	if( month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12 )
		return 31;
	if( month == 4 || month == 6 || month == 9 || month == 11 )
		return 30;
	//february
	if( (year % 4) == 0 )
		return 29;//leap year
	return 28;
}
int Time::DaysPerYear( const int year )
{
	if( (year % 4) == 0 ) return 366;
	return 365;
}
tm Time::PriorDay( tm Today )
{
	//Rollover the month?
	if( --Today.tm_mday <= 0 )
	{
		if( --Today.tm_mon < 0 )
		{
			--Today.tm_year;
			Today.tm_mon = 11;
		}
		Today.tm_mday = Time::DaysPerMonth( Today.tm_mon+1, Today.tm_year+1900 );
	}
	if( --Today.tm_wday < 0 ) Today.tm_wday = 6;
	if( --Today.tm_yday < 0 ) Today.tm_yday = Time::DaysPerYear( Today.tm_year+1900 )-1;
	return Today;
}
tm Time::NextDay( tm Today )
{
	//Rollover the month?
	if( ++Today.tm_mday > Time::DaysPerMonth( Today.tm_mon+1, Today.tm_year+1900 ) )
	{
		if( ++Today.tm_mon > 11 )
		{
			++Today.tm_year;
			Today.tm_mon = 0;
		}
		Today.tm_mday = 1;
	}
	if( ++Today.tm_wday > 6 ) Today.tm_wday = 0;
	if( ++Today.tm_yday >= Time::DaysPerYear( Today.tm_year+1900 ) ) Today.tm_yday = 0;
	return Today;
}
tm Time::Yesterday()
{
	return Time::PriorDay( Time::TimeToDate(time(0)) );
}
tm Time::Tomorrow()
{
	return Time::NextDay( Time::TimeToDate(time(0)) );
}
/* Set all units below the 'day' to zero values */
tm Time::FloorDay( tm Today )
{
	Today.tm_hour = 0;
	Today.tm_min = 0;
	Today.tm_sec = 0;
	return Today;
}
bool Time::IsValidTime( tm *t )
{
	if( t && t->tm_sec >= 0 && t->tm_min >= 0 && t->tm_hour >= 0 && t->tm_mday > 0 && t->tm_mon >= 0 &&
		t->tm_year >= 0 && t->tm_sec < 60 && t->tm_min < 60 && t->tm_hour < 24 && t->tm_mon < 12 &&
		t->tm_mday <= Time::DaysPerMonth(t->tm_mon+1,t->tm_year))
		return true;
	return false;
}

std::string Time::FormatDate(const std::string &Format, const tm *t )
{
	char Buffer[1024];
	strftime(Buffer, 1024, Format.c_str(), t);
	return Buffer;
}

std::string Time::FormatDate(const std::string &Format, const time_t t)
{
	return Time::FormatDate( Format, localtime(&t) );
}

std::string itos(int i)
{
	static char sInt[1024];

	sprintf(sInt, "%d", i);
	return sInt;
}

std::string ftos(float i)
{
	static char sFloat[1024];

	sprintf(sFloat, "%f", i);
	return sFloat;
}

std::string dtos(double i)
{
	static char sDouble[1024];

	sprintf(sDouble, "%f", i);
	return sDouble;
}

int strlenIgnoreColors( const char* str )
{
	int count = 0;
	while( *str )
	{
		if( *str == 27 )
		{
			while (*(str++) != 'm' );
			continue;
		}
		++str; ++count;
	}
	return count;
}
// Serai - returns 1 if word is starWord with the stars resolved, or 0 if not
int compareWordWithStars(const char *word, const char *starWord)
{
	if (!word || !starWord)
		return (0);

	for(const char *cword = word, *cstar = starWord, *cnext;; ++cword)
	{
		if (!*cword || *cstar == '*' && *(cstar + 1) == '\0')
			return (1);

		if (*cstar == '*')
		{
			cnext = strchr(++cstar, '*');

			if (!cnext)
				cnext = strchr(cstar, '\0');

			while((int)strlen(cword) >= (int)(cnext - cstar))
			{
				if (!strncmp(cstar, cword, cnext - cstar))
				{
					cword += cnext - cstar;
					break;
				}

				++cword;
			}

			cstar = cnext;

			if (*(cword + (cnext - cstar)) == '\0' && *cstar)
				return (0);

			if (!*cword && !*cstar)
				return (1);

			continue;
		}

		if (!*cstar)
			return (0);

		if (LOWER(*cword) != LOWER(*cstar))
			return (0);

		++cstar;
	}
}
/* Is there ANY parameter? */
bool HasParameter( const std::string &Buffer ) {
	return !(Buffer.find("--") == std::string::npos);
}

/* Given "-param" as Buffer, and "param" as Param, this function should return true. */
bool HasParameter( const std::string &Buffer, const std::string &Param ) {

	std::string::size_type pos = Buffer.find_first_of( "-" ), endpos;
	if( pos != std::string::npos ) {
		for(endpos = pos+1;endpos < Buffer.size() && isalnum(Buffer[endpos]);++endpos);//Find the end of the param.
		return (!str_cmp(Buffer.substr(pos+1, endpos-(pos+1)), Param));
	}
	return false;
}

//Buffer: "-param=value", return the "value" portion for parameter of name Param.
std::string ParamValue( const std::string &Buffer, const std::string &Param ) {
	std::string::size_type pos = Buffer.find_first_of( ("-"+Param) ), endpos;

	if( pos != std::string::npos ) {
		pos += Param.length()+1;

		if( pos < Buffer.length() && Buffer[pos] == '=' ) {
			for(endpos=++pos;endpos < Buffer.length() && isalnum(Buffer[endpos]);++endpos);
			return Buffer.substr(pos, (endpos-pos));
		}
	}
	return (std::string(""));
}

std::string eatwhite( std::string str )
{
	std::string::size_type i;
	while( (i = str.find(" ")) != std::string::npos )
		str.erase(i,i+1);
	return str;
}

/* Create a duplicate of a string using C++ array alloation(use strdup() for C) */
char *str_dup(const char *source)
{
	char *new_z;

	if(!source)
		return NULL;

	new_z = new char[strlen(source) + 1];

	return (strcpy(new_z, source));
}

/* Return a string with 'n' counts of 'ch' */
std::string getNChars(char ch, int n)
{
	std::string str;

	for(int i = 0;i < n;++i)
		str += ch;

	return str;
}

//strcmp() for STD strings, use as possible optimization tool.
int strcmp(const std::string &str1, const std::string &str2)
{
	return strcmp(str1.c_str(), str2.c_str());
}


//Rewritten by Galnor on June 27th, 2006.
int str_cmp(const char *str1, const char *str2)
{
	int ck;
	while(*str1 || *str2)
	{
		if(ck = UPPER(*(str1)) - UPPER(*(str2)) != 0)
			return ck;
		++str1;
		++str2;
	}
	return 0;
}
int str_cmp(const std::string &str1, const char *str2)
{
	return str_cmp(str1.c_str(), str2);
}
int str_cmp(const char *str1, const std::string &str2)
{
	return str_cmp(str1, str2.c_str());
}
int str_cmp(const std::string &str1, const std::string &str2)
{
	return str_cmp(str1.c_str(), str2.c_str());
}


//Rewritten by Galnor on June 28th, 2006.
int strn_cmp(const char *str1, const char *str2, size_t n)
{
	int ck;
	while((*str1 || *str2) && n)
	{
		if(ck = UPPER(*(str1)) - UPPER(*(str2)) != 0)
			return ck;
		++str1;
		++str2;
		--n;
	}
	return 0;
}
int strn_cmp(const std::string &str1, const char *str2, size_t n)
{
	return strn_cmp(str1.c_str(), str2, n);
}
int strn_cmp(const char *str1, const std::string &str2, size_t n)
{
	return strn_cmp(str1, str2.c_str(), n);
}
int strn_cmp(const std::string &str1, const std::string &str2, size_t n)
{
	return strn_cmp(str1.c_str(), str2.c_str(), n);
}

// Sprintf but for std strings.
std::string &sprintf(std::string &str, const char *format, ...)
{
	static char cStr[1024*16];

	va_list args;
	va_start(args, format);
	//	vsnprintf(cStr, sizeof(cStr), format, args);
	vsprintf(cStr, format, args);
	va_end(args);

	str = cStr;
	return str;
}

std::string &strcat(std::string &str, const char *format, ...)
{
	static char cStr[MAX_STRING_LENGTH];

	*cStr = '\0';

	va_list args;
	va_start(args, format);
	//vsnprintf(cStr, sizeof(cStr), format, args);
	vsprintf(cStr, format, args);
	va_end(args);

	str += cStr;
	return str;
}

/*
 * New variable argument log() function.  Works the same as the old for
 * previously written code but is very nice for new code.
 */
void BasicMudLog(const char *format, ...)
{
	static boost::mutex logMutex;

	va_list args;
	time_t ct = time(0);
	char *time_s = asctime(localtime(&ct));

	time_s[strlen(time_s) - 1] = '\0';

	if (logfile == NULL)
		abort();

	{//Lock mutex
		boost::mutex::scoped_lock( logMutex );
		fprintf(logfile, "%-15.15s :: ", time_s + 4);

		va_start(args, format);
		vfprintf(logfile, format, args);
		va_end(args);

		fprintf(logfile, "\n");
		fflush(logfile);
	}//Release mutex
}

/* the "touch" command, essentially. */
int Touch(const std::string &Path)
{
	std::ifstream File( Path.c_str() );

	if ( !File.is_open() )
	{
		perror(Path.c_str());
		return -1;
	}

	else
	{
		File.close();
		return 0;
	}
}

/*
 * MudLog -- log mud messages to a file & to online imm's syslogs
 * based on syslog by Fen Jul 3, 1992
 */
void MudLog(int type, int level, int file, const char *str, ...)
{
	FILE *logger;
	char buf[MAX_STRING_LENGTH], format[MAX_STRING_LENGTH], tp;
	Descriptor *i;
	va_list args;

	va_start(args, str);
	//	vsnprintf(format, sizeof(format), str, args);
	vsprintf(format, str, args);
	va_end(args);

	if (file)
	{
		Log("%s", format);
		
		std::string fileName = STDERR;
		if(subroutine.empty() == false)
			fileName = std::string("misc/") + subroutine;

		if(!(logger = fopen(fileName.c_str(), "a+")))
		{
			Log("ERROR OPENING MudLog FILE!");
			return;
		}
		fprintf(logger, "%s :: %s\n", Time::FormatDate("%m-%d-%Y, %H:%M:%S", time(0)).c_str(), format);
		fclose(logger);
	}

	if (level < 0)
		return;

	sprintf(buf, "[ %s ]\r\n", format);

	for (i = descriptor_list; i; i = i->next)
	{
		if (STATE(i) == CON_PLAYING && !PLR_FLAGGED(i->character, PLR_WRITING))
		{
			tp = ((PRF_FLAGGED(i->character, PRF_LOG1) ? 1 : 0) +
			      (PRF_FLAGGED(i->character, PRF_LOG2) ? 2 : 0));

			if ((GET_LEVEL(i->character) >= level) && (tp >= type))
			{
				i->Send(COLOR_GREEN(i->character, CL_NORMAL));
				i->Send(StringUtil::vaEscape(buf).c_str());
				i->Send(COLOR_NORMAL(i->character, CL_NORMAL));
			}
		}
	}
}
/* Print out respective 'names' for any bits set in 'bitvector'
 * delim, colVec, and colDel are optional
 */
void sprintbit(std::bitset<64> &bitvector, const char *names[], char *result, const std::string delim,
			   const std::string colVec, const std::string colDel)
{
	bool first=true, end=false;

	*result = '\0';

	for( unsigned int i = 0;i < bitvector.size();++i )
	{
		if( end || *names[i] == '\n' )
			end = true;
		if( bitvector.test(i) == true )
		{
			if( first == false )
				sprintf(result+strlen(result), "%s%s", colDel.c_str(), delim.c_str());
			first=false;

			strcat(result, colVec.c_str());
			if ( !end )
				strcat(result, names[i]);
			else
				strcat(result, "UNDEFINED");
		}
	}

	if (!*result)
		strcpy(result, "NOBITS ");
	if( !colDel.empty() || !colVec.empty() )
		strcat(result, NORMAL);
}


void sprintbit(long bitvector, const char *names[], char *result, const std::string delim,
			   const std::string colVec, const std::string colDel)
{
	std::bitset<64> bSet(bitvector);
	sprintbit(bSet, names, result, delim, colVec, colDel);
}

void sprinttype(int type, const char *names[], char *result)
{
	int nr = 0;

	while (type && *names[nr] != '\n')
	{
		--type;
		++nr;
	}

	if (*names[nr] != '\n')
		strcpy(result, names[nr]);

	else
		strcpy(result, "UNDEFINED");
}


/* Calculate the REAL time passed over the last t2-t1 centuries (secs) */
struct GameTime *real_time_passed(time_t t2, time_t t1)
{
	long secs;
	static struct GameTime now;

	secs = (long) (t2 - t1);

	now.hours = (secs / SECS_PER_REAL_HOUR) % 24;	/* 0..23 hours */
	secs -= SECS_PER_REAL_HOUR * now.hours;

	now.day = (secs / SECS_PER_REAL_DAY);	/* 0..34 days  */
	//secs -= SECS_PER_REAL_DAY * now.day;  Not Used

	now.month = -1;
	now.year = -1;

	return &now;
}

/* Calculate the MUD time passed over the last t2-t1 centuries (secs) */
struct GameTime *mud_time_passed(time_t t2, time_t t1)
{
	long secs;
	static struct GameTime now;

	secs = (long) (t2 - t1);

	now.hours = (secs / SECS_PER_MUD_HOUR) % 24;	/* 0..23 hours */
	secs -= SECS_PER_MUD_HOUR * now.hours;

	now.day = (secs / SECS_PER_MUD_DAY) % 35;	/* 0..34 days  */
	secs -= SECS_PER_MUD_DAY * now.day;

	now.month = (secs / SECS_PER_MUD_MONTH) % 17;	/* 0..16 months */
	secs -= SECS_PER_MUD_MONTH * now.month;

	now.year = (secs / SECS_PER_MUD_YEAR);	/* 0..XX? years */

	return &now;
}

struct GameTime *age(Character * ch)
{
	static struct GameTime player_age;

	player_age = *mud_time_passed(time(0), ch->player.time.birth.getTime());

	player_age.year += 17;	/* All players start at 17 */

	return &player_age;
}

/*
 * get_line reads the next non-blank line off of the input stream.
 * The newline character is removed from the input.  Lines which begin
 * with '*' are considered to be comments.
 *
 * Returns the number of lines advanced in the file.
 */
int get_line(FILE * fl, char *buf)
{
	char temp[256];
	int lines = 0;

	memset(temp, 0, sizeof(temp));

	do
	{
		++lines;
		fgets(temp, 256, fl);

		if (*temp)
			temp[strlen(temp) - 1] = '\0';
	}

	while (!feof(fl) && (*temp == '*' || !*temp));

	if (feof(fl))
	{
		*buf = '\0';
		return 0;
	}

	else
	{
		strcpy(buf, temp);
		return lines;
	}
}

int get_filename(const std::string &orig_name, char *filename, int mode)
{
	const char *prefix, *middle, *suffix;
	char name[64], *ptr;

	switch (mode)
	{
		case PLOG_FILE:
			prefix = LIB_PLRLOGS;
			suffix = SUF_PLOG;
			break;
		default:
			return 0;
	}

	if (orig_name.empty())
		return 0;

	strcpy(name, orig_name.c_str());

	for (ptr = name; *ptr;++ptr)
		*ptr = tolower(*ptr);



	sprintf(filename, "%s%s"SLASH"%s.%s", prefix, middle, name, suffix);
	return 1;
}

void sprintbitarray(int bitvector[], const char *names[], int maxar, char *result)
{
	int nr, teller, found = FALSE;

	*result = '\0';

	for(teller = 0; teller < maxar && !found;++teller)
		for (nr = 0; nr < 32 && !found;++nr)
		{
			if (IS_SET_AR(bitvector, (teller*32)+nr))
				if (*names[(teller*32)+nr] != '\n')
				{
					if (*names[(teller*32)+nr] != '\0')
					{
						strcat(result, names[(teller*32)+nr]);
						strcat(result, " ");
					}
				}

				else
				{
					strcat(result, "UNDEFINED ");
				}

			if (*names[(teller*32)+nr] == '\n')
				found = TRUE;
		}

	if (!*result)
		strcpy(result, "NOBITS ");
}

/*
 * This function (derived from basic fork(); abort(); idea by Erwin S.
 * Andreasen) causes your MUD to dump core (assuming you can) but
 * continue running.  The core dump will allow post-mortem debugging
 * that is less severe than assert();  Don't call this directly as
 * core_dump_unix() but as simply 'core_dump()' so that it will be
 * excluded from systems not supporting them. (e.g. Windows '95).
 *
 * XXX: Wonder if flushing streams includes sockets?
 */
void core_dump_real(const char *who, ush_int line)
{
	Log("SYSERR: Assertion failed at %s:%d!", who, line);

#if defined(CIRCLE_UNIX)
	/* These would be duplicated otherwise... */
	fflush(stdout);
	fflush(stderr);
	fflush(logfile);

	/*
	 * Kill the child so the debugger or script doesn't think the MUD
	 * crashed.  The 'autorun' script would otherwise run it again.
	*/

	if (fork() == 0)
		abort();
#endif
}



/* re-formats message type formatted char * */
/* (for strings edited with d->str) (mostly olc and mail)     */
void format_text(char **ptr_string, int mode, Descriptor *d, int maxlen)
{

	int total_chars, cap_next = TRUE, cap_next_next = FALSE;
	char *flow, *start = NULL, temp;
	/* warning: do not edit messages with max_str's of over this value */
	char formated[MAX_STRING_LENGTH];

	flow   = *ptr_string;
	if (!flow)
		return;

	if (IS_SET(mode, FORMAT_INDENT))
	{
		strcpy(formated, "   ");
		total_chars = 3;
	}

	else
	{
		*formated = '\0';
		total_chars = 0;
	}

	while (*flow != '\0')
	{
		while ((*flow == '\n') ||
		        (*flow == '\r') ||
		        (*flow == '\f') ||
		        (*flow == '\t') ||
		        (*flow == '\v') ||
		        (*flow == ' '))
			++flow;

		if (*flow != '\0')
		{

			start = flow++;

			while ((*flow != '\0') &&
			        (*flow != '\n') &&
			        (*flow != '\r') &&
			        (*flow != '\f') &&
			        (*flow != '\t') &&
			        (*flow != '\v') &&
			        (*flow != ' ') &&
			        (*flow != '.') &&
			        (*flow != '?') &&
			        (*flow != '!'))
				++flow;

			if (cap_next_next)
			{
				cap_next_next = FALSE;
				cap_next = TRUE;
			}

			/* this is so that if we stopped on a sentance .. we move off the sentance delim. */
			while ((*flow == '.') || (*flow == '!') || (*flow == '?'))
			{
				cap_next_next = TRUE;
				++flow;
			}

			temp = *flow;
			*flow = '\0';

			if ((total_chars + strlen(start) + 1) > 79)
			{
				strcat(formated, "\r\n");
				total_chars = 0;
			}

			if (!cap_next)
			{
				if (total_chars > 0)
				{
					strcat(formated, " ");
					total_chars++;
				}
			}

			else
			{
				cap_next = FALSE;
				*start = UPPER(*start);
			}

			total_chars += strlen(start);
			strcat(formated, start);

			*flow = temp;
		}

		if (cap_next_next)
		{
			if ((total_chars + 3) > 79)
			{
				strcat(formated, "\r\n");
				total_chars = 0;
			}

			else
			{
				strcat(formated, "  ");
				total_chars += 2;
			}
		}
	}

	strcat(formated, "\r\n");

	if (strlen(formated) > (unsigned int) maxlen)
		formated[maxlen] = '\0';

	if (*ptr_string)
		delete[] (*ptr_string);

	*ptr_string = new char[MIN( (size_t)maxlen, strlen(formated) + 3)];
	strcpy(*ptr_string, formated);
}

bool convert_string_to_bool(const std::string& s) {
	if(s=="0")
		return false;
	else
		return true;
}

std::vector<bool> split_to_bool_vector(const std::string &s)
{
	std::vector<bool> vb;
	for (int i = 0 ; i < (signed) s.size() ; ++i )
	{
		if (s.at(i) == '1')
			vb.push_back(true);
		else if (s.at(i) == '0')
			vb.push_back(false);
		else if (s.at(i) == ' ')
			continue;
		else
			vb.push_back(true);
	}
	return vb;
}
