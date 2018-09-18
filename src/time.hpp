#ifndef _ELF_TIME_HPP
#define _ELF_TIME_HPP

/****************************************
      ,             
	t -> struct tm
	tt-> std::time_t
	dt-> date time(           ~)
	v-> std::vector
	a->ascii
	w->unicode
	now->     
	when->      ,                  
****************************************/
#if defined(_MSC_VER) && defined(_WIN32)

#define TM_YEAR_BASE 1900
/*
* We do not implement alternate representations. However, we always
* check whether a given modifier is allowed for a certain conversion.
*/
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#define ALT_E 0x01
#define ALT_O 0x02
#define LEGAL_ALT(x)           \
    {                          \
        if (alt_format & ~(x)) \
            return (0);        \
    }

static int conv_num(const char **, int *, int, int);

static const char *day[7] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
	"Friday", "Saturday" };
static const char *abday[7] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static const char *mon[12] = {
	"January", "February", "March", "April", "May", "June", "July",
	"August", "September", "October", "November", "December" };
static const char *abmon[12] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static const char *am_pm[2] = {
	"AM", "PM" };

//window     strptime   linux    strptime
//strptime  windows     
char *
strptime_msvc(const char *buf, const char *fmt, struct tm *tm)
{
	char c;
	const char *bp;
	size_t len = 0;
	int alt_format, i, split_year = 0;

	bp = buf;

	while ((c = *fmt) != '\0')
	{
		/* Clear `alternate' modifier prior to new conversion. */
		alt_format = 0;

		/* Eat up white-space. */
		if (isspace(c))
		{
			while (isspace(*bp))
				bp++;

			fmt++;
			continue;
		}

		if ((c = *fmt++) != '%')
			goto literal;

	again:
		switch (c = *fmt++)
		{
		case '%': /* "%%" is converted to "%". */
			literal :
				if (c != *bp++)
					return (0);
			break;

			/*
			* "Alternative" modifiers. Just set the appropriate flag
			* and start over again.
			*/
		case 'E': /* "%E?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_E;
			goto again;

		case 'O': /* "%O?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_O;
			goto again;

			/*
			* "Complex" conversion rules, implemented through recursion.
			*/
		case 'c': /* Date and time, using the locale's format. */
			LEGAL_ALT(ALT_E);
			if (nullptr == (bp = strptime_msvc(bp, "%x %X", tm)))
				return (0);
			break;

		case 'D': /* The date as "%m/%d/%y". */
			LEGAL_ALT(0);
			if (nullptr == (bp = strptime_msvc(bp, "%m/%d/%y", tm)))
				return (0);
			break;

		case 'R': /* The time as "%H:%M". */
			LEGAL_ALT(0);
			if (nullptr == (bp = strptime_msvc(bp, "%H:%M", tm)))
				return (0);
			break;

		case 'r': /* The time in 12-hour clock representation. */
			LEGAL_ALT(0);
			if (nullptr == (bp = strptime_msvc(bp, "%I:%M:%S %p", tm)))
				return (0);
			break;

		case 'T': /* The time as "%H:%M:%S". */
			LEGAL_ALT(0);
			if (nullptr == (bp = strptime_msvc(bp, "%H:%M:%S", tm)))
				return (0);
			break;

		case 'X': /* The time, using the locale's format. */
			LEGAL_ALT(ALT_E);
			if (nullptr == (bp = strptime_msvc(bp, "%H:%M:%S", tm)))
				return (0);
			break;

		case 'x': /* The date, using the locale's format. */
			LEGAL_ALT(ALT_E);
			if (nullptr == (bp = strptime_msvc(bp, "%m/%d/%y", tm)))
				return (0);
			break;

			/*
			* "Elementary" conversion rules.
			*/
		case 'A': /* The day of week, using the locale's form. */
		case 'a':
			LEGAL_ALT(0);
			for (i = 0; i < 7; i++)
			{
				/* Full name. */
				len = strlen(day[i]);
				if (strncmp(day[i], bp, len) == 0)
					break;

				/* Abbreviated name. */
				len = strlen(abday[i]);
				if (strncmp(abday[i], bp, len) == 0)
					break;
			}

			/* Nothing matched. */
			if (i == 7)
				return (0);

			tm->tm_wday = i;
			bp += len;
			break;

		case 'B': /* The month, using the locale's form. */
		case 'b':
		case 'h':
			LEGAL_ALT(0);
			for (i = 0; i < 12; i++)
			{
				/* Full name. */
				len = strlen(mon[i]);
				if (strncmp(mon[i], bp, len) == 0)
					break;

				/* Abbreviated name. */
				len = strlen(abmon[i]);
				if (strncmp(abmon[i], bp, len) == 0)
					break;
			}

			/* Nothing matched. */
			if (i == 12)
				return (0);

			tm->tm_mon = i;
			bp += len;
			break;

		case 'C': /* The century number. */
			LEGAL_ALT(ALT_E);
			if (!(conv_num(&bp, &i, 0, 99)))
				return (0);

			if (split_year)
			{
				tm->tm_year = (tm->tm_year % 100) + (i * 100);
			}
			else
			{
				tm->tm_year = i * 100;
				split_year = 1;
			}
			break;

		case 'd': /* The day of month. */
		case 'e':
			LEGAL_ALT(ALT_O);
			if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
				return (0);
			break;

		case 'k': /* The hour (24-hour clock representation). */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'H':
			LEGAL_ALT(ALT_O);
			if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
				return (0);
			break;

		case 'l': /* The hour (12-hour clock representation). */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'I':
			LEGAL_ALT(ALT_O);
			if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
				return (0);
			if (tm->tm_hour == 12)
				tm->tm_hour = 0;
			break;

		case 'j': /* The day of year. */
			LEGAL_ALT(0);
			if (!(conv_num(&bp, &i, 1, 366)))
				return (0);
			tm->tm_yday = i - 1;
			break;

		case 'M': /* The minute. */
			LEGAL_ALT(ALT_O);
			if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
				return (0);
			break;

		case 'm': /* The month. */
			LEGAL_ALT(ALT_O);
			if (!(conv_num(&bp, &i, 1, 12)))
				return (0);
			tm->tm_mon = i - 1;
			break;

		case 'p': /* The locale's equivalent of AM/PM. */
			LEGAL_ALT(0);
			/* AM? */
			if (strcmp(am_pm[0], bp) == 0)
			{
				if (tm->tm_hour > 11)
					return (0);

				bp += strlen(am_pm[0]);
				break;
			}
			/* PM? */
			else if (strcmp(am_pm[1], bp) == 0)
			{
				if (tm->tm_hour > 11)
					return (0);

				tm->tm_hour += 12;
				bp += strlen(am_pm[1]);
				break;
			}

			/* Nothing matched. */
			return (0);

		case 'S': /* The seconds. */
			LEGAL_ALT(ALT_O);
			if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
				return (0);
			break;

		case 'U': /* The week of year, beginning on sunday. */
		case 'W': /* The week of year, beginning on monday. */
			LEGAL_ALT(ALT_O);
			/*
			* XXX This is bogus, as we can not assume any valid
			* information present in the tm structure at this
			* point to calculate a real value, so just check the
			* range for now.
			*/
			if (!(conv_num(&bp, &i, 0, 53)))
				return (0);
			break;

		case 'w': /* The day of week, beginning on sunday. */
			LEGAL_ALT(ALT_O);
			if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
				return (0);
			break;

		case 'Y': /* The year. */
			LEGAL_ALT(ALT_E);
			if (!(conv_num(&bp, &i, 0, 9999)))
				return (0);

			tm->tm_year = i - TM_YEAR_BASE;
			break;

		case 'y': /* The year within 100 years of the epoch. */
			LEGAL_ALT(ALT_E | ALT_O);
			if (!(conv_num(&bp, &i, 0, 99)))
				return (0);

			if (split_year)
			{
				tm->tm_year = ((tm->tm_year / 100) * 100) + i;
				break;
			}
			split_year = 1;
			if (i <= 68)
				tm->tm_year = i + 2000 - TM_YEAR_BASE;
			else
				tm->tm_year = i + 1900 - TM_YEAR_BASE;
			break;

			/*
			* Miscellaneous conversions.
			*/
		case 'n': /* Any kind of white-space. */
		case 't':
			LEGAL_ALT(0);
			while (isspace(*bp))
				bp++;
			break;

		default: /* Unknown/unsupported conversion. */
			return (0);
		}
	}

	/* LINTED functional specification */
	return ((char *)bp);
}

