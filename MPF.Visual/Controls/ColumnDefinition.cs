using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ColumnDefinition : DefinitionBase
    {
        public static readonly DependencyProperty<GridLength> WidthProperty = DependencyProperty.Register(nameof(Width), typeof(RowDefinition),
            new PropertyMetadata<GridLength>(GridLength.Star, OnSizePropertyChanged));
        public static readonly DependencyProperty<float> MinWidthProperty = DependencyProperty.Register(nameof(MinWidth), typeof(RowDefinition),
            new PropertyMetadata<float>(0.0f, OnMinMaxSizePropertyChanged));
        public static readonly DependencyProperty<float> MaxWidthProperty = DependencyProperty.Register(nameof(MaxWidth), typeof(RowDefinition),
            new PropertyMetadata<float>(float.PositiveInfinity, OnMinMaxSizePropertyChanged));
        
        public GridLength Width
        {
            get => GetValue(WidthProperty);
            set => this.SetLocalValue(WidthProperty, value);
        }

        public float MinWidth
        {
            get => GetValue(MinWidthProperty);
            set => this.SetLocalValue(MinWidthProperty, value);
        }

        public float MaxWidth
        {
            get => GetValue(MaxWidthProperty);
            set => this.SetLocalValue(MaxWidthProperty, value);
        }

        protected override DependencyProperty<GridLength> SizeProperty => WidthProperty;
        protected override DependencyProperty<float> MinSizeProperty => MinWidthProperty;
        protected override DependencyProperty<float> MaxSizeProperty => MaxWidthProperty;
    }

    public class ColumnDefinitionCollection : DefinitionCollectionBase<ColumnDefinition>
    {
        public ColumnDefinitionCollection(Grid grid) : base(grid)
        {
        }
    }
}
