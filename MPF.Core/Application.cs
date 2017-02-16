using MPF.Input;
using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace MPF
{
    public class Application
    {
        private readonly INativeApplication _nativeApp;
        private readonly Action _onUpdate;
        private static Application _current;
        public static Application Current => _current;
        internal event EventHandler Update;

        public Application()
        {
            if (Interlocked.CompareExchange(ref _current, this, null) != null)
                throw new InvalidOperationException("Application is already created.");

            _nativeApp = Platform.CreateNativeApplication();
            _onUpdate = OnUpdate;
            _nativeApp.SetUpdateCallback(_onUpdate);
        }

        private void OnUpdate()
        {
            Update?.Invoke(this, EventArgs.Empty);
            DeviceContext.Current?.Update();
        }

        public void Run()
        {
            InputManager.Current.Initialize();

            _nativeApp.Run();
        }
    }
}
