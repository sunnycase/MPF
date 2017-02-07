using MPF.Data;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
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
        public RowDefinitionCollection RowDefinitions
        {
            get
            {
                VerifyAccess();
                if (_layoutData == null)
                    _layoutData = new LayoutData();
                return _layoutData.RowDefinitionCollection ?? (_layoutData.RowDefinitionCollection = new RowDefinitionCollection(this));
            }
        }

        public ColumnDefinitionCollection ColumnDefinitions
        {
            get
            {
                VerifyAccess();
                if (_layoutData == null)
                    _layoutData = new LayoutData();
                return _layoutData.ColumnDefinitionCollection ?? (_layoutData.ColumnDefinitionCollection = new ColumnDefinitionCollection(this));
            }
        }

        internal bool _isMeasureInProgress = false;
        private bool _cellsStrutureDirty = false;

        public Grid()
        {
            _spanPreferDistComparer = new SpanPreferredDistributionOrderComparer(GetDefinationCache);
            _spanMaxDistComparer = new SpanMaxDistributionOrderComparer(GetDefinationCache);
            _starDistOrderComparer = new StarDistributionOrderComparer(GetDefinationCache);
        }

        protected override Size ArrangeOverride(Size finalSize)
        {
            var layoutData = _layoutData;
            if (layoutData == null)
            {
                foreach (var child in Children)
                    child.Arrange(new Rect(Point.Zero, finalSize));
            }
            else
            {
                SetFinalSize(layoutData.DefinitionsH, finalSize.Width);
                SetFinalSize(layoutData.DefinitionsV, finalSize.Height);

                for (int i = 0; i < layoutData.CellCaches.Length; i++)
                {
                    ref var cell = ref layoutData.CellCaches[i];
                    var element = Children[i];

                    var columnIndex = cell.ColumnIndex;
                    var columnSpan = cell.ColumnSpan;
                    var rowIndex = cell.RowIndex;
                    var rowSpan = cell.RowSpan;
                    var finalRect = new Rect((columnIndex == 0) ? 0.0f : GetDefinationCache(layoutData.DefinitionsH[columnIndex]).FinalOffset,
                        (rowIndex == 0) ? 0.0f : GetDefinationCache(layoutData.DefinitionsV[rowIndex]).FinalOffset,
                        GetFinalSizeForRange(layoutData.DefinitionsH, columnIndex, columnSpan),
                        GetFinalSizeForRange(layoutData.DefinitionsV, rowIndex, rowSpan));
                    element.Arrange(finalRect);
                }
            }
            return finalSize;
        }

        private float GetFinalSizeForRange(IReadOnlyList<DefinitionBase> definitions, int start, int count)
        {
            return definitions.Skip(start).Take(count).Sum(o => GetDefinationCache(o).SizeCache);
        }

        private void SetFinalSize(IReadOnlyList<DefinitionBase> definitions, float finalSize)
        {
            var layoutData = _layoutData;
            var starCount = 0;
            var noStarIndex = definitions.Count;
            float sumSize = 0.0f;
            for (int i = 0; i < definitions.Count; i++)
            {
                var def = definitions[i];
                var cache = GetDefinationCache(def);

                var unitType = def.UserSize.UnitType;
                if (unitType == GridUnitType.Star)
                {
                    var size = def.UserSize.Value;
                    if (size == 0.0f)
                    {
                        cache.MeasureSize = 0.0f;
                        cache.SizeCache = 0.0f;
                    }
                    else
                    {
                        size = Math.Min(size, _starClip);
                        cache.MeasureSize = size;
                        var maxSize = Math.Max(cache.MinSize, def.UserMaxSize);
                        maxSize = Math.Min(maxSize, _starClip);
                        cache.SizeCache = maxSize / size;
                    }
                    layoutData.DefinitionIndexes[starCount++] = i;
                }
                else
                {
                    float size = 0.0f;
                    if (unitType != GridUnitType.Auto)
                    {
                        if (unitType == GridUnitType.Pixel)
                            size = def.UserSize.Value;
                    }
                    else
                        size = cache.MinSize;
                    float maxSize = def.UserMaxSize;
                    cache.SizeCache = Math.Max(cache.MinSize, Math.Min(size, maxSize));
                    sumSize += cache.SizeCache;
                    layoutData.DefinitionIndexes[--noStarIndex] = i;
                }
            }

            if (starCount > 0)
            {
                Array.Sort(layoutData.DefinitionIndexes, 0, starCount, new StarDistributionOrderIndexComparer(definitions, GetDefinationCache));
                float starSum = 0.0f;
                int cntIndex = starCount - 1;
                do
                {
                    var cache = GetDefinationCache(definitions[cntIndex]);
                    starSum += cache.MeasureSize;
                    cache.SizeCache = starSum;
                }
                while (--cntIndex >= 0);
                cntIndex = 0;
                do
                {
                    var def = definitions[cntIndex];
                    var cache = GetDefinationCache(def);
                    var measureSize = cache.MeasureSize;
                    float size = 0.0f;
                    if (measureSize == 0)
                        size = cache.MinSize;
                    else
                    {
                        var value = Math.Max(finalSize - sumSize, 0.0f) * (measureSize / cache.SizeCache);
                        size = Math.Min(value, def.UserMaxSize);
                        size = Math.Max(size, cache.MinSize);
                    }
                    cache.SizeCache = size;
                    sumSize += cache.SizeCache;
                }
                while (++cntIndex < starCount);
            }
            if (sumSize > finalSize)
            {
                Array.Sort(layoutData.DefinitionIndexes, 0, definitions.Count, new StarDistributionOrderIndexComparer(definitions, GetDefinationCache));
                float exceed = finalSize - sumSize;
                for (int i = 0; i < definitions.Count; i++)
                {
                    var index = layoutData.DefinitionIndexes[i];
                    var def = definitions[i];
                    var cache = GetDefinationCache(def);
                    var size = cache.SizeCache + exceed / (definitions.Count - i);
                    size = Math.Max(size, cache.MinSize);
                    size = Math.Min(size, cache.SizeCache);
                    exceed -= size - cache.SizeCache;
                    cache.SizeCache = size;
                }
                sumSize = finalSize - exceed;
            }
            GetDefinationCache(definitions[0]).FinalOffset = 0.0f;
            for (int i = 0; i < definitions.Count; i++)
            {
                var cache1 = GetDefinationCache(definitions[i]);
                var cache2 = GetDefinationCache(definitions[(i + 1) % definitions.Count]);
                cache2.FinalOffset = cache1.FinalOffset + cache1.SizeCache;
            }
        }

        private const int _maxLayoutLoopCount = 5;
        private const float _starClip = float.MaxValue;

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
                    ValidateDefinations(layoutData.DefinitionsH, float.IsPositiveInfinity(availableSize.Width));
                    ValidateDefinations(layoutData.DefinitionsV, float.IsPositiveInfinity(availableSize.Height));
                    ValidateCells();
                    MeasureCellsGroup(layoutData.CellGroup1, availableSize, false, false);
                    if (layoutData.HasGroup3CellsInAutoRows)
                    {
                        if (!layoutData.CellGroup2.HasValue)
                        {
                            if (layoutData.HasStarCellsH)
                                ResolveStar(layoutData.DefinitionsH, availableSize.Width);
                            MeasureCellsGroup(layoutData.CellGroup3, availableSize, false, false);
                            if (layoutData.HasStarCellsV)
                                ResolveStar(layoutData.DefinitionsV, availableSize.Height);
                        }
                        else
                        {
                            var minSizesGroup2H = CacheMinSizes(layoutData.CellGroup2, layoutData.DefinitionsH, o => o.ColumnIndex);
                            var minSizesGroup3V = CacheMinSizes(layoutData.CellGroup2, layoutData.DefinitionsH, o => o.RowIndex);
                            MeasureCellsGroup(layoutData.CellGroup2, availableSize, false, true);
                            bool hasDesiredSizeHChanged = false;
                            int leftLoopCount = _maxLayoutLoopCount - 1;
                            do
                            {
                                if (hasDesiredSizeHChanged)
                                    ApplyCachedMinSizes(minSizesGroup3V, layoutData.DefinitionsV);
                                if (layoutData.HasStarCellsH)
                                    ResolveStar(layoutData.DefinitionsH, availableSize.Width);
                                MeasureCellsGroup(layoutData.CellGroup3, availableSize, false, false);
                                ApplyCachedMinSizes(minSizesGroup2H, layoutData.DefinitionsH);
                                if (layoutData.HasStarCellsV)
                                    ResolveStar(layoutData.DefinitionsV, availableSize.Height);
                                MeasureCellsGroup(layoutData.CellGroup2, availableSize, leftLoopCount == 0, false, out hasDesiredSizeHChanged);
                            }
                            while (hasDesiredSizeHChanged && --leftLoopCount >= 0);
                        }
                    }
                    else
                    {
                        if (layoutData.HasStarCellsV)
                            ResolveStar(layoutData.DefinitionsV, availableSize.Height);
                        MeasureCellsGroup(layoutData.CellGroup2, availableSize, false, false);
                        if (layoutData.HasStarCellsH)
                            ResolveStar(layoutData.DefinitionsH, availableSize.Width);
                        MeasureCellsGroup(layoutData.CellGroup3, availableSize, false, false);
                    }
                    MeasureCellsGroup(layoutData.CellGroup4, availableSize, false, false);
                    return new Size(ComputeDesiredSize(layoutData.DefinitionsH), ComputeDesiredSize(layoutData.DefinitionsV));
                }
            }
            finally
            {
                _isMeasureInProgress = false;
            }
        }

        private float ComputeDesiredSize(IReadOnlyList<DefinitionBase> definitions)
        {
            return definitions.Sum(o => GetDefinationCache(o).MinSize);
        }

        private void ApplyCachedMinSizes(float[] minSizes, IReadOnlyList<DefinitionBase> definitions)
        {
            for (int i = 0; i < minSizes.Length; i++)
            {
                var minSize = minSizes[i];
                if (minSize >= 0)
                    GetDefinationCache(definitions[i]).MinSize = minSize;
            }
        }

        private float[] CacheMinSizes(int? cellsHead, IReadOnlyList<DefinitionBase> definitions, Func<CellCache, int> indexGetter)
        {
            var sizes = new float[definitions.Count];
            for (int i = 0; i < sizes.Length; i++)
                sizes[i] = -1.0f;

            var layoutData = _layoutData;
            var cell = cellsHead;
            while (cell.HasValue)
            {
                var index = indexGetter(layoutData.CellCaches[cell.Value]);
                sizes[index] = GetDefinationCache(definitions[index]).MinSize;
                cell = layoutData.CellCaches[cell.Value].Next;
            }
            return sizes;
        }

        private void MeasureCellsGroup(int? cellsHead, Size referenceSize, bool ignoreDesiredSizeH, bool forceInfinityV)
        {
            bool changed;
            MeasureCellsGroup(cellsHead, referenceSize, ignoreDesiredSizeH, forceInfinityV, out changed);
        }

        private void MeasureCellsGroup(int? cellsHead, Size referenceSize, bool ignoreDesiredSizeH, bool forceInfinityV, out bool hasDesiredSizeHChanged)
        {
            hasDesiredSizeHChanged = false;

            if (cellsHead.HasValue)
            {
                var layoutData = _layoutData;
                var cell = cellsHead;
                Dictionary<SpanKey, float> spanDic = null;
                do
                {
                    var index = cell.Value;
                    var element = Children[index];
                    var width = element.DesiredSize.Width;
                    MeasureCell(index, forceInfinityV);
                    hasDesiredSizeHChanged |= width != element.DesiredSize.Width;

                    var cellCache = layoutData.CellCaches[index];
                    if (!ignoreDesiredSizeH)
                    {
                        var def = layoutData.DefinitionsH[cellCache.ColumnIndex];
                        var cache = GetDefinationCache(def);
                        if (cellCache.ColumnSpan == 1)
                            cache.MinSize = Math.Min(element.DesiredSize.Width, def.UserMaxSize);
                        else
                            RegisterSpan(ref spanDic, cellCache.ColumnIndex, cellCache.ColumnSpan, true, element.DesiredSize.Width);
                    }
                    if (!forceInfinityV)
                    {
                        var def = layoutData.DefinitionsV[cellCache.RowIndex];
                        var cache = GetDefinationCache(def);
                        if (cellCache.RowIndex == 1)
                            cache.MinSize = Math.Min(element.DesiredSize.Height, def.UserMaxSize);
                        else
                            RegisterSpan(ref spanDic, cellCache.RowIndex, cellCache.RowSpan, false, element.DesiredSize.Height);
                    }
                    cell = cellCache.Next;
                }
                while (cell.HasValue);

                if (spanDic != null)
                {
                    foreach (var item in spanDic)
                    {
                        var key = item.Key;
                        EnsureMinSizeInDefinitionRange(key.IsH ? (IReadOnlyList<DefinitionBase>)layoutData.DefinitionsH : layoutData.DefinitionsV,
                            key.Start, key.Count, item.Value, key.IsH ? referenceSize.Width : referenceSize.Height);
                    }
                }
            }
        }

        private readonly SpanPreferredDistributionOrderComparer _spanPreferDistComparer;
        private readonly SpanMaxDistributionOrderComparer _spanMaxDistComparer;

        private void EnsureMinSizeInDefinitionRange(IReadOnlyList<DefinitionBase> definitions, int start, int count, float requestedSize, float referenceSize)
        {
            if (requestedSize != 0.0f)
            {
                var tempDefinitions = _layoutData.TempDefinitions;
                var end = start + count;
                int autoCount = 0;
                float minSizeSum = 0;
                float preferSizeSum = 0;
                float maxSizeSum = 0;
                float mostMaxSize = 0;

                for (int i = start; i < end; i++)
                {
                    var def = definitions[i];
                    var cache = GetDefinationCache(def);
                    var minSize = cache.MinSize;
                    var preferSize = cache.PreferredSize;
                    var maxSize = Math.Max(def.UserMaxSize, minSize);

                    minSizeSum += minSize;
                    preferSizeSum += preferSize;
                    maxSizeSum += maxSize;

                    cache.SizeCache = maxSize;
                    if (mostMaxSize < maxSize)
                        mostMaxSize = maxSize;
                    if (def.UserSize.UnitType == GridUnitType.Auto)
                        autoCount++;
                    tempDefinitions[i - start] = def;
                }

                if (requestedSize > minSizeSum)
                {
                    if (requestedSize <= preferSizeSum)
                    {
                        Array.Sort(tempDefinitions, 0, count, _spanPreferDistComparer);
                        int i = 0;
                        float restSize = requestedSize;
                        while (i < autoCount)
                        {
                            restSize -= GetDefinationCache(tempDefinitions[i]).MinSize;
                            i++;
                        }
                        while (i < count)
                        {
                            var def = tempDefinitions[i];
                            var cache = GetDefinationCache(def);
                            var minSize = Math.Min(restSize / (count - i), cache.PreferredSize);
                            if (minSize > cache.MinSize)
                                cache.MinSize = minSize;
                            restSize -= minSize;
                            i++;
                        }
                        return;
                    }
                    if (requestedSize <= maxSizeSum)
                    {
                        Array.Sort(tempDefinitions, 0, count, _spanMaxDistComparer);
                        int i = 0;
                        float restSize = requestedSize - preferSizeSum;
                        while (i < count - autoCount)
                        {
                            var def = tempDefinitions[i];
                            var cache = GetDefinationCache(def);
                            var preferSize = cache.PreferredSize;
                            var value = preferSize + restSize / (count - autoCount - i);
                            cache.MinSize = Math.Min(value, cache.SizeCache);
                            restSize -= cache.MinSize - preferSize;
                            i++;
                        }
                        while (i < count)
                        {
                            var def = tempDefinitions[i];
                            var cache = GetDefinationCache(def);
                            var minSize = cache.MinSize;
                            var value = minSize + restSize / (count - i);
                            cache.MinSize = Math.Min(value, cache.SizeCache);
                            restSize -= cache.MinSize - minSize;
                            i++;
                        }
                        return;
                    }
                    var avgSize = requestedSize / count;
                    if (avgSize < mostMaxSize)
                    {
                        var exceedSize = mostMaxSize * count - maxSizeSum;
                        var restSize = requestedSize - maxSizeSum;
                        for (int i = 0; i < count; i++)
                        {
                            var def = tempDefinitions[i];
                            var cache = GetDefinationCache(def);
                            var size = (mostMaxSize - cache.SizeCache) * restSize / exceedSize;
                            cache.MinSize = (cache.SizeCache + size);
                        }
                        return;
                    }
                    for (int i = 0; i < count; i++)
                        GetDefinationCache(tempDefinitions[i]).MinSize = (avgSize);
                }
            }
        }

        private void RegisterSpan(ref Dictionary<SpanKey, float> spanDic, int start, int count, bool isH, float size)
        {
            if (spanDic == null)
                spanDic = new Dictionary<SpanKey, float>();
            var key = new SpanKey(start, count, isH);
            float oldSize;
            if (!spanDic.TryGetValue(key, out oldSize) || size > oldSize)
                spanDic[key] = size;
        }

        private void MeasureCell(int cell, bool forceInfinityV)
        {
            var cellCaches = _layoutData.CellCaches;

            float width;
            var cellCache = cellCaches[cell];
            if (cellCache.SizeFlagsH.HasFlag(LayoutTimeSizeFlags.Auto) && !cellCache.SizeFlagsH.HasFlag(LayoutTimeSizeFlags.Star))
                width = float.PositiveInfinity;
            else
                width = ComputeMeasureSizeForRange(_layoutData.DefinitionsH, cellCache.ColumnIndex, cellCache.ColumnSpan);

            float height;
            if (forceInfinityV)
                height = float.PositiveInfinity;
            else if (cellCache.SizeFlagsV.HasFlag(LayoutTimeSizeFlags.Auto) && !cellCache.SizeFlagsV.HasFlag(LayoutTimeSizeFlags.Star))
                height = float.PositiveInfinity;
            else
                height = ComputeMeasureSizeForRange(_layoutData.DefinitionsV, cellCache.RowIndex, cellCache.RowSpan);
            var element = Children[cell];
            element.Measure(new Size(width, height));
        }

        private float ComputeMeasureSizeForRange(IReadOnlyList<DefinitionBase> definitions, int start, int count)
        {
            float size = 0.0f;
            var index = start + count - 1;
            do
            {
                var def = definitions[index];
                var cache = GetDefinationCache(def);
                size += (cache.SizeFlags == LayoutTimeSizeFlags.Auto) ? cache.MinSize : cache.MeasureSize;
            }
            while (--index >= start);
            return size;
        }

        private readonly StarDistributionOrderComparer _starDistOrderComparer;

        private void ResolveStar(IReadOnlyList<DefinitionBase> definitions, float availableSize)
        {
            var tempDefinitions = _layoutData.TempDefinitions;
            int starCount = 0;
            float noStarSize = 0.0f;

            for (int i = 0; i < definitions.Count; i++)
            {
                var def = definitions[i];
                var cache = GetDefinationCache(def);
                switch (cache.SizeFlags)
                {
                    case LayoutTimeSizeFlags.Pixel:
                        noStarSize += cache.MeasureSize;
                        break;
                    case LayoutTimeSizeFlags.Auto:
                        noStarSize += cache.MinSize;
                        break;
                    case LayoutTimeSizeFlags.Star:
                        {
                            tempDefinitions[starCount++] = def;
                            float value = def.UserSize.Value;
                            if (value == 0.0f)
                            {
                                cache.MeasureSize = 0.0f;
                                cache.SizeCache = 0.0f;
                            }
                            else
                            {
                                value = Math.Min(value, _starClip);
                                cache.MinSize = value;
                                var maxSize = Math.Max(cache.MinSize, def.UserMaxSize);
                                maxSize = Math.Min(maxSize, _starClip);
                                cache.SizeCache = maxSize / value;
                            }
                        }
                        break;
                    default:
                        break;
                }
            }
            if (starCount > 0)
            {
                Array.Sort(tempDefinitions, 0, starCount, _starDistOrderComparer);
                float size = 0.0f;
                int cnt = starCount - 1;
                do
                {
                    var cache = GetDefinationCache(tempDefinitions[cnt]);
                    size += cache.MeasureSize;
                    cache.SizeCache = size;
                } while (--cnt >= 0);
                cnt = 0;
                do
                {
                    var def = tempDefinitions[cnt];
                    var cache = GetDefinationCache(def);
                    var measureSize = cache.MeasureSize;
                    float minSize;
                    if (measureSize == 0)
                        minSize = cache.MinSize;
                    else
                    {
                        var val = Math.Max(availableSize - noStarSize, 0.0f) * (measureSize / cache.SizeCache);
                        minSize = Math.Min(val, def.UserMaxSize);
                        minSize = Math.Max(cache.MinSize, minSize);
                    }
                    cache.MeasureSize = minSize;
                    noStarSize += minSize;
                }
                while (++cnt < starCount);
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
            var layoutData = _layoutData;
            layoutData.CellGroup1 = layoutData.CellGroup2 = layoutData.CellGroup3 = layoutData.CellGroup4 = null;

            var cellCaches = layoutData.CellCaches;
            if (cellCaches?.Length != Children.Count)
                cellCaches = (layoutData.CellCaches = new CellCache[Children.Count]);

            var maxColumnSpan = Math.Max(1, layoutData.ColumnDefinitionCollection?.Count ?? 0);
            var maxRowSpan = Math.Max(1, layoutData.RowDefinitionCollection?.Count ?? 0);
            var maxColumnIndex = maxColumnSpan - 1;
            var maxRowIndex = maxRowSpan - 1;

            bool hasGroup3CellsInAutoRows = false;
            bool hasStarCellsH = false;
            bool hasStarCellsV = false;

            for (int i = Children.Count - 1; i >= 0; i--)
            {
                var element = Children[i];
                var cellCache = new CellCache
                {
                    ColumnIndex = Math.Max(0, Math.Min(Grid.GetColumn(element), maxColumnIndex)),
                    RowIndex = Math.Max(0, Math.Min(Grid.GetRow(element), maxRowIndex))
                };
                cellCache.ColumnSpan = Math.Max(1, Math.Min(Grid.GetColumnSpan(element), maxColumnSpan - cellCache.ColumnIndex));
                cellCache.RowSpan = Math.Max(1, Math.Min(Grid.GetRowSpan(element), maxRowSpan - cellCache.RowIndex));
                cellCache.SizeFlagsH = ComputeSizeFlagsForRange(layoutData.DefinitionsH, cellCache.ColumnIndex, cellCache.ColumnSpan);
                cellCache.SizeFlagsV = ComputeSizeFlagsForRange(layoutData.DefinitionsV, cellCache.RowIndex, cellCache.RowSpan);
                hasStarCellsH |= cellCache.SizeFlagsH.HasFlag(LayoutTimeSizeFlags.Star);
                hasStarCellsV |= cellCache.SizeFlagsV.HasFlag(LayoutTimeSizeFlags.Star);

                if (!cellCache.SizeFlagsV.HasFlag(LayoutTimeSizeFlags.Star))
                {
                    if (!cellCache.SizeFlagsH.HasFlag(LayoutTimeSizeFlags.Star))
                    {
                        cellCache.Next = layoutData.CellGroup1;
                        layoutData.CellGroup1 = i;
                    }
                    else
                    {
                        cellCache.Next = layoutData.CellGroup3;
                        layoutData.CellGroup3 = i;
                        hasGroup3CellsInAutoRows |= cellCache.SizeFlagsV.HasFlag(LayoutTimeSizeFlags.Auto);
                    }
                }
                else if (cellCache.SizeFlagsH.HasFlag(LayoutTimeSizeFlags.Auto) && !cellCache.SizeFlagsH.HasFlag(LayoutTimeSizeFlags.Star))
                {
                    cellCache.Next = layoutData.CellGroup2;
                    layoutData.CellGroup2 = i;
                }
                else
                {
                    cellCache.Next = layoutData.CellGroup4;
                    layoutData.CellGroup4 = i;
                }
                layoutData.CellCaches[i] = cellCache;
            }
            layoutData.HasGroup3CellsInAutoRows = hasGroup3CellsInAutoRows;
            layoutData.HasStarCellsH = hasStarCellsH;
            layoutData.HasStarCellsV = hasStarCellsV;
        }

        private LayoutTimeSizeFlags ComputeSizeFlagsForRange(IEnumerable<DefinitionBase> definitions, int start, int count)
        {
            return definitions.Skip(start).Take(count).Aggregate(LayoutTimeSizeFlags.None, (s, c) => s | GetDefinationCache(c).SizeFlags);
        }

        private void ValidateDefinations(IEnumerable<DefinitionBase> definitions, bool treateStarAsAuto)
        {
            foreach (var def in definitions)
            {
                var minSize = def.UserMinSize;
                var maxSize = def.UserMaxSize;
                var size = def.UserSize;
                float value = 0.0f;
                var cache = GetDefinationCache(def);
                if (size.UnitType == GridUnitType.Auto || (size.UnitType == GridUnitType.Star && treateStarAsAuto))
                {
                    cache.SizeFlags = LayoutTimeSizeFlags.Auto;
                    value = float.PositiveInfinity;
                }
                else if (size.UnitType == GridUnitType.Pixel)
                {
                    cache.SizeFlags = LayoutTimeSizeFlags.Pixel;
                    value = size.Value;
                    minSize = Math.Max(minSize, Math.Min(value, maxSize));
                }
                else
                {
                    cache.SizeFlags = LayoutTimeSizeFlags.Star;
                    value = float.PositiveInfinity;
                }
                cache.MinSize = minSize;
                cache.MeasureSize = Math.Max(minSize, Math.Min(value, maxSize));
            }
        }

        internal void InvalidateCells()
        {
            _cellsStrutureDirty = true;
            InvalidateMeasure();
        }

        private DefinationCache GetDefinationCache(DefinitionBase item)
        {
            return _layoutData.DefinitionCaches[item];
        }

        internal void AddDefination(DefinitionBase item)
        {
            _layoutData.DefinitionCaches.Add(item, new DefinationCache());
        }

        internal void RemoveDefination(DefinitionBase item)
        {
            _layoutData.DefinitionCaches.Remove(item);
        }

        private static void OnRowColumnPropertyChanged(object sender, PropertyChangedEventArgs<int> e)
        {
            var grid = VisualTreeHelper.GetParent((Visual)sender) as Grid;
            if (grid != null && grid._layoutData != null)
                grid.InvalidateCells();
        }

        class DefinationCache
        {
            public Grid.LayoutTimeSizeFlags SizeFlags;
            private float _minSize;
            public float MinSize
            {
                get => _minSize;
                set => _minSize = value;
            }

            public float MeasureSize;
            public float FinalOffset;
            public float SizeCache;

            public float PreferredSize
            {
                get
                {
                    var value = MinSize;
                    if (SizeFlags != LayoutTimeSizeFlags.Auto && value < MeasureSize)
                        return MeasureSize;
                    return value;
                }
            }
        }

        private class LayoutData
        {
            public ColumnDefinitionCollection ColumnDefinitionCollection;
            public RowDefinitionCollection RowDefinitionCollection;
            public CellCache[] CellCaches;
            public int? CellGroup1;
            public int? CellGroup2;
            public int? CellGroup3;
            public int? CellGroup4;
            public bool HasGroup3CellsInAutoRows;
            public bool HasStarCellsH;
            public bool HasStarCellsV;
            public readonly Dictionary<DefinitionBase, DefinationCache> DefinitionCaches = new Dictionary<DefinitionBase, DefinationCache>();

            private bool _definitionsHIsDummy;
            private IReadOnlyList<ColumnDefinition> _definitionsH;
            private bool _definitionsVIsDummy;
            private IReadOnlyList<RowDefinition> _definitionsV;

            public IReadOnlyList<ColumnDefinition> DefinitionsH
            {
                get
                {
                    if ((ColumnDefinitionCollection?.Count ?? 0) == 0)
                    {
                        if (_definitionsH == null)
                        {
                            _definitionsHIsDummy = true;
                            _definitionsH = new[] { new ColumnDefinition() };
                            DefinitionCaches.Add(_definitionsH[0], new DefinationCache());
                        }
                    }
                    else
                    {
                        if (_definitionsHIsDummy)
                        {
                            foreach (var item in _definitionsH)
                                DefinitionCaches.Remove(item);
                            _definitionsHIsDummy = false;
                        }
                        _definitionsH = ColumnDefinitionCollection;
                    }
                    return _definitionsH;
                }
            }

            public IReadOnlyList<RowDefinition> DefinitionsV
            {
                get
                {
                    if ((RowDefinitionCollection?.Count ?? 0) == 0)
                    {
                        if (_definitionsV == null)
                        {
                            _definitionsVIsDummy = true;
                            _definitionsV = new[] { new RowDefinition() };
                            DefinitionCaches.Add(_definitionsV[0], new DefinationCache());
                        }
                    }
                    else
                    {
                        if (_definitionsVIsDummy)
                        {
                            foreach (var item in _definitionsV)
                                DefinitionCaches.Remove(item);
                            _definitionsVIsDummy = false;
                        }
                        _definitionsV = RowDefinitionCollection;
                    }
                    return _definitionsV;
                }
            }

            private int[] _definitionIndexes;
            public int[] DefinitionIndexes
            {
                get
                {
                    var num = Math.Max(DefinitionsH.Count, DefinitionsV.Count);
                    if (_definitionIndexes == null || _definitionIndexes.Length < num)
                        _definitionIndexes = new int[num];
                    return _definitionIndexes;
                }
            }

            private DefinitionBase[] _tempDefinitions;
            public DefinitionBase[] TempDefinitions
            {
                get
                {
                    var num = Math.Max(DefinitionsH.Count, DefinitionsV.Count) * 2;
                    if (_tempDefinitions == null || _tempDefinitions.Length < num)
                        _tempDefinitions = new DefinitionBase[num];
                    return _tempDefinitions;
                }
            }
        }

        struct CellCache
        {
            public int ColumnIndex;
            public int RowIndex;
            public int ColumnSpan;
            public int RowSpan;
            public LayoutTimeSizeFlags SizeFlagsH;
            public LayoutTimeSizeFlags SizeFlagsV;
            public int? Next;
        }

        [Flags]
        internal enum LayoutTimeSizeFlags
        {
            None = 0,
            Pixel = 1,
            Auto = 2,
            Star = 4
        }

        class StarDistributionOrderIndexComparer : IComparer<int>
        {
            private readonly IReadOnlyList<DefinitionBase> _definitions;
            private readonly Func<DefinitionBase, DefinationCache> _cacheGetter;

            internal StarDistributionOrderIndexComparer(IReadOnlyList<DefinitionBase> definitions, Func<DefinitionBase, DefinationCache> cacheGetter)
            {
                _definitions = definitions;
                _cacheGetter = cacheGetter;
            }

            public int Compare(int x, int y)
            {
                return _cacheGetter(_definitions[x]).SizeCache.CompareTo(_cacheGetter(_definitions[y]).SizeCache);
            }
        }

        class StarDistributionOrderComparer : IComparer<DefinitionBase>
        {
            private readonly Func<DefinitionBase, DefinationCache> _cacheGetter;
            public StarDistributionOrderComparer(Func<DefinitionBase, DefinationCache> cacheGetter)
            {
                _cacheGetter = cacheGetter;
            }

            public int Compare(DefinitionBase x, DefinitionBase y)
            {
                return _cacheGetter(x).SizeCache.CompareTo(_cacheGetter(y).SizeCache);
            }
        }

        struct SpanKey : IEquatable<SpanKey>
        {
            public int Start { get; }
            public int Count { get; }
            public bool IsH { get; }

            public SpanKey(int start, int count, bool isH)
            {
                Start = start;
                Count = count;
                IsH = isH;
            }

            public bool Equals(SpanKey other)
            {
                return Start == other.Start && Count == other.Count && IsH == other.IsH;
            }

            public override bool Equals(object obj)
            {
                if (obj is SpanKey)
                    return Equals((SpanKey)obj);
                return false;
            }

            public override int GetHashCode()
            {
                return Start.GetHashCode() ^ Count.GetHashCode() ^ IsH.GetHashCode();
            }
        }

        class SpanPreferredDistributionOrderComparer : IComparer<DefinitionBase>
        {
            private readonly Func<DefinitionBase, DefinationCache> _cacheGetter;
            public SpanPreferredDistributionOrderComparer(Func<DefinitionBase, DefinationCache> cacheGetter)
            {
                _cacheGetter = cacheGetter;
            }

            public int Compare(DefinitionBase x, DefinitionBase y)
            {
                if (x.UserSize.UnitType == GridUnitType.Auto)
                {
                    if (y.UserSize.UnitType == GridUnitType.Auto)
                        return _cacheGetter(x).MinSize.CompareTo(_cacheGetter(y).MinSize);
                    return -1;
                }
                else if (y.UserSize.UnitType == GridUnitType.Auto)
                    return 1;
                else
                    return _cacheGetter(x).PreferredSize.CompareTo(_cacheGetter(y).PreferredSize);
            }
        }

        class SpanMaxDistributionOrderComparer : IComparer<DefinitionBase>
        {
            private readonly Func<DefinitionBase, DefinationCache> _cacheGetter;
            public SpanMaxDistributionOrderComparer(Func<DefinitionBase, DefinationCache> cacheGetter)
            {
                _cacheGetter = cacheGetter;
            }

            public int Compare(DefinitionBase x, DefinitionBase y)
            {
                if (x.UserSize.UnitType == GridUnitType.Auto)
                {
                    if (y.UserSize.UnitType == GridUnitType.Auto)
                        return _cacheGetter(x).SizeCache.CompareTo(_cacheGetter(y).SizeCache);
                    return 1;
                }
                else if (y.UserSize.UnitType == GridUnitType.Auto)
                    return -1;
                else
                    return _cacheGetter(x).SizeCache.CompareTo(_cacheGetter(y).SizeCache);
            }
        }
    }
}
