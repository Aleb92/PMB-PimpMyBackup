using System;
using System.IO;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;

namespace PMB_Gui
{
    /// <summary>
    /// Logica di interazione per Connection.xaml
    /// </summary>
    public partial class Connection : Page
    {
        private IAsyncResult res;
        private App app = (App.Current as App);
        private Socket sock;

        public Connection()
        {
            InitializeComponent();
            //DEBUG
            Loaded += delegate
             {
                 tryConnection();
             };
        }

        private void RetryConnection(object sender, RoutedEventArgs e)
        {
            tryConnection();
        }

        public void tryConnection()
        {
            if (res != null)
                return;

            Dispatcher.Invoke(delegate
            {
                NoConnection.Visibility = Visibility.Hidden;
                Loading.Visibility = Visibility.Visible;
                sock = new Socket(SocketType.Stream, ProtocolType.IP);

                res = sock.BeginConnect(new IPEndPoint(IPAddress.Parse(app.settings.server_ip), app.settings.server_port), new AsyncCallback(connectResult), null);
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
                                bw.Write(App.CurrentApp.settings.username.Length);
                                bw.Write(Encoding.UTF8.GetBytes(App.CurrentApp.settings.username));

                                bw.Write(App.CurrentApp.settings.password.Length);
                                bw.Write(Encoding.UTF8.GetBytes(App.CurrentApp.settings.password));

                                //Auth ok o no?
                                if (!br.ReadBoolean())
                                {
                                    App.CurrentApp.ni.Icon = Properties.Resources.icon_error;
                                    App.ActiveWindow.ShowLogin();
                                }
                            }
                        }
                    }
                }
                //QUI LA CONNESSIONE C'è
                app.ConnAvailable = true;
                Dispatcher.Invoke(delegate
                {
                    (app.MainWindow as MainWindow).ShowVersions();
                });
            }
            catch {
                //QUI LA CONNESSIONE NON C'è
                app.ConnAvailable = false;
                Dispatcher.Invoke(delegate
                {
                    NoConnection.Visibility = Visibility.Visible;
                    Loading.Visibility = Visibility.Hidden;

                    Task.Delay(4000).ContinueWith(_ =>
                    {
                        tryConnection();
                    });
                });
            }
        }
    }
}
