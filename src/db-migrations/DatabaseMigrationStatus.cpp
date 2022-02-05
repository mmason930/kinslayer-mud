#include "DatabaseMigrationStatus.h"

DatabaseMigrationStatus *DatabaseMigrationStatus::PENDING = new DatabaseMigrationStatus(0, "PENDING");
DatabaseMigrationStatus *DatabaseMigrationStatus::RUNNING = new DatabaseMigrationStatus(1, "RUNNING");
DatabaseMigrationStatus *DatabaseMigrationStatus::FAILED = new DatabaseMigrationStatus(2, "FAILED");
DatabaseMigrationStatus *DatabaseMigrationStatus::EXECUTED = new DatabaseMigrationStatus(3, "EXECUTED");
