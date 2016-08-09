using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal class SwapChain
    {
        private readonly ISwapChain _swapChain;

        public UIElement RootVisual { get; set; }

        public event EventHandler Draw;

        public SwapChain(ISwapChain swapChain)
        {
            if (swapChain == null)
                throw new ArgumentNullException(nameof(swapChain));
            _swapChain = swapChain;
            swapChain.SetCallback(new Callback(this));
        }

        private void OnDraw()
        {
            Draw?.Invoke(this, EventArgs.Empty);
            RenderContent();
        }

        private void RenderContent()
        {
            var rootVisual = RootVisual;
            if (rootVisual != null)
                RenderContent(rootVisual);
        }

        private void RenderContent(UIElement element)
        {
            if(element.UIFlags.HasFlag(UIElementFlags.Visible))
            {
                element.RenderContent();

                var children = element.LogicalChildren;
                if (children != null)
                    foreach (var child in children)
                        RenderContent(child);
            }
        }

        private void OnUpdate()
        {
            var rootVisual = RootVisual;
            if (rootVisual != null)
                OnUpdate(rootVisual, null, false);
        }

        private void OnUpdate(UIElement element, UIElement parent, bool forceArrange)
        {
            var flags = element.UIFlags;
            if (forceArrange || flags.HasFlag(UIElementFlags.ArrangeDirty))
            {
                forceArrange = true;
                element.Arrange(GetArrangeRect(parent));
            }
            if (forceArrange || flags.HasFlag(UIElementFlags.MeasureDirty))
                element.Measure(parent?.RenderSize ?? new Size(float.NaN, float.NaN));
            if (forceArrange || flags.HasFlag(UIElementFlags.RenderDirty))
                element.Render();
            var children = element.LogicalChildren;
            if (children != null)
                foreach (var child in children)
                    OnUpdate(child, element, forceArrange);
        }

        private Rect GetArrangeRect(UIElement element)
        {
            if (element != null)
                return new Rect((Point)element.VisualOffset, element.RenderSize);
            return new Rect(0, 0, float.NaN, float.NaN);
        }

        private class Callback : ISwapChainCallback
        {
            private readonly WeakReference<SwapChain> _swapChain;

            public Callback(SwapChain swapChain)
            {
                _swapChain = new WeakReference<SwapChain>(swapChain);
            }

            public void OnUpdate()
            {
                GetTarget()?.OnUpdate();
            }

            public void OnDraw()
            {
                GetTarget()?.OnDraw();
            }

            private SwapChain GetTarget()
            {
                SwapChain obj;
                if (_swapChain.TryGetTarget(out obj))
                    return obj;
                return null;
            }
        }
    }
}
