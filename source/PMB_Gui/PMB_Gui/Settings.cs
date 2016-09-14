
using System.IO;

namespace PMB_Gui
{
    class Settings
    {
        public string watchedDir, pipeName, tempDir, settingsFileName;

        public Settings() {
            string line;
            string[] words;

            settingsFileName = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().GetName().CodeBase) + "\\..\\settings.ini";

            using (StreamReader sr = new StreamReader(
                Path.GetDirectoryName(settingsFileName)))
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
