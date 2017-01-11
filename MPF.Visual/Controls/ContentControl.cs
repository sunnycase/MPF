using MPF.Data;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ContentControl : Control
    {
        public static readonly DependencyProperty<DataTemplate> ContentTemplateProperty = ContentPresenter.ContentTemplateProperty.AddOwner(typeof(ContentControl),
            new FrameworkPropertyMetadata<DataTemplate>(DependencyProperty.UnsetValue, FrameworkPropertyMetadataOptions.None, UIPropertyMetadataOptions.None));

        public static readonly DependencyProperty<object> ContentProperty = ContentPresenter.ContentProperty.AddOwner(typeof(ContentControl),
            new FrameworkPropertyMetadata<object>(DependencyProperty.UnsetValue, FrameworkPropertyMetadataOptions.None, UIPropertyMetadataOptions.None));

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

        static ContentControl()
        {
            ContentControl.TemplateProperty.OverrideMetadata(typeof(ContentControl),
                new UIPropertyMetadata<ControlTemplate>(ControlTemplate.Create<ContentControl>(t => new ContentPresenter())));
        }

        public ContentControl()
        {

        }
    }
}
