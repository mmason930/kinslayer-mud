#ifndef CHARACTER_UTIL_UTIL_H
#define CHARACTER_UTIL_UTIL_H

class Character;

class CharacterUtil
{
private:
protected:
public:
	static Character *loadCharacter(const int userId);
	static Character *loadCharacter(const std::string &username);
};

#endif
