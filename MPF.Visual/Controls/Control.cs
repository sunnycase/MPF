using MPF.Data;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using MPF.Media;

namespace MPF.Controls
{
    public class Control : FrameworkElement, IHasTemplateChild
    {
        public static readonly DependencyProperty<ControlTemplate> TemplateProperty = DependencyProperty.Register(nameof(Template),
            typeof(Control), new UIPropertyMetadata<ControlTemplate>(null, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender));

        public ControlTemplate Template
        {
            get { return GetValue(TemplateProperty); }
            set { this.SetLocalValue(TemplateProperty, value); }
        }

        public override int VisualChildrenCount => TemplateChild == null ? 0 : 1;

        private bool _templateChildLoaded = false;
        private UIElement _templateChild;
        private UIElement TemplateChild
        {
            get
            {
                if (!_templateChildLoaded)
                {
                    _templateChildLoaded = true;
                    if (_templateChild != null)
                        RemoveVisualChild(_templateChild);

                    _templateChild = (UIElement)Template?.LoadContent(this);
                    if (_templateChild != null)
                        AddVisualChild(_templateChild);
                    OnApplyTemplate();
                }
                return _templateChild;
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

        public override Visual GetVisualChildAt(int index)
        {
            if (TemplateChild == null || index != 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            return TemplateChild;
        }

        object IHasTemplateChild.GetTemplateChild(FrameworkTemplate template)
        {
            return template == Template ? TemplateChild : null;
        }
    }
}
