using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Controls
{
    public class TextBlock : FrameworkElement
    {
        public static readonly DependencyProperty<FontFamily> FontFamilyProperty = DependencyProperty.Register(nameof(FontFamily),
            typeof(TextBlock), new UIPropertyMetadata<FontFamily>(new FontFamily("Arial"), UIPropertyMetadataOptions.AffectMeasure, OnFontFamilyPropertyChanged));

        public static readonly DependencyProperty<float> FontSizeProperty = DependencyProperty.Register(nameof(FontSize),
            typeof(TextBlock), new UIPropertyMetadata<float>(12, UIPropertyMetadataOptions.AffectMeasure));

        public static readonly DependencyProperty<Brush> ForegroundProperty = DependencyProperty.Register(nameof(Foreground),
            typeof(TextBlock), new UIPropertyMetadata<Brush>(new SolidColorBrush { Color = Colors.Black }, UIPropertyMetadataOptions.AffectMeasure));

        public static readonly DependencyProperty<string> TextProperty = DependencyProperty.Register(nameof(Text), typeof(TextBlock),
            new UIPropertyMetadata<string>(DependencyProperty.UnsetValue, UIPropertyMetadataOptions.AffectMeasure, OnTextPropertyChanged));

        public string Text
        {
            get { return GetValue(TextProperty); }
            set { SetValue(TextProperty, value); }
        }

        public float FontSize
        {
            get { return GetValue(FontSizeProperty); }
            set { SetValue(FontSizeProperty, value); }
        }

        public Brush Foreground
        {
            get { return GetValue(ForegroundProperty); }
            set { SetValue(ForegroundProperty, value); }
        }

        public TextBlock()
        {

        }

        private void OnTextChanged(string text)
        {

        }

        private static void OnFontFamilyPropertyChanged(object sender, PropertyChangedEventArgs<FontFamily> e)
        {

        }

        private static void OnTextPropertyChanged(object sender, PropertyChangedEventArgs<string> e)
        {
            ((TextBlock)sender).OnTextChanged(e.NewValue);
        }
    }
}
