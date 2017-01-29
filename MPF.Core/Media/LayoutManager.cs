using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace MPF.Media
{
    internal class LayoutManager
    {
        public static LayoutManager Current { get; } = new LayoutManager();

        private ConcurrentBag<UIElement> _arrangeSetLast = new ConcurrentBag<UIElement>();
        private ConcurrentBag<UIElement> _arrangeSet = new ConcurrentBag<UIElement>();
        private ConcurrentBag<UIElement> _measureSetLast = new ConcurrentBag<UIElement>();
        private ConcurrentBag<UIElement> _measureSet = new ConcurrentBag<UIElement>();
        private ConcurrentBag<UIElement> _renderSetLast = new ConcurrentBag<UIElement>();
        private ConcurrentBag<UIElement> _renderSet = new ConcurrentBag<UIElement>();
        private ConcurrentBag<FrameworkElement> _initializeSetLast = new ConcurrentBag<FrameworkElement>();
        private ConcurrentBag<FrameworkElement> _initializeSet = new ConcurrentBag<FrameworkElement>();

        public void Update()
        {
            //Swap(ref _measureSet, ref _measureSetLast);
            //foreach (var item in _measureSetLast)
            //{
            //    var size = (item.Parent as UIElement)?.RenderSize ?? new Size(float.NaN, float.NaN);
            //    item.Measure(size);
            //}
            //_measureSetLast.Clear();

            //Swap(ref _arrangeSet, ref _arrangeSetLast);
            //foreach (var item in _arrangeSetLast)
            //{
            //    if (item.UIFlags.HasFlag(UIElementFlags.ArrangeDirty))
            //        OnUpdate(item, item.Parent as UIElement, false);
            //}
            //_arrangeSetLast.Clear();

            //Swap(ref _renderSet, ref _renderSetLast);
            //foreach (var item in _renderSetLast)
            //    item.Render();
            //_renderSetLast.Clear();

            Swap(ref _initializeSet, ref _initializeSetLast);
            foreach (var item in _initializeSetLast.Distinct().OrderBy(o => o.VisualLevel))
                item.OnInitialize();
            _initializeSetLast = new ConcurrentBag<FrameworkElement>();
        }

        private Rect GetArrangeRect(UIElement element)
        {
            if (element != null)
                return new Rect((Point)element.VisualOffset, element.RenderSize);
            return new Rect(0, 0, float.NaN, float.NaN);
        }

        private void OnUpdate(UIElement element, UIElement parent, bool forceArrange)
        {
            var flags = element.UIFlags;
            if (forceArrange || flags.HasFlag(UIElementFlags.MeasureDirty))
                element.Measure(parent?.RenderSize ?? new Size(float.NaN, float.NaN));
            if (forceArrange || flags.HasFlag(UIElementFlags.ArrangeDirty))
            {
                forceArrange = true;
                element.Arrange(GetArrangeRect(parent));
            }
            if (forceArrange || flags.HasFlag(UIElementFlags.RenderDirty))
                element.Render();

            foreach (UIElement child in element.VisualChildren)
                OnUpdate(child, element, forceArrange);
        }

        public void RegisterArrange(UIElement element)
        {
            _arrangeSet.Add(element);
        }

        public void RegisterMeasure(UIElement element)
        {
            _measureSet.Add(element);
        }

        public void RegisterRender(UIElement element)
        {
            _renderSet.Add(element);
        }

        public void RegisterInitialize(FrameworkElement element)
        {
            _initializeSet.Add(element);
        }

        private void Swap<T>(ref T a, ref T b) where T : class
        {
            b = Interlocked.Exchange(ref a, b);
        }

        class VisualLevelComparer : IComparer<UIElement>
        {
            public static IComparer<UIElement> Default { get; } = new VisualLevelComparer();

            public int Compare(UIElement x, UIElement y)
            {
                return x.VisualLevel - y.VisualLevel;
            }
        }
    }
}
