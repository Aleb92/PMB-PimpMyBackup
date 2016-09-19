using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.ServiceProcess;
using System.Text;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;
using System.Threading;

namespace PMB_Gui
{
    /// <summary>
    /// Logica di interazione per Versions.xaml
    /// </summary>
    public partial class Versions : Page
    {
        private static readonly ImageSource 
            folder = Imaging.CreateBitmapSourceFromHBitmap(
                        Properties.Resources.folder.GetHbitmap(), 
                        IntPtr.Zero, Int32Rect.Empty, 
                        System.Windows.Media.Imaging.BitmapSizeOptions.FromEmptyOptions()),
            file = Imaging.CreateBitmapSourceFromHBitmap(
                        Properties.Resources.file.GetHbitmap(),
                        IntPtr.Zero, Int32Rect.Empty, 
                        System.Windows.Media.Imaging.BitmapSizeOptions.FromEmptyOptions());

        private string currentFilename;

        public Versions()
        {
            InitializeComponent();
        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            LoadDirsAndFiles();
            watched_dir.Content = App.CurrentApp.settings.watchedDir;
        }

        private StackPanel IconAndText(ImageSource IS, string text)
        {
            StackPanel sp = new StackPanel
            {
                Orientation = Orientation.Horizontal
            };

            sp.Children.Add(new Image
            {
                Source = IS,
                Width = 16,
                Height = 16
            });

            sp.Children.Add(new Label
            {
                Content = text
            });

            return sp;
        }

        internal void LoadDirsAndFiles()
        {
            FileView.Items.Clear();
            if(Directory.Exists(App.CurrentApp.settings.watchedDir))
                LoadDirsAndFiles(FileView.Items, App.CurrentApp.settings.watchedDir);
        }

        private void LoadDirsAndFiles(ItemCollection ic, string path)
        {
            foreach (string s in Directory.GetDirectories(path))
            {
                TreeViewItem item = new TreeViewItem
                {
                    Header = IconAndText(folder, s.Substring(path.Length)),
                    FontWeight = FontWeights.Normal
                };

                ic.Add(item);
                LoadDirsAndFiles(item.Items, s);
            }

            foreach (string s in Directory.GetFiles(path))
            {
                TreeViewItem item = new TreeViewItem
                {
                    Header = IconAndText(file, s.Substring(path.Length)),
                    Tag = s.Substring(App.CurrentApp.settings.watchedDir.Length),
                    FontWeight = FontWeights.Normal
                };

                item.Selected += File_Selected;
                ic.Add(item);
            }
        }

        private void Change_Dir(object sender, RoutedEventArgs e)
        {
            App.ActiveWindow.ShowLoadDialog("Updating backup directory...");

            string newWatchedDir;

            System.Windows.Forms.FolderBrowserDialog folderDialog = new System.Windows.Forms.FolderBrowserDialog();
            folderDialog.SelectedPath = App.CurrentApp.settings.watchedDir;

        retry:
            if (folderDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                newWatchedDir = PathAddBackslash(folderDialog.SelectedPath);
                if (Directory.EnumerateFileSystemEntries(newWatchedDir).Any())
                {
                    MessageBox.Show("The folder is not empty. Please choose an empty one.", "Warning", MessageBoxButton.OK, MessageBoxImage.Warning);
                    goto retry;
                }

                Task.Run(delegate
                {
                    try
                    {
                        List<Tuple<string, long>> files;
                        using (Socket sock = new Socket(SocketType.Stream, ProtocolType.IP))
                        {
                            sock.Connect(new IPEndPoint(IPAddress.Parse(App.CurrentApp.settings.server_ip), App.CurrentApp.settings.server_port));
                            using (NetworkStream ns = new NetworkStream(sock))
                            {
                                using (BinaryWriter bw = new BinaryWriter(ns))
                                {
                                    using (BinaryReader br = new BinaryReader(ns))
                                    {
                                        bw.Write(IPAddress.HostToNetworkOrder(App.CurrentApp.settings.username.Length));
                                        bw.Write(Encoding.UTF8.GetBytes(App.CurrentApp.settings.username));

                                        bw.Write(IPAddress.HostToNetworkOrder(App.CurrentApp.settings.password.Length));
                                        bw.Write(Encoding.UTF8.GetBytes(App.CurrentApp.settings.password));

                                        bw.Write((int)0);

                                        //Auth ok o no?
                                        if (!br.ReadBoolean())
                                        {
                                            Dispatcher.Invoke(delegate
                                            {
                                                App.CurrentApp.ConnAvailable = false;
                                                App.CurrentApp.ni.Icon = Properties.Resources.icon_error;
                                                App.ActiveWindow.ShowLogin();
                                            });
                                        }
                                        else
                                        {
                                            Dispatcher.Invoke(delegate
                                            {
                                                App.CurrentApp.ConnAvailable = true;
                                                App.ActiveWindow.ShowVersions();
                                            });
                                        }

                                        byte opcode = 0;
                                        bw.Write(opcode);

                                        int size = IPAddress.NetworkToHostOrder(br.ReadInt32());

                                        files = new List<Tuple<string, long>>(size);

                                        for (; size > 0; size--)
                                        {
                                            int length = IPAddress.NetworkToHostOrder(br.ReadInt32());
                                            files.Add(new Tuple<string, long>(
                                                Encoding.UTF8.GetString(br.ReadBytes(length)),
                                                IPAddress.NetworkToHostOrder(br.ReadInt64())
                                                ));
                                        }
                                    }
                                }
                            }
                        }
                        App.CurrentApp.stopService();
                        while (true)
                            try
                            {
                                File.Delete(App.CurrentApp.settings.logFileName);
                                break;
                            }
                            catch (IOException) {
                                Thread.Sleep(500);
                            }

                        Directory.Delete(App.CurrentApp.settings.watchedDir, true);
                        Directory.Delete(App.CurrentApp.settings.tempDir, true);
                        

                        App.CurrentApp.settings.resetWatchedDir(newWatchedDir);

                        App.CurrentApp.startService();

                        foreach (var tupla in files)
                            App.CurrentApp.pipe.selectVersion(tupla.Item1, tupla.Item2);
                    }
                    catch (SocketException sex)
                    {
                        Dispatcher.Invoke(delegate
                        {
                            App.ActiveWindow.DismissLoadDialog();
                            App.ActiveWindow.ShowConnection();
                        });
                    }

                    Dispatcher.Invoke(delegate
                    {
                        App.ActiveWindow.DismissLoadDialog();
                    });
                });
            }
            else
                App.ActiveWindow.DismissLoadDialog();
        }

        
        private void Restore_Version(object sender, RoutedEventArgs e)
        {
            long timestamp = (long)(version_list.SelectedItem as Label).Tag;
            App.CurrentApp.pipe.selectVersion(currentFilename, timestamp);
        }

