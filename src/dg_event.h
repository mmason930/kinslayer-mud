/**
 * @file dg_event.h
 * @author
 * @brief 
 * @version
 * @date
 * 
 * @copyright Copyright (c) 2022
 * 
 * This file was originally part of the Death Gate scripts system.
 * We have since removed the system, but we used DG events in
 * various parts of the game, so we've kept this portion.
 * 
 */

/*
** how often will heartbeat() call our event function?
*/
const int PULSE_DG_EVENT = 1; 

/*
** macro used to prototype the callback function for an event
*/
#define EVENT(function) void (function)(void *info)

/*
** define event related structures
*/
class event_info
{
public:
	int time_remaining;
	EVENT(*func);
	void *info;
	class event_info *next;

	event_info();
	~event_info();
};

/*
** prototype event functions
*/
class event_info *add_event(int time, EVENT(*func), void *info);
void remove_event(class event_info *event);
void process_events(void);


















