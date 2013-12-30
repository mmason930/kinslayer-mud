// weather.cpp
// This is the weather system for the mud. It is written so that 
// each zone has its own weather which should change and regulate
// itself mostly independant of other zones -- mostly.
// Each of the three main gauges on the weather system
// -- temperature, precipitation, and intensity (of storms) --
// should vary in a semi-realistic fashion, and regulate
// themselves so they don't go crazy.
// But this is where it get interesting -- we have a few weather-
// effecting weaves (spells), so we have to prepare to be screwed
// with. This is where the mostly independant part comes in.
//
// When the weather is changed by a weave, it attempts to
// keep itself sane with its given resources, but if it can't,
// it has the possibility of screwing with another zone's
// weather. This simulates people messing with things they
// should not be messing with. Sometimes it just screws with the
// precipitation or temperature a bit, but it occasionally
// creates 'events'. These are things like droughts, floods,
// heatwaves, and cold snaps. They make the weather of the
// affected zones Act wierd for a while.
//
// We then have the storm system. There are a variety of storm
// types, with more being easy to create. Each of them give
// different messages, and optionally can have different affects.
// Which one is created is decided by polling the conditions of
// the weather -- a snowstorm will probabily be created in a
// cold, wet enviroment, while it would rain in a wet and
// warm one. Storms' intensity follow the amount of water
// available. The storms depend on a certain amount of
// precipitation being available, but they also expend
// precipitation every tic, making
// them short-circuit themselves after a while if left alone. The
// intensity of the storm decides what effects the storm has on
// the zone (emotes, damage, reduced visibility, etc).

#include "conf.h"
#include "sysdep.h"


#include "structs.h"
#include "utils.h"
#include "comm.h"
#include "handler.h"
#include "interpreter.h"
#include "db.h"
#include "weather.h"
#include "zones.h"

extern struct GameTime time_info;

extern Character *character_list;
void another_hour(int mode);

// Sun state
Sun Weather::sunlight;

void sendToOutdoor(int zone, const char * messg)
{
	for ( Character * ch = character_list; ch != 0 ; ch = ch->next)
	{
		if( ch->IsPurged() ) continue;
		if (OUTSIDE(ch))
			if (ch->in_room->zone == zone)
				ch->send(messg);
	}	
}

void weather_and_time(int mode)
{
	another_hour(mode);
	Zone *zone;
//	for (int i = 0 ;(zone=ZoneManager::GetManager().GetZoneByRnum(i)) != NULL; ++i )
//	{
//		if( zone->weather ) zone->weather->change();
//	}
}

void another_hour(int mode)
{
	Character *ch;
	++time_info.hours;

	if (mode)
	{
		switch (time_info.hours)
		{
			case 5:
				for(ch = character_list;ch;ch = ch->next)
				{
					if( ch->IsPurged() ) continue;
					if( !ch->in_room->GetZone() )
						MudLog(CMP, LVL_GOD, TRUE, "%s has an invalid zone!", GET_NAME(ch));
					else
					{
						Weather *MyWeather = ch->in_room->GetZone()->GetWeather();
						MyWeather->set_sun(SUN_RISE);
						if(OUTSIDE(ch) && AWAKE(ch))
						{
							Zone *zone = ch->in_room->GetZone();
							if(zone->GetSunrise().empty())
								ch->send("The sun rises from the east.\r\n");
							else
								ch->send("%s\r\n", zone->GetSunrise().c_str());
						}
					}
				}
				break;

			case 6:
				for (int i = 0 ; i < ZoneManager::GetManager().NumZones() ; ++i )
				{
					sendToOutdoor(ZoneManager::GetManager().GetZoneByRnum(i)->getVnum(), "The day has begun.\r\n");
				}
				break;

			case 21:
				for(ch = character_list;ch;ch = ch->next)
				{
					if( ch->IsPurged() ) continue;
					Zone *zone = ch->in_room->GetZone();
					if( (zone) == NULL )
					{
						MudLog(CMP, TRUE, LVL_GOD, "%s has an invalid zone!", GET_NAME(ch));
					}
					else
					{
						zone->GetWeather()->set_sun(SUN_SET);
						if(OUTSIDE(ch) && AWAKE(ch))
						{
							if(zone->GetSunset().empty())
								ch->send("The sun sets in the west.\r\n");
							else
								ch->send("%s\r\n", zone->GetSunset().c_str());
						}
					}
				}
				break;

			case 22:
			{
				Zone *zone;
				for (int i = 0 ;(zone = ZoneManager::GetManager().GetZoneByRnum(i)) != NULL; ++i )
				{
					zone->GetWeather()->set_sun(SUN_DARK);
					sendToOutdoor(zone->getVnum(), "The night has begun.\r\n");
				}
				break;
			}

			default:
				break;

		}
	}

	if (time_info.hours > 23)
	{	/* Changed by HHS due to bug ??? */
		time_info.hours -= 24;
		++time_info.day;

		if (time_info.day > 34)
		{
			time_info.day = 0;
			time_info.month++;

			if (time_info.month > 16)
			{
				time_info.month = 0;
				time_info.year++;
			}
		}
	}
}

