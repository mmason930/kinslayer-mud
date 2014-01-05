package org.kinslayermud.util;

import java.sql.Connection;
import java.sql.Statement;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.kinslayermud.advertising.FeaturedMUDListing;
import org.kinslayermud.advertising.FeaturedMUDListingUtil;
import org.kinslayermud.auction.AuctionBid;
import org.kinslayermud.auction.AuctionItem;
import org.kinslayermud.auction.AuctionUtil;
import org.kinslayermud.character.User;
import org.kinslayermud.comm.Comm;
import org.kinslayermud.comm.CommUtil;
import org.kinslayermud.exception.DataInterfaceException;
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
      
      System.out.println("Loading Home Resources...");
      
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
  
  public User performLogin(String username, String password) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      User user = UserUtil.performLogin(statement, username, password);
      
      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return user;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Map<Integer, AuctionBid> getAuctionBidIdToHighestAuctionBidMap(Set<Integer> auctionItemIdSet) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      Map<Integer, AuctionBid> auctionItemIdToHighestAuctionBidMap = AuctionUtil.getAuctionItemIdToHighestAuctionBid(statement, auctionItemIdSet);
      
      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return auctionItemIdToHighestAuctionBidMap;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public void createUserSession(int userId, String sessionId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      UserUtil.createUserSession(statement, userId, sessionId);
      
      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public User getUserFromSession(String sessionId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      User user = UserUtil.getUserFromSession(statement, sessionId);
      
      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return user;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public void performPlayerPortalSignOut(String sessionId, int userId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      UserUtil.performPlayerPortalSignOut(statement, sessionId, userId);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  

  public List<UserLogRecord> getUserLogRecords(int userId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<UserLogRecord> userLogRecords = UserLogUtil.getUserLogRecords(statement, userId);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return userLogRecords;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public UserLog getUserLog(int userLogId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      UserLog userLog = UserLogUtil.getUserLog(statement, userLogId, false);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return userLog;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public String getInstanceDomain() {
    
    return provider.getInstanceDomain();
  }

  public MobPrototype getMobPrototype(int mobPrototypeId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      MobPrototype mobPrototype = MobUtil.getMobPrototype(statement, mobPrototypeId, false);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return mobPrototype;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Map<Integer, MobPrototype> getMobPrototypeMap(Collection<Integer> mobPrototypeIdCollection) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      Map<Integer, MobPrototype> mobPrototypeMap = MobUtil.getMobPrototypeMap(statement, mobPrototypeIdCollection);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return mobPrototypeMap;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Zone getZoneLoadingSuperMobPrototype(int mobPrototypeId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      Zone zone = ZoneUtil.getZoneLoadingSuperMobPrototype(statement, mobPrototypeId, false);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return zone;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public KitWithItemsAndObjectPrototypes getKitWithItemsAndObjectPrototypes(int kitId) throws DataInterfaceException {
    
    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      KitWithItemsAndObjectPrototypes kitWithItemsAndObjectPrototypes = KitUtil.getKitWithItemsAndObjectPrototypes(statement, kitId, false);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return kitWithItemsAndObjectPrototypes;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Map<Integer, User> getUserMap(Collection<Integer> userIdCollection) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      Map<Integer, User> userMap = UserUtil.getUserMap(statement, userIdCollection);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return userMap;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  public List<PlayerKill> getLastSoManyPlayerKills(int howMany) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<PlayerKill> playerKills = PKUtil.getLastSoManyPlayerKills(statement, howMany);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return playerKills;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<SuperMob> getAllOpenSuperMobs() throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<SuperMob> superMobs = MobUtil.getAllOpenSuperMobs(statement);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return superMobs;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<ObjectPrototype> getObjectPrototypesByWearType(ObjectWearType objectWearType) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<ObjectPrototype> superMobs = ObjectUtil.getObjectPrototypesByWearType(statement, objectWearType);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return superMobs;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }

  public List<ObjectPrototype> getObjectPrototypesByWeaponType(ObjectWeaponType objectWeaponType) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<ObjectPrototype> superMobs = ObjectUtil.getObjectPrototypesByWeaponType(statement, objectWeaponType);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return superMobs;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<PlayerKill> getPlayerKillsByKillerId(int killerUserId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<PlayerKill> playerKills = PKUtil.getPlayerKillsByKillerId(statement, killerUserId);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return playerKills;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<UserLogin> getUserLoginsByUserId(int userId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<UserLogin> userLogins = UserLoginUtil.getUserLoginsByUserId(statement, userId);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return userLogins;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<Comm> getTellHistory(int userId, int offset, int fetchSize) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<Comm> tellHistory = CommUtil.getTellHistory(statement, userId, offset, fetchSize);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return tellHistory;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public void putUserLog(UserLog userLog) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      UserLogUtil.putUserLog(statement, userLog);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public FeaturedMUDListing getRandomFeaturedMUDListing(Collection<Integer> featuredMUDListingIdCollectionToExclude) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      FeaturedMUDListing featuredMUDListing = FeaturedMUDListingUtil.getRandomFeaturedMUDListing(statement, featuredMUDListingIdCollectionToExclude);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return featuredMUDListing;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public HelpFile getHelpFile(int helpFileId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      HelpFile helpFile = HelpUtil.getHelpFile(statement, helpFileId, false);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return helpFile;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }

  public HelpFile getHelpFileByModRewrittenName(String modRewrittenName, Integer parentId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      HelpFile helpFile = HelpUtil.getHelpFileByModRewrittenName(statement, modRewrittenName, parentId, false);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return helpFile;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }

  public List<HelpFile> getHelpFileChain(int helpFileId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<HelpFile> helpFileChain = HelpUtil.getHelpFileChain(statement, helpFileId);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return helpFileChain;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<HelpFile> getHelpFilesByParentId(Integer parentId) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<HelpFile> helpFiles = HelpUtil.getHelpFilesByParentId(statement, parentId);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return helpFiles;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public List<AuctionItem> getActiveAuctionItems() throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      List<AuctionItem> auctionItems = AuctionUtil.getActiveAuctionItems(statement);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return auctionItems;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public Map<String, Obj> getObjectMap(Set<String> objectIdSet) throws DataInterfaceException {

    Connection connection = null;
    Statement statement = null;
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();
      
      Map<String, Obj> objectMap = ObjectUtil.getObjectMap(statement, objectIdSet);

      statement.close();
      statement = null;
      
      connection.commit();
      connection.close();
      connection = null;
      
      return objectMap;
    }
    catch(Throwable throwable) {
      
      throw new DataInterfaceException(throwable);
    }
    finally {
      
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public int getPlayerPortalServerPort() {
    
    return provider.getPlayerPortalServerPort();
  }
}
