using System.ServiceProcess;
using System.Windows.Controls;
namespace PMB_Gui
{
    /// <summary>
    /// Logica di interazione per Login.xaml
    /// </summary>
    public partial class Login : Page
    {

        public Login()
        {
            InitializeComponent();
        }

        private void Login_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            //App.CurrentApp.PMBservice.Stop();
            //App.CurrentApp.PMBservice.WaitForStatus(ServiceControllerStatus.Stopped);

            App.ActiveWindow.ShowConnection();
            App.CurrentApp.settings.resetCredentials(Username.Text, Password.Password);
            (App.ActiveWindow.Connection.Content as Connection).tryConnection();

            //App.CurrentApp.PMBservice.Start();
            //App.CurrentApp.PMBservice.WaitForStatus(ServiceControllerStatus.Running);
        }
    }
}
