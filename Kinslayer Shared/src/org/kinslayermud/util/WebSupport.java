package org.kinslayermud.util;

import java.util.Collection;
import java.util.List;
import java.util.Map;

import org.kinslayermud.advertising.FeaturedMUDListing;
import org.kinslayermud.character.User;
import org.kinslayermud.comm.Comm;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.kit.KitWithItemsAndObjectPrototypes;
import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.mob.SuperMob;
import org.kinslayermud.object.ObjectPrototype;
import org.kinslayermud.object.ObjectWeaponType;
import org.kinslayermud.object.ObjectWearType;
import org.kinslayermud.playerkilling.PlayerKill;
import org.kinslayermud.user.login.UserLogin;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.userlog.UserLogRecord;
import org.kinslayermud.zone.Zone;


public interface WebSupport {
  
  public User performLogin(String username, String password) throws DataInterfaceException;
  public void createUserSession(int userId, String sessionId) throws DataInterfaceException;
  public User getUserFromSession(String sessionId) throws DataInterfaceException;
  public void performPlayerPortalSignOut(String sessionId, int userId) throws DataInterfaceException;
  public List<UserLogRecord> getUserLogRecords(int userId) throws DataInterfaceException;
  public UserLog getUserLog(int userLogId) throws DataInterfaceException;
  public String getInstanceDomain();
  public MobPrototype getMobPrototype(int mobPrototypeId) throws DataInterfaceException;
  public Map<Integer, MobPrototype> getMobPrototypeMap(Collection<Integer> mobPrototypeIdCollection) throws DataInterfaceException;
  public Zone getZoneLoadingSuperMobPrototype(int mobPrototypeId) throws DataInterfaceException;
  public KitWithItemsAndObjectPrototypes getKitWithItemsAndObjectPrototypes(int kitId) throws DataInterfaceException;
  public Map<Integer, User> getUserMap(Collection<Integer> userIdCollection) throws DataInterfaceException;
  public List<PlayerKill> getLastSoManyPlayerKills(int howMany) throws DataInterfaceException;
  public List<SuperMob> getAllOpenSuperMobs() throws DataInterfaceException;
  public List<ObjectPrototype> getObjectPrototypesByWearType(ObjectWearType objectWearType) throws DataInterfaceException;
  public List<ObjectPrototype> getObjectPrototypesByWeaponType(ObjectWeaponType objectWeaponType) throws DataInterfaceException;
  public List<PlayerKill> getPlayerKillsByKillerId(int killerUserId) throws DataInterfaceException;
  public List<UserLogin> getUserLoginsByUserId(int userId) throws DataInterfaceException;
  public List<Comm> getTellHistory(int userId, int offset, int fetchSize) throws DataInterfaceException;
  public void putUserLog(UserLog userLog) throws DataInterfaceException;
  public FeaturedMUDListing getRandomFeaturedMUDListing(Collection<Integer> featuredMUDListingIdCollectionToExclude) throws DataInterfaceException;
}
