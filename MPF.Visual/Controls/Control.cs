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

        public static readonly DependencyProperty<Brush> BorderBrushProperty = Border.BorderBrushProperty.AddOwner(typeof(Control), new PropertyMetadata<Brush>(null));

        public static readonly DependencyProperty<Thickness> BorderThicknessProperty = Border.BorderThicknessProperty.AddOwner(typeof(Control), new UIPropertyMetadata<Thickness>(default(Thickness)));

        public static readonly DependencyProperty<Brush> BackgroundProperty = Border.BackgroundProperty.AddOwner(typeof(Control), new UIPropertyMetadata<Brush>(null));

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
