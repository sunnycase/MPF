using MPF.Data;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF.Controls
{
    public class Border : Decorator
    {
        public static readonly DependencyProperty<Brush> BorderBrushProperty = DependencyProperty.Register(nameof(BorderBrush),
            typeof(Border), new PropertyMetadata<Brush>(null, OnBorderBrushPropertyChanged));

        public static readonly DependencyProperty<Thickness> BorderThicknessProperty = DependencyProperty.Register(nameof(BorderThickness),
            typeof(Border), new UIPropertyMetadata<Thickness>(default(Thickness), UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender, OnBorderThicknessPropertyChanged));

        public static readonly DependencyProperty<Brush> BackgroundProperty = DependencyProperty.Register(nameof(Background),
            typeof(Border), new UIPropertyMetadata<Brush>(null, UIPropertyMetadataOptions.AffectRender));

        public Brush BorderBrush
        {
            get { return GetValue(BorderBrushProperty); }
            set { this.SetLocalValue(BorderBrushProperty, value); }
        }

        public Thickness BorderThickness
        {
            get { return GetValue(BorderThicknessProperty); }
            set { this.SetLocalValue(BorderThicknessProperty, value); }
        }

        public Brush Background
        {
            get { return GetValue(BackgroundProperty); }
            set { this.SetLocalValue(BackgroundProperty, value); }
        }

        private readonly Pen _cachedPen = new Pen();

        public Border()
        {
        }

        protected override void OnRender(IDrawingContext drawingContext)
        {
            drawingContext.DrawGeometry(new RectangleGeometry
            {
                LeftTop = new Point(_cachedPen.Thickness * 0.5f, _cachedPen.Thickness * 0.5f),
                RigthBottom = new Point(RenderSize.Width - _cachedPen.Thickness * 0.5f, RenderSize.Height - _cachedPen.Thickness * 0.5f)
            }, _cachedPen, Background);
        }

        private static void OnBorderBrushPropertyChanged(object sender, PropertyChangedEventArgs<Brush> e)
        {
            ((Border)sender)._cachedPen.Brush = e.NewValue;
        }

        private static void OnBorderThicknessPropertyChanged(object sender, PropertyChangedEventArgs<Thickness> e)
        {
            ((Border)sender)._cachedPen.Thickness = e.NewValue.Left;
        }
    }
}
