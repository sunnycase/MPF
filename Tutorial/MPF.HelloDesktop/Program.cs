using MPF.Controls;
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
        private Geometry _geometry;

        public Application()
        {
            _window = new Window();
            _window.Show();
            ChangeMaximizeBox();
            _geometry = new LineGeometry();
            //ShowWindow2();
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
