using MPF.Data;
using MPF.Input;
using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Threading.Tasks;

namespace MPF
{
    [Flags]
    internal enum UIElementFlags : uint
    {
        None = 0x0,
        MeasureDirty = 0x1,
        RenderDirty = 0x2,
        ArrangeDirty = 0x4
    }

    public class UIElement : Visual, IInputElement
    {
        private UIElementFlags _uiFlags = UIElementFlags.ArrangeDirty;
        internal UIElementFlags UIFlags => _uiFlags;
        private Size _renderSize;

        public Size RenderSize
        {
            get { return _renderSize; }
            set { _renderSize = value; }
        }

        public static readonly DependencyProperty<Visibility> VisibilityProperty = DependencyProperty.Register(nameof(Visibility), typeof(UIElement),
            new PropertyMetadata<Visibility>(Visibility.Visible, OnVisibilityPropertyChanged));

        public static readonly DependencyProperty<bool> IsEnabledProperty = DependencyProperty.Register(nameof(IsEnabled), typeof(UIElement),
            new PropertyMetadata<bool>(true));

        public Visibility Visibility
        {
            get { return GetValue(VisibilityProperty); }
            set { this.SetLocalValue(VisibilityProperty, value); }
        }

        public bool IsEnabled
        {
            get { return GetValue(IsEnabledProperty); }
            set { this.SetLocalValue(IsEnabledProperty, value); }
        }

        public static readonly RoutedEvent<PointerRoutedEventArgs> PointerMoveEvent = RoutedEvent.Register<PointerRoutedEventArgs>(nameof(PointerMove),
            typeof(UIElement), RoutingStrategy.Bubble);
        public static readonly RoutedEvent<PointerButtonRoutedEventArgs> PointerPressedEvent = RoutedEvent.Register<PointerButtonRoutedEventArgs>(nameof(PointerPressed),
            typeof(UIElement), RoutingStrategy.Bubble);
        public static readonly RoutedEvent<PointerButtonRoutedEventArgs> PointerReleasedEvent = RoutedEvent.Register<PointerButtonRoutedEventArgs>(nameof(PointerReleased),
            typeof(UIElement), RoutingStrategy.Bubble);

        public event EventHandler<PointerRoutedEventArgs> PointerMove
        {
            add { AddHandler(PointerMoveEvent, value); }
            remove { RemoveHandler(PointerMoveEvent, value); }
        }

        public event EventHandler<PointerButtonRoutedEventArgs> PointerPressed
        {
            add { AddHandler(PointerPressedEvent, value); }
            remove { RemoveHandler(PointerPressedEvent, value); }
        }

        public event EventHandler<PointerButtonRoutedEventArgs> PointerReleased
        {
            add { AddHandler(PointerReleasedEvent, value); }
            remove { RemoveHandler(PointerReleasedEvent, value); }
        }

        private Size _desiredSize;
        public Size DesiredSize => Visibility == Visibility.Collapsed ? default(Size) : _desiredSize;
        internal Rect? LastFinalRect { get; private set; }
        internal Size LastAvailableSize { get; private set; }

        public UIElement()
        {
            UpdateVisualVisibility(Visibility);
            InvalidateArrange();
            InvalidateRender();
            InvalidateBoundingBox();
        }

        internal new void Render()
        {
            ClearFlags(UIElementFlags.RenderDirty);
            base.Render();
        }

        public void InvalidateArrange()
        {
            SetUIFlags(UIElementFlags.ArrangeDirty);
            LayoutManager.Current.RegisterArrange(this);
        }

        protected override void OnParentChanged(Visual visual)
        {
            (visual as UIElement)?.InvalidateArrange();
            base.OnParentChanged(visual);
        }

        public void Arrange(Rect finalRect)
        {
            LastFinalRect = finalRect;
            ClearFlags(UIElementFlags.ArrangeDirty);
            ArrangeCore(finalRect);
            InvalidateBoundingBox();
            InvalidateRender();
        }

        protected virtual void ArrangeCore(Rect finalRect)
        {
            RenderSize = finalRect.Size;
            VisualOffset = (Vector2)finalRect.Location;
        }

        public void InvalidateMeasure()
        {
            SetUIFlags(UIElementFlags.MeasureDirty);
            LayoutManager.Current.RegisterMeasure(this);
        }

        public void Measure(Size availableSize)
        {
            LastAvailableSize = availableSize;
            ClearFlags(UIElementFlags.MeasureDirty);
            _desiredSize = MeasureCore(availableSize);
            InvalidateArrange();
        }

        protected virtual Size MeasureCore(Size availableSize)
        {
            return Size.Zero;
        }

        private void SetUIFlags(UIElementFlags flags)
        {
            _uiFlags |= flags;
        }

        private void ClearFlags(UIElementFlags flags)
        {
            _uiFlags &= ~flags;
        }

        internal void InvalidateRender()
        {
            SetUIFlags(UIElementFlags.RenderDirty);
            LayoutManager.Current.RegisterRender(this);
        }

        private void UpdateVisualVisibility(Visibility value)
        {
            if (value == Visibility.Visible)
                IsVisualVisible = true;
            else
                IsVisualVisible = false;
        }

        private static void OnVisibilityPropertyChanged(object sender, PropertyChangedEventArgs<Visibility> e)
        {
            ((UIElement)sender).UpdateVisualVisibility(e.NewValue);
        }

