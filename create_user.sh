#! /bin/bash

#Carico le impostazioni
source "server_settings.ini"

echo "INSERT INTO users (username, password, lastSync) VALUES ('$1', '$2', 0);" | sqlite3 $db_name
mkdir -p "${save_folder}$1"