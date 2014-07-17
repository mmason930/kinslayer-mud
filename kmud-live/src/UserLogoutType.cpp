#include "UserLogoutType.h"

template < >
std::list<Enum<UserLogoutType>*> Enum<UserLogoutType>::enums = std::list<Enum<UserLogoutType>*>();

UserLogoutType *UserLogoutType::rent = new UserLogoutType(0, "Rent");
UserLogoutType *UserLogoutType::idle  = new UserLogoutType(1, "Idle");
UserLogoutType *UserLogoutType::linklessIdle  = new UserLogoutType(2, "Linkless Idle");
UserLogoutType *UserLogoutType::quit  = new UserLogoutType(3, "Quit");
UserLogoutType *UserLogoutType::quitWithRent  = new UserLogoutType(4, "Quit With Rent");
UserLogoutType *UserLogoutType::purge  = new UserLogoutType(5, "Purge");
UserLogoutType *UserLogoutType::reboot = new UserLogoutType(6, "Reboot");
UserLogoutType *UserLogoutType::scriptExtraction = new UserLogoutType(7, "Script Extraction");
UserLogoutType *UserLogoutType::unknown  = new UserLogoutType(8, "Unknown");
UserLogoutType *UserLogoutType::notRealLogout = new UserLogoutType(9, "Not Real Logout");
