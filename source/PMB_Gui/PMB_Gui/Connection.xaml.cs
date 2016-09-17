using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Threading;

namespace PMB_Gui
{
    /// <summary>
    /// Logica di interazione per Connection.xaml
    /// </summary>
    public partial class Connection : Page
    {
        private IAsyncResult res;
        private Socket sock;
        private DispatcherTimer connection_timer = new DispatcherTimer {
            Interval = TimeSpan.FromMilliseconds(10000)
        };

        public Connection()
        {
            InitializeComponent();
            connection_timer.Tick += tryConnection;
            connection_timer.Start();
        }

        public void tryConnection(object sender, EventArgs e)
        {
            if (res != null)
                return;

            connection_timer.Stop();

            Dispatcher.Invoke(delegate
            {
                NoConnection.Visibility = Visibility.Hidden;
                Loading.Visibility = Visibility.Visible;
                sock = new Socket(SocketType.Stream, ProtocolType.IP);
                sock.SendTimeout = sock.ReceiveTimeout = 5000;
                res = sock.BeginConnect(new IPEndPoint(IPAddress.Parse(App.CurrentApp.settings.server_ip), App.CurrentApp.settings.server_port), connectResult, null);
            });
        }

        private void connectResult(IAsyncResult ar)
        {
            res = null;
            try {

                sock.EndConnect(ar);
                using (sock)
                {
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

                                //opcode
                                bw.Write((byte)64);

                                //Auth ok o no?
                                if (!br.ReadBoolean())
                                {
                                    Dispatcher.Invoke(delegate
                                    {
                                        App.CurrentApp.ConnAvailable = false;
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
                            }
                        }
                    }
                }
            }
            catch {
                //QUI LA CONNESSIONE NON C'è
                App.CurrentApp.ConnAvailable = false;
                Dispatcher.Invoke(delegate
                {
                    App.ActiveWindow.ShowConnection();
                    NoConnection.Visibility = Visibility.Visible;
                    Loading.Visibility = Visibility.Hidden;
                });
            }

            connection_timer.Start();
        }
        
    }
}
