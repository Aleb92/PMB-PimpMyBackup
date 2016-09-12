PRAGMA foreign_keys = ON;
PRAGMA recursive_triggers = true;
/* INSERT INTO files (username, path, time_stamp, file_id) VALUES ("root", "dir\a.txt", 0, '0');
INSERT INTO files (username, path, time_stamp, file_id) VALUES ("root", "a.txt", 2, '2');
INSERT INTO files (username, path, time_stamp, file_id) VALUES ("root", "b.txt", 1, '1');
INSERT INTO files (username, path, time_stamp, file_id) VALUES ("hhh", "dir\a.txt", 0, '0');
INSERT INTO files (username, path, time_stamp, file_id) VALUES ("hhh", "a.txt", 2, '2');
INSERT INTO files (username, path, time_stamp, file_id) VALUES ("hhh", "b.txt", 1, '1');

INSERT INTO GROUP_CHANGES (T, username, path, time_stamp) VALUES ('d', 'root', 'ppp', 11);
INSERT INTO GROUP_CHANGES (T, username, path, time_stamp, new_path) VALUES ('m', 'root', 'dir', 10, 'ppp');
INSERT INTO GROUP_CHANGES (T, username, path, time_stamp) VALUES ('d', 'root', 'dir\c', 9);
INSERT INTO GROUP_CHANGES (T, username, path, time_stamp, new_path) VALUES ('m', 'root', 'dir\v', 8, 'dir\c');
INSERT INTO files (username, path, time_stamp, file_id) VALUES ("root", "dir\v", 6, '6');
INSERT INTO files (username, path, time_stamp, file_id) VALUES ("root", "dir\b", 5, '5');

UPDATE users SET lastSync = 12 WHERE username = 'root';


INSERT INTO dispatch_write (username, path, time_stamp, file_id) VALUES ("root", "dir\v", 7, "myID");



INSERT INTO files (username, path, time_stamp, file_id) VALUES ("root", "b\b.txt", 1, '1');
INSERT INTO GROUP_CHANGES (T, username, path, time_stamp, new_path) VALUES ('m', 'root', 'b', 10, 'ppp');
INSERT INTO GROUP_CHANGES (T, username, path, time_stamp, new_path) VALUES ('m', 'root', 'b\b.txt', 9, 'b\ppp');
INSERT INTO GROUP_CHANGES (T, username, path, time_stamp) VALUES ('d', 'root', 'b\b.txt', 5);

INSERT INTO dispatch_write (username, path, time_stamp, file_id) VALUES ("root", "b\b.txt", 7, "myID");
INSERT INTO files (username, path, time_stamp, file_id) VALUES ("root", "b\b.txt", 6, '6');
UPDATE users SET lastSync = 12 WHERE username = 'root';

INSERT INTO dispatch_write (username, path, time_stamp, file_id) VALUES ("root", "a.txt", 3, "myID");
INSERT INTO files (username, path, time_stamp, file_id) VALUES ("root", "a.txt", 2, '2');


*/

INSERT INTO files (username, path, time_stamp, file_id) VALUES ("root", "a", 1, '1');
INSERT INTO files (username, path, time_stamp, file_id) VALUES ("root", "tmp", 2, '1');
INSERT INTO dispatch_write (username, path, time_stamp, file_id) VALUES ("root", "tmp", 3, "fjasofasj");
INSERT INTO GROUP_CHANGES (T, username, path, time_stamp, new_path) VALUES ('m', 'root', 'tmp', 10, 'a');
UPDATE users SET lastSync = 12 WHERE username = 'root';

