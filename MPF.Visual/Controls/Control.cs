using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class Control : FrameworkElement
    {
        public static readonly DependencyProperty<ControlTemplate> TemplateProperty = DependencyProperty.Register(nameof(Template),
            typeof(Control), new UIPropertyMetadata<ControlTemplate>(null, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender));

        public ControlTemplate Template
        {
            get { return GetValue(TemplateProperty); }
            set { SetValue(TemplateProperty, value); }
        }

        public Control()
        {

        }

        protected override Size MeasureOverride(Size availableSize)
        {
            return base.MeasureOverride(availableSize);
        }

        protected virtual void OnApplyTemplate()
        {

        }
    }
}
