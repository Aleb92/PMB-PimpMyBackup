/* AUTH */
SELECT username, password WHERE username=?;

/* CREATE! */
INSERT INTO files (username, path, time_stamp) VALUES(?,?,?);

/* CHMOD */
UPDATE files SET time_stamp=?, mod=? WHERE username=? AND path=?;

/* WRITE */
UPDATE files SET time_stamp=?, file_id=? WHERE username=? AND path=?;

/* MOVE */ 
UPDATE files SET time_stamp=?, path=? WHERE username=? AND path=?;

/* DELETE */
DELETE FROM files WHERE username=? AND path=?;

/* VERSION CHANGE */
WITH tmp AS (
	SELECT time_stamp, mod, file_id FROM history WHERE WHERE username=?1 AND path=?2 AND time_stamp=?3
)
UPDATE files SET
	time_stamp=(SELECT time_stamp FROM tmp),mod=(SELECT mod FROM tmp),file_id=(SELECT file_id FROM tmp) WHERE username=?1 AND path=?2;

/* LIST VERSIONS */
SELECT time_stamp FROM history WHERE username=? AND path=?;