void Weather::change(void)
{
	this->change_temp();
	this->change_precip();
	this->change_intensity();
	this->change_event();
	if ((MiscUtil::random(1, 50) == 7) && this->event->event_type == WE_NONE)
		this->add_event();
	this->change_storms();
	this->change_side_effects();
}

void Weather::change_side_effects()
{}

void Storm::reset()
{
	event_type = WE_NONE;
	intensity = 0;
	type = ST_NONE;
	fake = false;
	lifespan = 0;
	tics_left = 0;
	zing = false;
}

void Weather::add_storm()
{
}

void Weather::change_temp()
{
	sh_int diff = 0;
	sh_int variance = 0;

	// In general, we want to go towards 0.
	
	if (temperature > 75)
		variance = -3;
	else if (temperature > 50)
		variance = -2;
	else if (temperature > 25)
		variance = -1;
	else if (temperature < -75)
		variance = 3;
	else if (temperature < -50)
		variance = 2;
	else if (temperature < -25)
		variance = 1;
	else
		variance = 0;

	diff = (MiscUtil::dice(1, 3) * (variance + MiscUtil::dice(-1, 1)));

	// It is warmer during the day then at night.
	if (sunlight == SUN_DARK)
		diff -= 1;
	else if (sunlight == SUN_LIGHT)
		diff += 1;

	// We are colder in winter, and warmer in summer.
	if ((time_info.month >= 9) && (time_info.month <= 16))
		diff -= 2;
	else
		diff += 2;

	switch (event->type)
	{
		case WE_NONE:
			break;
		case COLD_SNAP:
			diff -= 4;
			break;
		case HEAT_WAVE:
			diff -= 4;
			break;
		default:
			break;
	}
	


	base_temperature += diff;
	temperature =+ diff ;

	base_temperature = MAX(sh_int(0), base_temperature);
	base_temperature = MIN(sh_int(100), base_temperature);

	temperature = MAX(sh_int(0), temperature);
	temperature = MIN(sh_int(100), temperature);

}

void Weather::change_precip()
{
	int p = precipitation;

	// Here we are doing some handling for special weather events.
	switch (event->event_type)
	{
		case DROUGHT:
			p += 0;
			break;
		case FLOOD:
			p += 15; // That's a lot of rain!
			break;
		case WE_NONE:
		case HEAT_WAVE:
		case COLD_SNAP:
		default:
			break;
	}		

	
	// Here we are getting rid of built up precipitation when it storms.
	// Since we do this every tic, and storms depend on precipitation,
	// we will naturally kill storms after a certain amount of tics.
	switch (storm->type)
	{
		case ST_NONE:
			if (event->event_type != WE_NONE)
				p += 5;
			break; 
		case DRIZZLE:
			p -= 2;
			break;
		case RAIN:
			p -= 4;
			break;
		case FREEZING_DRIZZLE:
			p -= 1;
			break;
		case FREEZING_RAIN:
			p -= 3;
			break;
		case SNOW:
			p -= 4;
			break;
		case SNOW_PELLETS:
			p -= 3;
			break;
		case SNOW_GRAINS:
			p -= 2;
			break;
		case ICE_PELLETS:
			p -= 4;
			break;
		case HAIL: // We want hail storms to stop fairly quickly.
			p -= 7;
			break;
		case BLIZZARD:
			p -= 6;
			break;
		case FOG:
			p -= 2;
			break;
		default:
			break;
	}

	int diff = precipitation - base_precipitation;
	
	p = MAX(0, p);
	p = MIN(100, p);
	base_precipitation = p;
	precipitation = p + diff;
}

void Weather::change_intensity()
{
	storm->change_intensity(this);
}

void Storm::change_intensity(Weather * weather)
{
	if (fake)
		if (tics_left > 0)
			return; // We leave the storm alone if its fake.

	if (type == ST_NONE)
		return; // Nothing to see here, move along.

	if (weather->precipitation > intensity)
		intensity += MiscUtil::random(-3, 13);
	else
		intensity += MiscUtil::random(-13, 3);

	if (intensity > 65 && zing == false)
	{
		zing = true;
		sendToOutdoor(weather->zone, "The sky begins to light up with flashes of lightning.\r\n");
	}
	else if (intensity < 65 && zing == true)
	{
		zing = false;
		sendToOutdoor(weather->zone, "The sky is no longer lit by flashes of lightning.\r\n");
	}
}
	
