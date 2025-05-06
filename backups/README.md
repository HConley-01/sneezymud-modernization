# SneezyMUD Backup and Restore Guide

This directory contains scripts for backing up and restoring the SneezyMUD databases and Docker container configurations.

## Backup Scripts

- `create_backup.sh`: Creates timestamped backups of the databases and container configurations
- `restore_databases.sh`: Restores the databases from backup files

## Backup Contents

When you run the `create_backup.sh` script, it will create the following files:

- `sneezy_backup_TIMESTAMP.sql`: Backup of the main SneezyMUD database
- `immortal_backup_TIMESTAMP.sql`: Backup of the immortal database
- `sneezy-mud-config_TIMESTAMP.json`: Configuration of the SneezyMUD Docker container
- `sneezy-db-config_TIMESTAMP.json`: Configuration of the MariaDB Docker container
- `docker-compose_TIMESTAMP.yml`: Docker Compose configuration file (if available)

It will also create symlinks to the latest backups:

- `sneezy_backup.sql`: Symlink to the latest sneezy database backup
- `immortal_backup.sql`: Symlink to the latest immortal database backup
- `sneezy-mud-config.json`: Symlink to the latest SneezyMUD container configuration
- `sneezy-db-config.json`: Symlink to the latest MariaDB container configuration
- `docker-compose.yml`: Symlink to the latest Docker Compose configuration file

## Restoring from Backup

### Automatic Restoration

To automatically restore the databases and restart the SneezyMUD server:

1. Make sure Docker is running
2. Run the restore script:
   ```bash
   ./restore_databases.sh
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
   docker cp sneezy_backup.sql sneezy-db:/tmp/
   docker cp immortal_backup.sql sneezy-db:/tmp/
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

## Creating New Backups

### Automatic Backup

To automatically create new backups of the current state:

1. Run the backup script:
   ```bash
   ./create_backup.sh
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
   docker cp sneezy-db:/tmp/sneezy_backup.sql ./
   docker cp sneezy-db:/tmp/immortal_backup.sql ./
   ```

2. Backup the container configurations:
   ```bash
   docker inspect sneezy-mud > sneezy-mud-config.json
   docker inspect sneezy-db > sneezy-db-config.json
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
   head -n 10 sneezy_backup.sql
   head -n 10 immortal_backup.sql
   ```

4. Make sure you have sufficient disk space:
   ```bash
   df -h
   ```

5. Check if the database is accessible:
   ```bash
   docker exec -it sneezy-db bash -c "mysql -u root -e 'SHOW DATABASES;'"
   ```

For additional help, refer to the [SneezyMUD Database Setup Guide](../docs/database/setup.md).
