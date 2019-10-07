#ifndef _TYPES_H
#define _TYPES_H

//#include <string>
#include <list>
#include "date.h"
//#include <ostream>

using namespace std;

struct DeleteItem {
	template <typename T>
	void operator()(T *ptr) { delete ptr; }
};

// Custom data types

typedef enum { VCALENDAR, VEVENT, VALARM } Component;
typedef enum { DISPLAYSOMETHING=0, PROCEDURE, AUDIO, EMAIL } AlarmAction;

struct WeekMask {
	bool Sunday;
	bool Monday;
	bool Tuesday;
	bool Wednesday;
	bool Thursday;
	bool Friday;
	bool Saturday;
};

struct Recurrence {
	Recurrence(): Freq(YEAR), Interval(0), Count(0) {}
	operator string() const;
	bool IsEmpty() const { return (Interval == 0); }
	void Clear() { Interval = 0; }
	
	TimeUnit Freq;
	unsigned short Interval, Count;
	Date Until;
	WeekMask ByDay = {1,1,1,1,1,1,1};
};

struct AlarmTrigger {
	AlarmTrigger(): Before(true), Value(0), Unit(MINUTE) {}
	AlarmTrigger &operator =(const string &Text);
	operator string() const;
	
	bool Before;
	unsigned short Value;
	TimeUnit Unit;
};

struct Alarm {
	Alarm(): Active(false), Action(DISPLAYSOMETHING) {}
	operator string() const;
	void Clear() {
		Description.clear();
	}
	
	bool Active;
	AlarmAction Action;
	AlarmTrigger Trigger;
	string Description;
};

struct Event {
	Event(): /*Alarms(new list<Alarm>),*/ RecurrenceNo(0), BaseEvent(this) {
		DtEnd = new tm;
		DtStart = new tm;
		DtStamp = new tm;
	}
	Event(const Event &Base):
		UID(Base.UID),
		Summary(Base.Summary),
		Description(Base.Description),
		Categories(Base.Categories),
		DtStamp(Base.DtStamp),
		DtStart(Base.DtStart),
		DtEnd(Base.DtEnd),
		RRule(Base.RRule),
		//Alarms(Base.Alarms),
		RecurrenceNo(Base.RecurrenceNo)
	{
		BaseEvent = Base.BaseEvent == (Event *)&Base ? (Event *)&Base : Base.BaseEvent;
	}
	~Event() {
		//if (BaseEvent == this)
			//delete Alarms;
		delete DtStamp;
		delete DtStart;
		delete DtEnd;
		//Alarms->clear();
		//delete BaseEvent;
	}
	operator string() const;
	bool HasAlarm(const Date &From, const Date &To);

	string UID, Summary, Description, Categories;
	tm *DtStamp, *DtStart, *DtEnd;
	Recurrence RRule;
	//list<Alarm> *Alarms;
	unsigned short RecurrenceNo;
	Event *BaseEvent;


