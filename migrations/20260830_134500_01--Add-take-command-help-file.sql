INSERT INTO `helpFile` (
  `name`, `syntax`, `description`, `parent_id`, `keywords`,
  `created_by_user_id`, `created_datetime`, `last_modified_by_user_id`, `last_modified_datetime`
) VALUES (
  'Take',
  'take <item>, take <item> <container>',
  '(function() {
    
    var buffer = "The " + bld + cyn + "TAKE" + nrm + " command is used to pick up an item from your current room and place it into your " + bld + cyn + "INVENTORY" + nrm + ". To do so, you must specify the item you wish to take. For example, the command " + bld + yel + "take sword" + nrm + " will pick up the first item with the keyword \'sword\' lying in your room. The " + bld + cyn + "GET" + nrm + " command does exactly the same thing.\\n\\n";
    
    buffer += "If there are many items with the same keyword in the room, you can take all of them with a single command. For example, entering the command " + bld + yel + "take all.sword" + nrm + " will cause you to pick up all of the items with the keyword \'sword\'. You can also specify which individual item to take. For example, entering " + bld + yel + "take 3.sword" + nrm + " will cause you to pick up the third sword in the room. You can take everything in the room by entering the command " + bld + yel + "take all" + nrm + ".\\n\\n";
    
    buffer += "To take an item out of a container, name the container as a second argument. For example, entering " + bld + yel + "take sword sack" + nrm + " will take the first item named \'sword\' out of the first item named \'sack\'. The container may be in your inventory, worn as equipment, or lying in your room, and it must be open. To take the coins out of a container or a corpse, enter " + bld + yel + "take coins corpse" + nrm + ".\\n\\n";
    
    buffer += "You can only carry so many items at once. If your arms are already full, you will need to drop something or put it into a container before you can take anything else.\\n\\n";
    
    buffer += "To drop an item from your inventory, use the " + bld + cyn + "DROP" + nrm + " command.\\n\\n";
    
    buffer += "To put an item into a container, use the " + bld + cyn + "PUT" + nrm + " command.\\n\\n";
    
    buffer += "To see your current inventory, use the " + bld + cyn + "INVENTORY" + nrm + " command.";
    
    return strColFormat(buffer, 66);
})();',
  610,
  'take, tak, ta, get, g',
  1, NOW(), 1, NOW()
);
