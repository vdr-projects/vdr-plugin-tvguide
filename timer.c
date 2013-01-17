#include "timer.h"

cMyTime::~cMyTime(void) {
}

void cMyTime::Now() {	
	t = time(0);
	tStart = t;
	tStart = GetRounded();
	tStop = tStart + (tvguideConfig.osdHeight - tvguideConfig.headerHeight - tvguideConfig.footerHeight)/tvguideConfig.minuteHeight*60;
}

void cMyTime::AddStep(int step) {
	tStart += step*60;
	tStop  += step*60;
}

bool cMyTime::DelStep(int step) {
	if ((tStart - step*60)+30*60 < t) {
		return true;
	}
	tStart -= step*60;
	tStop -= step*60;
	return false;
}

void cMyTime::SetTime(time_t newTime) {
	tStart = newTime;
	tStop = tStart + (tvguideConfig.osdHeight - tvguideConfig.headerHeight - tvguideConfig.footerHeight)/tvguideConfig.minuteHeight*60;
}

time_t cMyTime::getPrevPrimetime(time_t current) {
	tm *st = localtime(&current);
	if (st->tm_hour < 21) {
		current -= 24 * 60* 60;
		st = localtime(&current);
	}
	st->tm_hour = 20;
	st->tm_min = 0;
	time_t primeTime = mktime(st);
	return primeTime;
}

time_t cMyTime::getNextPrimetime(time_t current){
	tm *st = localtime(&current);
	if (st->tm_hour > 19) {
		current += 24 * 60* 60;
		st = localtime(&current);
	}
	st->tm_hour = 20;
	st->tm_min = 0;
	time_t primeTime = mktime(st);
	return primeTime;
}

bool cMyTime::tooFarInPast(time_t current) {
	if (current < t) {
		return true;
	}
	return false;
}

cString cMyTime::GetCurrentTime() {
	char buf[25];
	t = time(0);
	tm *st = localtime(&t);
	//snprintf(text, sizeof(text), "%d:%02d", st->tm_hour, st->tm_min);
	if (tvguideConfig.timeFormat == e12Hours) {
		strftime(buf, sizeof(buf), "%I:%M %p", st);
	} else if (tvguideConfig.timeFormat == e24Hours)
		strftime(buf, sizeof(buf), "%H:%M", st);
	return buf;
	
}

cString cMyTime::GetDate() {
	char text[6];
	tm *st = localtime(&tStart);
	snprintf(text, sizeof(text), "%d.%d", st->tm_mday, st->tm_mon+1);
	return text;
}

cString cMyTime::GetWeekday() {
	return WeekDayName(tStart);
}

int cMyTime::GetTimelineOffset() {
	tm *st = localtime(&tStart);
	int offset = st->tm_hour*60;
	offset += st->tm_min;
	return offset;
}

time_t cMyTime::GetRounded() {
	tm *rounded = localtime ( &tStart );
	rounded->tm_sec = 0;
	if (rounded->tm_min > 29)
		rounded->tm_min = 30;
	else
		rounded->tm_min = 0;
	return mktime(rounded);
}

void cMyTime::debug() {
	esyslog("t: %s, tStart: %s, tStop: %s", *TimeString(t), *TimeString(tStart), *TimeString(tStop));
}
