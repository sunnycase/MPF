using MPF.Data;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ItemsControl : Control
    {
        public static readonly DependencyProperty<DataTemplate> ItemTemplateProperty = DependencyProperty.Register(nameof(ItemTemplate),
            typeof(ItemsControl), new FrameworkPropertyMetadata<DataTemplate>(null));

        public static readonly DependencyProperty<ItemsPanelTemplate> ItemsPanelTemplateProperty = DependencyProperty.Register(nameof(ItemsPanelTemplate),
            typeof(ItemsControl), new FrameworkPropertyMetadata<ItemsPanelTemplate>(new ItemsPanelTemplate(t => new StackPanel())));

        public static readonly DependencyProperty<IEnumerable> ItemsSourceProperty = DependencyProperty.Register(nameof(ItemsSource),
            typeof(ItemsControl), new PropertyMetadata<IEnumerable>(null));

        public static readonly DependencyProperty<Func<ItemsControl, ItemContainerGenerator>> ItemContainerGeneratorActivatorProperty = DependencyProperty.Register(nameof(ItemContainerGeneratorActivator),
            typeof(ItemsControl), new PropertyMetadata<Func<ItemsControl, ItemContainerGenerator>>(null, OnItemContainerGeneratorActivatorPropertyChanged));

        public static readonly DependencyProperty<ItemContainerGenerator> ItemContainerGeneratorProperty = DependencyProperty.Register(nameof(ItemContainerGenerator),
            typeof(ItemsControl), new PropertyMetadata<ItemContainerGenerator>(null));

        public DataTemplate ItemTemplate
        {
            get => GetValue(ItemTemplateProperty);
            set => this.SetLocalValue(ItemTemplateProperty, value);
        }

        public ItemsPanelTemplate ItemsPanelTemplate
        {
            get => GetValue(ItemsPanelTemplateProperty);
            set => this.SetLocalValue(ItemsPanelTemplateProperty, value);
        }

        public IEnumerable ItemsSource
        {
            get => GetValue(ItemsSourceProperty);
            set => this.SetLocalValue(ItemsSourceProperty, value);
        }

        public ItemContainerGenerator ItemContainerGenerator => GetValue(ItemContainerGeneratorProperty);

        public Func<ItemsControl, ItemContainerGenerator> ItemContainerGeneratorActivator
        {
            get => GetValue(ItemContainerGeneratorActivatorProperty);
            set => this.SetLocalValue(ItemContainerGeneratorActivatorProperty, value);
        }

        protected override void OnInitializeOverride()
        {
            OnOnItemContainerGeneratorActivatorChanged(ItemContainerGeneratorActivator);
        }

        private void OnOnItemContainerGeneratorActivatorChanged(Func<ItemsControl, ItemContainerGenerator> activator)
        {
            this.SetLocalValue(ItemContainerGeneratorProperty, activator?.Invoke(this));
        }

        private static void OnItemContainerGeneratorActivatorPropertyChanged(object sender, PropertyChangedEventArgs<Func<ItemsControl, ItemContainerGenerator>> e)
        {
            ((ItemsControl)sender).OnOnItemContainerGeneratorActivatorChanged(e.NewValue);
        }
    }
}
