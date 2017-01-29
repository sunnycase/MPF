using MPF.Controls;
using MPF.Data;
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
            new FrameworkPropertyMetadata<FontFamily>(DependencyProperty.UnsetValue, FrameworkPropertyMetadataOptions.None, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender, OnFontFamilyPropertyChanged));

        public static readonly DependencyProperty<float> FontSizeProperty = TextBlock.FontSizeProperty.AddOwner(typeof(Run),
            new FrameworkPropertyMetadata<float>(DependencyProperty.UnsetValue, FrameworkPropertyMetadataOptions.None, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender, OnFontSizePropertyChanged));

        public static readonly DependencyProperty<Brush> ForegroundProperty = TextBlock.ForegroundProperty.AddOwner(typeof(Run),
            new FrameworkPropertyMetadata<Brush>(DependencyProperty.UnsetValue, FrameworkPropertyMetadataOptions.None, UIPropertyMetadataOptions.AffectRender));

        public static readonly DependencyProperty<string> TextProperty = DependencyProperty.Register(nameof(Text),
            typeof(Run), new UIPropertyMetadata<string>(string.Empty, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender,
                OnTextPropertyChanged));

        public FontFamily FontFamily
        {
            get { return GetValue(FontFamilyProperty); }
            set { this.SetLocalValue(FontFamilyProperty, value); }
        }

        public float FontSize
        {
            get { return GetValue(FontSizeProperty); }
            set { this.SetLocalValue(FontSizeProperty, value); }
        }

        public string Text
        {
            get { return GetValue(TextProperty); }
            set { this.SetLocalValue(TextProperty, value); }
        }

        public Brush Foreground
        {
            get { return GetValue(ForegroundProperty); }
            set { this.SetLocalValue(ForegroundProperty, value); }
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            var text = Text;
            if (_glyphRun == null && !string.IsNullOrEmpty(text))
                _glyphRun = new GlyphRun(FontFamily, text.ToCharArray(), FontSize);
            return _glyphRun?.Size ?? Size.Zero;
        }

        protected override void OnRender(IDrawingContext drawingContext)
        {
            var glyph = _glyphRun;
            if (glyph != null)
                glyph.Draw(drawingContext, new Pen
                {
                    Brush = Foreground,
                    Thickness = 1
                }, Foreground);
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
