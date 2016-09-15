using System;
using System.IO;

namespace PMB_Gui
{
    public class Settings
    {
        public string watchedDir, pipeName, tempDir, settingsFileName, server_ip;
        public int server_port;

        public Settings() {
            string line;
            string[] words;

            settingsFileName = @"C:\Users\mrcmn\workspace\PMB\client_settings.ini";

            using (StreamReader sr = new StreamReader(
                settingsFileName))
            {
                line = sr.ReadLine();

                while (line != null)
                {
                    words = line.Split('=');
                    switch (words[0])
                    {
                        case "watched_dir":
                            watchedDir = words[1];
                            break;

                        case "pipe_name":
                            pipeName = words[1];
                            break;

                        case "temp_dir":
                            tempDir = words[1];
                            break;

                        case "server_port":
                            server_port = Int32.Parse(words[1]);
                            break;

                        case "server_host":
                            server_ip = words[1];
                            break;

                        default: break;
                    }

                    line = sr.ReadLine();
                }
            }
        }

        public void resetCredential(string username, string password) {

            using (StreamWriter sw = new StreamWriter(
                settingsFileName, true))
            {
                sw.WriteLine("username="+ username);
                sw.WriteLine("password=" + password);
            }
        }

        public void resetWatchedDir(string newWatchedDir) {

            using (StreamWriter sw = new StreamWriter(
                settingsFileName, true))
            {
                sw.WriteLine("watched_dir=" + newWatchedDir);
                watchedDir = newWatchedDir;
            }
        }
    }
}
