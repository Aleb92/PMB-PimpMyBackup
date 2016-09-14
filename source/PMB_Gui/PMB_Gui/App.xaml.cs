using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;

namespace PMB_Gui
{
    /// <summary>
    /// Logica di interazione per App.xaml
    /// </summary>
    public partial class App : System.Windows.Application
    {
        private NotifyIcon ni;
            
        protected override void OnStartup(StartupEventArgs e)
        {
            base.OnStartup(e);
            ni = new NotifyIcon();
            ni.Icon = PMB_Gui.Properties.Resources.Icon1;
            ni.Visible = true;
            ni.DoubleClick += delegate
            {
                if (MainWindow.IsVisible)
                    MainWindow.Hide();
                else
                {
                    MainWindow.Left = SystemParameters.FullPrimaryScreenWidth - MainWindow.ActualWidth;
                    MainWindow.Top = SystemParameters.FullPrimaryScreenHeight - MainWindow.ActualHeight;
                    MainWindow.Show();
                }
            };
        }
    }
}
