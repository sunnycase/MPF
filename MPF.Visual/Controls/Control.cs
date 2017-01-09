using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class Control : FrameworkElement, IHasTemplatedChild
    {
        public static readonly DependencyProperty<ControlTemplate> TemplateProperty = DependencyProperty.Register(nameof(Template),
            typeof(Control), new UIPropertyMetadata<ControlTemplate>(null, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender));

        public ControlTemplate Template
        {
            get { return GetValue(TemplateProperty); }
            set { this.SetLocalValue(TemplateProperty, value); }
        }

        protected override IEnumerable<UIElement> LogicalChildren
        {
            get { yield return TemplatedChild; }
        }

        private bool _templatedChildLoaded = false;
        private UIElement _templatedChild;
        private UIElement TemplatedChild
        {
            get
            {
                if(!_templatedChildLoaded)
                {
                    _templatedChildLoaded = true;
                    _templatedChild = (UIElement)Template?.LoadContent(this);
                    OnApplyTemplate();
                }
                return _templatedChild;
            }
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

        object IHasTemplatedChild.GetTemplatedChild(FrameworkTemplate template)
        {
            return template == Template ? TemplatedChild : null;
        }
    }
}
