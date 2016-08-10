using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Documents
{
    public class Run : UIElement
    {
        private GlyphRun _glyphRun;

        public static readonly DependencyProperty<FontFamily> FontFamilyProperty = DependencyProperty.Register(nameof(FontFamily),
            typeof(Run), new UIPropertyMetadata<FontFamily>(new FontFamily("Arial"), UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender,
                OnFontFamilyPropertyChanged));

        public static readonly DependencyProperty<string> TextProperty = DependencyProperty.Register(nameof(FontFamily),
            typeof(Run), new UIPropertyMetadata<string>(string.Empty, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender,
                OnTextPropertyChanged));

        public FontFamily FontFamily
        {
            get { return GetValue(FontFamilyProperty); }
            set { SetValue(FontFamilyProperty, value); }
        }

        public string Text
        {
            get { return GetValue(TextProperty); }
            set { SetValue(TextProperty, value); }
        }

        protected override void ArrangeOverride(Rect finalRect)
        {
            if(_glyphRun == null)
                _glyphRun = new GlyphRun(FontFamily, Text.ToCharArray(), 12);
            base.ArrangeOverride(finalRect);
        }

        protected override void OnRender(IDrawingContext drawingContext)
        {
            var glyph = _glyphRun;
            if (glyph != null)
                glyph.Draw(drawingContext, new Pen
                {
                    Brush = new SolidColorBrush { Color = Color.FromArgb(0xFF3388ee) },
                    Thickness = 1
                });
        }

        private static void OnTextPropertyChanged(object sender, PropertyChangedEventArgs<string> e)
        {
            ((Run)sender)?.InvalidateGlyphRun();
        }

        private void InvalidateGlyphRun()
        {
            _glyphRun = null;
        }

        private static void OnFontFamilyPropertyChanged(object sender, PropertyChangedEventArgs<FontFamily> e)
        {
            ((Run)sender)?.InvalidateGlyphRun();
        }
    }
}
