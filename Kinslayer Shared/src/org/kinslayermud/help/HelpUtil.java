package org.kinslayermud.help;

import java.sql.ResultSet;
import java.sql.SQLException;
import java.sql.Statement;
import java.util.LinkedList;
import java.util.List;

import org.kinslayermud.exception.NonExistentObjectException;
import org.kinslayermud.util.QueryUtil;
import org.kinslayermud.util.SQLUtil;

public class HelpUtil {

  public static HelpFile getHelpFile(ResultSet resultSet) throws SQLException {
    
    HelpFile helpFile = new HelpFile();
    
    helpFile.setId(resultSet.getInt("id"));
    helpFile.setName(resultSet.getString("name"));
    helpFile.setPageContent(resultSet.getString("page_content"));
    helpFile.setParentId(QueryUtil.getInteger(resultSet, "parent_id"));
    helpFile.setCreatedDatetime(resultSet.getTimestamp("created_datetime"));
    helpFile.setCreatedByUserId(resultSet.getInt("created_by_user_id"));
    helpFile.setLastModifiedDatetime(resultSet.getTimestamp("last_modified_datetime"));
    helpFile.setLastModifiedByUserId(resultSet.getInt("last_modified_by_user_id"));
    helpFile.setModRewrittenName(resultSet.getString("mod_rewritten_name"));
    
    return helpFile;
  }
  
  public static HelpFile getHelpFileMeetingCriteria(Statement statement, String criteria, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String sql = " SELECT *"
               + " FROM websiteHelpFile"
               + " WHERE " + criteria;
    ResultSet resultSet = statement.executeQuery(sql);
    
    if(resultSet.next()) {
      
      return getHelpFile(resultSet);
    }
    
    if(throwIfNotFound) {
      
      throw new NonExistentObjectException("No help file found. Criteria = " + criteria);
    }
    
    return null;
  }
  
  public static HelpFile getHelpFile(Statement statement, int helpFileId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    return getHelpFileMeetingCriteria(statement, "id = " + helpFileId, throwIfNotFound);
  }
  
  public static HelpFile getHelpFileByModRewrittenName(Statement statement, String modRewrittenName, Integer parentId, boolean throwIfNotFound) throws SQLException, NonExistentObjectException {
    
    String criteria = "mod_rewritten_name = " + SQLUtil.escapeQuoteString(modRewrittenName)
                    + (parentId != null ? (" AND parent_id = " + parentId) : " AND parent_id IS NULL");
    
    System.out.println("CRITERIA: " + criteria);
    return getHelpFileMeetingCriteria(statement, criteria, throwIfNotFound);
  }
  
  public static List<HelpFile> getHelpFilesMeetingCriteria(Statement statement, String criteria) throws SQLException {
    
    List<HelpFile> helpFiles = new LinkedList<HelpFile>();
    String sql = " SELECT *"
               + " FROM websiteHelpFile"
               + " WHERE " + criteria;
    
    ResultSet resultSet = statement.executeQuery(sql);
    
    while(resultSet.next()) {
      
      HelpFile helpFile = getHelpFile(resultSet);
      helpFiles.add(helpFile);
    }
    
    return helpFiles;
  }
  
  public static List<HelpFile> getHelpFileChain(Statement statement, int helpFileId) throws SQLException, NonExistentObjectException {
    
    List<HelpFile> helpFileChain = new LinkedList<HelpFile>();
    HelpFile helpFile = getHelpFile(statement, helpFileId, true);
    
    while(helpFile.getParentId() != null) {
      
      helpFile = getHelpFile(statement, helpFileId, true);
      helpFileChain.add(0, helpFile);
    }
    
    return helpFileChain;
  }
  
  public static List<HelpFile> getHelpFilesByParentId(Statement statement, Integer parentId) throws SQLException {
    
    String criteria = (parentId == null ? "parent_id IS NULL" : ("parent_id = " + parentId));
    return getHelpFilesMeetingCriteria(statement, criteria);
  }
}
