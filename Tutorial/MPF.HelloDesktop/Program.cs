using MPF.Controls;
using MPF.Documents;
using MPF.Input;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MPF.HelloDesktop
{
    class Program
    {
        public static void Main(string[] args)
        {
            var app = new Application();
            app.Run();
        }
    }

    class Application : MPF.Application
    {
        private readonly Window _window;
        private Window _window2;

        public Application()
        {
            InputManager.Current.HIDAware = HIDUsages.Default;

            _window = new Window
            {
                Size = new Size(1024, 768)
            };
            _window.Content = new Border
            {
                BorderThickness = new Thickness(1.0f),
                BorderBrush = new SolidColorBrush { Color = Color.FromArgb(0xFF888888) },
                Background = new SolidColorBrush { Color = Color.FromArgb(0x2200FF00) },
                Margin = new Thickness(5),
                Child = new ContentControl
                {
                    ContentTemplate = new DataTemplate(t =>
                    {
                        var tb = new TextBlock
                        {
                            Margin = new Thickness(5),
                            Text = "baka帝球",
                            Style = new Style(typeof(TextBlock))
                                .SetLocalValue(TextBlock.FontFamilyProperty, new FontFamily("Microsoft YaHei"))
                                .SetLocalValue(TextBlock.FontSizeProperty, 25)
                                .SetLocalValue(TextBlock.HorizontalAlignmentProperty, HorizontalAlignment.Center)
                                .SetLocalValue(TextBlock.VerticalAlignmentProperty, VerticalAlignment.Center)
                        };
                        tb.PointerPressed += window_PointerPressed;
                        return tb;
                    })
                }
            };
            _window.PointerPressed += window_PointerPressed;
            _window.Show();
            ChangeMaximizeBox();
        }

        private void window_PointerPressed(object sender, Input.PointerRoutedEventArgs e)
        {
            Console.WriteLine($"{e.Source}({e.OriginalSource}) : {e.RoutedEvent.Name}");
            e.Handled = true;
        }

        private void ShowWindow2()
        {
            _window2 = new Window();
            _window2.Title = "Hello MPF Multi Window";
            _window2.Show();
        }

        private async void ChangeMaximizeBox()
        {
            _window.HasMaximize = false;
            _window.Title = "Hello MPF";

            await Task.Delay(3000);
            //_window.Child.Visibility = Visibility.Collapsed;
        }
    }
}
