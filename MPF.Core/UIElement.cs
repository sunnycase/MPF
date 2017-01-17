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
        ArrangeDirty = 0x4,
        Visible = 0x8
    }

    public class UIElement : Visual
    {
        private UIElementFlags _uiFlags = UIElementFlags.ArrangeDirty | UIElementFlags.Visible;
        internal UIElementFlags UIFlags => _uiFlags;
        private Size _renderSize;

        protected internal virtual IEnumerable<UIElement> LogicalChildren
        {
            get { yield break; }
        }

        public Size RenderSize
        {
            get { return _renderSize; }
            set { _renderSize = value; }
        }

        public static readonly DependencyProperty<Visibility> VisibilityProperty = DependencyProperty.Register(nameof(Visibility), typeof(UIElement),
            new PropertyMetadata<Visibility>(Visibility.Visible, OnVisibilityPropertyChanged));

        public Visibility Visibility
        {
            get { return GetValue(VisibilityProperty); }
            set { this.SetLocalValue(VisibilityProperty, value); }
        }

        public static readonly RoutedEvent<PointerRoutedEventArgs> PointerMoveEvent = RoutedEvent.Register<PointerRoutedEventArgs>(nameof(PointerMove),
            typeof(UIElement), RoutingStrategy.Bubble);
        public static readonly RoutedEvent<PointerRoutedEventArgs> PointerPressedEvent = RoutedEvent.Register<PointerRoutedEventArgs>(nameof(PointerPressed),
            typeof(UIElement), RoutingStrategy.Bubble);

        public event EventHandler<PointerRoutedEventArgs> PointerPressed
        {
            add { AddHandler(PointerPressedEvent, value); }
            remove { RemoveHandler(PointerPressedEvent, value); }
        }

        public event EventHandler<PointerRoutedEventArgs> PointerMove
        {
            add { AddHandler(PointerMoveEvent, value); }
            remove { RemoveHandler(PointerMoveEvent, value); }
        }

        private Size _desiredSize;
        public Size DesiredSize => Visibility == Visibility.Collapsed ? default(Size) : _desiredSize;

        public UIElement()
        {
            UpdateVisibilityFlag(Visibility);
        }

        internal new void Render()
        {
            ClearFlags(UIElementFlags.RenderDirty);
            base.Render();
        }

        public void InvalidateArrange()
        {
            SetUIFlags(UIElementFlags.ArrangeDirty);
        }

        public void Arrange(Rect finalRect)
        {
            ClearFlags(UIElementFlags.ArrangeDirty);
            ArrangeOverride(finalRect);
            InvalidateRender();
        }

        protected virtual void ArrangeOverride(Rect finalRect)
        {
            RenderSize = finalRect.Size;
            VisualOffset = (Vector2)finalRect.Location;
        }

        public void InvalidateMeasure()
        {
            SetUIFlags(UIElementFlags.MeasureDirty);
        }

        public void Measure(Size availableSize)
        {
            ClearFlags(UIElementFlags.MeasureDirty);
            _desiredSize = MeasureOverride(availableSize);
            InvalidateArrange();
        }

        protected virtual Size MeasureOverride(Size availableSize)
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
        }

        private void UpdateVisibilityFlag(Visibility value)
        {
            if (value == Visibility.Visible)
                SetUIFlags(UIElementFlags.Visible);
            else
                ClearFlags(UIElementFlags.Visible);
        }

        private static void OnVisibilityPropertyChanged(object sender, PropertyChangedEventArgs<Visibility> e)
        {
            ((UIElement)sender).UpdateVisibilityFlag(e.NewValue);
        }


        private readonly ConcurrentDictionary<RoutedEvent, List<Tuple<Delegate, bool>>> _eventHandlers = new ConcurrentDictionary<RoutedEvent, List<Tuple<Delegate, bool>>>();

        public void AddHandler<TArgs>(RoutedEvent<TArgs> routedEvent, EventHandler<TArgs> handler, bool handledEventsToo = false) where TArgs : RoutedEventArgs
        {
            var storage = _eventHandlers.GetOrAdd(routedEvent, k => new List<Tuple<Delegate, bool>>());
            storage.Add(Tuple.Create((Delegate)handler, handledEventsToo));
        }

        public void RemoveHandler<TArgs>(RoutedEvent<TArgs> routedEvent, EventHandler<TArgs> handler) where TArgs : RoutedEventArgs
        {
            List<Tuple<Delegate, bool>> storage;
            if(_eventHandlers.TryGetValue(routedEvent, out storage))
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
    }
}