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
                Width = 1024,
                Height = 768
            };

            var button = new Button { Content = "Hello" };
            var buttonStyle = new Style(typeof(Button), Button.StyleProperty.GetMetadata(typeof(Button)).DefaultValue)
                .SetLocalValue(Button.BorderThicknessProperty, 1)
                .SetLocalValue(Button.BorderBrushProperty, Color.FromArgb(0xFF888888))
                .SetLocalValue(Button.BackgroundProperty, Color.FromArgb(0x2200FF00))
                .SetLocalValue(Button.FontSizeProperty, 25)
                .SetLocalValue(Button.MarginProperty, 5);
            button.Style = buttonStyle;
            button.Click += Button_Click;
            var grid = new Grid();
            grid.RowDefinitions.Add(new RowDefinition());
            grid.RowDefinitions.Add(new RowDefinition());
            grid.ColumnDefinitions.Add(new ColumnDefinition());
            grid.ColumnDefinitions.Add(new ColumnDefinition());
            grid.Children.Add(button);
            button = new Button { Style = buttonStyle, Content = "Baka" };
            Grid.SetRow(button, 1);
            grid.Children.Add(button);
            button = new Button { Style = buttonStyle, Content = "Xiahuan" };
            Grid.SetColumn(button, 1);
            Grid.SetRowSpan(button, 2);
            grid.Children.Add(button);
            _window.Content = grid;
            _window.PointerPressed += window_PointerPressed;
            _window.Show();
            ChangeMaximizeBox();
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Console.WriteLine("Button Clicked");
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
