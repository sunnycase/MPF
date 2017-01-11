using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ContentPresenter : FrameworkElement
    {
        public static readonly DependencyProperty<DataTemplate> ContentTemplateProperty = DependencyProperty.Register(nameof(ContentTemplate),
            typeof(ContentPresenter), new FrameworkPropertyMetadata<DataTemplate>(null, FrameworkPropertyMetadataOptions.Inherits, UIPropertyMetadataOptions.AffectMeasure | UIPropertyMetadataOptions.AffectRender, OnContentTemplatePropertyChanged));

        public static readonly DependencyProperty<object> ContentProperty = DependencyProperty.Register(nameof(Content),
            typeof(ContentPresenter), new FrameworkPropertyMetadata<object>(null, FrameworkPropertyMetadataOptions.Inherits, UIPropertyMetadataOptions.None, OnContentPropertyChanged));

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

        protected override IEnumerable<UIElement> LogicalChildren
        {
            get
            {
                if (TemplatedChild != null)
                    yield return TemplatedChild;
            }
        }

        private bool _templatedChildLoaded = false;
        private FrameworkElement _templatedChild;
        public FrameworkElement TemplatedChild
        {
            get
            {
                if(!_templatedChildLoaded)
                {
                    _templatedChildLoaded = true;
                    if (_templatedChild != null)
                        RemoveVisualChild(_templatedChild);
                    _templatedChild = (FrameworkElement)ContentTemplate?.LoadContent(this);
                    if (_templatedChild != null)
                        AddVisualChild(_templatedChild);
                }
                return _templatedChild;
            }
        }

        private static void OnContentPropertyChanged(object sender, PropertyChangedEventArgs<object> e)
        {
            (sender as ContentPresenter)?.OnContentChanged();
        }

        private void OnContentChanged()
        {
            if (_templatedChild != null)
                _templatedChild.DataContext = Content;
        }

        private static void OnContentTemplatePropertyChanged(object sender, PropertyChangedEventArgs<DataTemplate> e)
        {
            (sender as ContentPresenter)?.OnContentTemplateChanged();
        }

        private void OnContentTemplateChanged()
        {
            _templatedChildLoaded = false;
        }
    }
}
