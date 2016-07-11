/* Tabella degli utenti */
CREATE TABLE log (
	message TEXT
);

CREATE TABLE users (
	username VARCHAR(32) PRIMARY KEY, 
	password VARCHAR(16) NOT NULL
);

CREATE TABLE files (
	username VARCHAR(32) NOT NULL,
	path	TEXT NOT NULL,
	time_stamp DATE NOT NULL,
	mod INTEGER,
	file_id VARCHAR(255) NOT NULL,
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

CREATE VIEW dispatch_create (
	username,
	path,
	time_stamp,
	mod,
	file_id) AS
	SELECT * FROM files;

CREATE VIEW dispatch_write (
	username,
	path,
	time_stamp,
	mod,
	file_id) AS
	SELECT * FROM files;


CREATE TABLE GROUP_CHANGES (
	T CHAR NOT NULL,
	username VARCHAR(32) NOT NULL,
	path	TEXT NOT NULL,
	time_stamp DATE NOT NULL,
	new_path TEXT,
	PRIMARY KEY(username, time_stamp)
);

CREATE TABLE STAGING (
	ID DATE NOT NULL,
	username VARCHAR(32) NOT NULL,
	path	TEXT NOT NULL,
	time_stamp DATE NOT NULL,
	mod INTEGER,
	file_id VARCHAR(255)
);

CREATE VIEW CHANGE_LOOPER (
	ID,
	GROUP_CHANGES_TYPE,
	username,
	path,
	time_stamp,
	new_path)
	AS SELECT date('now'), ' ', * FROM GROUP_CHANGES WHERE FALSE;


CREATE TRIGGER write_dispatch
INSTEAD OF INSERT ON dispatch_write
FOR EACH ROW
BEGIN
	-- Piazziamo una eccezione a manetta
	SELECT RAISE(ABORT, "Write before create is forbidden!")
	WHERE NOT EXISTS (
		SELECT * FROM GROUP_CHANGES 
		WHERE 
			username = NEW.username AND
			time_stamp > NEW.time_stamp AND
			(path=NEW.path OR NEW.path LIKE (path || '\%'))
	)
	AND NOT EXISTS (
		SELECT * FROM files
		WHERE
			username = NEW.username AND
			time_stamp < NEW.time_stamp AND
			path = NEW.path
	);

	-- Queste le inserisco nel caso non ci sia move/delete
	UPDATE files
	SET time_stamp = NEW.time_stamp, file_id = NEW.file_id
	WHERE 
		username = NEW.username AND 
		path = NEW.path 
		AND NOT EXISTS (
			SELECT * FROM GROUP_CHANGES 
			WHERE 
				username = NEW.username AND
				time_stamp > NEW.time_stamp AND
				(path=NEW.path OR NEW.path LIKE (path || '\%'))
		);

	-- Se c'è stata una delete, allora questo deve finire nella history
	INSERT INTO history (username, path, time_stamp, file_id)
	SELECT NEW.username, NEW.path, NEW.time_stamp, NEW.file_id 
	WHERE EXISTS
		(SELECT * FROM GROUP_CHANGES 
		 WHERE 
			username = NEW.username AND
			time_stamp > NEW.time_stamp AND
			(path=NEW.path OR NEW.path LIKE (path || '\%')));

	UPDATE files SET file_id = NEW.file_id
	WHERE 
		username = NEW.username AND 
		time_stamp > NEW.time_stamp AND 
		file_id = (
			SELECT file_id 
			FROM history
			WHERE
				username = NEW.username AND
				time_stamp < NEW.time_stamp AND
				path = NEW.path
			ORDER BY time_stamp DESC LIMIT 1
		);
		
	UPDATE history SET file_id = NEW.file_id
	WHERE 
		username = NEW.username AND 
		time_stamp > NEW.time_stamp AND 
		file_id = (
			SELECT file_id 
			FROM history
			WHERE
				username = NEW.username AND
				time_stamp < NEW.time_stamp AND
				path = NEW.path
			ORDER BY time_stamp DESC LIMIT 1
		);
END;

CREATE TRIGGER historic_dispatch_redo
AFTER INSERT ON GROUP_CHANGES
FOR EACH ROW
BEGIN
	INSERT INTO log (message) VALUES ("historic_dispatch_redo");
	
	INSERT INTO 
		CHANGE_LOOPER (ID, GROUP_CHANGES_TYPE, username, path, time_stamp, new_path)
		SELECT 0, * 
		FROM GROUP_CHANGES 
		WHERE time_stamp >= NEW.time_stamp AND username = NEW.username
		ORDER BY time_stamp ASC;
END;


CREATE TRIGGER history_delete_new
AFTER DELETE ON files
FOR EACH ROW
BEGIN
	INSERT INTO log (message) VALUES ("history_delete_new");
	
	INSERT INTO history (username, path, time_stamp, mod, file_id)
	VALUES (OLD.username, OLD.path, OLD.time_stamp, OLD.mod, OLD.file_id);
END;

CREATE TRIGGER history_update_ok
AFTER UPDATE ON files
FOR EACH ROW
WHEN (OLD.time_stamp < NEW.time_stamp)
BEGIN
	INSERT INTO log (message) VALUES ("history_update_ok");
	INSERT INTO history (username, path, time_stamp, mod, file_id)
	VALUES (OLD.username, OLD.path, OLD.time_stamp, OLD.mod, OLD.file_id);
END;

CREATE TRIGGER history_update_ignore
BEFORE UPDATE ON files
FOR EACH ROW
WHEN (OLD.time_stamp > NEW.time_stamp)
BEGIN
	INSERT INTO log (message) VALUES ("history_update_ignore");

	INSERT INTO history (username, path, time_stamp, mod, file_id)
	VALUES (NEW.username, NEW.path, NEW.time_stamp, NEW.mod, NEW.file_id);
	
	SELECT RAISE(IGNORE);
END;

CREATE TRIGGER historic_dispatch_create
INSTEAD OF INSERT ON dispatch_create
FOR EACH ROW
BEGIN
	INSERT INTO log (message) VALUES ("historic_dispatch_create");
	-- Inserisco nella coda di esecuzione.
	INSERT INTO
		STAGING (ID, username, path, time_stamp, mod, file_id) 
	VALUES 
		(NEW.time_stamp, NEW.username, NEW.path, NEW.time_stamp, NEW.mod, COALESCE(NEW.file_id, '../null'));

	-- Do i comandi da eseguire
	INSERT INTO 
		CHANGE_LOOPER (ID, GROUP_CHANGES_TYPE, username, path, time_stamp, new_path)
		SELECT NEW.time_stamp, * 
		FROM GROUP_CHANGES 
		WHERE time_stamp > NEW.time_stamp AND username = NEW.username
		ORDER BY time_stamp ASC;
	
	INSERT INTO 
		CHANGE_LOOPER (ID, GROUP_CHANGES_TYPE, username, path, time_stamp)
		VALUES (NEW.time_stamp, 'f', NEW.username, '', 9223372036854775807);
END;

CREATE TRIGGER finalize
INSTEAD OF INSERT ON CHANGE_LOOPER
FOR EACH ROW
WHEN(NEW.GROUP_CHANGES_TYPE='f')
BEGIN
	INSERT INTO log (message) VALUES ("finalize");
	
	UPDATE files 
	SET 
		time_stamp = (SELECT time_stamp FROM STAGING WHERE ID = NEW.ID AND username = NEW.username),
		file_id = (SELECT file_id FROM STAGING WHERE ID = NEW.ID AND username = NEW.username)
	WHERE
		username = NEW.username AND path=(SELECT path FROM STAGING WHERE ID = NEW.ID AND username = NEW.username);
	
	-- Inserisco il risultato, se c'è
	INSERT OR IGNORE INTO files
		SELECT username, path, time_stamp, mod, COALESCE(file_id, '../null')
		FROM STAGING 
		WHERE ID = NEW.ID AND username = NEW.username;
	DELETE FROM STAGING WHERE ID = NEW.ID AND username = NEW.username;
END;

-- Apporta le modifiche
CREATE TRIGGER apply_create_move
INSTEAD OF INSERT ON CHANGE_LOOPER
FOR EACH ROW
WHEN (NEW.GROUP_CHANGES_TYPE = 'm' AND NEW.ID <> 0)
BEGIN
	INSERT INTO log (message) VALUES ("apply_create_move");
	
	-- Applico la move
	INSERT INTO history 
		SELECT username, path, time_stamp, mod, file_id
		FROM STAGING 
		WHERE ID = NEW.ID AND username = NEW.username AND 
			(path=NEW.path OR path LIKE (NEW.path || '\%'));
	
	-- Aggiorno nel caso di files singoli
	UPDATE STAGING SET path=NEW.new_path, time_stamp=NEW.time_stamp
	WHERE ID = NEW.ID AND username = NEW.username AND 
				path=NEW.path;

	-- Aggiorno nel caso delle cartelle
	UPDATE STAGING SET path=REPLACE('*' || path, '*' || NEW.path, NEW.new_path), time_stamp = NEW.time_stamp
		WHERE ID = NEW.ID AND username = NEW.username AND 
			path LIKE (NEW.path || '\%');
END;

-- Apporta le modifiche alle create
CREATE TRIGGER apply_create_delete
INSTEAD OF INSERT ON CHANGE_LOOPER
FOR EACH ROW
WHEN (NEW.GROUP_CHANGES_TYPE = 'd' AND NEW.ID <> 0)
BEGIN
	INSERT INTO log (message) VALUES ("apply_create_delete");
	
	-- Salvo il file come history
	INSERT INTO history 
		SELECT username, path, time_stamp, mod, file_id
			FROM STAGING 
			WHERE ID = NEW.ID AND username = NEW.username AND 
				(path=NEW.path OR path LIKE (NEW.path || '\%'));
	
	-- NO MORE.
	DELETE FROM STAGING
	WHERE ID = NEW.ID AND username = NEW.username AND 
		(path=NEW.path OR path LIKE (NEW.path || '\%'));
END;

CREATE TRIGGER apply_live_move
INSTEAD OF INSERT ON CHANGE_LOOPER
FOR EACH ROW
WHEN (NEW.GROUP_CHANGES_TYPE = 'm' AND NEW.ID = 0)
BEGIN
	INSERT INTO log (message) VALUES ("apply_live_move");
	
	-- Aggiorno nel caso di files singoli
	UPDATE files SET path=NEW.new_path, time_stamp=NEW.time_stamp
	WHERE username = NEW.username AND 
		path=NEW.path AND time_stamp < NEW.time_stamp;

	-- Aggiorno nel caso delle cartelle
	UPDATE files SET path=REPLACE('*' || path, '*' || NEW.path, NEW.new_path), time_stamp = NEW.time_stamp
	WHERE username = NEW.username AND 
		path LIKE (NEW.path || '\%') AND time_stamp < NEW.time_stamp;
END;

-- Apporta le modifiche alle create
CREATE TRIGGER apply_live_delete
INSTEAD OF INSERT ON CHANGE_LOOPER
FOR EACH ROW
WHEN (NEW.GROUP_CHANGES_TYPE = 'd' AND NEW.ID = 0)
BEGIN
	INSERT INTO log (message) VALUES ("apply_live_delete");
	
	DELETE FROM files 
	WHERE username = NEW.username AND 
		(path=NEW.path OR path LIKE (NEW.path || '\%')) 
		AND time_stamp < NEW.time_stamp;
END;

INSERT INTO users VALUES ('root', 'toor');
INSERT INTO users VALUES ('hhh', 'ppp');