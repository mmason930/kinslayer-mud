/*************************************************************
 *                                                           *
 *   jedit.cpp - This file should declare variable, classes, *
 *      and functions that support the MUD's combat system.  *
 *                                                           *
 *   Originally Added By: Galnor (mikemason930@gmail.com)    *
 *   (C) July 25th, 2009                                     *
 *                                                           *
 *                                                           *
 *                                                           *
 *                                                           *
 *************************************************************/

/* Galnor 07/25/2009 - Handles the userPlayerKill MySQL table. */
class PKManager
{
private:
	static PKManager *Self;
	unsigned int nextKillID;
	PKManager();
	~PKManager();
public:
	static PKManager &GetManager();
	void Free();

	void RegisterKill( Character *Killer, Character *Victim, const unsigned int &wtrans, const time_t &when,
		const int kill_id  );
	const unsigned int GetNextKillID();
	const unsigned int GetCurrKillID();
};

