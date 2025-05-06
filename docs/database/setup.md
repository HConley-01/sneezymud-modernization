Pleasedaddy# SneezyMUD Database Setup Guide

This document outlines the process for setting up and troubleshooting the SneezyMUD database in Docker.

## Database Structure

SneezyMUD uses two databases:

1. **sneezy** - The main database containing game data
2. **immortal** - A separate database for immortal (admin) data

## Initial Setup

When setting up SneezyMUD for the first time, follow these steps:

1. Create both databases:
   ```sql
   CREATE DATABASE IF NOT EXISTS sneezy;
   CREATE DATABASE IF NOT EXISTS immortal;
   GRANT ALL PRIVILEGES ON sneezy.* TO 'root'@'%';
   GRANT ALL PRIVILEGES ON immortal.* TO 'root'@'%';
   FLUSH PRIVILEGES;
   ```

2. Initialize the immortal database tables:
   ```bash
   # Create tables
   for file in /home/mint/sneezymud-dev/_Setup-data/sql_tables/immortal/*.sql; do
     docker exec -i sneezy-db bash -c "mysql -u root immortal" < "$file"
   done

   # Load data
   for file in /home/mint/sneezymud-dev/_Setup-data/sql_data/immortal/*.sql; do
     docker exec -i sneezy-db bash -c "mysql -u root immortal" < "$file"
   done
   ```

3. Create a configuration table in the immortal database:
   ```sql
   CREATE TABLE configuration (
     id int primary key auto_increment not null,
     config varchar(100) unique not null,
     value varchar(999) null
   );
   INSERT INTO configuration (config, value) VALUES ("version", "0");
   ```

## Troubleshooting Database Migration Issues

If the server crashes during startup with errors related to database migrations, follow these steps:

### Issue: Server crashes when trying to create tables that already exist

The server uses migrations to create and update database tables. If the migrations fail, the server will crash.

#### Solution 1: Reset the migration state

1. Drop the configuration table to allow migrations to start from scratch:
   ```sql
   DROP TABLE configuration;
   ```

2. Restart the server:
   ```bash
   docker restart sneezy-mud
   ```

#### Solution 2: Fix specific migration issues

If specific migrations are failing, you may need to:

1. Check which tables the migrations are trying to create:
   ```bash
   grep -A 10 "create table" /home/mint/sneezymud-dev/code/code/sys/migrations.cc
   ```

2. Drop problematic tables that are causing conflicts:
   ```sql
   DROP TABLE IF EXISTS alias;
   DROP TABLE IF EXISTS globaltweaks;
   DROP TABLE IF EXISTS savedrooms;
   DROP TABLE IF EXISTS savedroomsacct;
   DROP TABLE IF EXISTS accountnotes;
   DROP TABLE IF EXISTS playernotes;
   DROP TABLE IF EXISTS wizdata;
   ```

3. Reset the migration version:
   ```sql
   UPDATE configuration SET value = "0" WHERE config = "version";
   ```

4. Restart the server:
   ```bash
   docker restart sneezy-mud
   ```

## Migration Details

The migrations in `migrations.cc` perform the following operations:

1. **Migration 0**: Creates the configuration table
2. **Migration 1**: Creates the alias table for player aliases
3. **Migration 2**: Creates the globaltweaks table
4. **Migration 3**: Adds multiplay_limit column to account table
5. **Migration 4**: Renames Test Code 6 to DB Logging in globaltoggles
6. **Migration 5**: Creates the savedrooms table
7. **Migration 6**: Creates savedroomsacct table and migrates data from savedrooms
8. **Migration 7**: Creates accountnotes and playernotes tables
9. **Migration 8**: Creates wizdata table and migrates data from files

## Verifying Database Setup

To verify that the database is set up correctly:

1. Check the migration version:
   ```sql
   SELECT * FROM configuration WHERE config = "version";
   ```
   The value should be `9` if all migrations have been applied.

2. Verify that all required tables exist:
   ```sql
   SHOW TABLES;
   ```

3. Check that the server is running without errors:
   ```bash
   docker logs --tail 50 sneezy-mud
   ```
   Look for "Boot -- DONE." and "Entering game loop." messages.

## Common Issues

### Missing Tables

If the server crashes because it can't find a table, check if the table exists:

```sql
SHOW TABLES LIKE "table_name";
```

If the table doesn't exist, you may need to create it manually or reset the migrations.

### Foreign Key Constraints

If migrations fail with foreign key constraint errors, check the data types of the columns:

```sql
DESCRIBE table_name;
```

Make sure the data types match between the referenced columns.

### Account Creation Issues

If account creation fails, check that all required tables exist, especially:
- account
- player
- alias

## Backup and Restore

### Creating Backups

A backup script is provided in the `backups` directory to create timestamped backups of the databases and container configurations:

```bash
./backups/create_backup.sh
```

This will create backups of:
- The sneezy database
- The immortal database
- The Docker container configurations
- The docker-compose file (if available)

### Restoring from Backup

To restore the databases and restart the SneezyMUD server:

```bash
./backups/restore_databases.sh
```

This will:
- Restore the sneezy and immortal databases from the backup files
- Grant the necessary privileges
- Restart the SneezyMUD server

For more detailed information about the backup and restore process, see the README.md file in the `backups` directory.