static int conv_num(const char **buf, int *dest, int llim, int ulim)
{
	int result = 0;

	/* The limit also determines the number of valid digits. */
	int rulim = ulim;

	if (**buf < '0' || **buf > '9')
		return (0);

	do
	{
		result *= 10;
		result += *(*buf)++ - '0';
		rulim /= 10;
	} while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');

	if (result < llim || result > ulim)
		return (0);

	*dest = result;
	return (1);
}
#endif

#if defined(_WIN32)
#include <Windows.h>		//    widechartomutichar
#else
#include <stdlib.h>
#endif

#include <string>
#include <chrono>
#include <ctime>
#include <algorithm>
#include <vector>
/*using time_wstr = elf::time<std::wstring>;
using time_str = elf::time<std::string>;
using time_tt = elf::time<std::time_t>;
using time_tm = elf::time<struct tm>;*/

namespace elf {
	//declaraction start
    //template <class T>
    //class TimeHelper;

	typedef std::chrono::duration<int, std::ratio<1 * 60 * 60 * 24, 1>> day_t;
	typedef std::chrono::duration<int, std::ratio<1 * 60 * 60, 1>> hour_t;
	typedef std::chrono::duration<int, std::ratio<1 * 60, 1>> minute_t;
	typedef std::chrono::duration<long long, std::ratio<1, 1>> second_t;
	typedef std::chrono::duration<long long, std::ratio<1, 1000>> milli_t;
	typedef std::chrono::duration<long long, std::ratio<1, 1000000>> mirco_t;
	typedef std::chrono::duration<long long, std::ratio<1, 1000000000>> nano_t;

    const auto DT_PAT_N_A = "%Y-%m-%d %H:%M:%S";
    const auto DT_PAT_N_W = L"%Y-%m-%d %H:%M:%S";


	//declaraction start
	/************************************************************************************/
	class CommonTime {
	private:
		explicit CommonTime() = default;
		~CommonTime() = default;
	public:
		static struct tm now_t()
		{
			auto tt = now_tt();
			auto t = to_t(tt);
			return t;

		}

		static std::time_t now_tt()
		{
			return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		}

		static struct tm to_t(const std::time_t & tt)
		{
			struct tm t = { 0 };
#if defined(_WIN32) && defined(_MSC_VER)
			localtime_s(&t, &tt);
#else
			localtime_r(&tt, &t);
#endif
			return t;
		}

		static struct tm to_t(const std::string & dt, const char * const pat = nullptr)
		{
			struct tm t = { 0 };
#if defined(_WIN32) && defined(_MSC_VER)
			strptime_msvc(dt.c_str(), nullptr == pat ? DT_PAT_N_A : pat, &t);
#else
			strptime(date_time.c_str(), nullptr == pat ? DT_PAT_N_A : pat, &t);
#endif
			return t;
		}

		static struct tm to_t(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			std::string dt_a = CommonTime::w_to_a(dt);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			auto t = to_t(dt_a.c_str(), pat_a.c_str());
			return t;
		}

		static std::time_t to_tt(const struct tm t)
		{
			return mktime(const_cast<struct tm *>(&t));
		}

		static std::string to_string(const struct tm & t, const  char * const  pat)
		{
			std::string dt(24, '\0');
			auto size = strftime(const_cast<char *>(dt.data()),
				24,
				nullptr == pat ? DT_PAT_N_A : pat,
				&t);
			if (0 == size)
			{
				dt = std::string(48, '\0');
				strftime(const_cast<char *>(dt.data()),
					48,
					nullptr == pat ? DT_PAT_N_A : pat,
					&t);
			}
			return dt;
		}

		static std::wstring to_wstring(const struct tm & t, const  wchar_t * const pat = nullptr)
		{
			std::wstring dt(24, L'\0');
			auto size = wcsftime(const_cast<wchar_t *>(dt.data()),
				24,
				nullptr == pat ? DT_PAT_N_W : pat,
				&t);
			if (0 == size)
			{
				dt = std::wstring(48, L'\0');
				wcsftime(const_cast<wchar_t *>(dt.data()),
					48,
					nullptr == pat ? DT_PAT_N_W : pat,
					&t);
			}
			return dt;
		}

		static std::time_t to_min_tt(const struct tm & t)
		{
			struct tm t0 = t;
			t0.tm_hour = 0;
			t0.tm_min = 0;
			t0.tm_sec = 0;
			auto tt = to_tt(t0);
			return tt;
		}
		static std::time_t to_min_tt(const std::time_t tt)
		{
			auto t = to_t(tt);
			auto tt0 = to_min_tt(t);
			return tt0;
		}

		static std::time_t to_max_tt(const struct tm & t)
		{
			struct tm t0 = t;
			t0.tm_hour = 23;
			t0.tm_min = 59;
			t0.tm_sec = 59;
			auto tt = to_tt(t0);
			return tt;
		}

		static std::time_t to_max_tt(const std::time_t tt)
		{
			auto t = to_t(tt);
			auto tt0 = to_max_tt(t);
			return tt0;
		}



		static std::string w_to_a(const std::wstring & w)
		{
			std::string a(w.length(), '\0');
#if defined(_WIN32) && defined(_MSC_VER)
			//std::copy(w.cbegin(), w.cend(), a.begin());
			WideCharToMultiByte(CP_ACP, NULL, w.c_str(), w.length(), const_cast<char *>(a.data()), w.length(), NULL, NULL);
#else
			wcstombs(const_cast<char *>(a.data()), w.c_str(), w.length());
#endif
			return a;
		}

