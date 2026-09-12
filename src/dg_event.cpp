/*
** dg_event.c: This file contains a simplified event system to allow
** DG Script triggers to use the "wait" command, causing a delay in the
** middle of a script.
**
** NOTE: This is NOT the dg events package that is available!
**
** By: Mark A. Heilpern (Sammy @ eQuoria MUD   equoria.com:4000)
*/
#include "conf.h"
#include "utils.h"
#include "dg_event.h"


/*
** define statics
*/
event_info *event_list = nullptr;

event_info::event_info()
{
	this->time_remaining = 0;
	this->func = nullptr;
	this->info = nullptr;
	this->next = nullptr;
}
event_info::~event_info()
{
}

/*
** Add an event to the current list
*/
event_info *add_event(int time, EVENT(*func), void *info)
{
	event_info *this_data, *prev, *curr;

	CREATE(this_data, event_info, 1);
	this_data->time_remaining = time;
	this_data->func = func;
	this_data->info = info;

	/* sort the event into the list in next-to-fire order */
	if (event_list==nullptr)
		event_list = this_data;

	else if (this_data->time_remaining <= event_list->time_remaining)
	{
		this_data->next = event_list;
		event_list = this_data;
	}

	else
	{
		prev = event_list;
		curr = prev->next;

		while (curr && (curr->time_remaining > this_data->time_remaining))
		{
			prev = curr;
			curr = curr->next;
		}

		this_data->next = curr;
		prev->next = this_data;
	}

	return this_data;
}

void remove_event(event_info *event)
{
	event_info *curr;

	if (event_list == event)
	{
		event_list = event->next;
	}

	else
	{
		curr = event_list;
		while (curr && (curr->next != event))
			curr = curr->next;

		if (!curr)
			return; /* failed to find it */

		curr->next = curr->next->next;
	}

	delete (event);
}

void process_events()
{
	event_info *e = event_list;
	event_info *del;

	while (e)
	{
		if (--(e->time_remaining) <= 0)
		{
			e->func(e->info);
			del = e;
			e = e->next;
			remove_event(del);
		}
		else
			e = e->next;
	}
}


















