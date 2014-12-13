package org.kinslayermud.util;

import java.sql.Connection;
import java.sql.Statement;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.kinslayermud.achievement.Achievement;
import org.kinslayermud.advertising.FeaturedMUDListing;
import org.kinslayermud.advertising.FeaturedMUDListingUtil;
import org.kinslayermud.auction.AuctionBid;
import org.kinslayermud.auction.AuctionItem;
import org.kinslayermud.auction.AuctionUtil;
import org.kinslayermud.character.User;
import org.kinslayermud.character.UserClan;
import org.kinslayermud.clan.ClanWithRanks;
import org.kinslayermud.comm.Comm;
import org.kinslayermud.comm.CommUtil;
import org.kinslayermud.dbutils.StatementConnectionExecutor;
import org.kinslayermud.dbutils.StatementExecutor;
import org.kinslayermud.exception.DataInterfaceException;
import org.kinslayermud.forum.ForumUser;
import org.kinslayermud.help.HelpFile;
import org.kinslayermud.help.HelpUtil;
import org.kinslayermud.kit.KitUtil;
import org.kinslayermud.kit.KitWithItemsAndObjectPrototypes;
import org.kinslayermud.misc.Provider;
import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.mob.MobUtil;
import org.kinslayermud.mob.SuperMob;
import org.kinslayermud.object.Obj;
import org.kinslayermud.object.ObjectPrototype;
import org.kinslayermud.object.ObjectUtil;
import org.kinslayermud.object.ObjectWeaponType;
import org.kinslayermud.object.ObjectWearType;
import org.kinslayermud.playerkilling.PKUtil;
import org.kinslayermud.playerkilling.PlayerKill;
import org.kinslayermud.user.login.UserLogin;
import org.kinslayermud.user.login.UserLoginUtil;
import org.kinslayermud.userlog.UserLog;
import org.kinslayermud.userlog.UserLogRecord;
import org.kinslayermud.userlog.UserLogUtil;
import org.kinslayermud.zone.Zone;
import org.kinslayermud.zone.ZoneUtil;


public class WebSupport {

  protected Provider provider;
  protected volatile List<PlayerKill> homePlayerKills;
  protected volatile Map<Integer, User> homeUserMap;
  protected volatile List<AuctionItem> homeAuctionItems;
  protected volatile Map<String, Obj> homeObjectMap;
  protected volatile Map<Integer, AuctionBid> homeAuctionItemIdToHighestAuctionBidMap;
  
  public List<PlayerKill> getHomePlayerKills() {
    
    return homePlayerKills;
  }
  
  public Map<Integer, User> getHomeUserMap() {
    
    return homeUserMap;
  }
  
  public List<AuctionItem> getHomeAuctionItems() {
    
    return homeAuctionItems;
  }
  
  public Map<String, Obj> getHomeObjectMap() {
    
    return homeObjectMap;
  }
  
  public Map<Integer, AuctionBid> getHomeAuctionItemIdToHighestAuctionBidMap() {
    
    return homeAuctionItemIdToHighestAuctionBidMap;
  }
  
  public void loadHomeResources() {
    
    try {
      
      //System.out.println("Loading Home Resources...");
      
      List<PlayerKill> playerKills = getLastSoManyPlayerKills(10);
      List<AuctionItem> auctionItems = getActiveAuctionItems();
      Map<Integer, User> userMap;
      Map<String, Obj> objectMap;
      Set<String> objectIdSet = new HashSet<String>();
      Collection<Integer> userIdCollection = new HashSet<Integer>();
      Map<Integer, AuctionBid> auctionItemIdToHighestAuctionBidMap;
      Set<Integer> auctionItemIdSet = new HashSet<Integer>();
      
      for(AuctionItem auctionItem : auctionItems) {
        
        objectIdSet.add(auctionItem.getObjectId());
        userIdCollection.add(auctionItem.getOwnerId());
        auctionItemIdSet.add(auctionItem.getId());
      }
      
      objectMap = objectIdSet.size() > 0 ? getObjectMap(objectIdSet) : new HashMap<String, Obj>();
    
    
      for(PlayerKill playerKill : playerKills) {
      
        userIdCollection.addAll(playerKill.getUserIdSet());
      }
    
      userMap = getUserMap(userIdCollection);      
      auctionItemIdToHighestAuctionBidMap = auctionItemIdSet.size() > 0 ? getAuctionBidIdToHighestAuctionBidMap(auctionItemIdSet) : new HashMap<Integer, AuctionBid>();
      
      this.homePlayerKills = playerKills;
      this.homeUserMap = userMap;
      this.homeAuctionItems = auctionItems;
      this.homeObjectMap = objectMap;
      this.homeAuctionItemIdToHighestAuctionBidMap = auctionItemIdToHighestAuctionBidMap;
    }
    catch(Exception exception) {
      
      System.out.println("Could not load home resources.");
      exception.printStackTrace();
    }
  }
  
