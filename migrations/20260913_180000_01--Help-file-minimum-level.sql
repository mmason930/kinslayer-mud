-- Apply before deploying the help scripts. Existing files remain public.
ALTER TABLE `helpFile`
  ADD COLUMN `minimum_level` TINYINT UNSIGNED NOT NULL DEFAULT 0;
