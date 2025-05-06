#!/bin/bash
# Script to create backups of SneezyMUD databases and container configurations

set -e  # Exit on error

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
TIMESTAMP=$(date +"%Y%m%d_%H%M%S")

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

echo "Creating database backups..."
docker exec -it sneezy-db bash -c "mysqldump -u root sneezy > /tmp/sneezy_backup.sql"
docker exec -it sneezy-db bash -c "mysqldump -u root immortal > /tmp/immortal_backup.sql"

echo "Copying backups from container..."
docker cp sneezy-db:/tmp/sneezy_backup.sql "$SCRIPT_DIR/sneezy_backup_$TIMESTAMP.sql"
docker cp sneezy-db:/tmp/immortal_backup.sql "$SCRIPT_DIR/immortal_backup_$TIMESTAMP.sql"

echo "Creating container configuration backups..."
docker inspect sneezy-mud > "$SCRIPT_DIR/sneezy-mud-config_$TIMESTAMP.json"
docker inspect sneezy-db > "$SCRIPT_DIR/sneezy-db-config_$TIMESTAMP.json"

# Copy docker-compose file if it exists
if [ -f "../docker-compose.yml" ]; then
    cp "../docker-compose.yml" "$SCRIPT_DIR/docker-compose_$TIMESTAMP.yml"
fi

echo "Creating symlinks to latest backups..."
ln -sf "sneezy_backup_$TIMESTAMP.sql" "$SCRIPT_DIR/sneezy_backup.sql"
ln -sf "immortal_backup_$TIMESTAMP.sql" "$SCRIPT_DIR/immortal_backup.sql"
ln -sf "sneezy-mud-config_$TIMESTAMP.json" "$SCRIPT_DIR/sneezy-mud-config.json"
ln -sf "sneezy-db-config_$TIMESTAMP.json" "$SCRIPT_DIR/sneezy-db-config.json"
if [ -f "$SCRIPT_DIR/docker-compose_$TIMESTAMP.yml" ]; then
    ln -sf "docker-compose_$TIMESTAMP.yml" "$SCRIPT_DIR/docker-compose.yml"
fi

echo "Backup completed successfully!"
echo "Backup files:"
echo "  - $SCRIPT_DIR/sneezy_backup_$TIMESTAMP.sql"
echo "  - $SCRIPT_DIR/immortal_backup_$TIMESTAMP.sql"
echo "  - $SCRIPT_DIR/sneezy-mud-config_$TIMESTAMP.json"
echo "  - $SCRIPT_DIR/sneezy-db-config_$TIMESTAMP.json"
if [ -f "$SCRIPT_DIR/docker-compose_$TIMESTAMP.yml" ]; then
    echo "  - $SCRIPT_DIR/docker-compose_$TIMESTAMP.yml"
fi
