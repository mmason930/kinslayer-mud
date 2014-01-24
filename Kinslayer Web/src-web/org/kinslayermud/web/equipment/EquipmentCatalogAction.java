package org.kinslayermud.web.equipment;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import org.kinslayermud.object.EquipmentListingType;
import org.kinslayermud.object.ObjectPrototype;
import org.kinslayermud.object.ObjectWeaponType;
import org.kinslayermud.object.ObjectWearType;
import org.kinslayermud.util.WebSiteUrlUtil;
import org.kinslayermud.util.WebSupport;
import org.kinslayermud.web.actions.StandardAction;

public class EquipmentCatalogAction extends StandardAction {
  
  public static class PageDescription {
    
    public String h1;
    public String pageContent;
    public String metaDescription;
    public String title;
  }
  
  public static class LeftNavListItem {
    
    protected String anchorDisplay;
    protected String anchorUrl;
    protected boolean isSelected;
    
    public String getAnchorDisplay() {
      
      return anchorDisplay;
    }
    
    public void setAnchorDisplay(String anchorDisplay) {
      
      this.anchorDisplay = anchorDisplay;
    }
    
    public String getAnchorUrl() {
      
      return anchorUrl;
    }
    
    public void setAnchorUrl(String anchorUrl) {
      
      this.anchorUrl = anchorUrl;
    }
    
    public boolean getIsSelected() {
      
      return isSelected;
    }
    
    public void setIsSelected(boolean isSelected) {
      
      this.isSelected = isSelected;
    }
  }

  protected String SUCCESS_FORWARD = "Success";
  
  protected String EQUIPMENTLISTINGTYPE_PARAMETER = "EquipmentListingType";
  protected String OBJECTWEARTYPE_PARAMETER = "ObjectWearType";
  protected String OBJECTWEAPONTYPE_PARAMETER = "ObjectWeaponType";
  
  public String execute(WebSupport webSupport) throws Exception {

    List<LeftNavListItem> leftNavListItems = null;
    EquipmentListingType equipmentListingType = null;
    ObjectWearType objectWearType = null;
    ObjectWeaponType objectWeaponType = null;
    List<ObjectPrototype> objectPrototypes = new LinkedList<ObjectPrototype>();
    
    try {
      equipmentListingType = EquipmentListingType.getEnum(Integer.valueOf(request.getParameter(EQUIPMENTLISTINGTYPE_PARAMETER)));
    }
    catch(Throwable throwable) {}
    
    try {
      objectWearType = ObjectWearType.getEnum(Integer.valueOf(request.getParameter(OBJECTWEARTYPE_PARAMETER)));
    }
    catch(Throwable throwable) {}
    
    try {
      objectWeaponType = ObjectWeaponType.getEnum(Integer.valueOf(request.getParameter(OBJECTWEAPONTYPE_PARAMETER)));
    }
    catch(Throwable throwable) {}
    
    if(equipmentListingType != null) {
      
      if(equipmentListingType.equals(EquipmentListingType.armor)) {
        leftNavListItems = getLeftNavListItemsForEquipment(objectWearType);
        
        if(objectWearType != null && objectWearType.getShowOnWebsite()) {
          
          objectPrototypes = webSupport.getObjectPrototypesByWearType(objectWearType);
        }
      }
      else if(equipmentListingType.equals(EquipmentListingType.weapon)) {
        
        leftNavListItems = getLeftNavListItemsForWeapons(objectWeaponType);
        
        if(objectWeaponType != null) {

          objectPrototypes = webSupport.getObjectPrototypesByWeaponType(objectWeaponType);
        }
      }
    }
    
    PageDescription pageDescription = getPageDescription(equipmentListingType, objectWearType, objectWeaponType);
    
    request.setAttribute("PageContent", pageDescription.pageContent);
    request.setAttribute("H1", pageDescription.h1);
    request.setAttribute("Title", pageDescription.title);
    request.setAttribute("MetaDescription", pageDescription.metaDescription);
    request.setAttribute("EquipmentListingType", equipmentListingType);
    request.setAttribute("LestNavListItems", leftNavListItems);
    request.setAttribute("ObjectPrototypes", objectPrototypes);
    
    return SUCCESS_FORWARD;
  }
  
