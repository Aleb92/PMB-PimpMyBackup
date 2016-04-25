/* Tabella degli utenti */
CREATE TABLE users (
	username VARCHAR(32) PRIMARY KEY, 
	password VARCHAR(16) NOT NULL
);

-- TODO: mod deve avere un valore di default durante la creazione!
CREATE TABLE files (
	username VARCHAR(32) NOT NULL,
	path	TEXT NOT NULL,
	time_stamp DATE NOT NULL,
	mod INTEGER,
	file_id VARCHAR(255) DEFAULT '/dev/null',
	FOREIGN KEY(username) REFERENCES users(username) ON DELETE CASCADE,
	PRIMARY KEY(username, path)
);

CREATE TABLE history (
	username VARCHAR(32) NOT NULL,
	path	TEXT NOT NULL,
	time_stamp DATE NOT NULL,
	mod INTEGER,
	file_id VARCHAR(255),
	FOREIGN KEY(username) REFERENCES users(username) ON DELETE CASCADE,
	PRIMARY KEY(username, path, time_stamp)
);

/* Triggers per salvare tutte le versioni di file */
CREATE TRIGGER historic_up
AFTER UPDATE ON files
FOR EACH ROW
BEGIN
	INSERT OR IGNORE INTO history (username, path, time_stamp, mod, file_id)
	VALUES (OLD.username, OLD.path, OLD.time_stamp, OLD.mod, OLD.file_id);
END;

CREATE TRIGGER historic_del
AFTER DELETE ON files
FOR EACH ROW
BEGIN
	INSERT INTO history (username, path, time_stamp, mod, file_id)
	VALUES (OLD.username, OLD.path, OLD.time_stamp, OLD.mod, OLD.file_id);
END;
