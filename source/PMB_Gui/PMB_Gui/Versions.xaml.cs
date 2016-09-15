using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Interop;
using System.Windows.Media;

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

        
        public Versions()
        {
            InitializeComponent();

        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            LoadDirsAndFiles(FileView.Items, App.CurrentApp.settings.watchedDir);
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

        private void LoadDirsAndFiles(ItemCollection ic, string path)
        {
            foreach (string s in Directory.GetDirectories(path))
            {
                TreeViewItem item = new TreeViewItem
                {
                    Header = IconAndText(folder, s.Substring(path.Length + 1)),
                    Tag = s,
                    FontWeight = FontWeights.Normal
                };

                ic.Add(item);
                LoadDirsAndFiles(item.Items, s);
            }

            foreach (string s in Directory.GetFiles(path))
            {
                TreeViewItem item = new TreeViewItem
                {
                    Header = IconAndText(file, s.Substring(path.Length + 1)),
                    Tag = s,
                    FontWeight = FontWeights.Normal
                };

                item.Selected += File_Selected;
                ic.Add(item);
            }
        }

        void File_Selected(object sender, RoutedEventArgs e)
        {
            string filename = (sender as TreeViewItem).Tag as string;
            Prova.Items.Add(filename);
        }

    }
}
