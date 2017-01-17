using MPF.Data;
using MPF.Documents;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Collections;

namespace MPF.Controls
{
    public class TextBlock : FrameworkElement
    {
        public static readonly DependencyProperty<FontFamily> FontFamilyProperty = DependencyProperty.Register(nameof(FontFamily),
            typeof(TextBlock), new UIPropertyMetadata<FontFamily>(new FontFamily("Arial"), UIPropertyMetadataOptions.AffectMeasure));

        public static readonly DependencyProperty<float> FontSizeProperty = DependencyProperty.Register(nameof(FontSize),
            typeof(TextBlock), new UIPropertyMetadata<float>(12, UIPropertyMetadataOptions.AffectMeasure));

        public static readonly DependencyProperty<Brush> ForegroundProperty = DependencyProperty.Register(nameof(Foreground),
            typeof(TextBlock), new UIPropertyMetadata<Brush>(new SolidColorBrush { Color = Colors.Black }, UIPropertyMetadataOptions.AffectMeasure));

        public static readonly DependencyProperty<string> TextProperty = DependencyProperty.Register(nameof(Text), typeof(TextBlock),
            new UIPropertyMetadata<string>(DependencyProperty.UnsetValue, UIPropertyMetadataOptions.AffectMeasure));

        public string Text
        {
            get { return GetValue(TextProperty); }
            set { this.SetLocalValue(TextProperty, value); }
        }

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

        public Brush Foreground
        {
            get { return GetValue(ForegroundProperty); }
            set { this.SetLocalValue(ForegroundProperty, value); }
        }

        protected override IEnumerator LogicalChildren
        {
            get
            {
                if (Text != null)
                    yield return Text;
            }
        }

        public override int VisualChildrenCount => 1;

        private readonly Run _run = new Run();

        public TextBlock()
        {
            BindingOperations.SetBinding(_run, Run.FontFamilyProperty, new Binding { Source = this, Path = "FontFamily" });
            BindingOperations.SetBinding(_run, Run.FontSizeProperty, new Binding { Source = this, Path = "FontSize" });
            BindingOperations.SetBinding(_run, Run.ForegroundProperty, new Binding { Source = this, Path = "Foreground" });
            BindingOperations.SetBinding(_run, Run.TextProperty, new Binding { Source = this, Path = "Text" });
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            _run.Measure(availableSize);
            return _run.DesiredSize;
        }

        public override Visual GetVisualChildAt(int index)
        {
            if (index != 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            return _run;
        }
    }
}
