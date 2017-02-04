using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ColumnDefination : DefinationBase
    {
        public static readonly DependencyProperty<GridLength> WidthProperty = DependencyProperty.Register(nameof(Width), typeof(RowDefination),
            new PropertyMetadata<GridLength>(GridLength.Auto, OnSizePropertyChanged));

        public GridLength Width
        {
            get => GetValue(WidthProperty);
            set => this.SetLocalValue(WidthProperty, value);
        }

        protected override DependencyProperty<GridLength> SizeProperty => WidthProperty;
    }

    public class ColumnDefinationCollection : DefinationCollectionBase<ColumnDefination>
    {
        public ColumnDefinationCollection(Grid grid) : base(grid)
        {
        }
    }
}
