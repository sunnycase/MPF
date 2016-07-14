using MPF.Controls;
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

        public Application()
        {
            _window = new Window();
            _window.Show();
            ChangeMaximizeBox();
            //_window.HasMaximize = false;
        }

        private async void ChangeMaximizeBox()
        {
            await Task.Delay(2000);
            _window.HasMaximize = false;
            _window.Title = "Hello MPF";
        }
    }
}
