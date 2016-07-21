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

        internal SwapChain(ISwapChain swapChain)
        {
            if (swapChain == null)
                throw new ArgumentNullException(nameof(swapChain));
            _swapChain = swapChain;
        }
    }
}
