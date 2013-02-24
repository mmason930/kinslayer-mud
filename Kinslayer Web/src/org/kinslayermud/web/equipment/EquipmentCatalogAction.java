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
    
    request.setAttribute("EquipmentListingType", equipmentListingType);
    request.setAttribute("LestNavListItems", leftNavListItems);
    request.setAttribute("ObjectPrototypes", objectPrototypes);
    
    return SUCCESS_FORWARD;
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
