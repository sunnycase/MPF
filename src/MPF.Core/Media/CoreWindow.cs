using MPF.Interop;
using System;
using System.Collections.Concurrent;
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
        public event EventHandler LocationChanged;
        public event EventHandler SizeChanged;

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

        public Point Location
        {
            get { return _nativeWindow.Location; }
            set { _nativeWindow.Location = value; }
        }

        public Size Size
        {
            get { return _nativeWindow.Size; }
            set
            {
                if (_nativeWindow.Size != value)
                    _nativeWindow.Size = value;
            }
        }

        public Size ClientSize
        {
            get { return _nativeWindow.ClientSize; }
        }

        private UIElement _rootVisual;
        public UIElement RootVisual => _rootVisual;

        public CoreWindow()
        {
            _nativeWindow = Platform.CreateNativeWindow(new Callback(this));
            _coreWindows[_nativeWindow] = this;
            _swapChain = DeviceContext.Current.CreateSwapChain(_nativeWindow);
        }

        private void OnClosing()
        {
            var e = new CancellableEventArgs();
            Closing?.Invoke(this, e);
            if (!e.Cancelled)
            {
                _nativeWindow.Destroy();
                CoreWindow cw;
                _coreWindows.TryRemove(_nativeWindow, out cw);
            }
        }

        private void OnSizeChanged(Size size)
        {
            SizeChanged?.Invoke(this, EventArgs.Empty);
        }

        private void OnLocationChanged(Point location)
        {
            LocationChanged?.Invoke(this, EventArgs.Empty);
        }

        public void Show()
        {
            _nativeWindow.Show();
        }

        public void Hide()
        {
            _nativeWindow.Hide();
        }

        public void SetRootVisual(UIElement visual)
        {
            if (_rootVisual != null)
                throw new InvalidOperationException("Root Visual is already set.");
            _rootVisual = visual;
            _swapChain.RootVisual = _rootVisual;
        }

        private class Callback : INativeWindowCallback
        {
            private readonly WeakReference<CoreWindow> _window;

            public Callback(CoreWindow window)
            {
                _window = new WeakReference<CoreWindow>(window);
            }

            private CoreWindow GetTarget()
            {
                CoreWindow obj;
                if (_window.TryGetTarget(out obj))
                    return obj;
                return null;
            }

            public void OnClosing()
            {
                GetTarget()?.OnClosing();
            }

            public void OnLocationChanged(Point location)
            {
                GetTarget()?.OnLocationChanged(location);
            }

            public void OnSizeChanged(Size size)
            {
                GetTarget()?.OnSizeChanged(size);
            }
        }

        private static ConcurrentDictionary<INativeWindow, CoreWindow> _coreWindows = new ConcurrentDictionary<INativeWindow, CoreWindow>();
        internal static CoreWindow FindWindow(INativeWindow window)
        {
            CoreWindow cw;
            if (_coreWindows.TryGetValue(window, out cw))
                return cw;
            return null;
        }
    }
}
