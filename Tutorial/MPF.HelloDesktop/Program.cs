using MPF.Controls;
using MPF.Documents;
using MPF.Input;
using MPF.Media;
using MPF.Media3D;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
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
            button = new Button
            {
                Style = buttonStyle,
                HorizontalContentAlignment = HorizontalAlignment.Stretch,
                VerticalContentAlignment = VerticalAlignment.Stretch
            };
            Grid.SetRow(button, 1);
            grid.Children.Add(button);
            var scene = new Scene
                {
                    new Visual3D()
                };
            var viewport = new Viewport3D
            {
                Camera = new MatrixCamera
                {
                    ViewMatrix = Matrix4x4.CreateLookAt(new Vector3(150, 150, 250), new Vector3(50, 50, 0), Vector3.UnitY)
                },
                Scene = scene
            };
            viewport.SizeChanged += Viewport_SizeChanged;
            button.Content = viewport;
            viewport = new Viewport3D
            {
                Camera = new MatrixCamera
                {
                    ViewMatrix = Matrix4x4.CreateLookAt(new Vector3(-150, 150, 250), new Vector3(50, 50, 0), Vector3.UnitY)
                },
                Scene = scene
            };
            viewport.SizeChanged += Viewport_SizeChanged;
            Grid.SetColumn(viewport, 1);
            Grid.SetRowSpan(viewport, 2);
            grid.Children.Add(viewport);
            _window.Content = grid;
            _window.PointerPressed += window_PointerPressed;
            _window.Show();
            //ShowWindow2(scene);
            ChangeMaximizeBox();
        }

        private void Viewport_SizeChanged(object sender, EventArgs e)
        {
            var viewport = (Viewport3D)sender;
            var camera = (MatrixCamera)viewport.Camera;
            camera.ProjectionMatrix = Matrix4x4.CreatePerspectiveFieldOfView((float)Math.PI / 2.0f, viewport.RenderSize.Width / viewport.RenderSize.Height, 0.3f, 1000.0f);
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

        private void ShowWindow2(Scene scene)
        {
            _window2 = new Window
            {
                Width = 300,
                Height = 200
            };
            _window2.Title = "Hello MPF Multi Window";
            var viewport = new Viewport3D
            {
                Camera = new MatrixCamera
                {
                    ViewMatrix = Matrix4x4.CreateLookAt(new Vector3(-150, -150, 250), new Vector3(50, 50, 0), Vector3.UnitY)
                },
                Scene = scene
            };
            viewport.SizeChanged += Viewport_SizeChanged;
            _window2.Content = viewport;
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