  public WebSupport(Provider provider) {
    this.provider = provider;
    
    loadHomeResources();
  }
  
  public<T> T executeConnectionStatement(StatementConnectionExecutor<T> statementConnectionExecutor) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      T t = statementConnectionExecutor.execute(connection, statement);
      
      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return t;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public<T> T executeStatement(StatementExecutor<T> statementExecutor) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      T t = statementExecutor.execute(statement);
      
      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return t;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public User performLogin(String username, String password) throws DataInterfaceException {
    
    return executeStatement( statement -> UserUtil.performLogin(statement, username, password) );
  }
  
  public Map<Integer, AuctionBid> getAuctionBidIdToHighestAuctionBidMap(Set<Integer> auctionItemIdSet) throws DataInterfaceException {
    
    return executeStatement(statement -> AuctionUtil.getAuctionItemIdToHighestAuctionBid(statement, auctionItemIdSet));
  }
  
  public void createUserSession(int userId, String sessionId) throws DataInterfaceException {
    
    executeStatement(statement ->  { UserUtil.createUserSession(statement, userId, sessionId); return null; } );
  }
  
  public User getUserFromSession(String sessionId) throws DataInterfaceException {
    
    return executeStatement(statement -> UserUtil.getUserFromSession(statement, sessionId));
  }
  
  public void performPlayerPortalSignOut(String sessionId, int userId) throws DataInterfaceException {
    
    executeStatement(statement -> {UserUtil.performPlayerPortalSignOut(statement, sessionId, userId); return null;} );
  }
  
  public List<UserLogRecord> getUserLogRecords(int userId) throws DataInterfaceException {
    
    return executeStatement(statement -> UserLogUtil.getUserLogRecords(statement, userId));
  }
  
  public UserLog getUserLog(int userLogId) throws DataInterfaceException {
    
    return executeStatement(statement -> UserLogUtil.getUserLog(statement, userLogId, false));
  }
  
  public String getInstanceDomain() {
    
    return provider.getInstanceDomain();
  }

  public MobPrototype getMobPrototype(int mobPrototypeId) throws DataInterfaceException {
    
    return executeStatement(statement -> MobUtil.getMobPrototype(statement, mobPrototypeId, false));
  }
  
  public Map<Integer, MobPrototype> getMobPrototypeMap(Collection<Integer> mobPrototypeIdCollection) throws DataInterfaceException {
    
    return executeStatement(statement -> MobUtil.getMobPrototypeMap(statement, mobPrototypeIdCollection));
  }
  
  public Zone getZoneLoadingSuperMobPrototype(int mobPrototypeId) throws DataInterfaceException {
    
    return executeStatement(statement -> ZoneUtil.getZoneLoadingSuperMobPrototype(statement, mobPrototypeId, false));
  }
  
  public KitWithItemsAndObjectPrototypes getKitWithItemsAndObjectPrototypes(int kitId) throws DataInterfaceException {
    
    return executeStatement(statement -> KitUtil.getKitWithItemsAndObjectPrototypes(statement, kitId, false));
  }
  
  public Map<Integer, User> getUserMap(Collection<Integer> userIdCollection) throws DataInterfaceException {

    return executeStatement(statement -> UserUtil.getUserMap(statement, userIdCollection));
  }
  
  public List<PlayerKill> getLastSoManyPlayerKills(int howMany) throws DataInterfaceException {

    return executeStatement(statement -> PKUtil.getLastSoManyPlayerKills(statement, howMany));
  }
  
  public List<SuperMob> getAllOpenSuperMobs() throws DataInterfaceException {

    return executeStatement(statement -> MobUtil.getAllOpenSuperMobs(statement));
  }
  