  public PageDescription getPageDescription(EquipmentListingType equipmentListingType, ObjectWearType objectWearType, ObjectWeaponType objectWeaponType) {
    
    PageDescription pageDescription = new PageDescription();

    if(equipmentListingType == null && objectWearType == null && objectWeaponType == null) {
      
      pageDescription.h1 = "Equipment Catalog";
      pageDescription.pageContent
        = "<p>Equipment is an essential part of your character's development; without it, survival and progress will be impossible."
        + " Your character's equipment consists of weapons and armor collected throughout the game: from quests, super mobs,"
        + " mining, smithing and shops. Every item has status modifiers and will either strenthen or hinder your character's"
        + " ability to fight and move throughout the game.</p>"
        + " While exploring the world of Kinslayer MUD, you will run across many items featured in <h2 class='inlineHeader'>Robert Jordan's"
        + " 'The Wheel of Time'</h2> series. Begin viewing the Equipment Catalog by selecting either <strong>Armor</strong> or"
        + " <strong>Weapons</strong> above.";
      pageDescription.title = "Equipment Catalog | Search Game Equipment Online | Kinslayer MUD";
      pageDescription.metaDescription = "Search the Kinslayer MUD equipment catalog. Find all weapons and armor online available in the game.";
    }
    
    if(equipmentListingType != null && equipmentListingType.equals(EquipmentListingType.weapon)) {
      
      //Weapons page
      if(objectWeaponType == null) {
        
        pageDescription.h1 = "Weapon Catalog";
        pageDescription.pageContent
          = "<p>Essential to your character's ability both to defend from and attack opponents, <h2='inlineHeader'>weapons in Kinslayer MUD come in many"
          + " varying classes</h2>. Each weapon class requires a different skill to be trained at skill trainers located across the game."
          + " The more proficient your skill in the weapon class, the more the weapon's status modifiers will affect your character.</p>"
          + "<p>With the exception of Short Blades, every weapon class belongs to the Warrrior class of skills, giving Warriors"
          + " a greater flexibility when it comes to choosing their weapon class of choice. Any class, however, is able to "
          + " obtain, practice, and wield weapons from any class they choose. Some factors do limit the ability to wield a weapon,"
          + " such as your character's Strength attribute and the weight of the weapon in question.</p>"
          + " <p>Select an option from the left to view a detailed list of all equipment in the Weapon Class.</p>";
        pageDescription.title = "Weapon Catalog | View Game Weapons Online | Kinslayer MUD";
        pageDescription.metaDescription = "Search all weapons available on Kinslayer MUD online in the equipment catalog. Many weapon classes available in game.";
      }
      else {
        
        pageDescription.title = objectWeaponType.getPluralName() + " | Weapon Equipment Catalog | Kinslayer MUD";
        pageDescription.metaDescription = "See all " + objectWeaponType.getSingularName().toLowerCase() + " weapons on Kinslayer MUD using the equipment catalog online.";
      }
    }
    
    if(equipmentListingType != null && equipmentListingType.equals(EquipmentListingType.armor)) {
      
      //Weapons page
      if(objectWearType == null) {
        
        pageDescription.h1 = "Armor Catalog";
        pageDescription.pageContent
          = "<p>Helmets, gauntlets, boots, shields and more. Armor is necessary for your character's defense. The main status modifiers"
          + " that armor provides are: Dodge, Parry, and Absorb. The higher your Dodge and Parry bonuses, the more proficient your character"
          + " will be at avoiding melee attacks. Absorb, on the other hand, allows your armor to diminish the damage that"
          + " your opponent deals against you.</p>"
          + " <p>Absorbtion armor is a passive ability and requires no skill training in order to benefit from its affects. Dodge and Parry,"
          + " on the other hand, require skills to be trained both in the Warrior & Thief classes. Top tier dodge and parry equipment"
          + " can also be more difficult to come across, and can be worn on every equipment slot available to your character.</p>"
          + "<p>While armor is essential to your success in the game, its benefits are minimal against Channelers, whose Weaves"
          + " typically bypass the three defensive status modifiers. Channelers, meanwhile, usually need to avoid being hit while"
          + " their weave is being carried out. With the high penalties for skill training for the Channeler class, this can be"
          + " a very difficult area to dominate.</p>"
          + " To see a detailed list of all equipment available for a given armor equipment slot, make a selection from the list to the left.</p>";
        pageDescription.title = "Armor Catalog | View Game Armor Online | Kinslayer MUD";
        pageDescription.metaDescription = "Search all armor available in the Kinslayer MUD online game. There are many armor classes available in game.";
      }
      else {

        pageDescription.title = objectWearType.getDisplayTerm() + " | Armor Equipment Catalog | Kinslayer MUD";
        pageDescription.metaDescription = "See all " + objectWearType.getSingularTerm().toLowerCase() + " armor on Kinslayer MUD using the equipment catalog online.";
      }
    }
    
    return pageDescription;
  }
  
  public List<LeftNavListItem> getLeftNavListItemsForEquipment(ObjectWearType selectedObjectWearType) {
    
    List<LeftNavListItem> leftNavListItems = new LinkedList<LeftNavListItem>();
    Iterator<ObjectWearType> iter =  ObjectWearType.getSetIterator();
    while(iter.hasNext()) {
      
      ObjectWearType objectWearType = iter.next();
      if(objectWearType.getShowOnWebsite()) {
        
        LeftNavListItem leftNavListItem = new LeftNavListItem();
        leftNavListItem.setAnchorDisplay(objectWearType.getDisplayTerm());
        leftNavListItem.setAnchorUrl(WebSiteUrlUtil.getEquipmentListingUrl(webSupport.getInstanceDomain(), EquipmentListingType.armor, objectWearType, null));
        leftNavListItem.setIsSelected(selectedObjectWearType != null && selectedObjectWearType.equals(objectWearType));
        
        leftNavListItems.add(leftNavListItem);
      }
    }
    
    return leftNavListItems;
  }
  
  public List<LeftNavListItem> getLeftNavListItemsForWeapons(ObjectWeaponType selectedObjectWeaponType) {
    
    List<LeftNavListItem> leftNavListItems = new LinkedList<LeftNavListItem>();
    Iterator<ObjectWeaponType> iter =  ObjectWeaponType.getSetIterator();
    while(iter.hasNext()) {
      
      ObjectWeaponType objectWeaponType = iter.next();
        
      LeftNavListItem leftNavListItem = new LeftNavListItem();
      leftNavListItem.setAnchorDisplay(objectWeaponType.getPluralName());
      leftNavListItem.setAnchorUrl(WebSiteUrlUtil.getEquipmentListingUrl(webSupport.getInstanceDomain(), EquipmentListingType.weapon, null, objectWeaponType));
      leftNavListItem.setIsSelected(selectedObjectWeaponType != null && selectedObjectWeaponType.equals(objectWeaponType));
      
      
      leftNavListItems.add(leftNavListItem);
    }
    
    return leftNavListItems;
  }
}
