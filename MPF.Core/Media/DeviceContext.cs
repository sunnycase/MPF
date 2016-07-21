using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal class DeviceContext
    {
        private static readonly Lazy<DeviceContext> _current = new Lazy<DeviceContext>(() => new DeviceContext(), true);

        public static DeviceContext Current => _current.Value;

        private readonly IDeviceContext _deviceContext;
        private DeviceContext()
        {
            _deviceContext = Platform.CreateDeviceContext();
        }

        public SwapChain CreateSwapChain(INativeWindow window)
        {
            return new SwapChain(_deviceContext.CreateSwapChain(window));
        }
    }
}
