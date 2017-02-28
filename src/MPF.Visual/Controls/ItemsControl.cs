using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ItemsControl : Control
    {
        public static readonly DependencyProperty<DataTemplate> ItemTemplateProperty = DependencyProperty.Register(nameof(ItemTemplate),
            typeof(ItemsControl), new FrameworkPropertyMetadata<DataTemplate>(null, propertyChangedHandler: OnItemTemplatePropertyChanged));

        public DataTemplate ItemTemplate
        {
            get => GetValue(ItemTemplateProperty);
            set => this.SetLocalValue(ItemTemplateProperty, value);
        }

        private static void OnItemTemplatePropertyChanged(object sender, PropertyChangedEventArgs<DataTemplate> e)
        {

        }
    }
}
