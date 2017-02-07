using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class RowDefinition : DefinitionBase
    {
        public static readonly DependencyProperty<GridLength> HeightProperty = DependencyProperty.Register(nameof(Height), typeof(RowDefinition),
            new PropertyMetadata<GridLength>(GridLength.Star, OnSizePropertyChanged));
        public static readonly DependencyProperty<float> MinHeightProperty = DependencyProperty.Register(nameof(MinHeight), typeof(RowDefinition),
            new PropertyMetadata<float>(0.0f, OnMinMaxSizePropertyChanged));
        public static readonly DependencyProperty<float> MaxHeightProperty = DependencyProperty.Register(nameof(MaxHeight), typeof(RowDefinition),
            new PropertyMetadata<float>(float.PositiveInfinity, OnMinMaxSizePropertyChanged));

        public GridLength Height
        {
            get => GetValue(HeightProperty);
            set => this.SetLocalValue(HeightProperty, value);
        }

        public float MinHeight
        {
            get => GetValue(MinHeightProperty);
            set => this.SetLocalValue(MinHeightProperty, value);
        }

        public float MaxHeight
        {
            get => GetValue(MaxHeightProperty);
            set => this.SetLocalValue(MaxHeightProperty, value);
        }

        protected override DependencyProperty<GridLength> SizeProperty => HeightProperty;
        protected override DependencyProperty<float> MinSizeProperty => MinHeightProperty;
        protected override DependencyProperty<float> MaxSizeProperty => MaxHeightProperty;
    }

    public class RowDefinitionCollection : DefinitionCollectionBase<RowDefinition>
    {
        public RowDefinitionCollection(Grid grid) : base(grid)
        {
        }
    }
}