		static std::wstring a_to_w(const std::string & a)
		{
			std::wstring w(a.length(), L'\0');
#if defined(_WIN32) && defined(_MSC_VER)
			MultiByteToWideChar(CP_ACP, NULL, a.c_str(), a.length(), const_cast<wchar_t*>(w.data()), a.length());
#else
			mbstowcs(const_cast<wchar_t *>(w.data()), a.c_str(), a.length());
#endif
			return w;
		}
	};
	/************************************************************************************/


	/************************************************************************************/
	template <class T>
    class TimeHelper{
        friend class elf::CommonTime;
	private:
		explicit TimeHelper() = default;
		~TimeHelper() = default;
	public:
		static T time(const char * const pat = nullptr)
		{
			return T(0);
		}
		static T when(const struct tm & t, const char * const pat = nullptr)
		{
			return T(0);
		}

		static T when(const std::time_t tt, const char * const pat = nullptr)
		{
			return T(0);
		}

		static T when(const std::string & dt, const char * const pat = nullptr)
		{
			return T(0);
		}

		static T when(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return T(0);
		}


		static T min_t(const char * const pat = nullptr)
		{
			return T(0);
		}
		static T min_t(const struct tm & t, const char * const pat = nullptr)
		{
			return T(0);
		}
		static T min_t(const std::time_t tt, const char * const pat = nullptr)
		{
			return T(0);
		}
		static T min_t(const std::string & dt, const char * const pat = nullptr)
		{
			return T(0);
		}
		static T min_t(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return T(0);
		}

		static T max_t(const char * const pat = nullptr)
		{
			return T(0);
		}
		static T max_t(const struct tm & t, const char * const pat = nullptr)
		{
			return T(0);
		}
		static T max_t(const std::time_t tt, const char * const pat = nullptr)
		{
			return T(0);
		}
		static T max_t(const std::string & dt, const char * const pat = nullptr)
		{
			return T(0);
		}
		static T max_t(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return T(0);
		}


		static T plus_days(int days, const char * const pat = nullptr)
		{
			return T(0);
		}
		static T plus_days(int days, const struct tm & t, const char * const pat = nullptr)
		{
			return T(0);
		}
		static T plus_days(int days, const std::time_t tt, const char * const pat = nullptr)
		{
			return T(0);
		}
		static T plus_days(int days, const std::string & dt, const char * const pat = nullptr)
		{
			return (0);
		}
		static T plus_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return (0);
		}

		static T tidy(const char * const pat = nullptr)
		{
			return T(0);
		}
		static T tidy(const struct tm & tm, const char * const pat = nullptr)
		{
			return T(0);
		}
		static T tidy(const std::time_t tt, const char * const pat = nullptr)
		{
			return T(0);	
		}
        static T tidy(const std::string & dt, const char * const pat = nullptr)
		{
			return T(0);
		}
        static T tidy(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return T(0);
		}

