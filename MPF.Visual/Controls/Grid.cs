using MPF.Data;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class Grid : Panel
    {
        public static readonly DependencyProperty<int> RowProperty = DependencyProperty.RegisterAttached("Row", typeof(Grid),
            new FrameworkPropertyMetadata<int>(0, propertyChangedHandler: OnRowColumnPropertyChanged));
        public static readonly DependencyProperty<int> ColumnProperty = DependencyProperty.RegisterAttached("Column", typeof(Grid),
            new FrameworkPropertyMetadata<int>(0, propertyChangedHandler: OnRowColumnPropertyChanged));
        public static readonly DependencyProperty<int> RowSpanProperty = DependencyProperty.RegisterAttached("RowSpan", typeof(Grid),
            new FrameworkPropertyMetadata<int>(1, propertyChangedHandler: OnRowColumnPropertyChanged));
        public static readonly DependencyProperty<int> ColumnSpanProperty = DependencyProperty.RegisterAttached("ColumnSpan", typeof(Grid),
            new FrameworkPropertyMetadata<int>(1, propertyChangedHandler: OnRowColumnPropertyChanged));

        public static int GetRow(UIElement element) => element.GetValue(RowProperty);
        public static void SetRow(UIElement element, int value) => element.SetLocalValue(RowProperty, value);

        public static int GetColumn(UIElement element) => element.GetValue(ColumnProperty);
        public static void SetColumn(UIElement element, int value) => element.SetLocalValue(ColumnProperty, value);

        public static int GetRowSpan(UIElement element) => element.GetValue(RowSpanProperty);
        public static void SetRowSpan(UIElement element, int value) => element.SetLocalValue(RowSpanProperty, value);

        public static int GetColumnSpan(UIElement element) => element.GetValue(ColumnSpanProperty);
        public static void SetColumnSpan(UIElement element, int value) => element.SetLocalValue(ColumnSpanProperty, value);

        private LayoutData _layoutData;
        public RowDefinationCollection RowDefinations
        {
            get
            {
                VerifyAccess();
                if (_layoutData == null)
                    _layoutData = new LayoutData();
                return _layoutData.RowDefinationCollection ?? (_layoutData.RowDefinationCollection = new RowDefinationCollection(this));
            }
        }

        public ColumnDefinationCollection ColumnDefinations
        {
            get
            {
                VerifyAccess();
                if (_layoutData == null)
                    _layoutData = new LayoutData();
                return _layoutData.ColumnDefinationCollection ?? (_layoutData.ColumnDefinationCollection = new ColumnDefinationCollection(this));
            }
        }

        internal bool _isMeasureInProgress = false;
        private bool _cellsStrutureDirty = false;

        public Grid()
        {

        }

        protected override Size MeasureOverride(Size availableSize)
        {
            try
            {
                _isMeasureInProgress = true;
                var layoutData = _layoutData;
                if (layoutData == null)
                {
                    var result = default(Size);
                    foreach (var child in Children)
                    {
                        child.Measure(availableSize);
                        result.Width = Math.Max(result.Width, child.DesiredSize.Width);
                        result.Height = Math.Max(result.Height, child.DesiredSize.Height);
                    }
                    return result;
                }
                else
                {
                    ValidateCells();
                    return default(Size);
                }
            }
            finally
            {
                _isMeasureInProgress = false;
            }
        }

        private void ValidateCells()
        {
            if (_cellsStrutureDirty)
            {
                ValidateCellsCore();
                _cellsStrutureDirty = false;
            }
        }

        private void ValidateCellsCore()
        {
            var cellCaches = _layoutData.CellCaches;
            if (cellCaches?.Length != Children.Count)
                cellCaches = (_layoutData.CellCaches = new CellCache[Children.Count]);

            var maxColumnSpan = Math.Max(1, _layoutData.ColumnDefinationCollection?.Count ?? 0);
            var maxRowSpan = Math.Max(1, _layoutData.RowDefinationCollection?.Count ?? 0);
            var maxColumnIndex = maxColumnSpan - 1;
            var maxRowIndex = maxRowSpan - 1;

            for (int i = Children.Count - 1; i >= 0; i--)
            {
                var element = Children[i];
                var cellCache = new CellCache
                {
                    ColumnIndex = Math.Max(0, Math.Min(Grid.GetColumn(element), maxColumnIndex)),
                    RowIndex = Math.Max(0, Math.Min(Grid.GetRow(element), maxRowIndex))
                };
                cellCache.ColumnSpan = Math.Max(1, maxColumnSpan - cellCache.ColumnIndex);
                cellCache.RowSpan = Math.Max(1, maxRowSpan - cellCache.RowIndex);
                //cellCache.SizeFlagsH = 
            }
        }

        internal void InvalidateCells()
        {
            _cellsStrutureDirty = true;
            InvalidateMeasure();
        }

        private static void OnRowColumnPropertyChanged(object sender, PropertyChangedEventArgs<int> e)
        {
            var grid = VisualTreeHelper.GetParent((Visual)sender) as Grid;
            if (grid != null && grid._layoutData != null)
                grid.InvalidateCells();
        }

        private class LayoutData
        {
            public ColumnDefinationCollection ColumnDefinationCollection;
            public RowDefinationCollection RowDefinationCollection;
            public CellCache[] CellCaches;
        }

        struct CellCache
        {
            public int ColumnIndex;
            public int RowIndex;
            public int ColumnSpan;
            public int RowSpan;
            public LayoutTimeSizeFlags SizeFlagsH;
            public LayoutTimeSizeFlags SizeFlagsV;
            public int Next;
        }

        [Flags]
        enum LayoutTimeSizeFlags
        {
            None = 0,
            Pixel = 1,
            Auto = 2,
            Star = 4
        }
    }
}
