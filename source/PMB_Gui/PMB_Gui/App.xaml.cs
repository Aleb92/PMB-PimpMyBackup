using System;
using System.ServiceProcess;
using System.Windows;
using System.Windows.Forms;

namespace PMB_Gui
{
    /// <summary>
    /// Logica di interazione per App.xaml
    /// </summary>
    public partial class App : System.Windows.Application
    {
        public NotifyIcon ni;
        public Settings settings;
        public Pipe pipe;
        public ServiceController PMBservice = new ServiceController("PMB", Environment.MachineName);

        private bool connAvailable = false, shuttingDown = false;

        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            ni = new NotifyIcon()
            {
                Icon = PMB_Gui.Properties.Resources.icon_working,
                Visible = true
            };
            ni.DoubleClick += toggleMainWindow;

            pipe = new Pipe(settings.pipeName);
            pipe.InvalidLogin += InvalidLogin;
            pipe.WorkingCount += WorkingCount;
        }

        public bool ShuttingDown
        {
            get
            {
                return shuttingDown;
            }
        }

        public static MainWindow ActiveWindow
        {
            get
            {
                return Current.MainWindow as MainWindow;
            }
        }

        public static App CurrentApp
        {
            get
            {
                return Current as App;
            }
        }

        public bool ConnAvailable
        {
            get
            {
                return connAvailable;
            }
            set
            {
                if (!value)
                    ni.Icon = PMB_Gui.Properties.Resources.icon_error;
                else
                    ni.Icon = PMB_Gui.Properties.Resources.icon_working;
                connAvailable = value;
            }
        }

        protected override void OnSessionEnding(SessionEndingCancelEventArgs e)
        {
            base.OnSessionEnding(e);
            shuttingDown = true;
        }

        private void WorkingCount(int working_count)
        {
            if (connAvailable) {
                if (working_count == 0)
                    ni.Icon = PMB_Gui.Properties.Resources.icon_ok;
                else
                    ni.Icon = PMB_Gui.Properties.Resources.icon_working;
            }
        }

        private void InvalidLogin()
        {
            showMainWindow();
            (MainWindow as MainWindow).ShowLogin();
        }

        private void showMainWindow() {

            if (MainWindow.IsVisible)
                return;

            MainWindow.Left = SystemParameters.FullPrimaryScreenWidth - MainWindow.ActualWidth;
            MainWindow.Top = SystemParameters.FullPrimaryScreenHeight - MainWindow.ActualHeight;
            MainWindow.Show();
        }

        private void toggleMainWindow(object sender, EventArgs e) {

            if (MainWindow.IsVisible)
                MainWindow.Hide();
            else showMainWindow();
        }
    }
}
