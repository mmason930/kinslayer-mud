// #ifdef WEATHER_H
// #define WEATHER_H

/* Sun state for Weather */
enum Sun
{
	SUN_DARK,
	SUN_RISE,
	SUN_LIGHT,
	SUN_SET
};
/* Sky conditions for Weather */
enum Sky
{
	SKY_CLOUDLESS,
	SKY_CLOUDY,
	SKY_RAINING,
	SKY_SNOWING,
	SKY_HAILING,
	SKY_BLIZZARD,
	SKY_FOGGY
};

enum StormType
{
	ST_NONE,
	DRIZZLE,
	RAIN,
	FREEZING_DRIZZLE,
	FREEZING_RAIN,
	SNOW,
	SNOW_PELLETS,
	SNOW_GRAINS,
	ICE_PELLETS,
	HAIL,
	BLIZZARD,
	FOG
};

enum WeatherEvent
{
	WE_NONE,
	HEAT_WAVE,
	COLD_SNAP,
	DROUGHT,
	FLOOD
};


class Storm
{
	public:

		int lifespan; // How long is the total storm lifetime.
		int tics_left; // How longer will it last (if fake).
		StormType type; // What type of storm is it.
		int intensity; // How strong is the storm.
		bool fake; // if it was made by the create storm weave.
		WeatherEvent event_type; // What type of event is it (if it is an event).
		bool zing; // Is there lightning?
	
		void reset();
		void messege(class Weather * weather, const bool start) const;
		void change(class Weather * weather);
		void change_intensity(class Weather * weather);

		int get_type() const
		{
			return type;
		}

		int get_event_type() const
		{
			return event_type;
		}
		
		Storm()
		{
			event_type = WE_NONE;// no event;
			lifespan = 1;
			tics_left = 1;
			type = ST_NONE; // no storm;
			fake = false;
			intensity = 0;
			zing = false;
		}
};


class Weather
{
	friend class Storm;
	
	private:
		Sky	sky;	// How is the sky.
		static Sun sunlight;	// How much sun. Note: this is static because we have no time zones, so all zones should be synced.
		sh_int temperature; // Temperature (in an internal fake -100 - 100 scale)
		sh_int base_temperature; // Temperature that is not affected by weaves.
		sh_int visibility; // How is visibility (higher is poorer)
		Storm * storm; // What type of storm is happening (default (0) is no storm)
		sh_int precipitation; // What percent of maximium water is in the atmosphere.
		sh_int base_precipitation; // Precipitation that is not affected by weaves.
		bool t_modified; // If temperature has been screwed with.
		bool p_modified; // If precipitation has been screwed with.
		bool i_modified; // If intensity has been screwed with.
		Storm * event; // Special weather events.
		sh_int zone; // What zone this weather is for.
		sh_int keep_current; // This is how many tics we are forstd::cing the weather to keep how it is (from the change weather weave).
	
		void change_temp();
		void change_precip();
		void change_intensity();
		void change_sky();
		void change_event();
		void add_event();
		void change_storms();
		void add_storm();
		void change_side_effects();

	public:
		void change();

		void force_change_precip(const bool wet, const int percent_known);
		void force_change_temp(const bool hot, const int percent_known);
		void force_change_intensity(const bool worse, const int percent_known);

		void sun_message();
		
		static int getSun()
		{
			return sunlight;
		}

		static void setSun(Sun i)
		{
			Weather::sunlight = i;
		}

		int get_sky() const
		{
			return sky;
		}
		int get_precipitation() const
		{
			return precipitation;
		}
		int get_temp() const
		{
			return temperature;
		}
		int get_visibility() const
		{
			return visibility;
		}

		Storm * get_storm()
		{
			return storm;
		}

		Storm * get_event()
		{
			return event;
		}
		
		Weather(const sh_int z)
		{
			sky = SKY_CLOUDLESS;
			base_temperature = 20;
			temperature = 20;
			visibility = 0;
			t_modified = false;
			p_modified = false;
			i_modified = false;
			precipitation = 20;
			base_precipitation = 20;
			event = new Storm();
			zone = z;
			storm = new Storm();
		}
		Weather()
		{
			sky = SKY_CLOUDLESS;
			base_temperature = 20;
			temperature = 20;
			visibility = 0;
			t_modified = false;
			p_modified = false;
			i_modified = false;
			precipitation = 20;
			base_precipitation = 20;
			event = new Storm();
			this->zone = -1;
			storm = new Storm();
		}

		~Weather()
		{
			delete event;
			delete storm;
		}
};







void weather_and_time(int mode);

void sendToOutdoor(Weather * weather, int zone, const char * messg);

// #endif
