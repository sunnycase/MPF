using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Controls
{
    public class TextBlock : FrameworkElement
    {
        public static readonly DependencyProperty<string> TextProperty = DependencyProperty.Register(nameof(Text), typeof(TextBlock),
            new UIPropertyMetadata<string>(DependencyProperty.UnsetValue, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender, OnTextPropertyChanged));

        public string Text
        {
            get { return GetValue(TextProperty); }
            set { SetValue(TextProperty, value); }
        }

        public TextBlock()
        {

        }

        private void OnTextChanged(string text)
        {

        }

        private static void OnTextPropertyChanged(object sender, PropertyChangedEventArgs<string> e)
        {
            ((TextBlock)sender).OnTextChanged(e.NewValue);
        }
    }
}
