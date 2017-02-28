using MPF.Data;
using MPF.Media;
using System;
using System.Collections;
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

        public static readonly DependencyProperty<HorizontalAlignment> HorizontalContentAlignmentProperty = DependencyProperty.Register(nameof(HorizontalContentAlignment),
            typeof(FrameworkElement), new UIPropertyMetadata<HorizontalAlignment>(HorizontalAlignment.Stretch, UIPropertyMetadataOptions.AffectArrange));

        public static readonly DependencyProperty<VerticalAlignment> VerticalContentAlignmentProperty = DependencyProperty.Register(nameof(VerticalContentAlignment),
            typeof(FrameworkElement), new UIPropertyMetadata<VerticalAlignment>(VerticalAlignment.Stretch, UIPropertyMetadataOptions.AffectArrange));

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

        public HorizontalAlignment HorizontalContentAlignment
        {
            get { return GetValue(HorizontalContentAlignmentProperty); }
            set { this.SetLocalValue(HorizontalContentAlignmentProperty, value); }
        }

        public VerticalAlignment VerticalContentAlignment
        {
            get { return GetValue(VerticalContentAlignmentProperty); }
            set { this.SetLocalValue(VerticalContentAlignmentProperty, value); }
        }

        protected override IEnumerator LogicalChildren
        {
            get
            {
                if (Content != null)
                    yield return Content;
            }
        }

        static ContentControl()
        {
            ContentControl.TemplateProperty.OverrideMetadata(typeof(ContentControl),
                new UIPropertyMetadata<ControlTemplate>(ControlTemplate.Create<ContentControl>(t =>
                {
                    var cp = new ContentPresenter();
                    BindingOperations.SetBinding(cp, ContentPresenter.ContentProperty, new Binding { Path = "Content", Source = t, Mode = BindingMode.OneWay });
                    BindingOperations.SetBinding(cp, ContentPresenter.ContentTemplateProperty, new Binding { Path = "ContentTemplate", Source = t, Mode = BindingMode.OneWay });
                    BindingOperations.SetBinding(cp, ContentPresenter.HorizontalAlignmentProperty, new Binding { Path = "HorizontalContentAlignment", Source = t, Mode = BindingMode.OneWay });
                    BindingOperations.SetBinding(cp, ContentPresenter.VerticalAlignmentProperty, new Binding { Path = "VerticalContentAlignment", Source = t, Mode = BindingMode.OneWay });

                    return cp;
                })));
        }

        public ContentControl()
        {

        }
    }
}
