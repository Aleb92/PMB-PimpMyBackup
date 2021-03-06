﻿using System;
using System.IO;
using System.Threading;

namespace PMB_Gui
{
    public class Settings
    {
        public string watchedDir, pipeName, tempDir, settingsFileName, server_ip, username, password;
        public int server_port;
        internal string logFileName;

        public Settings() {
            string line;
            string[] words;

            settingsFileName = Directory.GetCurrentDirectory() + "\\client_settings.ini";

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

                        case "username":
                            username = words[1];
                            break;

                        case "password":
                            password = words[1];
                            break;

                        case "log_filename":
                            logFileName = words[1];
                            break;

                        default: break;
                    }

                    line = sr.ReadLine();
                }
            }
        }

        public void resetCredentials(string username, string password) {

            using (StreamWriter sw = new StreamWriter(
                settingsFileName, true))
            {
                sw.WriteLine("username="+ username);
                sw.WriteLine("password=" + password);

                this.username = username;
                this.password = password; 
            }
        }

        public void resetWatchedDir(string newWatchedDir) {
            while(true)
                try
                {
                    using (StreamWriter sw = new StreamWriter(
                        settingsFileName, true))
                    {
                        sw.WriteLine("watched_dir=" + newWatchedDir);
                        watchedDir = newWatchedDir;
                        App.CurrentApp.Dispatcher.Invoke(delegate
                        {
                            App.ActiveWindow.versions.watched_dir.Content = newWatchedDir;
                        });
                        
                    }
                    return;
                }catch(IOException){
                    Thread.Sleep(500);
                }
        }
    }
}