void Weather::change_sky()
{
	switch (storm->type)
	{
		case ST_NONE:
			if (precipitation > 50)
				sky = SKY_CLOUDY;
			else if (precipitation < 50)
				sky = SKY_CLOUDLESS;
			break;
		case DRIZZLE:
		case RAIN:
		case FREEZING_DRIZZLE:
		case FREEZING_RAIN:
			sky = SKY_RAINING;
			break;

		case SNOW:
		case SNOW_PELLETS:
		case SNOW_GRAINS:
			sky = SKY_SNOWING;
			break;

		case HAIL:
		case ICE_PELLETS:
			sky = SKY_HAILING;
			break;

		case BLIZZARD:
			sky = SKY_BLIZZARD;
			break;
		case FOG:
			sky = SKY_FOGGY;
			break;
	}
}


void Storm::messege(Weather * weather, bool start) const
{
	switch (weather->storm->type)
	{
		case ST_NONE:
			if (weather->precipitation > 50)
				sendToOutdoor(weather->zone, "The sky starts to get cloudy.\r\n");
			else if (weather->precipitation < 50)
				sendToOutdoor(weather->zone, "The clouds disappear.\r\n");
			break;

		case RAIN:
			if (start)
				sendToOutdoor(weather->zone, "It starts to rain.\r\n");
			else
				sendToOutdoor(weather->zone, "The rain stops.\r\n");
			break;

		case DRIZZLE:
			if (start)
				sendToOutdoor(weather->zone, "It starts to drizzle.\r\n");
			else
				sendToOutdoor(weather->zone, "The drizzling rain fades away.\r\n");
			break;

		case FREEZING_DRIZZLE:
			if (start)
				sendToOutdoor(weather->zone, "A freezing drizzle begins to fall.\r\n");
			else
				sendToOutdoor(weather->zone, "The freezing drizzle is suddenly gone.\r\n");
			break;

		case FREEZING_RAIN:
			if (start)
				sendToOutdoor(weather->zone, "A freezing rain beings to fall all around you, coating the world in ice.\r\n");
			else
				sendToOutdoor(weather->zone, "The dreary freezing rain stops.\r\n");
			break;

		case SNOW:
			if (start)
				sendToOutdoor(weather->zone, "Large flakes of snow begin to fall all around you.\r\n");
			else
				sendToOutdoor(weather->zone, "The snow ceases to fall.\r\n");
			break;
			
		case SNOW_GRAINS:
			if (start)
				sendToOutdoor(weather->zone, "A thin, powdery snow begins to cover the wolrd in white.\r\n");
			else
				sendToOutdoor(weather->zone, "The powerdy snowfall slowly fades away.\r\n");
			break;
			
		case HAIL:
			if (start)
				sendToOutdoor(weather->zone, "You start to hear dings and bangs all around you as a cloud of hail begins to fall.\r\n");
			else
				sendToOutdoor(weather->zone, "The dings and bangs cease as the hail ceases to fall.\r\n");
			break;

		case BLIZZARD:
			if (start)
				sendToOutdoor(weather->zone, "With a great howl, a mighty blizzard has covered the sky.\r\n");
			else
				sendToOutdoor(weather->zone, "The last of its fury gone, the blizzard blows away.\r\n");
			break;
		default:
			break;
	}
}

void Weather::force_change_precip(bool wet, int percent_known)
{
	float p = float(precipitation);
	float k = percent_known / 100.0f + .4;
	
	if (!wet)
	{
		if (p < 20 && p > 0)
			p -= 5 * k;
		else if (p < 40)
			p -= 6 * k;
		else if (p < 60)
			p -= 8 * k;
		else if (p < 80)
			p -= 10 * k;
		else if (p < 100)
			p -= 20 * k;
	}	
	else
	{
		if (p < 20 && p > 0)
			p += 15 * k;
		else if (p < 40)
			p += 10 * k;
		else if (p < 60)
			p += 8 * k;
		else if (p < 80)
			p += 7 * k;
		else if (p < 100)
			p += 6 * k;
	}
	
	// To make sure we didn't go out of range.
	p = MIN(int(p), 100);
	p = MAX(int(p), 0);

	precipitation = int(p);
	p_modified = true;

	change_storms();
}

