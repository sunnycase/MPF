using MPF.Controls;
using MPF.Documents;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.HelloWorld
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
            _window = new Window
            {
                Width = 1024,
                Height = 768
            };

            _window.Content = new Border
            {
                BorderThickness = new Thickness(1.0f),
                BorderBrush = new SolidColorBrush { Color = Color.FromArgb(0xFF888888) },
                Margin = new Thickness(5),
                Child = new Run
                {
                    Margin = new Thickness(5),
                    FontFamily = new FontFamily("Microsoft YaHei"),
                    FontSize = 20,
                    Text = "帝球baka"
                }
            };
            _window.Show();
            ChangeMaximizeBox();
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
        }
    }
}