        #region Routed Events

        private readonly ConcurrentDictionary<RoutedEvent, List<Tuple<Delegate, bool>>> _eventHandlers = new ConcurrentDictionary<RoutedEvent, List<Tuple<Delegate, bool>>>();

        public void AddHandler<TArgs>(RoutedEvent<TArgs> routedEvent, EventHandler<TArgs> handler, bool handledEventsToo = false) where TArgs : RoutedEventArgs
        {
            var storage = _eventHandlers.GetOrAdd(routedEvent, k => new List<Tuple<Delegate, bool>>());
            storage.Add(Tuple.Create((Delegate)handler, handledEventsToo));
        }

        public void RemoveHandler<TArgs>(RoutedEvent<TArgs> routedEvent, EventHandler<TArgs> handler) where TArgs : RoutedEventArgs
        {
            List<Tuple<Delegate, bool>> storage;
            if (_eventHandlers.TryGetValue(routedEvent, out storage))
            {
                var index = storage.FindLastIndex(o => o.Item1 == (Delegate)handler);
                if (index != -1)
                    storage.RemoveAt(index);
            }
        }

        public static void RaiseEvent<TArgs>(TArgs eventArgs) where TArgs : RoutedEventArgs
        {
            var routingStrategy = eventArgs.RoutedEvent.RoutingStrategy;
            if (routingStrategy == RoutingStrategy.Direct)
                (eventArgs.OriginalSource as UIElement)?.InvokeEventHandlers(eventArgs);
            else if (routingStrategy == RoutingStrategy.Bubble)
            {
                var parent = eventArgs.OriginalSource as UIElement;
                while (parent != null)
                {
                    parent.InvokeEventHandlers(eventArgs);
                    parent = VisualTreeHelper.GetParent(parent) as UIElement;
                }
            }
            else if (routingStrategy == RoutingStrategy.Tunnel)
            {
                var elements = new Stack<UIElement>();
                var parent = eventArgs.OriginalSource as UIElement;
                while (parent != null)
                {
                    elements.Push(parent);
                    parent = VisualTreeHelper.GetParent(parent) as UIElement;
                }
                while (elements.Count != 0)
                    elements.Pop().InvokeEventHandlers(eventArgs);
            }
            else
                throw new InvalidOperationException("Invalid routing strategy.");
        }

        private void InvokeEventHandlers<TArgs>(TArgs eventArgs) where TArgs : RoutedEventArgs
        {
            List<Tuple<Delegate, bool>> storage;
            if (_eventHandlers.TryGetValue(eventArgs.RoutedEvent, out storage))
            {
                eventArgs.Source = this;
                foreach (var handler in storage)
                {
                    if (!eventArgs.Handled || handler.Item2)
                        ((EventHandler<TArgs>)handler.Item1)(this, eventArgs);
                }
            }
        }

        #endregion

        protected override Rect GetContentBounds()
        {
            return new Rect((Point)VisualOffset, RenderSize);
        }

        public IInputElement InputHitTest(Point point)
        {
            IInputElement result;
            IInputElement inputElement;
            InputHitTest(point, out result, out inputElement);
            return result;
        }

        private void InputHitTest(Point pt, out IInputElement enabledHit, out IInputElement rawHit)
        {
            PointHitTestResult hitTestResult;
            InputHitTest(pt, out enabledHit, out rawHit, out hitTestResult);
        }

        private void InputHitTest(Point pt, out IInputElement enabledHit, out IInputElement rawHit, out PointHitTestResult rawHitResult)
        {
            var hitTestParameters = new PointHitTestParameters(pt);
            var inputHitTestResult = new InputHitTestResult();
            VisualTreeHelper.HitTest(this, hitTestParameters, InputHitTestFilterCallback, inputHitTestResult.InputHitTestResultCallback);
            var visualHit = inputHitTestResult.Result;
            rawHit = (visualHit as IInputElement);
            rawHitResult = inputHitTestResult.HitTestResult;
            enabledHit = null;
            while (visualHit != null)
            {
                var uIElement = visualHit as UIElement;
                if (uIElement != null)
                {
                    if (rawHit == null)
                    {
                        rawHit = uIElement;
                        rawHitResult = null;
                    }
                    if (uIElement.IsEnabled)
                    {
                        enabledHit = uIElement;
                        return;
                    }
                }
                if (visualHit == this)
                    break;
                visualHit = visualHit.Parent;
            }
        }

        class InputHitTestResult
        {
            public Visual Result => HitTestResult?.VisualHit;

            public PointHitTestResult HitTestResult { get; private set; }

            public HitTestResultBehavior InputHitTestResultCallback(PointHitTestResult result)
            {
                HitTestResult = result;
                return HitTestResultBehavior.Stop;
            }
        }

        private HitTestFilterBehavior InputHitTestFilterCallback(Visual currentNode)
        {
            var result = HitTestFilterBehavior.Continue;
            if (currentNode is UIElement)
            {
                if (!((UIElement)currentNode).IsVisualVisible)
                    result = HitTestFilterBehavior.ContinueSkipSelfAndChildren;
                else if (!((UIElement)currentNode).IsHitTestVisible)
                    result = HitTestFilterBehavior.ContinueSkipSelfAndChildren;
            }
            else
                result = HitTestFilterBehavior.Continue;
            return result;
        }
    }
}