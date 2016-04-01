/* AUTH */
SELECT password WHERE username=?1;

/* CREATE! */
INSERT INTO files (username, path, time_stamp) VALUES(?1,?2,?3);

/* CHMOD */
UPDATE files SET time_stamp=?3, mod=?4 WHERE username=?1 AND path=?2;

/* WRITE */
UPDATE files SET time_stamp=?3, file_id=?4 WHERE username=?1 AND path=?2;

/* MOVE */ 
UPDATE files SET time_stamp=?3, path=?4 WHERE username=?1 AND path=?2;

/* DELETE */
DELETE FROM files WHERE username=?1 AND path=?2;

/* VERSION CHANGE */
WITH tmp AS (
	SELECT time_stamp, mod, file_id FROM history WHERE WHERE username=?1 AND path=?2 AND time_stamp=?3
)
UPDATE files SET
	time_stamp=(SELECT time_stamp FROM tmp),mod=(SELECT mod FROM tmp),file_id=(SELECT file_id FROM tmp) WHERE username=?1 AND path=?2;

/* LIST VERSIONS */
SELECT time_stamp FROM history WHERE username=?1 AND path=?2 ORDER BY time_stamp DESC;
