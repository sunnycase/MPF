using MPF.Controls;
using MPF.Documents;
using MPF.Input;
using MPF.Media;
using MPF.Media.Imaging;
using MPF.Media3D;
using MPF.Media3D.Shaders;
using System;
using System.Collections.Generic;
using System.IO;
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
            var scene = LoadScene();
            var viewport = new Viewport3D
            {
                Camera = new MatrixCamera
                {
                    ViewMatrix = Matrix4x4.CreateLookAt(new Vector3(15, 15, -25), new Vector3(0, 0, 0), Vector3.UnitY)
                },
                Scene = scene
            };
            viewport.SizeChanged += Viewport_SizeChanged;
            button.Content = viewport;
            viewport = new Viewport3D
            {
                Camera = new MatrixCamera
                {
                    ViewMatrix = Matrix4x4.CreateLookAt(new Vector3(-11, 15, -20), new Vector3(0, 15, 0), Vector3.UnitY)
                },
                Scene = scene
            };
            viewport.SizeChanged += Viewport_SizeChanged;
            Grid.SetColumn(viewport, 1);
            Grid.SetRowSpan(viewport, 3);
            grid.Children.Add(viewport);

            var listBox = new ListBox
            {
                ItemsSource = new[] { "I'm", "a ListBox" }
            };
            Grid.SetRow(listBox, 2);
            grid.Children.Add(listBox);
            _window.Content = grid;
            _window.PointerPressed += window_PointerPressed;
            _window.Show();
            //ShowWindow2(scene);
            ChangeMaximizeBox();
        }

        private Scene LoadScene()
        {
            //var mesh2 = new MeshGeometry3D
            //{
            //    Positions = new FreezableCollection<Point3D>
            //    {
            //        (0, 0, 0), (100, 0, 0), (100, 100, 0), (0, 100, 0)
            //    },
            //    Indices = new FreezableCollection<uint>
            //    {
            //        0, 1, 2, 2, 3, 0
            //    }
            //};
            var dstScene = new Scene();
#if false
            var bitmap = BitmapDecoder.Create(File.OpenRead(Path.Combine(@"Content\", "heart.png")));
            var frame = bitmap.Frames[0];
            var brush = new ImageBrush { Source = frame };
            for (int i = 0; i < 5; i++)
            {
                dstScene.Add(new Visual3D
                {
                    Geometry = new BoxGeometry3D { Width = 10, Height = 2, Depth = 20 },
                    Material = new Material<StandardShaderParameters>
                    {
                        Shader = new StandardShadersGroup(),
                        Parameters = new StandardShaderParameters
                        {
                            MainTexture = brush
                        }
                    }
                });
            }
#else
            var context = new Assimp.AssimpContext();
            var scene = context.ImportFile(@"Content\Reimu\reimu_Sheep3D_0.957.fbx", Assimp.PostProcessSteps.GenerateSmoothNormals | Assimp.PostProcessSteps.GenerateUVCoords);
            foreach(var src in scene.Meshes)
            {
                var mesh = new MeshGeometry3D
                {
                    Positions = new FreezableCollection<Point3D>(from v in src.Vertices
                                                                 select new Point3D(v.X, v.Y, v.Z)),
                    Normals = new FreezableCollection<Vector3>(from v in src.Normals
                                                               select new Vector3(v.X, v.Y, v.Z)),
                    TextureCoordinates = new FreezableCollection<Point>(from v in src.TextureCoordinateChannels[0]
                                                                        select new Point(v.X, v.Y)),
                    Indices = new FreezableCollection<uint>(from f in src.Faces
                                                            from i in f.Indices
                                                            select (uint)i)
                };
                var srcMaterial = scene.Materials[src.MaterialIndex];
                Brush mainTexture;
                if (srcMaterial.HasTextureDiffuse)
                {
                    var bitmap = BitmapDecoder.Create(File.OpenRead(Path.Combine(@"Content\Reimu\", srcMaterial.TextureDiffuse.FilePath)));
                    var frame = bitmap.Frames[0];
                    mainTexture = new ImageBrush { Source = frame };
                }
                else
                    mainTexture = new SolidColorBrush { Color = Colors.Black };
                var visual = new Visual3D
                {
                    Geometry = mesh,
                    Material = new Material<StandardShaderParameters>
                    {
                        Shader = new StandardShadersGroup(),
                        Parameters = new StandardShaderParameters
                        {
                            MainTexture = mainTexture
                        }
                    }
                };
                dstScene.Add(visual);
            }
#endif

            //Console.WriteLine($"Total Vertex Count: { dstScene.Sum(o => ((MeshGeometry3D)o.Geometry).Positions.Count) }");
            return dstScene;
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
