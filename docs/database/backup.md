# SneezyMUD Database Backup and Restore

This document outlines the process for backing up and restoring the SneezyMUD databases in Docker.

## Overview

SneezyMUD uses two databases:

1. **sneezy** - The main database containing game data
2. **immortal** - A separate database for immortal (admin) data

Both databases need to be backed up to ensure a complete restoration is possible.

## Backup Scripts

The following scripts are provided in the `backups` directory:

1. **create_backup.sh** - Creates timestamped backups of the databases and container configurations
2. **restore_databases.sh** - Restores the databases from backup files

## Creating Backups

### Automatic Backup

To automatically create new backups of the current state:

1. Run the backup script:
   ```bash
   ./backups/create_backup.sh
   ```

The script will:
- Create timestamped backups of the databases and container configurations
- Create symlinks to the latest backups
- Display the paths to the backup files

### Manual Backup

If you prefer to create backups manually:

1. Backup the databases:
   ```bash
   docker exec -it sneezy-db bash -c "mysqldump -u root sneezy > /tmp/sneezy_backup.sql"
   docker exec -it sneezy-db bash -c "mysqldump -u root immortal > /tmp/immortal_backup.sql"
   docker cp sneezy-db:/tmp/sneezy_backup.sql ./backups/
   docker cp sneezy-db:/tmp/immortal_backup.sql ./backups/
   ```

2. Backup the container configurations:
   ```bash
   docker inspect sneezy-mud > ./backups/sneezy-mud-config.json
   docker inspect sneezy-db > ./backups/sneezy-db-config.json
   ```

## Restoring from Backup

### Automatic Restoration

To automatically restore the databases and restart the SneezyMUD server:

1. Make sure Docker is running
2. Run the restore script:
   ```bash
   ./backups/restore_databases.sh
   ```

The script will:
- Check if the backup files exist
- Check if Docker is running
- Check if the sneezy-db container is running (and start it if not)
- Copy the backup files to the container
- Restore the sneezy and immortal databases
- Grant the necessary privileges
- Restart the sneezy-mud container

### Manual Restoration

If you prefer to restore the databases manually:

1. Start the database container:
   ```bash
   docker start sneezy-db
   ```

2. Copy the backup files to the container:
   ```bash
   docker cp ./backups/sneezy_backup.sql sneezy-db:/tmp/
   docker cp ./backups/immortal_backup.sql sneezy-db:/tmp/
   ```

3. Restore the sneezy database:
   ```bash
   docker exec -it sneezy-db bash -c "mysql -u root -e 'DROP DATABASE IF EXISTS sneezy; CREATE DATABASE sneezy;'"
   docker exec -it sneezy-db bash -c "mysql -u root sneezy < /tmp/sneezy_backup.sql"
   ```

4. Restore the immortal database:
   ```bash
   docker exec -it sneezy-db bash -c "mysql -u root -e 'DROP DATABASE IF EXISTS immortal; CREATE DATABASE immortal;'"
   docker exec -it sneezy-db bash -c "mysql -u root immortal < /tmp/immortal_backup.sql"
   ```

5. Grant privileges:
   ```bash
   docker exec -it sneezy-db bash -c "mysql -u root -e 'GRANT ALL PRIVILEGES ON sneezy.* TO \"root\"@\"%\"; GRANT ALL PRIVILEGES ON immortal.* TO \"root\"@\"%\"; FLUSH PRIVILEGES;'"
   ```

6. Restart the SneezyMUD server:
   ```bash
   docker restart sneezy-mud
   ```

## Troubleshooting

If you encounter issues during restoration:

1. Check if the Docker containers are running:
   ```bash
   docker ps
   ```

2. Check the Docker logs for errors:
   ```bash
   docker logs sneezy-db
   docker logs sneezy-mud
   ```

3. Verify that the backup files are not corrupted:
   ```bash
   head -n 10 ./backups/sneezy_backup.sql
   head -n 10 ./backups/immortal_backup.sql
   ```

4. Make sure you have sufficient disk space:
   ```bash
   df -h
   ```

5. Check if the database is accessible:
   ```bash
   docker exec -it sneezy-db bash -c "mysql -u root -e 'SHOW DATABASES;'"
   ```

For additional help, refer to the [SneezyMUD Database Setup Guide](setup.md).
