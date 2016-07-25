using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public class SwapChain
    {
        private readonly ISwapChain _swapChain;

        public event EventHandler Draw;

        internal SwapChain(ISwapChain swapChain)
        {
            if (swapChain == null)
                throw new ArgumentNullException(nameof(swapChain));
            _swapChain = swapChain;
            swapChain.SetCallback(new Callback(this));
        }

        private void OnDraw()
        {
            Draw?.Invoke(this, EventArgs.Empty);
        }

        private class Callback : ISwapChainCallback
        {
            private readonly WeakReference<SwapChain> _swapChain;

            public Callback(SwapChain swapChain)
            {
                _swapChain = new WeakReference<SwapChain>(swapChain);
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
