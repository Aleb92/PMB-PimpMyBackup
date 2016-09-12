using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ServiceProcess;
using System.Runtime.InteropServices;
using System.Timers;


namespace PMB_Gui
{
    /// <summary>
    /// Logica di interazione per MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        [DllImport("AdvApi32")]
        public static extern bool QueryServiceStatus(IntPtr serviceHandle, [Out] SERVICE_STATUS status);

        [StructLayout(LayoutKind.Sequential, Pack = 1)]
        public class SERVICE_STATUS
        {
            public int dwServiceType;
            public ServiceControllerStatus dwCurrentState;
            public int dwControlsAccepted;
            public int dwWin32ExitCode;
            public int dwServiceSpecificExitCode;
            public int dwCheckPoint;
            public int dwWaitHint;
        };

        ServiceController service = new ServiceController("PMB", Environment.MachineName);

        public MainWindow()
        {
            InitializeComponent();
            Timer t = new Timer(500);
            t.Elapsed += t_Elapsed;
            t.Start();
        }

        void t_Elapsed(object sender, ElapsedEventArgs e)
        {
            if(service.Status == ServiceControllerStatus.Running)
            {
                SERVICE_STATUS ss = new SERVICE_STATUS();
                QueryServiceStatus(service.ServiceHandle.DangerousGetHandle(), ss);
                Bah.Content = "Checkpoint: " + ss.dwCheckPoint;
            }
        }


        private void Button_Click(object sender, RoutedEventArgs e)
        {
            if (service.Status == ServiceControllerStatus.Running) {
                service.Stop();
                service.WaitForStatus(ServiceControllerStatus.Stopped);
                (sender as Button).Content = "Start";
            }
            else
            {
                service.Start();
                service.WaitForStatus(ServiceControllerStatus.Running);
                (sender as Button).Content = "Stop";
            }
        }
    }
}
