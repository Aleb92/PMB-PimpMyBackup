using System.ServiceProcess;
using System.Threading.Tasks;
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
            App.ActiveWindow.ShowLoadDialog("Trying login...");
            string user = Username.Text, password = Password.Password;
            Task.Run(delegate {
                App.CurrentApp.stopService();

                Dispatcher.Invoke(delegate
                {
                    App.ActiveWindow.DismissLoadDialog();
                    App.ActiveWindow.ShowConnection();
                });
                App.CurrentApp.settings.resetCredentials(user, password);
                Dispatcher.Invoke(delegate
                {
                    (App.ActiveWindow.Connection.Content as Connection).tryConnection(null, null);
                });

                App.CurrentApp.startService();
            });
            
        }
    }
}