		static std::vector<T> gone_days(int days, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_days(int days, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_days(int days, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_days(int days, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}


		static std::vector<T> gone_hours(int hours, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_hours(int hours, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_hours(int hours, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_hours(int hours, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}

		static std::vector<T> gone_mins(int mins, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_mins(int mins, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_mins(int mins, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_mins(int mins, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		static std::vector<T> gone_mins(int mins, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			return T(0);
		}
		/**    list              */
		static std::vector<T> gone_secs(int secs, const char * const pat = nullptr)
		{
			return T(0);
		}
		static std::vector<T> gone_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			return T(0);
		}
		static std::vector<T> gone_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			return T(0);
		}
		static std::vector<T> gone_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			return T(0);
		}
		static std::vector<T> gone_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return T(0);
        }
    };

	template<>
	class TimeHelper<std::time_t> {
		friend class elf::CommonTime;
	private:
		explicit TimeHelper() = default;
		~TimeHelper() = default;
	public:
		static std::time_t time(const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return tt;
		}

		static std::time_t when(const struct tm & t, const char * const pat = nullptr)
		{
			return CommonTime::to_tt(t);
		}

		static std::time_t when(const std::time_t tt, const char * const pat = nullptr)
		{
			return tt;
		}

		static std::time_t when(const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return tt;
		}

		static std::time_t when(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return tt;
		}

		static std::time_t min_t(const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			tt = CommonTime::to_min_tt(tt);
			return tt;
		}
		static std::time_t min_t(const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_min_tt(t);
			return tt;
		}
		static std::time_t min_t(const std::time_t tt, const char * const pat = nullptr)
		{
			auto tt0 = CommonTime::to_min_tt(tt);
			return tt0;
		}
		static std::time_t min_t(const std::string & dt, const char * const pat = nullptr)
		{
			auto tt = when(dt, pat);
			tt = min_t(tt, pat);
			return tt;
		}
		static std::time_t min_t(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto tt = when(dt, pat);
			tt = min_t(tt);
			return tt;
		}

		static std::time_t max_t(const char * const pat = nullptr)
		{
			auto t = CommonTime::now_t();
			return max_t(t);
		}
		static std::time_t max_t(const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_max_tt(t);
			return tt;
		}
		static std::time_t max_t(const std::time_t tt, const char * const pat = nullptr)
		{
			auto tt0 = CommonTime::to_max_tt(tt);
			return tt0;
		}
		static std::time_t max_t(const std::string & dt, const char * const pat = nullptr)
		{
			auto tt = when(dt, pat);
			tt = CommonTime::to_max_tt(tt);
			return tt;
		}
		static std::time_t max_t(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto tt = when(dt, pat);
			tt = CommonTime::to_max_tt(tt);
			return tt;
		}


		static std::time_t plus_days(int days, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_days(days, tt, pat);
		}
		static std::time_t plus_days(int days, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_days(days, tt, pat);
		}
		static std::time_t plus_days(int days, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == days)
				return tt;
			auto tt0 = tt;
			tt0 += std::chrono::duration_cast<elf::second_t>(elf::day_t(days)).count();
			return tt0;
		}
		static std::time_t plus_days(int days, const std::string & dt, const char * const pat = nullptr)
		{
			//auto t = CommonTime::to_t(dt, pat);
			//auto tt = CommonTime::to_tt(t);
			auto tt = when(dt, pat);
			return plus_days(days, tt, pat);
		}
		static std::time_t plus_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			//auto t = CommonTime::to_t(dt, pat);
			//auto tt = CommonTime::to_tt(t);
			auto tt = when(dt, pat);
			return plus_days(days, tt);
		}

		static std::time_t plus_hours(int hours, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_hours(hours, tt, pat);
		}
		static std::time_t plus_hours(int hours, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_hours(hours, tt, pat);
		}
		static std::time_t plus_hours(int hours, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == hours)
				return tt;
			auto tt0 = tt;
			tt0 += std::chrono::duration_cast<elf::second_t>(elf::hour_t(hours)).count();
			return tt0;
		}
		static std::time_t plus_hours(int hours, const std::string & dt, const char * const pat = nullptr)
		{
			auto tt = when(dt, pat);
			return plus_hours(hours, tt, pat);
		}
		static std::time_t plus_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto tt = when(dt, pat);
			return plus_hours(hours, tt);		//         overload api,   pat   
		}


		static std::time_t plus_mins(int minutes, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_mins(minutes, tt, pat);
		}
		static std::time_t plus_mins(int minutes, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_mins(minutes, tt, pat);
		}
		static std::time_t plus_mins(int minutes, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == minutes)
				return tt;
			auto tt0 = tt;
			tt0 += std::chrono::duration_cast<elf::second_t>(elf::minute_t(minutes)).count();
			return tt0;
		}
		static std::time_t plus_mins(int minutes, const std::string & dt, const char * const pat = nullptr)
		{
			auto tt = when(dt, pat);
			return plus_mins(minutes, tt, pat);
		}
		static std::time_t plus_mins(int minutes, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto tt = when(dt, pat);
			return plus_mins(minutes, tt);
		}

		static std::time_t plus_secs(int secs, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_secs(secs, tt, pat);
		}
		static std::time_t plus_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_secs(secs, tt, pat);
		}
		static std::time_t plus_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == secs)
				return tt;
			auto tt0 = tt;
			tt0 += secs;
			return tt0;
		}
		static std::time_t plus_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			auto tt = when(dt, pat);
			return plus_secs(secs, tt, pat);
		}
		static std::time_t plus_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto tt = when(dt, pat);
			return plus_secs(secs, tt);
		}


		static std::time_t tidy(const char * const pat = nullptr)
		{
			auto t = CommonTime::now_t();
			return tidy(t, pat);
		}
		static std::time_t tidy(const struct tm & t, const char * const pat = nullptr)
		{
			auto t0 = t;
			t0.tm_min = 0;
			t0.tm_sec = 0;
			return when(t0, pat);
		}
		static std::time_t tidy(const std::time_t tt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(tt);
			return tidy(t, pat);
		}
		static std::time_t tidy(const std::string &dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return tidy(t, pat);
		}
		static std::time_t tidy(const std::wstring &dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return tidy(t, pat_a.c_str());
		}

		static std::vector<std::time_t> gone_days(int days, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_days(days, tt, pat, keepTidy);
		}
		static std::vector<std::time_t> gone_days(int days, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_days(days, tt, pat, keepTidy);
		}
		static std::vector<std::time_t> gone_days(int days, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt0);
				t.tm_hour = 0;
				t.tm_min = 0;
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = days > 0;
			auto base = order ? days : -days;
			std::vector<std::time_t> v_tt;
			for (int i = 0; i < base; ++i)
			{
				
				//auto tt1 = tt0 - std::chrono::duration_cast<elf::second_t>(elf::day_t(base - i - 1)).count();
				auto tt1 = plus_days(order ? i - days + 1 : i, tt0, pat);
				v_tt.push_back(tt1);
			}
			return v_tt;
		}
		static std::vector<std::time_t> gone_days(int days, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_days(days, t, pat, keepTidy);
		}
		static std::vector<std::time_t> gone_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_days(days, t, pat_a.c_str(), keepTidy);
		}


		static std::vector<std::time_t> gone_hours(int hours, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_hours(hours, tt, pat, keepTidy);
		}
		static std::vector<std::time_t> gone_hours(int hours, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_hours(hours, tt, pat, keepTidy);
        }
        static std::vector<std::time_t> gone_hours(int hours, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
        {
            auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_min = 0;
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = hours > 0;
			auto base = order ? hours : -hours;
			std::vector<std::time_t> v_tt;
			for (int i = 0; i < base; ++i)
			{

				//auto tt1 = tt0 - std::chrono::duration_cast<elf::second_t>(elf::day_t(base - i - 1)).count();
				auto tt1 = plus_hours(order ? i - hours + 1 : i, tt0, pat);
				v_tt.push_back(tt1);
			}
			return v_tt;
		}
		static std::vector<std::time_t> gone_hours(int hours, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_hours(hours, t, pat, keepTidy);
		}
		static std::vector<std::time_t> gone_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_hours(hours, t, pat_a.c_str(), keepTidy);
		}

		static std::vector<std::time_t> gone_mins(int mins, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_mins(mins, tt, pat, keepTidy);
		}
		static std::vector<std::time_t> gone_mins(int mins, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_mins(mins, tt, pat, keepTidy);
		}
		static std::vector<std::time_t> gone_mins(int mins, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = mins > 0;
			auto base = order ? mins : -mins;
			std::vector<std::time_t> v_tt;
			for (int i = 0; i < base; ++i)
			{

				//auto tt1 = tt0 - std::chrono::duration_cast<elf::second_t>(elf::day_t(base - i - 1)).count();
				auto tt1 = plus_mins(order ? i - mins + 1 : i, tt0, pat);
				v_tt.push_back(tt1);
			}
			return v_tt;
		}
		static std::vector<std::time_t> gone_mins(int mins, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_mins(mins, t, pat, keepTidy);
		}
		static std::vector<std::time_t> gone_mins(int mins, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_mins(mins, t, pat_a.c_str(), keepTidy);
		}
		/**    list              */
		static std::vector<std::time_t> gone_secs(int secs, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return gone_secs(secs, tt, pat);
		}
		static std::vector<std::time_t> gone_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_secs(secs, tt, pat);
		}
		static std::vector<std::time_t> gone_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			auto tt0 = tt;
			bool order = secs > 0;
			auto base = order ? secs : -secs;
			std::vector<std::time_t> v_tt;
			for (int i = 0; i < base; ++i)
			{

				//auto tt1 = tt0 - std::chrono::duration_cast<elf::second_t>(elf::day_t(base - i - 1)).count();
				auto tt1 = plus_secs(order ? i - secs + 1 : i, tt0, pat);
				v_tt.push_back(tt1);
			}
			return v_tt;
		}
		static std::vector<std::time_t> gone_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_secs(secs, t, pat);
		}
		static std::vector<std::time_t> gone_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_secs(secs, t, pat_a.c_str());
		}
	};

	template<>
	class TimeHelper<struct tm> {
		friend class elf::CommonTime;
	private:
		explicit TimeHelper() = default;
		~TimeHelper() = default;
	public:
		static struct tm time(const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			auto t = CommonTime::to_t(tt);
			return t;
		}

		static struct tm when(const struct tm & t, const char * const pat = nullptr)
		{
			return t;
		}

		static struct tm when(const std::time_t tt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(tt);
			return t;
		}

		static struct tm when(const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return t;
		}

		static struct tm when(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return t;
		}

		static struct tm min_t(const char * const pat = nullptr)
		{
			auto t = CommonTime::now_t();
			t.tm_hour = 0;
			t.tm_min = 0;
			t.tm_sec = 0;
			return t;
		}
		static struct tm min_t(const struct tm & t, const char * const pat = nullptr)
		{
			struct tm t0 = t;
			t0.tm_hour = 0;
			t0.tm_min = 0;
			t0.tm_sec = 0;
			return t0;

		}
		static struct tm min_t(const std::time_t tt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(tt);
			return min_t(t);
		}
		static struct tm min_t(const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return min_t(t);
		}
		static struct tm min_t(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return min_t(t);
		}

		static struct tm max_t(const char * const pat = nullptr)
		{
			auto t = CommonTime::now_t();
			return max_t(t);
		}
		static struct tm max_t(const struct tm & t, const char * const pat = nullptr)
		{
			struct tm t0 = t;
			t0.tm_hour = 23;
			t0.tm_min = 59;
			t0.tm_sec = 59;
			return t;
		}
		static struct tm max_t(const std::time_t tt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(tt);
			return max_t(t);
		}
		static struct tm max_t(const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return max_t(t);
		}
		static struct tm max_t(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return max_t(t);
		}



		static struct tm plus_days(int days, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_days(days, tt, pat);
		}
		static struct tm plus_days(int days, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_days(days, tt, pat);
		}
		static struct tm plus_days(int days, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == days) {
				return CommonTime::to_t(tt);
			}
			auto tt0 = tt;
			tt0 += std::chrono::duration_cast<elf::second_t>(elf::day_t(days)).count();
			return CommonTime::to_t(tt0);
		}
		static struct tm plus_days(int days, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = when(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_days(days, tt, pat);
		}
		static struct tm plus_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = when(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_days(days, tt);
		}


		static struct tm plus_hours(int hours, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_hours(hours, tt, pat);
		}
		static struct tm plus_hours(int hours, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_hours(hours, tt, pat);
		}
		static struct tm plus_hours(int hours, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == hours)
				return CommonTime::to_t(tt);
			auto tt0 = tt;
			tt0 += std::chrono::duration_cast<elf::second_t>(elf::hour_t(hours)).count();
			return CommonTime::to_t(tt0);
		}
		static struct tm plus_hours(int hours, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = when(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_hours(hours, tt, pat);
		}
		static struct tm plus_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = when(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_hours(hours, tt);
		}


		static struct tm plus_mins(int minutes, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_mins(minutes, tt, pat);
		}
		static struct tm plus_mins(int minutes, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_mins(minutes, tt, pat);
		}
		static struct tm plus_mins(int minutes, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == minutes)
				return CommonTime::to_t(tt);
			auto tt0 = tt;
			tt0 += std::chrono::duration_cast<elf::second_t>(elf::minute_t(minutes)).count();
			return CommonTime::to_t(tt0);
		}
		static struct tm plus_mins(int minutes, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = when(dt, pat);
			//auto tt = CommonTime::to_tt(t);
			return plus_mins(minutes, t, pat);
		}
		static struct tm plus_mins(int minutes, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = when(dt, pat);
			//auto tt = CommonTime::to_tt(t);
			return plus_mins(minutes, t);
		}

		static struct tm plus_secs(int secs, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_secs(secs, tt, pat);
		}
		static struct tm plus_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_secs(secs, tt, pat);
		}
		static struct tm plus_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == secs)
				return CommonTime::to_t(tt);
			auto tt0 = tt;
			tt0 += secs;
			return CommonTime::to_t(tt0);
		}
		static struct tm plus_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = when(dt, pat);
			return plus_secs(secs, t, pat);
		}
		static struct tm plus_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = when(dt, pat);
			return plus_secs(secs, t);
		}

		static struct tm tidy(const char * const pat = nullptr)
		{
			auto t = CommonTime::now_t();
			t.tm_min = 0;
			t.tm_sec = 0;
			return t;
		}
		static struct tm tidy(const struct tm & t, const char * const pat = nullptr)
		{
			auto t0 = t;
			t0.tm_min = 0;
			t0.tm_sec = 0;
			return t0;
		}
		static struct tm tidy(const std::time_t tt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(tt);
			return tidy(t, pat);
		}
		static struct tm tidy(const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return tidy(t, pat);
		}
		static struct tm tidy(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return tidy(t, nullptr);
		}

		static std::vector<struct tm> gone_days(int days, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_days(days, tt, pat, keepTidy);
		}
		static std::vector<struct tm> gone_days(int days, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_days(days, tt, pat, keepTidy);
		}
		static std::vector<struct tm> gone_days(int days, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_hour = 0;
				t.tm_min = 0;
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = days > 0;
			auto base = order ? days : -days;
			std::vector<struct tm> v_t;
			for (int i = 0; i < base; ++i)
			{

				//auto tt1 = tt0 - std::chrono::duration_cast<elf::second_t>(elf::day_t(base - i - 1)).count();
				auto tt1 = plus_days(order ? i - days + 1 : i, tt0, pat);
				v_t.push_back(tt1);
			}
			return v_t;
		}
		static std::vector<struct tm> gone_days(int days, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_days(days, t, pat, keepTidy);
		}
		static std::vector<struct tm> gone_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_days(days, t, pat_a.c_str(), keepTidy);
		}


		static std::vector<struct tm> gone_hours(int hours, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_hours(hours, tt, pat, keepTidy);
		}
		static std::vector<struct tm> gone_hours(int hours, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_hours(hours, tt, pat, keepTidy);
		}
		static std::vector<struct tm> gone_hours(int hours, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_hour = 0;
				t.tm_min = 0;
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = hours > 0;
			auto base = order ? hours : -hours;
			std::vector<struct tm> v_t;
			for (int i = 0; i < base; ++i)
			{

				//auto tt1 = tt0 - std::chrono::duration_cast<elf::second_t>(elf::day_t(base - i - 1)).count();
				auto tt1 = plus_hours(order ? i - hours + 1 : i, tt0, pat);
				v_t.push_back(tt1);
			}
			return v_t;
		}
		static std::vector<struct tm> gone_hours(int hours, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_hours(hours, t, pat, keepTidy);
		}
		static std::vector<struct tm> gone_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_hours(hours, t, pat_a.c_str(), keepTidy);
		}

		static std::vector<struct tm> gone_mins(int mins, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_mins(mins, tt, pat, keepTidy);
		}
		static std::vector<struct tm> gone_mins(int mins, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_mins(mins, tt, pat, keepTidy);
		}
		static std::vector<struct tm> gone_mins(int mins, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_hour = 0;
				t.tm_min = 0;
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = mins > 0;
			auto base = order ? mins : -mins;
			std::vector<struct tm> v_t;
			for (int i = 0; i < base; ++i)
			{

				//auto tt1 = tt0 - std::chrono::duration_cast<elf::second_t>(elf::day_t(base - i - 1)).count();
				auto tt1 = plus_mins(order ? i - mins + 1 : i, tt0, pat);
				v_t.push_back(tt1);
			}
			return v_t;
		}
		static std::vector<struct tm> gone_mins(int mins, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_mins(mins, t, pat, keepTidy);
		}
		static std::vector<struct tm> gone_mins(int mins, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_mins(mins, t, pat_a.c_str(), keepTidy);
		}
		/**    list              */
		static std::vector<struct tm> gone_secs(int secs, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return gone_secs(secs, tt, pat);
		}
		static std::vector<struct tm> gone_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_secs(secs, tt, pat);
		}
		static std::vector<struct tm> gone_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			auto tt0 = tt;
			bool order = secs > 0;
			auto base = order ? secs : -secs;
			std::vector<struct tm> v_t;
			for (int i = 0; i < base; ++i)
			{

				//auto tt1 = tt0 - std::chrono::duration_cast<elf::second_t>(elf::day_t(base - i - 1)).count();
				auto tt1 = plus_secs(order ? i - secs + 1 : i, tt0, pat);
				v_t.push_back(tt1);
			}
			return v_t;
		}
		static std::vector<struct tm> gone_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_secs(secs, t, pat);
		}
		static std::vector<struct tm> gone_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_secs(secs, t, pat_a.c_str());
		}
	};

	template<>
	class TimeHelper<std::string> {
		friend class elf::CommonTime;
	private:
		explicit TimeHelper() = default;
		~TimeHelper() = default;
	public:
		static std::string time(const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			auto t = CommonTime::to_t(tt);
			return CommonTime::to_string(t, pat);
		}

		static std::string when(const struct tm & t, const char * const pat = nullptr)
		{
			auto dt = CommonTime::to_string(t, pat);
			return dt;
		}

		static std::string when(const std::time_t tt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(tt);
			auto dt = CommonTime::to_string(t, pat);
			return dt;
		}

		static std::string when(const std::string & dt, const char * const pat = nullptr)
		{
			return std::string(dt);
		}

		static std::string when(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return CommonTime::w_to_a(dt);
		}


		static std::string plus_days(int days, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_days(days, tt, pat);
		}
		static std::string plus_days(int days, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_days(days, tt, pat);
		}
		static std::string plus_days(int days, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == days)
				return when(tt, pat);
			auto tt0 = tt;
			tt0 += std::chrono::duration_cast<elf::second_t>(elf::day_t(days)).count();
			return when(tt0, pat);
		}
		static std::string plus_days(int days, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_days(days, tt, pat);
		}
		static std::string plus_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			tt += std::chrono::duration_cast<elf::second_t>(elf::day_t(days)).count();
			return when(tt, pat_a.c_str());
		}


		static std::string plus_hours(int hours, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_hours(hours, tt, pat);
		}
		static std::string plus_hours(int hours, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_hours(hours, tt, pat);
		}
		static std::string plus_hours(int hours, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == hours)
				return when(tt, pat);
			auto tt0 = tt;
			tt0 += std::chrono::duration_cast<elf::second_t>(elf::day_t(hours)).count();
			return when(tt0, pat);
		}
		static std::string plus_hours(int hours, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_hours(hours, tt, pat);
		}
		static std::string plus_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return plus_hours(hours, tt, pat_a.c_str());
		}


		static std::string plus_mins(int minutes, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_mins(minutes, tt, pat);	
		}
		static std::string plus_mins(int minutes, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_mins(minutes, tt, pat);
		}
		static std::string plus_mins(int minutes, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == minutes)
				return when(tt, pat);
			auto tt0 = tt;
			tt0 += std::chrono::duration_cast<elf::second_t>(elf::minute_t(minutes)).count();
			return when(tt0, pat);
		}
		static std::string plus_mins(int minutes, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_mins(minutes, tt, pat);
		}
		static std::string plus_mins(int minutes, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return plus_mins(minutes, tt, pat_a.c_str());
		}

		static std::string plus_secs(int secs, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_secs(secs, tt, pat);
		}
		static std::string plus_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_secs(secs, tt, pat);
		}
		static std::string plus_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == secs)
				return when(tt, pat);
			auto tt0 = tt + secs;
			return when(tt0, pat);
		}
		static std::string plus_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_secs(secs, tt, pat);
		}
		static std::string plus_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return plus_secs(secs, tt, pat_a.c_str());
		}

		static std::string tidy(const char * const pat = nullptr)
		{
			auto t = CommonTime::now_t();
			return tidy(t, pat);
		}
		static std::string tidy(const struct tm & t, const char * const pat = nullptr)
		{
			auto t0 = t;
			t0.tm_min = 0;
			t0.tm_sec = 0;
			return when(t, pat);
		}
		static std::string tidy(const std::time_t tt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(tt);
			return tidy(t, pat);
		}
		static std::string tidy(const std::string &dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return tidy(t, pat);
		}
		static std::string tidy(const std::wstring &dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return tidy(t, pat_a.c_str());
		}


		static std::vector<std::string> gone_days(int days, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_days(days, tt, pat, keepTidy);
		}
		static std::vector<std::string> gone_days(int days, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_days(days, tt, pat, keepTidy);
		}
		static std::vector<std::string> gone_days(int days, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_hour = 0;
				t.tm_min = 0;
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = days > 0;
			auto base = order ? days : -days;
			std::vector<std::string> v_dt;
			for (int i = 0; i < base; ++i)
			{
				auto dt = plus_days(order ? i - days + 1 : i, tt0, pat);
				v_dt.push_back(dt);
			}
			return v_dt;
		}
		static std::vector<std::string> gone_days(int days, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_days(days, t, pat, keepTidy);
		}
		static std::vector<std::string> gone_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_days(days, t, pat_a.c_str(), keepTidy);
		}


		static std::vector<std::string> gone_hours(int hours, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_hours(hours, tt, pat, keepTidy);
		}
		static std::vector<std::string> gone_hours(int hours, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_hours(hours, tt, pat, keepTidy);
		}
		static std::vector<std::string> gone_hours(int hours, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_min = 0;
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = hours > 0;
			auto base = order ? hours : -hours;
			std::vector<std::string> v_dt;
			for (int i = 0; i < base; ++i)
			{
				auto dt = plus_hours(order ? i - hours + 1 : i, tt0, pat);
				v_dt.push_back(dt);
			}
			return v_dt;
		}
		static std::vector<std::string> gone_hours(int hours, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_hours(hours, t, pat, keepTidy);
		}
		static std::vector<std::string> gone_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_hours(hours, t, pat_a.c_str(), keepTidy);
		}

		static std::vector<std::string> gone_mins(int mins, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_mins(mins, tt, pat, keepTidy);
		}
		static std::vector<std::string> gone_mins(int mins, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_mins(mins, tt, pat, keepTidy);
		}
		static std::vector<std::string> gone_mins(int mins, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
            bool order = mins > 0;
            auto base = order ? mins : -mins;
            std::vector<std::string> v_dt;
            for(auto i = 0; i < base; ++i){
                auto dt = plus_mins(order ? i - mins + 1 : i, tt0, pat);
                v_dt.push_back(dt);
            }
            return v_dt;
		}
		static std::vector<std::string> gone_mins(int mins, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_mins(mins, t, pat, keepTidy);
		}
		static std::vector<std::string> gone_mins(int mins, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_mins(mins, t, pat_a.c_str(), keepTidy);
		}
		/**    list              */
		static std::vector<std::string> gone_secs(int secs, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return gone_secs(secs, tt, pat);
		}
		static std::vector<std::string> gone_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_secs(secs, tt, pat);
		}
		static std::vector<std::string> gone_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			auto tt0 = tt;
			bool order = secs > 0;
			auto base = order ? secs : -secs;
			std::vector<std::string> v_dt;
			for (int i = 0; i < base; ++i)
			{
				auto dt = plus_secs(order ? i - secs + 1 : i, tt0, pat);
				v_dt.push_back(dt);
			}
			return v_dt;
		}
		static std::vector<std::string> gone_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_secs(secs, t, pat);
		}
		static std::vector<std::string> gone_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_secs(secs, t, pat_a.c_str());
		}
	};

	template<>
	class TimeHelper<std::wstring> {
		friend class elf::CommonTime;
	private:
		explicit TimeHelper() = default;
		~TimeHelper() = default;
	public:
		static std::wstring time(const char * const pat = nullptr)
		{
			auto t = CommonTime::now_t();
			return when(t, pat);
		}

		static std::wstring when(const struct tm & t, const char * const pat = nullptr)
		{
			auto dt = CommonTime::to_string(t, pat);
			return CommonTime::a_to_w(dt);
		}

		static std::wstring when(const std::time_t tt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(tt);
			return when(t, pat);
		}

		static std::wstring when(const std::string & dt, const char * const pat = nullptr)
		{
			return CommonTime::a_to_w(dt);
		}

		static std::wstring when(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return std::wstring(dt);
		}


		static std::wstring plus_days(int days, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_days(days, tt, pat);
		}
		static std::wstring plus_days(int days, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_days(days, tt, pat);
		}
		static std::wstring plus_days(int days, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == days)
				return when(tt, pat);
			auto tt0 = tt + std::chrono::duration_cast<elf::second_t>(elf::day_t(days)).count();
			return when(tt0, pat);
		}
		static std::wstring plus_days(int days, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_days(days, tt, pat);
		}
		static std::wstring plus_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return plus_days(days, tt, pat_a.c_str());
		}


		static std::wstring plus_hours(int hours, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_hours(hours, tt, pat);
		}
		static std::wstring plus_hours(int hours, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_hours(hours, tt, pat);
		}
		static std::wstring plus_hours(int hours, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == hours)
				return when(tt, pat);
			auto tt0 = tt;
			tt0 += std::chrono::duration_cast<elf::second_t>(elf::hour_t(hours)).count();
			return when(tt0, pat);
		}
		static std::wstring plus_hours(int hours, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_hours(hours, tt, pat);
		}
		static std::wstring plus_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return plus_hours(hours, tt, pat_a.c_str());
		}


		static std::wstring plus_mins(int minutes, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_mins(minutes, tt, pat);
		}
		static std::wstring plus_mins(int minutes, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_mins(minutes, tt, pat);
		}
		static std::wstring plus_mins(int minutes, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == minutes)
				return when(tt, pat);
			auto tt0 = tt + std::chrono::duration_cast<elf::second_t>(elf::minute_t(minutes)).count();
			return when(tt0, pat);
		}
		static std::wstring plus_mins(int minutes, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_mins(minutes, tt, pat);
		}
		static std::wstring plus_mins(int minutes, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return plus_mins(minutes, tt, pat_a.c_str());
		}

		static std::wstring plus_secs(int secs, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return plus_secs(secs, tt, pat);
		}
		static std::wstring plus_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return plus_secs(secs, tt, pat);
		}
		static std::wstring plus_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			if (0 == secs)
				return when(tt, pat);
			auto tt0 = tt + secs;
			return when(tt0, pat);
		}
		static std::wstring plus_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			return plus_secs(secs, tt, pat);
		}
		static std::wstring plus_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			auto tt = CommonTime::to_tt(t);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return plus_secs(secs, tt, pat_a.c_str());
		}

		static std::wstring tidy(const char * const pat = nullptr)
		{
			auto t = CommonTime::now_t();
			return tidy(t, pat);
		}
		static std::wstring tidy(const struct tm & t, const char * const pat = nullptr)
		{
			auto t0 = t;
			t0.tm_min = 0;
			t0.tm_sec = 0;
			return when(t0, pat);
		}
		static std::wstring tidy(const std::time_t tt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(tt);
			return tidy(t, pat);
		}
		static std::wstring tidy(const std::string &dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return tidy(t, pat);
		}
		static std::wstring tidy(const std::wstring &dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return tidy(t, pat_a.c_str());
		}


		static std::vector<std::wstring> gone_days(int days, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_days(days, tt, pat, keepTidy);
		}
		static std::vector<std::wstring> gone_days(int days, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_days(days, tt, pat, keepTidy);
		}
		static std::vector<std::wstring> gone_days(int days, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_hour = 0;
				t.tm_min = 0;
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = days > 0;
			auto base = order ? days : -days;
			std::vector<std::wstring> v_dt;
			for (int i = 0; i < base; ++i)
			{
				auto dt = plus_days(order ? i - days + 1 : i, tt0, pat);
				v_dt.push_back(dt);
			}
			return v_dt;
		}
		static std::vector<std::wstring> gone_days(int days, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_days(days, t, pat, keepTidy);
		}
		static std::vector<std::wstring> gone_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_days(days, t, pat_a.c_str(), keepTidy);
		}


		static std::vector<std::wstring> gone_hours(int hours, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_hours(hours, tt, pat, keepTidy);
		}
		static std::vector<std::wstring> gone_hours(int hours, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_hours(hours, tt, pat, keepTidy);
		}
		static std::vector<std::wstring> gone_hours(int hours, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_min = 0;
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = hours > 0;
			auto base = order ? hours : -hours;
			std::vector<std::wstring> v_dt;
			for (int i = 0; i < base; ++i)
			{
				auto dt = plus_hours(order ? i - hours + 1 : i, tt0, pat);
				v_dt.push_back(dt);
			}
			return v_dt;
		}
		static std::vector<std::wstring> gone_hours(int hours, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_hours(hours, t, pat, keepTidy);
		}
		static std::vector<std::wstring> gone_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_hours(hours, t, pat_a.c_str(), keepTidy);
		}

		static std::vector<std::wstring> gone_mins(int mins, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::now_tt();
			return gone_mins(mins, tt, pat, keepTidy);
		}
		static std::vector<std::wstring> gone_mins(int mins, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_mins(mins, tt, pat, keepTidy);
		}
		static std::vector<std::wstring> gone_mins(int mins, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto tt0 = tt;
			if (keepTidy) {
				auto t = CommonTime::to_t(tt);
				t.tm_sec = 0;
				tt0 = CommonTime::to_tt(t);
			}
			bool order = mins > 0;
			auto base = order ? mins : -mins;
			std::vector<std::wstring> v_dt;
			for (int i = 0; i < base; ++i)
			{
				auto dt = plus_mins(order ? i - mins + 1 : i, tt0, pat);
				v_dt.push_back(dt);
			}
			return v_dt;
		}
		static std::vector<std::wstring> gone_mins(int mins, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_mins(mins, t, pat, keepTidy);
		}
		static std::vector<std::wstring> gone_mins(int mins, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_mins(mins, t, pat_a.c_str(), keepTidy);
		}
		/**    list              */
		static std::vector<std::wstring> gone_secs(int secs, const char * const pat = nullptr)
		{
			auto tt = CommonTime::now_tt();
			return gone_secs(secs, tt, pat);
		}
		static std::vector<std::wstring> gone_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			auto tt = CommonTime::to_tt(t);
			return gone_secs(secs, pat);
		}
		static std::vector<std::wstring> gone_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			bool order = secs > 0;
			auto base = order ? secs : -secs;
			std::vector<std::wstring> v_dt;
			for (int i = 0; i < base; ++i)
			{
				auto dt = plus_hours(order ? i - secs + 1 : i, tt, pat);
				v_dt.push_back(dt);
			}
			return v_dt;
		}
		static std::vector<std::wstring> gone_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			return gone_secs(secs, t, pat);
		}
		static std::vector<std::wstring> gone_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			auto t = CommonTime::to_t(dt, pat);
			std::string pat_a = nullptr == pat ? DT_PAT_N_A : CommonTime::w_to_a(std::wstring(pat));
			return gone_secs(secs, t, pat_a.c_str());
		}
	};
	/************************************************************************************/


	/************************************************************************************/
	template <class T>
	class time {
		friend class elf::TimeHelper<T>;
		friend class elf::CommonTime;
	private:
		explicit time() = default;
		~time() = default;

	public:
		static T now(const char * const pat = nullptr)
		{
			return TimeHelper<T>::time(pat);
		}

		static T when(const struct tm & t, const char * const pat = nullptr)
		{
			return TimeHelper<T>::when(t, pat);
		}

		static T when(const std::time_t tt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::when(tt, pat);
		}

		static T when(const std::string & dt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::when(dt, pat);
		}

		static T when(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return TimeHelper<T>::when(dt, pat);
		}

		static T min_t(const char * const pat = nullptr)
		{
			return TimeHelper<T>::min_t(pat);
		}
		static T min_t(const struct tm & t, const char * const pat = nullptr)
		{
			return TimeHelper<T>::min_t(t, pat);
		}
		static T min_t(const std::time_t tt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::min_t(tt, pat);
		}
		static T min_t(const std::string & dt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::min_t(dt, pat);
		}
		static T min_t(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return TimeHelper<T>::min_t(dt, pat);
		}

		static T max_t(const char * const pat = nullptr)
		{
			return TimeHelper<T>::max_t(pat);
		}
		static T max_t(const struct tm & t, const char * const pat = nullptr)
		{
			return TimeHelper<T>::max_t(t, pat);
		}
		static T max_t(const std::time_t tt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::max_t(tt, pat);
		}
		static T max_t(const std::string & dt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::max_t(dt, pat);
		}
		static T max_t(const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return TimeHelper<T>::max_t(dt, pat);
		}

		static T plus_days(int days, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_days(days, pat);
		}
		static T plus_days(int days, const struct tm & t, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_days(days, t, pat);
		}
		static T plus_days(int days, const std::time_t tt,  const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_days(days, tt, pat);
		}
		static T plus_days(int days,const std::string & dt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_days(days, dt, pat);
		}
		static T plus_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return TimeHelper<T>::plus_days(days, dt, pat);
		}


		static T plus_hours(int hours, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_hours(hours, pat);
		}
		static T plus_hours(int hours, const struct tm & t, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_hours(hours, t, pat);
		}
		static T plus_hours(int hours, const std::time_t tt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_hours(hours, tt, pat);
		}
		static T plus_hours(int hours, const std::string & dt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_hours(hours, dt, pat);
		}
		static T plus_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return TimeHelper<T>::plus_hours(hours, dt, pat);
		}


		static T plus_mins(int minutes, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_mins(minutes, pat);
		}
		static T plus_mins(int minutes, const struct tm & t, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_mins(minutes, t, pat);
		}
		static T plus_mins(int minutes, const std::time_t tt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_mins(minutes, tt, pat);
		}
		static T plus_mins(int minutes, const std::string & dt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_mins(minutes, dt, pat);
		}
		static T plus_mins(int minutes, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return TimeHelper<T>::plus_mins(minutes, dt, pat);
		}

		static T plus_secs(int secs, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_secs(secs, pat);
		}
		static T plus_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_secs(secs, t, pat);
		}
		static T plus_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_secs(secs, tt, pat);
		}
		static T plus_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::plus_secs(secs, dt, pat);
		}
		static T plus_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return TimeHelper<T>::plus_secs(secs, dt, pat);
		}

		static T tidy(const char * const pat = nullptr)
		{
			return TimeHelper<T>::tidy(pat);
		}
		static T tidy(const struct tm & t, const char * const pat = nullptr)
		{
			return TimeHelper<T>::tidy(t, pat);
		}
		static T tidy(const std::time_t tt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::tidy(tt, pat);
		}
		static T tidy(const std::string &dt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::tidy(dt, pat);
		}
		static T tidy(const std::wstring &dt, const wchar_t * const pat = nullptr)
		{
			return TimeHelper<T>::tidy(dt, pat);
		}


		static std::vector<T> gone_days(int days, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_days(days, pat, keepTidy);
		}
		static std::vector<T> gone_days(int days, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_days(days, t, pat, keepTidy);
		}
		static std::vector<T> gone_days(int days, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_days(days, tt, pat, keepTidy);
		}
		static std::vector<T> gone_days(int days, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_days(days, dt, pat, keepTidy);
		}
		static std::vector<T> gone_days(int days, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_days(days, dt, pat, keepTidy);
		}


		static std::vector<T> gone_hours(int hours, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_hours(hours, pat, keepTidy);
		}
		static std::vector<T> gone_hours(int hours, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_hours(hours, t, pat, keepTidy);
		}
		static std::vector<T> gone_hours(int hours, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_hours(hours, tt, pat, keepTidy);
		}
		static std::vector<T> gone_hours(int hours,const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_hours(hours, dt, pat, keepTidy);
		}
		static std::vector<T> gone_hours(int hours, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_hours(hours, dt, pat, keepTidy);
		}

		static std::vector<T> gone_mins(int mins, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_mins(mins, pat, keepTidy);
		}
		static std::vector<T> gone_mins(int mins, const struct tm & t, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_mins(mins, t, pat, keepTidy);
		}
		static std::vector<T> gone_mins(int mins, const std::time_t tt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_mins(mins, tt, pat, keepTidy);
		}
		static std::vector<T> gone_mins(int mins, const std::string & dt, const char * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_mins(mins, dt, pat, keepTidy);
		}
		static std::vector<T> gone_mins(int mins, const std::wstring & dt, const wchar_t * const pat = nullptr, bool keepTidy = false)
		{
			return TimeHelper<T>::gone_mins(mins, dt, pat, keepTidy);
		}
		/**    list              */
		static std::vector<T> gone_secs(int secs, const char * const pat = nullptr)
		{
			return TimeHelper<T>::gone_secs(secs, pat);
		}
		static std::vector<T> gone_secs(int secs, const struct tm & t, const char * const pat = nullptr)
		{
			return TimeHelper<T>::gone_secs(secs, t, pat);
		}
		static std::vector<T> gone_secs(int secs, const std::time_t tt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::gone_secs(secs, tt, pat);
		}
		static std::vector<T> gone_secs(int secs, const std::string & dt, const char * const pat = nullptr)
		{
			return TimeHelper<T>::gone_secs(secs, dt, pat);
		}
		static std::vector<T> gone_secs(int secs, const std::wstring & dt, const wchar_t * const pat = nullptr)
		{
			return TimeHelper<T>::gone_secs(secs, dt, pat);
		}
	};
    /************************************************************************************/
}
#endif
