

//DO NOT CHANGE THE ORDER OF THESE CONSTANTS!

const int STAT_STR  = 0;
const int STAT_INT  = 1;
const int STAT_WIS  = 2;
const int STAT_DEX  = 4;
const int STAT_CON  = 5;
const int STAT_LUCK = 6;

class Stat
{
private:
    std::string Name;      //Full stat name...
    std::string Abbrev;    //Abbreviation of the stat name...
    int       StatID;      //Identification of this stat.

public:

    const int GetID();
    const std::string getName();
    const std::string GetAbbrev();

    Stat()  {}
	Stat(const std::string _Name, const std::string _Abbrev, const int _ID);
    ~Stat() {}
};

class StatGroup
{
public:
    byte chclass, race, sex;    //Identifiers...
    std::map< int, std::vector<byte> > StatRolls;  //List of stat rolls.

    StatGroup();
    StatGroup( StatGroup *source );
    StatGroup(const byte _class, const byte _race, const byte _sex);
};
class StatManager
{
public:
    std::list< Stat >         StatList;           //List of the standard game stats...
    std::list< StatGroup >    StatGroups;         //List of the game's stat groups for stat rolls.

    static StatManager& GetManager( void );

    StatGroup* FindStatGroup(const byte chclass, const byte race, const byte sex);
    Stat* StatByID( const int ID );

    bool RollStats(Character *ch);
    void Boot();
	void Save();
	void Free();

private:
    StatManager();
   ~StatManager();

    static StatGroup sgDefault;  //Stat group default.
    static StatManager* Self;

};
