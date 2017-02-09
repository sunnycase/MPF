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
            Application.Current.Update += OnUpdate;
        }

        private void OnDraw(ref SwapChainDrawingContext context)
        {
            Draw?.Invoke(this, EventArgs.Empty);
            RenderContent(ref context);
        }

        private void RenderContent(ref SwapChainDrawingContext context)
        {
            var rootVisual = RootVisual;
            if (rootVisual != null)
                RenderContent(rootVisual, ref context);
        }

        private void RenderContent(Visual visual, ref SwapChainDrawingContext context)
        {
            if (visual.IsVisualVisible)
            {
                visual.RenderContent(ref context);

                foreach (var child in visual.VisualChildren)
                    RenderContent(child, ref context);
            }
        }

        private void OnUpdate(object sender, EventArgs e)
        {
            LayoutManager.Current.Update();

            var rootVisual = RootVisual;
            if (rootVisual != null)
                OnUpdate(rootVisual, null, false);
        }

        private Rect GetArrangeRect(UIElement element)
        {
            var rect = element.LastFinalRect;
            if (rect == null)
                return new Rect(Point.Zero, element.DesiredSize);
            return rect.Value;
        }

        private void OnUpdate(Visual visual, Visual parent, bool forceArrange)
        {
            var element = visual as UIElement;
            if (element != null)
            {
                var flags = element.UIFlags;
                if (forceArrange || flags.HasFlag(UIElementFlags.MeasureDirty))
                    element.Measure(element.LastAvailableSize);
                if (forceArrange || flags.HasFlag(UIElementFlags.ArrangeDirty))
                {
                    forceArrange = true;
                    element.Arrange(GetArrangeRect(element));
                }
            }
            if (forceArrange || visual.VisualFlags.HasFlag(VisualFlags.RenderDirty))
                visual.Render();

            foreach (var child in visual.VisualChildren)
                OnUpdate(child, visual, forceArrange);
        }

        private class Callback : ISwapChainCallback
        {
            private readonly WeakReference<SwapChain> _swapChain;

            public Callback(SwapChain swapChain)
            {
                _swapChain = new WeakReference<SwapChain>(swapChain);
            }

            public void OnDraw(ISwapChainDrawingContext context)
            {
                var ctx = new SwapChainDrawingContext(context);
                GetTarget()?.OnDraw(ref ctx);
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
