package org.kinslayermud.scripts.sitemap;

import java.io.FileWriter;
import java.io.IOException;
import java.sql.Connection;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.kinslayermud.misc.Provider;
import org.kinslayermud.mob.MobPrototype;
import org.kinslayermud.mob.MobUtil;
import org.kinslayermud.mob.SuperMob;
import org.kinslayermud.object.EquipmentListingType;
import org.kinslayermud.object.ObjectWeaponType;
import org.kinslayermud.object.ObjectWearType;
import org.kinslayermud.scripts.KinslayerProcess;
import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.WebSiteUrlUtil;

public class GenerateDynamicSitemapProcess implements KinslayerProcess {

  protected static final String CHANGE_FREQUENCY_DAILY = "daily";
  protected static final String CHANGE_FREQUENCY_WEEKLY = "weekly";
  
  protected Provider provider = null;
  protected Connection connection = null;
  protected Statement statement = null;
  
  public void process(Provider provider) {
    
    this.provider = provider;
    
    try {
      connection = provider.getConnection();
      statement = connection.createStatement();

      FileWriter fileWriter = new FileWriter("/var/www-prod/sitemap-dynamic.xml");
      fileWriter.write("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
      fileWriter.write("<urlset xmlns=\"http://www.sitemaps.org/schemas/sitemap/0.9\">\n");
      
      writeWeaponPages(fileWriter);
      writeArmorPages(fileWriter);
      writeSuperMobPages(statement, fileWriter);
      
      fileWriter.write("</urlset>");
      fileWriter.close();
      
      connection.commit();
      statement.close();
      connection.close();
    }
    catch(Throwable throwable) {
      
      throwable.printStackTrace();
      QueryUtil.closeNoThrow(statement);
      QueryUtil.closeNoThrow(connection);
    }
  }
  
  public void writeUrl(FileWriter fileWriter, String url, String changeFrequency) throws IOException {
    
    fileWriter.write("<url>\n");
    fileWriter.write("<loc>" + url + "</loc>\n");
    fileWriter.write("<changefreq>" + changeFrequency + "</changefreq>\n");
    fileWriter.write("</url>\n");
  }
  
  public void writeWeaponPages(FileWriter fileWriter) throws IOException {
    
    Iterator<ObjectWeaponType> iter =  ObjectWeaponType.getSetIterator();
    while(iter.hasNext()) {

      ObjectWeaponType objectWeaponType = iter.next();
      writeUrl(fileWriter, WebSiteUrlUtil.getEquipmentListingUrl(provider.getInstanceDomain(), EquipmentListingType.weapon, null, objectWeaponType), CHANGE_FREQUENCY_WEEKLY);
    }
  }
  
  public void writeArmorPages(FileWriter fileWriter) throws IOException {
    
    Iterator<ObjectWearType> iter =  ObjectWearType.getSetIterator();
    while(iter.hasNext()) {
      
      ObjectWearType objectWearType = iter.next();
      if(objectWearType.getShowOnWebsite()) {
        
        writeUrl(fileWriter, WebSiteUrlUtil.getEquipmentListingUrl(provider.getInstanceDomain(), EquipmentListingType.armor, objectWearType, null), CHANGE_FREQUENCY_WEEKLY);
      }
    }
  }
  
  public void writeSuperMobPages(Statement statement, FileWriter fileWriter) throws SQLException, IOException {
    
    List<SuperMob> superMobs = MobUtil.getAllOpenSuperMobs(statement);
    Set<Integer> mobPrototypeIdSet = new HashSet<Integer>();
    for(SuperMob superMob : superMobs) {
      
      mobPrototypeIdSet.add(superMob.getMobId());
    }
    
    Map<Integer, MobPrototype> mobPrototypeMap = MobUtil.getMobPrototypeMap(statement, mobPrototypeIdSet);
    
    for(SuperMob superMob : superMobs) {
      
      MobPrototype mobPrototype = mobPrototypeMap.get(superMob.getMobId());
      writeUrl(fileWriter, WebSiteUrlUtil.getSuperMobListingUrl(provider.getInstanceDomain(), superMob.getMobId(), mobPrototype.getShortDescription()), CHANGE_FREQUENCY_WEEKLY);
    }
  }
}
