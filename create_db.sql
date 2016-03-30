/* Tabella degli utenti */
CREATE TABLE users (
	username VARCHAR(32) PRIMARY KEY, 
	password VARCHAR(16) NOT NULL
);

/* Trigger per avere come minimo una password di 8 caratteri */
CREATE TRIGGER pwd8
BEFORE INSERT ON users
FOR EACH ROW
BEGIN
	SELECT RAISE(ABORT, "Password must be at leasto of 8 chars")
	WHERE (length(NEW.password) < 8);
END;

-- TODO: mod deve avere un valore di default durante la creazione!
CREATE TABLE files (
	username VARCHAR(32) NOT NULL,
	path	TEXT NOT NULL,
	time_stamp DATE NOT NULL,
	mod INTEGER NOT NULL,
	file_id VARCHAR(255),
	FOREIGN KEY(username) REFERENCES users(username) ON DELETE CASCADE,
	PRIMARY KEY(username, path)
);

CREATE TABLE history (
	username VARCHAR(32) NOT NULL,
	path	TEXT NOT NULL,
	time_stamp DATE NOT NULL,
	mod INTEGER NOT NULL,
	file_id VARCHAR(255),
	FOREIGN KEY(username) REFERENCES users(username) ON DELETE CASCADE,
	PRIMARY KEY(username, path, time_stamp)
);

/* Triggers per salvare tutte le versioni di file */
CREATE TRIGGER historic_up
AFTER UPDATE ON files
FOR EACH ROW
BEGIN
	INSERT INTO history (username, path, time_stamp, mod, file_id)
	VALUES (OLD.username, OLD.path, OLD.time_stamp, OLD.mod, OLD.file_id);
END;

CREATE TRIGGER historic_del
AFTER DELETE ON files
FOR EACH ROW
BEGIN
	INSERT INTO history (username, path, time_stamp, mod, file_id)
	VALUES (OLD.username, OLD.path, OLD.time_stamp, OLD.mod, OLD.file_id);
END;
