/********************************************************************
 *	mobs.h - Implements the MobManager classes						*
 *																	*
 *	Coded by Galnor in April of 2009 (Sortak@gmail.com)				*
 *																	*
 ********************************************************************/

#define GET_NDD(mob) ((mob)->MobData->damnodice)
#define GET_SDD(mob) ((mob)->MobData->damsizedice)
#define GET_ALIAS(mob) ((mob)->player.name)
#define GET_SDESC(mob) ((mob)->player.short_descr)
#define GET_LDESC(mob) ((mob)->player.long_descr)
#define GET_DDESC(mob) ((mob)->player.description)
#define GET_ATTACK(mob) ((mob)->MobData->attack_type)

class MobManager
{
private:
	std::vector< Character* > MyMobProto;
	std::vector< Index* > MyMobIndex;
	static MobManager *Self;

	MobManager();
	~MobManager();
public:

	static MobManager &GetManager();

	void Free();
	void Reload();

	void BootPrototypes();
	void SavePrototype( const unsigned int index );
	void SavePrototypes( const unsigned int zone_vnum );
	void SavePrototypes( const unsigned int bottom_vnum, const unsigned int top_vnum );
	void DeletePrototypeFromDatabase( const unsigned int mob_rnum );
	std::list<std::string> GrabSaveQuery( const unsigned int mob_rnum );
	std::list<std::string> GrabDeleteQuery( const unsigned int mob_rnum );

	int RealMobile( const unsigned int vnum );
	int VirtualMobile( const unsigned int rnum );

	Index *GetIndex( const unsigned int rnum );

	unsigned int NumberOfPrototypes();

	void UpdateLiveMobilesFromPrototype( const unsigned int rnum );
	void UpdateLiveMobilesFromPrototype( Character *Prototype );
	void UpdateLiveMobile( Character *Live, Character *Prototype );
	void AddPrototype( Character *Proto, const int vnum );
	Character *BootPrototype( sql::Row &MyRow );
	Character *GetPrototype( const unsigned int rnum );
	Character *GetPrototype( Character *LiveMob );
	Character *GetPrototypeByVnum( const unsigned int vnum );
	Character *AllocateRawMobile();

	void CopyPrototype( Character *Destination, Character *Source );


};
