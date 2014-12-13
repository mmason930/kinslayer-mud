package org.kinslayermud.clan;

import java.sql.ResultSet;
import java.sql.SQLException;

import org.kinslayermud.dbutils.DBObject;
import org.kinslayermud.misc.DataObjectWithIntId;
import org.kinslayermud.util.QueryUtil;

public class Clan extends DataObjectWithIntId implements DBObject {

  protected String name;
  protected boolean isSecret;
  protected int warrantId;
  protected int hiddenLevel;
  protected int bonusSkillId;
  protected int bonusSkillModifier;
  protected int bonusSkillRank;
  
  public Clan() {
    
    id = NEW;
  }
  
  public void retrieveFromResultSet(ResultSet resultSet) throws SQLException {
    
    setId(resultSet.getInt("clan.id"));
    setName(resultSet.getString("clan.name"));
    setSecret(QueryUtil.getIntBoolean(resultSet, "clan.is_secret"));
    setWarrantId(resultSet.getInt("clan.warrant_id"));
    setHiddenLevel(resultSet.getInt("clan.hidden_level"));
    setBonusSkillId(resultSet.getInt("clan.bonus_skill_id"));
    setBonusSkillModifier(resultSet.getInt("clan.bonus_skill_modifier"));
    setBonusSkillRank(resultSet.getInt("clan.bonus_skill_rank"));
  }
  
  public String getName() {
    return name;
  }
  public void setName(String name) {
    this.name = name;
  }
  public boolean isSecret() {
    return isSecret;
  }
  public void setSecret(boolean isSecret) {
    this.isSecret = isSecret;
  }
  public int getWarrantId() {
    return warrantId;
  }
  public void setWarrantId(int warrantId) {
    this.warrantId = warrantId;
  }
  public int getHiddenLevel() {
    return hiddenLevel;
  }
  public void setHiddenLevel(int hiddenLevel) {
    this.hiddenLevel = hiddenLevel;
  }
  public int getBonusSkillId() {
    return bonusSkillId;
  }
  public void setBonusSkillId(int bonusSkillId) {
    this.bonusSkillId = bonusSkillId;
  }
  public int getBonusSkillModifier() {
    return bonusSkillModifier;
  }
  public void setBonusSkillModifier(int bonusSkillModifier) {
    this.bonusSkillModifier = bonusSkillModifier;
  }
  public int getBonusSkillRank() {
    return bonusSkillRank;
  }
  public void setBonusSkillRank(int bonusSkillRank) {
    this.bonusSkillRank = bonusSkillRank;
  }
}
