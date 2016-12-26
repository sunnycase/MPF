using MPF.Controls;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Documents
{
    public class Run : FrameworkElement
    {
        private GlyphRun _glyphRun;

        public static readonly DependencyProperty<FontFamily> FontFamilyProperty = TextBlock.FontFamilyProperty.AddOwner(typeof(Run),
            new UIPropertyMetadata<FontFamily>(new FontFamily("Arial"), UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender, OnFontFamilyPropertyChanged));
        public static readonly DependencyProperty<float> FontSizeProperty = TextBlock.FontSizeProperty.AddOwner(typeof(Run),
            new UIPropertyMetadata<float>(12, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender, OnFontSizePropertyChanged));

        public static readonly DependencyProperty<string> TextProperty = DependencyProperty.Register(nameof(FontFamily),
            typeof(Run), new UIPropertyMetadata<string>(string.Empty, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender,
                OnTextPropertyChanged));

        public FontFamily FontFamily
        {
            get { return GetValue(FontFamilyProperty); }
            set { SetValue(FontFamilyProperty, value); }
        }

        public float FontSize
        {
            get { return GetValue(FontSizeProperty); }
            set { SetValue(FontSizeProperty, value); }
        }

        public string Text
        {
            get { return GetValue(TextProperty); }
            set { SetValue(TextProperty, value); }
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            if (_glyphRun == null)
                _glyphRun = new GlyphRun(FontFamily, Text.ToCharArray(), FontSize);
            return base.MeasureOverride(availableSize);
        }

        protected override void OnRender(IDrawingContext drawingContext)
        {
            var glyph = _glyphRun;
            if (glyph != null)
                glyph.Draw(drawingContext, new Pen
                {
                    Brush = new SolidColorBrush { Color = Color.FromArgb(0xFF777777) },
                    Thickness = 1
                }, new SolidColorBrush { Color = Color.FromArgb(0xFF007777) });
        }

        private static void OnTextPropertyChanged(object sender, PropertyChangedEventArgs<string> e)
        {
            ((Run)sender)?.InvalidateGlyphRun();
        }

        private void InvalidateGlyphRun()
        {
            _glyphRun = null;
        }

        private static void OnFontSizePropertyChanged(object sender, PropertyChangedEventArgs<float> e)
        {
            ((Run)sender)?.InvalidateGlyphRun();
        }

        private static void OnFontFamilyPropertyChanged(object sender, PropertyChangedEventArgs<FontFamily> e)
        {
            ((Run)sender)?.InvalidateGlyphRun();
        }
    }
}
