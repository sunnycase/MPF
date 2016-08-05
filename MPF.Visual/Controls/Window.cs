using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices.ComTypes;
using System.Threading.Tasks;
using MPF.Interop;
using MPF.Media;

namespace MPF.Controls
{
    public class Window : UIElement
    {
        private readonly CoreWindow _coreWindow = new CoreWindow();

        public static readonly DependencyProperty<bool> HasMaximizeProperty = DependencyProperty.Register(nameof(HasMaximize), typeof(Window),
            new PropertyMetadata<bool>(true, propertyChangedHandler: OnHasMaximizePropertyChanged));
        public static readonly DependencyProperty<string> TitleProperty = DependencyProperty.Register(nameof(Title), typeof(Window), 
            new PropertyMetadata<string>(string.Empty, propertyChangedHandler: OnTitlePropertyChanged));

        public bool HasMaximize
        {
            get { return GetValue(HasMaximizeProperty); }
            set { SetValue(HasMaximizeProperty, value); }
        }

        public string Title
        {
            get { return GetValue(TitleProperty); }
            set { SetValue(TitleProperty, value); }
        }

        public Window()
        {
            _coreWindow.HasMaximize = HasMaximize;
            _coreWindow.Title = Title;

            _coreWindow.SetRootVisual(this);

            using (var ctx = _testGeometry4.Open())
            {
                ctx.MoveTo(new Point(10, 10));
                ctx.LineTo(new Point(350, 420));
                ctx.ArcTo(new Point(450, 420), 110);
            }
        }


        private readonly Geometry _testGeometry = new RectangleGeometry
        {
            LeftTop = new Point(200, 300),
            RigthBottom = new Point(400, 100)
        };
        private readonly Geometry _testGeometry2 = new RectangleGeometry
        {
            LeftTop = new Point(300, 400),
            RigthBottom = new Point(500, 200)
        };
        private readonly Geometry _testGeometry3 = new LineGeometry
        {
            StartPoint = new Point(300, 400),
            EndPoint = new Point(500, 200)
        };
        private readonly StreamGeometry _testGeometry4 = new StreamGeometry();
        private readonly Pen _testPen = new Pen
        {
            Brush = new SolidColorBrush { Color = Color.FromArgb(0xFF33EECC) },
            Thickness = 1
        };

        protected override void OnRender(IDrawingContext drawingContext)
        {
            base.OnRender(drawingContext);
            //drawingContext.DrawGeometry(_testGeometry, _testPen);
            drawingContext.DrawGeometry(_testGeometry2, _testPen);
            //drawingContext.DrawGeometry(_testGeometry3, _testPen);
            drawingContext.DrawGeometry(_testGeometry4, _testPen);
        }

        public void Show()
        {
            _coreWindow.Show();
        }

        public void Hide()
        {
            _coreWindow.Hide();
        }

        private static void OnHasMaximizePropertyChanged(object sender, PropertyChangedEventArgs<bool> e)
        {
            ((Window)sender)._coreWindow.HasMaximize = e.NewValue;
        }

        private static void OnTitlePropertyChanged(object sender, PropertyChangedEventArgs<string> e)
        {
            ((Window)sender)._coreWindow.Title = e.NewValue;
        }
    }
}
