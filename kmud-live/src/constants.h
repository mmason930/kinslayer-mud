extern const char *dirs[];
extern const char *room_bits[];
extern const char *exit_bits[];
extern const char *genders[];
extern const char *position_types[];
extern const char *player_bits[];
extern const char *action_bits[];
extern const char *preference_bits[];
extern const char *affected_bits[];
extern const char *connected_types[];
extern const char *where[];
extern const char *equipment_types[];
extern const char *item_types[];
extern const char *wear_bits[];
extern const char *extra_bits[];
extern const char *apply_types[];
extern const char *container_bits[];
extern const char *drinks[];
extern const char *drinknames[];
extern const char *color_liquid[];
extern const char *fullness[];
extern const char *weekdays[];
extern const char *month_name[];
extern const char *spell_wear_off_msg[];
extern const char *class_types[];
extern const char *loc[10];
extern const char *dist[9];
extern const char *weapon_types[];
extern const char * pc_race_types[];
extern const char * pc_class_types[];
extern const int rev_dir[];
extern const int drink_aff[][3];
extern const int usage_and_see_gain_constants[];
extern const int NUM_SEXES;
extern const int LENGTH_OF_AFFECTED_BITS;
extern const int LENGTH_OF_APPLY_TYPES;
extern const int NUM_STATS;
extern const int NUM_SUN_TYPES;
extern const int NUM_WEATHER_PATTERNS;
extern const int NUM_SECTORS;
extern const int NUM_SPECIAL_FUNCTIONS;
extern const int NUM_WEAPON_TYPES;
extern const std::string RealMonths[];
extern std::vector<int> CarryWeight;
extern std::vector<int> DaysPerRealMonth;


extern const char * weave_types[];
extern const char * weave_affect_types[];
extern const char * weave_target_types[];
extern const char * weave_damage_types[];
extern const char * weave_can_cast_flags[];
extern const char * weave_other_flags[];
extern const char * sex[];
extern const char * apply_types[];
extern const char * affected_bits_none_ended[];
extern const char * weave_can_cast[];
extern const char * weave_affect_usage[];
extern const char * stats[];
extern const char * sexes[];
extern const char * position_types[];
extern const char * weather_patterns[];
extern const char * sun_types[];
extern const char * weave_special_functions[];

#define YouCantDoThatHere "You can't do that here.\r\n"

std::string get_names_from_array(std::list<int> &values, const char * names[]);

std::string get_true_bool_names_from_array(const std::vector<bool> &values, const char * names[]);


void toggle_list_int(std::list<int> &li, int i);
extern const int COUNCIL_COMMANDS_MINIMUM_IMMORTAL_LEVEL;