using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace PMB_Gui
{
    /// <summary>
    /// Logica di interazione per Versions.xaml
    /// </summary>
    public partial class Versions : Page
    {
        private static readonly ImageSource 
            folder = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(
                        PMB_Gui.Properties.Resources.folder.GetHbitmap(), 
                        IntPtr.Zero, Int32Rect.Empty, 
                        System.Windows.Media.Imaging.BitmapSizeOptions.FromEmptyOptions()),
            file = System.Windows.Interop.Imaging.CreateBitmapSourceFromHBitmap(
                        PMB_Gui.Properties.Resources.file.GetHbitmap(),
                        IntPtr.Zero, Int32Rect.Empty, 
                        System.Windows.Media.Imaging.BitmapSizeOptions.FromEmptyOptions());

        
        public Versions()
        {
            InitializeComponent();

        }

        private void Page_Loaded(object sender, RoutedEventArgs e)
        {
            LoadDirsAndFiles(FileView.Items, @"C:\Users\mrcmn\workspace\PMB\build\client");
        }

        private StackPanel IconAndText(ImageSource IS, string text)
        {
            StackPanel sp = new StackPanel
            {
                Orientation = Orientation.Horizontal
            };

            sp.Children.Add(new System.Windows.Controls.Image
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
            Prova.Content = filename;
        }

    }
}
