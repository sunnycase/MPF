using MPF.Data;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ListBox : ItemsControl
    {
        static ListBox()
        {
            ListBox.ItemContainerGeneratorActivatorProperty.OverrideMetadata(typeof(ListBox), 
                new PropertyMetadata<Func<ItemsControl, ItemContainerGenerator>>(l => new ListBoxItemContainerGenerator((ListBox)l)));
            ListBox.StyleProperty.OverrideMetadata(typeof(ListBox), new FrameworkPropertyMetadata<Style>(
                new Style(typeof(ListBox))
                    .SetLocalValue(ListBox.TemplateProperty, ControlTemplate.Create<ListBox>(t =>
                    {
                        var itemsHost = new ItemsPresenter();
                        BindingOperations.SetBinding(itemsHost, ItemsPresenter.ItemsSourceProperty, new Binding { Path = "ItemsSource", Source = t });
                        BindingOperations.SetBinding(itemsHost, ItemsPresenter.ItemContainerGeneratorProperty, new Binding { Path = "ItemContainerGenerator", Source = t });
                        BindingOperations.SetBinding(itemsHost, ItemsPresenter.ItemsPanelTemplateProperty, new Binding { Path = "ItemsPanelTemplate", Source = t });

                        var root = new Border
                        {
                            Child = itemsHost
                        };

                        return root;
                    }))));
        }
    }

    public class ListBoxItemContainerGenerator : ItemContainerGenerator
    {
        public ListBoxItemContainerGenerator(ListBox listBox)
        {

        }

        public override UIElement GenerateNext()
        {
            return new Button { Height = 80, Background = Color.FromArgb(0xFFeeeeee), Margin = 5 };
        }

        public override void LinkContainerToItem(UIElement container, object item)
        {
            ((Button)container).Content = item;
        }
    }
}
