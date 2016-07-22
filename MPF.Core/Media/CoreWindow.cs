using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public class CancellableEventArgs : EventArgs
    {
        public bool Cancelled { get; set; }
    }

    public sealed class CoreWindow
    {
        private readonly INativeWindow _nativeWindow;
        private readonly SwapChain _swapChain;

        public event EventHandler<CancellableEventArgs> Closing;

        public bool HasMaximize
        {
            get { return _nativeWindow.HasMaximize; }
            set { _nativeWindow.HasMaximize = value; }
        }

        public string Title
        {
            get { return _nativeWindow.Title; }
            set { _nativeWindow.Title = value; }
        }

        private Visual _rootVisual;

        public CoreWindow()
        {
            _nativeWindow = Platform.CreateNativeWindow(OnNativeWindowMessage);
            _swapChain = DeviceContext.Current.CreateSwapChain(_nativeWindow);
            DeviceContext.Current.Render += OnDeviceContextRender;
        }

        private void OnDeviceContextRender(object sender, EventArgs e)
        {
            OnRender();
        }

        private void OnNativeWindowMessage(NativeWindowMessages message)
        {
            switch (message)
            {
                case NativeWindowMessages.Closing:
                    OnClosing();
                    break;
                default:
                    break;
            }
        }

        private void OnRender()
        {
            _rootVisual?.Render();
        }

        private void OnClosing()
        {
            var e = new CancellableEventArgs();
            Closing?.Invoke(this, e);
            if (!e.Cancelled)
                _nativeWindow.Destroy();
        }

        public void Show()
        {
            _nativeWindow.Show();
        }

        public void Hide()
        {
            _nativeWindow.Hide();
        }

        public void SetRootVisual(Visual visual)
        {
            if (_rootVisual != null)
                throw new InvalidOperationException("Root Visual is already set.");
            _rootVisual = visual;
        }
    }
}
