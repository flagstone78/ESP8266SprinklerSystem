#ifndef _ICALENDAR_H
#define _ICALENDAR_H

#include <fstream>
//#include <algorithm>
#include <cstdlib>
//#include <ctime>
#include <cstring>
#include "types.h"
//#include <ostream>
//#include<iomanip>

using namespace std;

class ICalendar {
public:
	ICalendar(const char *FileName): FileName((char *)FileName) { /*LoadFromFile();*/ }
	~ICalendar() {
		//delete all events
		Events.clear();
	}
	void LoadFromFile(string Line);
	Component CurrentComponent = VCALENDAR, PrevComponent = VCALENDAR;
	Event* NewEvent = NULL;
	Alarm NewAlarm;
	// for getting some UIDs for events without them
	unsigned int NoUID = 0;

	//Event* GetEventByUID(char *UID);

	//void AddEvent(Event *NewEvent);
	void DeleteEvent(Event *DeletedEvent);
	void ModifyEvent(Event *ModifiedEvent);

	class Query;
	list<Event*> Events;
	
private:
	 void GetTMFromString(tm* t, const string& Line) const {
		short y, m, d, h, min, s, i;
		y = m = d = h = min = s = i = 0;
		
		if (Line.length() >= 8) {
			i=sscanf(Line.c_str(), "%4hd%2hd%2hd", &y, &m, &d);
			y -= 1900;
			m -= 1;

			if (Line.length() >= 15) {
				i=sscanf(Line.c_str() + 9, "%2hd%2hd%2hd", &h, &min, &s);
			}


			tm t2 = { s,min,h,d,m,y };

			time_t tlong = mktime(&t2); //fills in missing and constrains to valid numbers

			//Z at end indicates zulu time. Must add time zone to get into local time
			if (Line.length() >= 16) {
				tlong += -7 * 3600; //TODO look up time zone Micro seems one hour behind local time?
				t2 = *localtime(&tlong);

			}
			*t = t2;
		}
	}

	string GetProperty(const string &Line) const {
		// if VALUE=DATE or VALUE=DATE-TIME used, then the date is not directly
		// after the property name so we just search for the string after ':'
		string Temp = Line.substr(Line.find_first_of(':')+1);
		unsigned int Length = Temp.length();
		if (Length > 0 && Temp[Length-1] == '\r')
			Temp.resize(Length-1);
		return Temp;
	}
	string GetSubProperty(const string &Line, const char *SubProperty) const {
		size_t Pos = Line.find(SubProperty);
		if (Pos == string::npos)
			return "";
		Pos += strlen(SubProperty) + 1;
		return Line.substr(Pos, Line.find_first_of(";\r", Pos)-Pos);
	}
	void FixLineEnd(string &Line, unsigned int Length) {
		if (Length > 0 && Line[Length-1] == '\r')
			Line.resize(Length-1);
		Line += "\r\n";
	}
	
	char *FileName;
};

/*class ICalendar::Query {
public:
	Query(ICalendar *Calendar): Calendar(Calendar), EventsIterator(Calendar->Events.begin()) {}
	~Query() { for_each(RecurrentEvents.begin(), RecurrentEvents.end(), DeleteItem()); }
	void ResetPosition() {
	  	for_each(RecurrentEvents.begin(), RecurrentEvents.end(), DeleteItem());
	  	RecurrentEvents.clear();
	  	EventsIterator = Calendar->Events.begin();
	}
	//Event* GetNextEvent(bool WithAlarm = false);

	
	EventsCriteria Criteria;
	
private:
	ICalendar *Calendar;
	list<Event *> RecurrentEvents;
	list<Event *>::iterator EventsIterator;
};*/

inline TimeUnit ConvertFrequency(string Name) {
	if (Name == "SECONDLY")
		return SECOND;
	if (Name == "MINUTELY")
		return MINUTE;
	if (Name == "HOURLY")
		return HOUR;
	if (Name == "DAILY")
		return DAY;
	if (Name == "WEEKLY")
		return WEEK;
	if (Name == "MONTHLY")
		return MONTH;
	
	return YEAR;
}

inline WeekMask ConvertByDayList(string bydaylist) {
	WeekMask w = { 0,0,0,0,0,0,0 };

	if (bydaylist.length() > 1) {
		int current = 0;
		char day[3];

		while (current < bydaylist.length()) {
			//get day
			day[0] = bydaylist[current];
			day[1] = bydaylist[current+1];
			day[2] = 0;

			//set mask
			if (strcmp(day, "SU") == 0) {
				w.Sunday = true;
			} 
			else if (strcmp(day, "MO") == 0) {
				w.Monday = true;
			}
			else if(strcmp(day, "TU") == 0) {
				w.Tuesday = true;
			}
			else if(strcmp(day, "WE") == 0) {
				w.Wednesday = true;
			}
			else if(strcmp(day, "TH") == 0) {
				w.Thursday = true;
			}
			else if(strcmp(day, "FR") == 0) {
				w.Friday = true;
			}
			else if(strcmp(day, "SA") == 0) {
				w.Saturday = true;
			}

			current += 3;
		}
	}

	return w;
}

inline AlarmAction ConvertAlarmAction(string Name) {
	if (Name == "AUDIO")
		return AUDIO;
	if (Name == "PROCEDURE")
		return PROCEDURE;
	if (Name == "EMAIL")
		return EMAIL;
	
	return DISPLAYSOMETHING;
}

#endif // _ICALENDAR_H
