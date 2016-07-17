using MPF.Controls;
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

        public Application()
        {
            _window = new Window();
            _window.Show();
            ChangeMaximizeBox();
        }

        private async void ChangeMaximizeBox()
        {
            await Task.Delay(1);
            _window.HasMaximize = false;
            _window.Title = "Hello MPF";
        }
    }
}
