

//DO NOT CHANGE THE ORDER OF THESE CONSTANTS!

const int STAT_STR  = 0;
const int STAT_INT  = 1;
const int STAT_WIS  = 2;
const int STAT_DEX  = 4;
const int STAT_CON  = 5;

class Stat
{
private:
    std::string Name;      //Full stat name...
    std::string Abbrev;    //Abbreviation of the stat name...
    int       StatID;      //Identification of this stat.

public:

    int GetID();
    const std::string getName();
    const std::string GetAbbrev();

    Stat()  {}
	Stat(const std::string _Name, const std::string _Abbrev, const int _ID);
    ~Stat() {}
};

class StatGroup
{
public:
    sbyte chclass, race, sex;    //Identifiers...
    std::map< int, std::vector<sbyte> > StatRolls;  //List of stat rolls.

    StatGroup();
    StatGroup( StatGroup *source );
    StatGroup(const sbyte _class, const sbyte _race, const sbyte _sex);
};
class StatManager
{
public:
    std::list< Stat >         StatList;           //List of the standard game stats...
    std::list< StatGroup >    StatGroups;         //List of the game's stat groups for stat rolls.

    static StatManager& GetManager( void );

    StatGroup* FindStatGroup(const sbyte chclass, const sbyte race, const sbyte sex);
    Stat* StatByID( const int ID );

    bool RollStats(Character *ch);
    void Boot();
	void save();
	void Free();

private:
    StatManager();
   ~StatManager();

    static StatGroup sgDefault;  //Stat group default.
    static StatManager* Self;

};