  public List<ObjectPrototype> getObjectPrototypesByWearType(ObjectWearType objectWearType) throws DataInterfaceException {

    return executeStatement(statement -> ObjectUtil.getObjectPrototypesByWearType(statement, objectWearType));
  }
  
  public List<ObjectPrototype> getObjectPrototypesByWeaponType(ObjectWeaponType objectWeaponType) throws DataInterfaceException {

    return executeStatement(statement -> ObjectUtil.getObjectPrototypesByWeaponType(statement, objectWeaponType));
  }
  
  public List<PlayerKill> getPlayerKillsByKillerId(int killerUserId, Integer offset, Integer fetchSize, boolean sort) throws DataInterfaceException {

    return executeStatement(statement -> PKUtil.getPlayerKillsByKillerId(statement, killerUserId, offset, fetchSize, sort));
  }
  
  public List<UserLogin> getUserLoginsByUserId(int userId) throws DataInterfaceException {

    return executeStatement(statement -> UserLoginUtil.getUserLoginsByUserId(statement, userId));
  }
  
  public List<Comm> getTellHistory(int userId, int offset, int fetchSize) throws DataInterfaceException {

    return executeStatement(statement -> CommUtil.getTellHistory(statement, userId, offset, fetchSize));
  }
  
  public void putUserLog(UserLog userLog) throws DataInterfaceException {

    executeStatement(statement -> {UserLogUtil.putUserLog(statement, userLog); return null;});
  }
  
  public FeaturedMUDListing getRandomFeaturedMUDListing(Collection<Integer> featuredMUDListingIdCollectionToExclude) throws DataInterfaceException {

    return executeStatement(statement -> FeaturedMUDListingUtil.getRandomFeaturedMUDListing(statement, featuredMUDListingIdCollectionToExclude));
  }
  
  public HelpFile getHelpFile(int helpFileId) throws DataInterfaceException {

    return executeStatement(statement -> HelpUtil.getHelpFile(statement, helpFileId, false));
  }

  public HelpFile getHelpFileByModRewrittenName(String modRewrittenName, Integer parentId) throws DataInterfaceException {

    return executeStatement(statement -> HelpUtil.getHelpFileByModRewrittenName(statement, modRewrittenName, parentId, false));
  }

  public List<HelpFile> getHelpFileChain(int helpFileId) throws DataInterfaceException {

    return executeStatement(statement -> HelpUtil.getHelpFileChain(statement, helpFileId));
  }
  
  public List<HelpFile> getHelpFilesByParentId(Integer parentId) throws DataInterfaceException {

    return executeStatement(statement -> HelpUtil.getHelpFilesByParentId(statement, parentId));
  }
  
  public List<AuctionItem> getActiveAuctionItems() throws DataInterfaceException {

    return executeStatement(statement -> AuctionUtil.getActiveAuctionItems(statement));
  }
  
  public Map<String, Obj> getObjectMap(Set<String> objectIdSet) throws DataInterfaceException {

    return executeStatement(statement -> ObjectUtil.getObjectMap(statement, objectIdSet));
  }
  
  public User getUserByUsername(String username) throws DataInterfaceException {
    
    return executeStatement(statement -> UserUtil.getUserByUsername(statement, username, false));
  }
  
  public List<UserClan> getUserClansByUserId(int userId) throws DataInterfaceException {
    
    return executeStatement(statement -> UserUtil.getUserClansByUserId(statement, userId));
  }
  
  public Map<Integer, ClanWithRanks> getClansWithRanksMap(Collection<Integer> clanIdCollection) throws DataInterfaceException {
    
    return executeStatement(statement -> ClanUtil.getClanIdToClanWithRanksMap(statement, clanIdCollection));
  }
  
  public List<Integer> getAchievementIdsByUser(int userId) throws DataInterfaceException {

    return executeStatement(statement -> UserUtil.getAchievementIdSetByUserId(statement, userId));
  }
  
  public Map<Integer, Achievement> getAchievementMap(Collection<Integer> achievementIdCollection) throws DataInterfaceException {
    
    return executeStatement(statement -> AchievementUtil.getAchievementMap(statement, achievementIdCollection));
  }
  
  public ForumUser getForumUser(int userId) throws DataInterfaceException {

    return executeStatement(statement -> ForumUtil.getForumUser(statement, userId, false));
  }
  
  public int getPlayerPortalServerPort() {
    
    return provider.getPlayerPortalServerPort();
  }
}