void Weather::force_change_temp(bool hot, int percent_known)
{
	float p = float(temperature);
	float k = (float)percent_known / 100.0f + .4f;
	
	if (!hot)
	{
		if (p < -100)
			p -= 1 * k;
		else if (p < -80)
			p -= 2 * k;
		else if (p < -60)
			p -= 4 * k;
		else if (p < -40)
			p -= 5 * k;
		else if (p < -20)
			p -= 6 * k;
		else if (p < 0)
			p -= 7 * k;
		else if (p < 20)
			p -= 9 * k;
		else if (p < 40)
			p -= 15 * k;
		else if (p < 60)
			p -= 20 * k;
		else if (p < 80)
			p -= 30 * k;
		else if (p < 100)
			p -= 35 * k;
	}
	else
	{
		if (p < 100)
			p += 1 * k;
		else if (p < 80)
			p += 2 * k;
		else if (p < 60)
			p += 4 * k;
		else if (p < 40)
			p += 5 * k;
		else if (p < 20)
			p += 6 * k;
		else if (p < 0)
			p += 7 * k;
		else if (p < -20)
			p += 9 * k;
		else if (p < -40)
			p += 15 * k;
		else if (p < -60)
			p += 20 * k;
		else if (p < -80)
			p += 30 * k;
		else if (p < -100)
			p += 35 * k;
	}
	
	// To make sure we didn't go out of range.
	p = MIN(p, 100.0f);
	p = MAX(p, -100.0f);

	temperature = int(p);
	t_modified = true;
	
	change_storms();
}

void Weather::force_change_intensity(bool worse, int percent_known)
{
	float p = storm->intensity;
	float k = percent_known / 100.0f + .4;

	if (storm->type == ST_NONE)
	{
		return;
	}
	
	if (!worse)
	{
		if (p < 20 && p > 0)
			p -= 5 * k;
		else if (p < 40)
			p -= 6 * k;
		else if (p < 60)
			p -= 8 * k;
		else if (p < 80)
			p -= 10 * k;
		else if (p < 100)
			p -= 20 * k;
	}
	else
	{
		if (p < 20 && p > 0)
			p += 15 * k;
		else if (p < 40)
			p += 10 * k;
		else if (p < 60)
			p += 8 * k;
		else if (p < 80)
			p += 7 * k;
		else if (p < 100)
			p += 6 * k;
	}
	
	// To make sure we didn't go out of range.
	p = MIN(int(p), 100);
	p = MAX(int(p), 0);

	storm->intensity = int(p);
	i_modified = true;
	
	change_storms();
}

void Weather::change_event()
{
	if (event->event_type != WE_NONE)
	{
		--event->tics_left;
		if (event->tics_left == 0)
		{
			// Resetting to defaults.
			event->event_type = WE_NONE;
			event->lifespan = 0;
			event->fake = false;
			event->type = ST_NONE;
			event->intensity = 0;
			event->zing = false;
		}
	}
}

void Weather::add_event()
{
	switch (MiscUtil::random(1, 3))
	{
		case 1:
			// Temperature events
			if (time_info.month > 9 && time_info.month < 16)
			{
				event->event_type = COLD_SNAP;
				event->lifespan = MiscUtil::random(20, 60);
				event->tics_left = event->lifespan;
			}
			else
			{
				event->event_type = HEAT_WAVE;
				event->lifespan = MiscUtil::random(20, 60);
				event->tics_left = event->lifespan;
			}
			break;
		case 2:
			switch (MiscUtil::random(1, 2))
			{
				case 1:
					event->event_type = DROUGHT;
					event->lifespan = MiscUtil::random(20, 60);
					event->tics_left = event->lifespan;
					break;
				case 2:
					event->event_type = FLOOD;
					event->lifespan = MiscUtil::random(20, 60);
					event->tics_left = event->lifespan;
					break;
			}
			break;
		default:
			break;
	}
}

void Weather::change_storms()
{
	storm->change(this);
}







void Storm::change(Weather * weather)
{
	if (type != ST_NONE)
	{
		if (fake)
		{
			--tics_left;
			if (tics_left < 1)
			{
				messege(weather, false); // false means its stopping.
				reset();
			}
		}
		else
			switch (type)
			{
				case DRIZZLE:
					if (weather->precipitation < 30)
					{
						messege(weather, false);
						reset();
					}
					break;
								
				case RAIN:
					if (weather->precipitation < 50)
					{
						messege(weather, false);
						reset();
					}
					break;
								
				case FREEZING_DRIZZLE:
					if (weather->precipitation < 30)
					{
						messege(weather, false);
						reset();
					}
					break;
								
				case FREEZING_RAIN:
					if (weather->precipitation < 50)
					{
						messege(weather, false);
						reset();
					}
					break;
								
				case SNOW:
					if (weather->precipitation < 50)
					{
						messege(weather, false);
						reset();
					}
					break;
				
				case SNOW_GRAINS:
					if (weather->precipitation < 30)
					{
						messege(weather, false);
						reset();
					}
					break;

				case HAIL:
					if (weather->precipitation < 10)
					{
						messege(weather, false);
						reset();
					}
					break;
				
				case BLIZZARD:
					if (weather->precipitation < 40)
					{
						messege(weather, false);
						reset();
					}
					break;

				case FOG:
					if (weather->precipitation < 30)
					{
						messege(weather, false);
						reset();
					}
					break;
				default:
					break;
			}
	}
	else if (MiscUtil::random(1, 20))
	{
		weather->add_storm();
	}
}
