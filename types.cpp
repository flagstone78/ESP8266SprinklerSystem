#include "types.h"

Recurrence::operator string() const {
	char Temp[5];
	string Text = "FREQ=";
	switch (Freq) {
		case YEAR: Text += "YEARLY"; break;
		case MONTH: Text += "MONTHLY"; break;
		case DAY: Text += "DAILY"; break;
		case HOUR: Text += "HOURLY"; break;
		case MINUTE: Text += "MINUTELY"; break;
		case SECOND: Text += "SECONDLY"; break;
		case WEEK: Text += "WEEKLY"; break;
	}
	Text += ";INTERVAL=";
	printf(Temp, "%d", Interval);
	Text += Temp;
	if (Count > 0) {
		Text += ";COUNT=";
		printf(Temp, "%d", Count);
		Text += Temp;
	} else if (!Until.IsEmpty()) {
		Text += ";UNTIL=";
		Text += Until;
	}

	string bydaystring = ";BYDAY=";
	int count = 0;
	
	if (ByDay.Sunday) {
		bydaystring += "SU";
		count += 1;
	}
	if (ByDay.Monday) {
		if (count > 0) {bydaystring += ",";}
		bydaystring += "MO";
		count += 1;
	}
	if (ByDay.Tuesday) {
		if (count > 0) { bydaystring += ","; }
		bydaystring += "TU";
		count += 1;
	}
	if (ByDay.Wednesday) {
		if (count > 0) { bydaystring += ","; }
		bydaystring += "WE";
		count += 1;
	}
	if (ByDay.Thursday) {
		if (count > 0) { bydaystring += ","; }
		bydaystring += "TH";
		count += 1;
	}
	if (ByDay.Friday) {
		if (count > 0) { bydaystring += ","; }
		bydaystring += "FR";
		count += 1;
	}
	if (ByDay.Saturday) {
		if (count > 0) { bydaystring += ","; }
		bydaystring += "SA";
		count += 1;
	}
	
	if (count > 0) Text += bydaystring;

	
	return Text;
}

AlarmTrigger &AlarmTrigger::operator =(const string &Text) {
	char UnitChar;
	// 1 because always at least 'P' before the value
	short i = 1;
	
	if (Text[0] == '-')
		Before = true;
	
	while (!isdigit(Text[i]))
		++i;
	
	sscanf(Text.c_str()+i, "%hd%c", (short *)&Value, &UnitChar);
	
	switch (UnitChar) {
		case 'H': Unit = HOUR; break;
		case 'D': Unit = DAY; break;
		case 'W': Unit = WEEK; break;
		default: Unit = MINUTE; break;
	}
	
	return *this;
}

AlarmTrigger::operator string() const {
	string Text;
	char Temp[6];
	
	printf(Temp, "%d", Value);
	
	if (Before)
		Text = '-';
	Text += 'P';
	if (Unit != WEEK && Unit != DAY)
		Text += 'T';
	Text += Temp;
	switch (Unit) {
		case HOUR: Text += 'H'; break;
		case DAY: Text += 'D'; break;
		case WEEK: Text += 'W'; break;
		default: Text += 'M'; break;
	}
	
	return Text;
}

Alarm::operator string() const {
	string Text = "\r\nBEGIN:VALARM";
	Text += "\r\nTRIGGER:";
	Text += Trigger;
	Text += "\r\nACTION:";
	switch (Action) {
		case AUDIO: Text += "AUDIO"; break;
		case DISPLAYSOMETHING: Text += "DISPLAYSOMETHING"; break;
		case PROCEDURE: Text += "PROCEDURE"; break;
		case EMAIL: Text += "EMAIL"; break;
	}
	if (!Description.empty()) {
		Text += "\r\nDESCRIPTION:";
		Text += Description;
	}
	Text += "\r\nEND:VALARM";
	
	return Text;
}

Event::operator string() const {
	string Text = "BEGIN:VEVENT";
	Text += "\r\nUID:";
	Text += UID;
	Text += "\r\nSUMMARY:";
	
	Text += Summary;
	// some programs do not add this property, let's leave it when modifying
	//if (!DtStamp.IsEmpty()) {
		Text += "\r\nDTSTAMP:";
		Text += asctime(DtStamp);//DtStamp;
	//}
	Text += "\r\nDTSTART:";
	Text += asctime(DtStart);// DtStart;
	//if (!DtEnd.IsEmpty()) {
		Text += "\r\nDTEND:";
		Text += asctime(DtEnd); // DtEnd;
	//}
	if (!Description.empty()) {
		Text += "\r\nDESCRIPTION:";
		Text += Description;
	}
	if (!Categories.empty()) {
		Text += "\r\nCATEGORIES:";
		Text += Categories;
	}
	if (!RRule.IsEmpty()) {
		Text += "\r\nRRULE:";
		Text += RRule;
	}
	
	/*if (!Alarms->empty()) {
		for (list<Alarm>::const_iterator AlarmsIterator = Alarms->begin(); AlarmsIterator != Alarms->end(); ++AlarmsIterator) {
			Text += *AlarmsIterator;
		}
	}*/
	
	Text += "\r\nEND:VEVENT\r\n";
	
	return Text;
}

bool Event::HasAlarm(const Date &From, const Date &To) {
	/*if (Alarms->empty())
		return false;
	
	bool HasAlarm = false;
	Date AlarmDate;
	
	for (list<Alarm>::iterator AlarmsIterator = Alarms->begin(); AlarmsIterator != Alarms->end(); ++AlarmsIterator) {
		//AlarmDate = DtStart;
		
		if ((*AlarmsIterator).Trigger.Before)
			AlarmDate[(*AlarmsIterator).Trigger.Unit] -= (*AlarmsIterator).Trigger.Value;
		else
			AlarmDate[(*AlarmsIterator).Trigger.Unit] += (*AlarmsIterator).Trigger.Value;
		
		if (AlarmDate >= From && AlarmDate <= To) {
			HasAlarm = true;
			(*AlarmsIterator).Active = true;
		} else {
			(*AlarmsIterator).Active = false;
		}
	}
	
	return HasAlarm;*/
	return false;
}
