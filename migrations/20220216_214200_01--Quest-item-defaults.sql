ALTER TABLE `questItem`
MODIFY `retoolExtraDesc` varchar(500) NOT NULL DEFAULT '',
MODIFY `slot_id` varchar(100) NOT NULL DEFAULT '',
MODIFY `slot_tier` tinyint NOT NULL DEFAULT 0;