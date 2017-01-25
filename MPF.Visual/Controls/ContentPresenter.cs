using MPF.Data;
using MPF.Media;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ContentPresenter : FrameworkElement
    {
        public static readonly DependencyProperty<DataTemplate> ContentTemplateProperty = DependencyProperty.Register(nameof(ContentTemplate),
            typeof(ContentPresenter), new FrameworkPropertyMetadata<DataTemplate>(null, FrameworkPropertyMetadataOptions.Inherits, UIPropertyMetadataOptions.AffectMeasure, OnContentTemplatePropertyChanged));

        public static readonly DependencyProperty<object> ContentProperty = DependencyProperty.Register(nameof(Content),
            typeof(ContentPresenter), new FrameworkPropertyMetadata<object>(null, FrameworkPropertyMetadataOptions.Inherits, UIPropertyMetadataOptions.AffectMeasure, OnContentPropertyChanged));

        public DataTemplate ContentTemplate
        {
            get { return GetValue(ContentTemplateProperty); }
            set { this.SetLocalValue(ContentTemplateProperty, value); }
        }

        public object Content
        {
            get { return GetValue(ContentProperty); }
            set { this.SetLocalValue(ContentProperty, value); }
        }

        protected override IEnumerator LogicalChildren
        {
            get
            {
                if (Content != null)
                    yield return Content;
            }
        }

        public override int VisualChildrenCount => TemplateChild == null ? 0 : 1;

        private bool _templateChildLoaded = false;
        private Visual _templateChild;
        public Visual TemplateChild
        {
            get
            {
                if (!_templateChildLoaded)
                {
                    _templateChildLoaded = true;
                    if (_templateChild != null)
                        RemoveVisualChild(_templateChild);
                    if (Content is Visual)
                    {
                        _templateChild = (Visual)Content;
                    }
                    else
                    {
                        var templateChild = (FrameworkElement)ContentTemplate?.LoadContent(this);
                        if (templateChild != null)
                            templateChild.DataContext = Content;
                        _templateChild = templateChild;
                    }
                    if (_templateChild != null)
                        AddVisualChild(_templateChild);
                }
                return _templateChild;
            }
        }

        private static void OnContentPropertyChanged(object sender, PropertyChangedEventArgs<object> e)
        {
            (sender as ContentPresenter)?.OnContentChanged();
        }

        private void OnContentChanged()
        {
            _templateChildLoaded = false;
        }

        private static void OnContentTemplatePropertyChanged(object sender, PropertyChangedEventArgs<DataTemplate> e)
        {
            (sender as ContentPresenter)?.OnContentTemplateChanged();
        }

        public override Visual GetVisualChildAt(int index)
        {
            if (TemplateChild == null || index != 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            return TemplateChild;
        }

        private void OnContentTemplateChanged()
        {
            _templateChildLoaded = false;
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            var child = TemplateChild as UIElement;
            if (child != null)
            {
                child.Measure(availableSize);
                return child.DesiredSize;
            }
            return default(Size);
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            return finalSize;
        }

        protected override void OnAfterArrange()
        {
            var child = TemplateChild as UIElement;
            child?.InvalidateArrange();
        }
    }
}
