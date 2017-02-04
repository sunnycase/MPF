using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class RowDefination : DefinationBase
    {
        public static readonly DependencyProperty<GridLength> HeightProperty = DependencyProperty.Register(nameof(Height), typeof(RowDefination),
            new PropertyMetadata<GridLength>(GridLength.Auto, OnSizePropertyChanged));

        public GridLength Height
        {
            get => GetValue(HeightProperty);
            set => this.SetLocalValue(HeightProperty, value);
        }

        protected override DependencyProperty<GridLength> SizeProperty => HeightProperty;
    }

    public class RowDefinationCollection : DefinationCollectionBase<RowDefination>
    {
        public RowDefinationCollection(Grid grid) : base(grid)
        {
        }
    }
}
