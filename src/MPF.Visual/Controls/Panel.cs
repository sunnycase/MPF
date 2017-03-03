using MPF.Data;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace MPF.Controls
{
    public class Panel : FrameworkElement
    {
        public static readonly DependencyProperty<Brush> BackgroundProperty = Border.BackgroundProperty.AddOwner(typeof(Panel), new UIPropertyMetadata<Brush>(null, UIPropertyMetadataOptions.AffectRender));
        public static readonly DependencyProperty<int> ZIndexProperty = DependencyProperty.RegisterAttached("ZIndex", typeof(Panel),
            new UIPropertyMetadata<int>(0, UIPropertyMetadataOptions.None, OnZIndexPropertyChanged));

        public Brush Background
        {
            get { return GetValue(BackgroundProperty); }
            set { this.SetLocalValue(BackgroundProperty, value); }
        }

        public static int GetZIndex(UIElement element)
        {
            return element.GetValue(ZIndexProperty);
        }

        public static void SetZIndex(UIElement element, int value)
        {
            element.SetLocalValue(ZIndexProperty, value);
        }

        public bool IsItemsHost { get; internal set; }
        private readonly object _cookie = new object();
        private readonly UIElementCollection _uiElements;
        public UIElementCollection Children => _uiElements;
        public override int VisualChildrenCount => _uiElements.Count;
        protected override IEnumerator LogicalChildren => _uiElements.GetEnumerator();

        public Panel()
        {
            _uiElements = new UIElementCollection(this, _cookie);
        }

        private static void OnZIndexPropertyChanged(object sender, PropertyChangedEventArgs<int> e)
        {
            
        }

        public override Visual GetVisualChildAt(int index)
        {
            return _uiElements[index];
        }

        protected override void OnRender(IDrawingContext drawingContext)
        {
            drawingContext.DrawGeometry(new RectangleGeometry
            {
                LeftTop = Point.Zero,
                RigthBottom = (Point)RenderSize
            }, null, Background);
        }

        internal void MapItemsToGenerator(ItemContainerGenerator generator, IEnumerable itemsSource)
        {
            _uiElements.Clear(_cookie);
            if(generator != null && itemsSource != null)
            {
                foreach (var item in itemsSource)
                {
                    var container = generator.GenerateNext();
                    generator.LinkContainerToItem(container, item);
                    _uiElements.Add(container, _cookie);
                }
            }
        }
    }
}
