using MPF.Data;
using MPF.Media;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ItemsPresenter : FrameworkElement
    {
        public static readonly DependencyProperty<ItemsPanelTemplate> ItemsPanelTemplateProperty = ItemsControl.ItemsPanelTemplateProperty.AddOwner(typeof(ItemsPresenter),
            new FrameworkPropertyMetadata<ItemsPanelTemplate>(DependencyProperty.UnsetValue, propertyChangedHandler: OnItemsPanelTemplatePropertyChanged));

        public static readonly DependencyProperty<ItemContainerGenerator> ItemContainerGeneratorProperty = ItemsControl.ItemContainerGeneratorProperty.AddOwner(typeof(ItemsPresenter),
            new FrameworkPropertyMetadata<ItemContainerGenerator>(DependencyProperty.UnsetValue, propertyChangedHandler: OnItemContainerGeneratorPropertyChanged));

        public static readonly DependencyProperty<IEnumerable> ItemsSourceProperty = ItemsControl.ItemsSourceProperty.AddOwner(typeof(ItemsPresenter),
            new PropertyMetadata<IEnumerable>(DependencyProperty.UnsetValue, propertyChangedHandler: OnItemsSourcePropertyChanged));

        public ItemsPanelTemplate ItemsPanelTemplate
        {
            get => GetValue(ItemsPanelTemplateProperty);
            set => this.SetLocalValue(ItemsPanelTemplateProperty, value);
        }

        public ItemContainerGenerator ItemContainerGenerator
        {
            get => GetValue(ItemContainerGeneratorProperty);
            set => this.SetLocalValue(ItemContainerGeneratorProperty, value);
        }

        public IEnumerable ItemsSource
        {
            get => GetValue(ItemsSourceProperty);
            set => this.SetLocalValue(ItemsSourceProperty, value);
        }

        public override int VisualChildrenCount => TemplateChild == null ? 0 : 1;

        private bool _templateChildLoaded = false;
        private Panel _templateChild;
        private Panel TemplateChild
        {
            get
            {
                if (!_templateChildLoaded && CheckAccess())
                {
                    _templateChildLoaded = true;
                    if (_templateChild != null)
                        RemoveVisualChild(_templateChild);

                    _templateChild = (Panel)ItemsPanelTemplate?.LoadContent(this);
                    if (_templateChild != null)
                    {
                        _templateChild.IsItemsHost = true;
                        AddVisualChild(_templateChild);
                    }
                    OnApplyTemplate();
                }
                return _templateChild;
            }
        }

        protected virtual void OnApplyTemplate()
        {

        }

        private static void OnItemsPanelTemplatePropertyChanged(object sender, PropertyChangedEventArgs<ItemsPanelTemplate> e)
        {
            ((ItemsPresenter)sender).OnTemplateChanged();
        }

        private void OnTemplateChanged()
        {
            _templateChildLoaded = false;
        }

        private static void OnItemContainerGeneratorPropertyChanged(object sender, PropertyChangedEventArgs<ItemContainerGenerator> e)
        {
            var itemsPresenter = (ItemsPresenter)sender;
            itemsPresenter.OnItemContainerGeneratorChanged(e.NewValue, itemsPresenter.ItemsSource);
        }

        private void OnItemContainerGeneratorChanged(ItemContainerGenerator generator, IEnumerable itemsSource)
        {
            TemplateChild?.MapItemsToGenerator(generator, itemsSource);
        }

        private static void OnItemsSourcePropertyChanged(object sender, PropertyChangedEventArgs<IEnumerable> e)
        {
            var itemsPresenter = (ItemsPresenter)sender;
            itemsPresenter.OnItemContainerGeneratorChanged(itemsPresenter.ItemContainerGenerator, e.NewValue);
        }

        public override Visual GetVisualChildAt(int index)
        {
            if (TemplateChild == null || index != 0)
                throw new ArgumentOutOfRangeException(nameof(index));
            return TemplateChild;
        }

        protected override Size MeasureOverride(Size availableSize)
        {
            var child = TemplateChild;
            if (child != null)
            {
                child.Measure(availableSize);
                return child.DesiredSize;
            }
            return default(Size);
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            var child = TemplateChild as UIElement;
            child?.Arrange(new Rect(Point.Zero, finalSize));
            return finalSize;
        }
    }
}
