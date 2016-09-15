using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media.Animation;

namespace PMB_Gui
{
    /// <summary>
    /// Logica di interazione per MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        private DoubleAnimation 
            show = new DoubleAnimation(484, new Duration(TimeSpan.FromMilliseconds(800))), 
            hide = new DoubleAnimation(0, new Duration(TimeSpan.FromMilliseconds(800)));

        public MainWindow()
        {
            InitializeComponent();
            hide.EasingFunction = show.EasingFunction = new BounceEase
            {
                EasingMode = EasingMode.EaseOut,
                Bounces = 2,
                Bounciness = 2.5
            };
            //DEBUGG
            Loaded += delegate
            {
                ShowConnection();
            };
        }

        public void ShowConnection()
        {
            Connection.BeginAnimation(Panel.WidthProperty, show);
            Login.BeginAnimation(Panel.WidthProperty, hide);
        }

        public void ShowVersions()
        {
            Connection.BeginAnimation(Frame.WidthProperty, hide);
            Login.BeginAnimation(Frame.WidthProperty, hide);
        }

        public void ShowLogin()
        {
            Connection.BeginAnimation(Frame.WidthProperty, hide);
            Login.BeginAnimation(Frame.WidthProperty, show);
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            if (!App.CurrentApp.ShuttingDown)
            {
                e.Cancel = true;
                Hide();
            }
        }
    }
}
