package org.kinslayermud.help;

import java.util.Date;

import org.kinslayermud.misc.DataObjectWithIntId;

public class HelpFile extends DataObjectWithIntId {

  protected String name;
  protected String pageContent;
  protected Integer parentId;
  protected Date createdDatetime;
  protected int createdByUserId;
  protected Date lastModifiedDatetime;
  protected int lastModifiedByUserId;
  protected String modRewrittenName;
  
  public String getName() {
    return name;
  }
  public void setName(String name) {
    this.name = name;
  }
  public String getPageContent() {
    return pageContent;
  }
  public void setPageContent(String pageContent) {
    this.pageContent = pageContent;
  }
  public Integer getParentId() {
    return parentId;
  }
  public void setParentId(Integer parentId) {
    this.parentId = parentId;
  }
  public Date getCreatedDatetime() {
    return createdDatetime;
  }
  public void setCreatedDatetime(Date createdDatetime) {
    this.createdDatetime = createdDatetime;
  }
  public int getCreatedByUserId() {
    return createdByUserId;
  }
  public void setCreatedByUserId(int createdByUserId) {
    this.createdByUserId = createdByUserId;
  }
  public Date getLastModifiedDatetime() {
    return lastModifiedDatetime;
  }
  public void setLastModifiedDatetime(Date lastModifiedDatetime) {
    this.lastModifiedDatetime = lastModifiedDatetime;
  }
  public int getLastModifiedByUserId() {
    return lastModifiedByUserId;
  }
  public void setLastModifiedByUserId(int lastModifiedByUserId) {
    this.lastModifiedByUserId = lastModifiedByUserId;
  }
  public String getModRewrittenName() {
    return modRewrittenName;
  }
  public void setModRewrittenName(String modRewrittenName) {
    this.modRewrittenName = modRewrittenName;
  }
}