	bool checkActive(tm* t) {
		bool checkYear = true;
		bool checkMonth = true;
		bool checkDay = true;
		bool checkWeekDay = false;
		bool checkHour = true;
		bool checkMinute = true;
		bool checkSecond = true;

		switch (RRule.Freq) {
		case WEEK:
			checkWeekDay = true;
			checkDay = false;
			checkMonth = false;
			checkYear = false;
			break;
		case SECOND:
			checkSecond = false;
		case MINUTE:
			checkMinute = false;
		case HOUR:
			checkHour = false;
		case DAY: //daily
			checkMonth = false;
		case MONTH: //monthly
			checkMonth = false;
		case YEAR: //yearly
			checkYear = false;
			break;
		}
		

		// check that the date is within a valid region
		// eg begin day may be the 27 but the end day is the 3 of the next month
		// if the current month is not the begining or ending month, dont check the day against beginning or ending day
		bool withinBeginningYear = true;
		bool withinEndingYear = true;
		bool withinBeginningMonth = true;
		bool withinEndingMonth = true;
		bool withinBeginningDay = true;
		bool withinEndingDay = true;
		bool withinBeginningHour = true;
		bool withinEndingHour = true;
		bool withinBeginningMinute = true;
		bool withinEndingMinute = true;
		

		if (checkYear) {
			withinBeginningYear = (t->tm_year == DtStart->tm_year);
			withinEndingYear = (t->tm_year == DtEnd->tm_year);
			if (t->tm_year < DtStart->tm_year || t->tm_year > DtEnd->tm_year)
				return false;
		}
		else {
			DtStart->tm_year = t->tm_year;
			DtEnd->tm_year = t->tm_year;
		}

		if (checkMonth) {
			withinBeginningMonth = withinBeginningYear && (t->tm_mon == DtStart->tm_mon);
			withinEndingMonth = withinEndingYear && (t->tm_mon == DtEnd->tm_mon);
			if (( withinBeginningYear && (t->tm_mon < DtStart->tm_mon)) || (withinEndingYear && (t->tm_mon > DtEnd->tm_mon)))
				return false;
		}
		else {
			DtStart->tm_mon = t->tm_mon;
			DtEnd->tm_mon = t->tm_mon;
		}

		if (checkDay) {
			withinBeginningDay = withinBeginningMonth && (t->tm_mday == DtStart->tm_mday);
			withinEndingDay = withinEndingMonth && (t->tm_mday == DtEnd->tm_mday);
			if ((withinBeginningMonth && (t->tm_mday < DtStart->tm_mday)) || (withinEndingMonth && (t->tm_mday > DtEnd->tm_mday)))
				return false;
		}
		else {
			DtStart->tm_mday = t->tm_mday;
			DtEnd->tm_mday = t->tm_mday;
		}

		if (checkHour) {
			withinBeginningHour = withinBeginningDay && (t->tm_hour == DtStart->tm_hour);
			withinEndingHour = withinEndingDay && (t->tm_hour == DtEnd->tm_hour);
			if((withinBeginningDay && (t->tm_hour < DtStart->tm_hour)) || (withinEndingDay && (t->tm_hour > DtEnd->tm_hour)))
			return false;
		}
		else {
			DtStart->tm_hour = t->tm_hour;
			DtEnd->tm_hour = t->tm_hour;
		}

		if (checkMinute) {
			withinBeginningMinute = withinBeginningHour && (t->tm_min == DtStart->tm_min);
			withinEndingMinute = withinEndingHour && (t->tm_min == DtEnd->tm_min);
			if((withinBeginningHour && (t->tm_min < DtStart->tm_min)) || (withinEndingHour && (t->tm_min > DtEnd->tm_min)))
			return false;
		}
		else {
			DtStart->tm_min = t->tm_min;
			DtEnd->tm_min = t->tm_min;
		}
		if (checkSecond) {
			if((withinEndingMinute && (t->tm_sec < DtStart->tm_sec)) || (withinEndingMinute && (t->tm_sec > DtEnd->tm_sec)))
			return false;
		}
		else {
			DtStart->tm_sec = t->tm_sec;
			DtEnd->tm_sec = t->tm_sec;
		}
		
		if (checkWeekDay) {
			if (t->tm_wday == 0 && !RRule.ByDay.Sunday)
				return false;
			if (t->tm_wday == 1 && !RRule.ByDay.Monday)
				return false;
			if (t->tm_wday == 2 && !RRule.ByDay.Tuesday)
				return false;
			if (t->tm_wday == 3 && !RRule.ByDay.Wednesday)
				return false;
			if (t->tm_wday == 4 && !RRule.ByDay.Thursday)
				return false;
			if (t->tm_wday == 5 && !RRule.ByDay.Friday)
				return false;
			if (t->tm_wday == 6 && !RRule.ByDay.Saturday)
				return false;
		}
		return true;
	}
};

struct EventsCriteria {
	EventsCriteria(): AllEvents(false), IncludeRecurrent(true) {}
	
	Date From, To;
	bool AllEvents, IncludeRecurrent;
};

/*inline ostream &operator <<(ostream &stream, const Recurrence &RRule) {
	stream << RRule.operator string();
	return stream;
}

inline ostream &operator <<(ostream &stream, const Alarm &Alarm) {
	stream << Alarm.operator string();
	return stream;
}

inline ostream &operator <<(ostream &stream, const Event &Event) {
	stream << Event.operator string();
	return stream;
}*/

#endif // _TYPES_H
