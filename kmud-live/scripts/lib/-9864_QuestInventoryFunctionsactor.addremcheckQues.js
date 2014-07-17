//Alder
//August 2010
/************** Quest Inventory Utilities **************/
JSCharacter.prototype.loadQuestInv = function () {
	var rs = sqlQuery("SELECT questInventory.* FROM questInventory LEFT JOIN users ON questInventory.user_id=users.user_id WHERE users.username='"+sqlEsc(this.name)+"';");
	var qInv = [];
	while ( rs.hasNextRow ) {
		var row = rs.getRow;
		var item = new Object();
		item.itemName = row.get("itemName");
		item.itemAmount = row.get("itemAmount");
		item.questName = row.get("questName");
		qInv.push(item);
	}
	this._questInv = qInv;
}
/********************************************************
 * itemName: string representing name of item           *
 * remCount: amount to decrement the amount of item     *
 * questName: quest associated with the item            *
 * NOTE: to assure all instances of an item are removed *
 *       without knowing exact amount, use -1 remCount  *
 ********************************************************/
JSCharacter.prototype.remQuestItem = function ( itemName, remCount, questName ) {
	itemName += "";
	remCount = Number(remCount);
	questName += "";
	var len = this._questInv.length;
	for ( var i = 0; i < len; i++ ) {
		var item = this._questInv[i];
		if ( item.itemName == itemName && item.questName == questName ) {
			var crntAmt = item.itemAmount;
			var newAmt = crntAmt - remCount;
			/**All instances of the item have been removed, delete item from quest inv and delete SQL entry**/
			if ( newAmt <= 0 || remCount < 0 ) {
				remCount = crntAmt;
				this._questInv.splice(i,1);
				sqlQuery("DELETE FROM questInventory WHERE user_id="+this.id+" && itemName='"+sqlEsc(itemName)+"' && questName='"+sqlEsc(questName)+"';");
			}
			/**Instances of the item still exist, just update quest inventory and SQL entry**/
			else {
				item.itemAmount = newAmt;
				sqlQuery("UPDATE questInventory SET itemAmount="+sqlEsc(newAmt)+" WHERE user_id=" + this.id + " AND itemName='"+sqlEsc(itemName)+"' && questName='"+sqlEsc(questName)+"';");
			}
			getCharCols(this);
			this.send(cyn+capFirstLetter(itemName)+nrm+bld+" has been removed from your quest inventory."+(remCount > 1 ? " ("+remCount+")" : "")+nrm);
			return true;
		}
	}
	return false;
}
/********************************************************
 * itemName: string representing name of item           *
 * addCount: amount to increment the amount of item     *
 * questName: quest associated with the item            *
 ********************************************************/
JSCharacter.prototype.addQuestItem = function ( itemName, addCount, questName ) {
	itemName += "";
	addCount = Number(addCount);
	questName += "";
	var itemCheck = this.checkQuestItem(itemName,1,questName);
	if ( itemCheck ) {
		itemCheck.itemAmount += addCount;
		sqlQuery("UPDATE questInventory SET itemAmount="+sqlEsc(itemCheck.itemAmount)+" WHERE user_id="+this.id+" && itemName='"+sqlEsc(itemName)+"' && questName='"+sqlEsc(questName)+"';");
	}
	else {
		var item = new Object();
		item.itemName = itemName;
		item.itemAmount = addCount;
		item.questName = questName;
		this._questInv.push(item);
		sqlQuery("INSERT INTO questInventory (`user_id`,`itemName`,`itemAmount`,`questName`) VALUES("+this.id+",'"+sqlEsc(itemName)+"',"+sqlEsc(addCount)+",'"+sqlEsc(questName)+"');");
		getCharCols(this);
	}
	this.send(cyn+capFirstLetter(itemName)+nrm+bld+" has been placed in your quest inventory."+(addCount > 1 ? " ("+addCount+")" : "")+nrm);
	return;
}
/********************************************************
 * itemName: string representing name of item           *
 * itemAmount: amount of the item to check for           *
 * questName: quest associated with the item            *
 * Returns: true if itemAmount <= actual amount of item  *
 ********************************************************/
JSCharacter.prototype.checkQuestItem = function ( itemName, itemAmount, questName ) {
	itemName += "";
	itemAmount = Number(itemAmount);
	questName += "";
	for each ( var item in this._questInv ) {
		if ( item.itemName == itemName && item.itemAmount >= itemAmount && item.questName == questName ) {
			return true;
		}
	}
	return false;
}
JSCharacter.prototype.clearQuestInv = function () {
	this._questInv = [];
	sqlQuery("DELETE FROM questInventory WHERE user_id="+this.id+";");
	return;
}

