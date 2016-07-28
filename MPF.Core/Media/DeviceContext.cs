using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace MPF.Media
{
    internal class DeviceContext
    {
        private static readonly Lazy<DeviceContext> _current = new Lazy<DeviceContext>(() => new DeviceContext(), true);

        public static DeviceContext Current => _current.Value;

        public event EventHandler Render;
        public event EventHandler UpdateResource;

        private readonly IDeviceContext _deviceContext;
        private DeviceContext()
        {
            _deviceContext = Platform.CreateDeviceContext(OnDeviceContextMessage);
        }

        private void OnDeviceContextMessage(DeviceContextMessage message)
        {
            switch (message)
            {
                case DeviceContextMessage.DCM_Render:
                    OnRender();
                    break;
                default:
                    break;
            }
        }

        private void OnRender()
        {
            var oldUpdateResource = Interlocked.Exchange(ref UpdateResource, null);
            oldUpdateResource?.Invoke(this, EventArgs.Empty);
            Render?.Invoke(this, EventArgs.Empty);
        }

        public SwapChain CreateSwapChain(INativeWindow window)
        {
            return new SwapChain(_deviceContext.CreateSwapChain(window));
        }

        public IRenderableObject CreateRenderableObject()
        {
            return _deviceContext.CreateRenderableObject();
        }

        public IResourceManager CreateResourceManager()
        {
            return _deviceContext.CreateResourceManager();
        }
    }
}