        void File_Selected(object sender, RoutedEventArgs e)
        {
            currentFilename = (sender as TreeViewItem).Tag as string;
            version_list.Items.Clear();

            Task.Run(delegate
            {
                try
                {
                    using (Socket sock = new Socket(SocketType.Stream, ProtocolType.IP))
                    {
                        sock.Connect(new IPEndPoint(IPAddress.Parse(App.CurrentApp.settings.server_ip), App.CurrentApp.settings.server_port));
                        using (NetworkStream ns = new NetworkStream(sock))
                        {
                            using (BinaryWriter bw = new BinaryWriter(ns))
                            {
                                using (BinaryReader br = new BinaryReader(ns))
                                {
                                    bw.Write(IPAddress.HostToNetworkOrder(App.CurrentApp.settings.username.Length));
                                    bw.Write(Encoding.UTF8.GetBytes(App.CurrentApp.settings.username));

                                    bw.Write(IPAddress.HostToNetworkOrder(App.CurrentApp.settings.password.Length));
                                    bw.Write(Encoding.UTF8.GetBytes(App.CurrentApp.settings.password));

                                    bw.Write(IPAddress.HostToNetworkOrder(currentFilename.Length));
                                    bw.Write(Encoding.UTF8.GetBytes(currentFilename));

                                    //Auth ok o no?
                                    if (!br.ReadBoolean())
                                    {
                                        Dispatcher.Invoke(delegate
                                        {
                                            App.CurrentApp.ConnAvailable = false;
                                            App.CurrentApp.ni.Icon = Properties.Resources.icon_error;
                                            App.ActiveWindow.ShowLogin();
                                        });

                                        return;
                                    }
                                    else
                                    {
                                        App.CurrentApp.ConnAvailable = true;
                                    }

                                    byte opcode = 64;
                                    bw.Write(opcode);

                                    for(int i=0; i<8; i++)
                                        bw.Write((long)0);
                                    
                                    for (int size = IPAddress.NetworkToHostOrder(br.ReadInt32()); size>0; size--) {
                                        Dispatcher.Invoke(delegate {
                                            Label l = new Label();
                                            l.Tag = IPAddress.NetworkToHostOrder(br.ReadInt64());
                                            l.Content = DateTime.FromFileTime((long)l.Tag);
                                            version_list.Items.Add(l);
                                        });
                                    }
                                }
                            }
                        }
                    }
                }
                catch (SocketException)
                {
                    Dispatcher.Invoke(delegate
                    {
                        App.ActiveWindow.ShowConnection();
                    });
                }
            });
        }

        string PathAddBackslash(string path)
        {
            string separator1 = Path.DirectorySeparatorChar.ToString();
            string separator2 = Path.AltDirectorySeparatorChar.ToString();

            path = path.TrimEnd();

            if (path.EndsWith(separator1) || path.EndsWith(separator2))
                return path;

            if (path.Contains(separator2))
                return path + separator2;
            
            return path + separator1;
        }
    }
}
