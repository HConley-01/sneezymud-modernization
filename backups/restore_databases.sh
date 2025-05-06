#!/bin/bash
# Script to restore SneezyMUD databases from backup

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SNEEZY_BACKUP="$SCRIPT_DIR/sneezy_backup.sql"
IMMORTAL_BACKUP="$SCRIPT_DIR/immortal_backup.sql"

# Check if backup files exist
if [ ! -f "$SNEEZY_BACKUP" ]; then
    echo "Error: Sneezy database backup file not found at $SNEEZY_BACKUP"
    exit 1
fi

if [ ! -f "$IMMORTAL_BACKUP" ]; then
    echo "Error: Immortal database backup file not found at $IMMORTAL_BACKUP"
    exit 1
fi

# Check if Docker is running
if ! docker info > /dev/null 2>&1; then
    echo "Error: Docker is not running or you don't have permission to use it"
    exit 1
fi

# Check if sneezy-db container is running
if ! docker ps | grep -q sneezy-db; then
    echo "Error: sneezy-db container is not running"
    echo "Starting sneezy-db container..."
    docker start sneezy-db || {
        echo "Failed to start sneezy-db container"
        exit 1
    }
    # Wait for container to start
    sleep 5
fi

echo "Copying backup files to container..."
docker cp "$SNEEZY_BACKUP" sneezy-db:/tmp/
docker cp "$IMMORTAL_BACKUP" sneezy-db:/tmp/

echo "Restoring sneezy database..."
docker exec -it sneezy-db bash -c "mysql -u root -e 'DROP DATABASE IF EXISTS sneezy; CREATE DATABASE sneezy;'"
docker exec -it sneezy-db bash -c "mysql -u root sneezy < /tmp/sneezy_backup.sql"

echo "Restoring immortal database..."
docker exec -it sneezy-db bash -c "mysql -u root -e 'DROP DATABASE IF EXISTS immortal; CREATE DATABASE immortal;'"
docker exec -it sneezy-db bash -c "mysql -u root immortal < /tmp/immortal_backup.sql"

echo "Granting privileges..."
docker exec -it sneezy-db bash -c "mysql -u root -e 'GRANT ALL PRIVILEGES ON sneezy.* TO \"root\"@\"%\"; GRANT ALL PRIVILEGES ON immortal.* TO \"root\"@\"%\"; FLUSH PRIVILEGES;'"

echo "Database restoration complete!"

# Restart sneezy-mud container if it exists
if docker ps -a | grep -q sneezy-mud; then
    echo "Restarting sneezy-mud container..."
    docker restart sneezy-mud
    echo "SneezyMUD server restarted!"
else
    echo "Warning: sneezy-mud container not found. You may need to start it manually."
fi

echo "Restoration process completed successfully!"
