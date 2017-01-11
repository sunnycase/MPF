using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace MPF.Input
{
    public sealed class InputManager
    {
        private readonly IInputManager _inputManager;
        private static readonly Lazy<InputManager> _current = new Lazy<InputManager>(() =>
        {
            var inputManager = new InputManager();
            inputManager.HIDAware = HIDUsages.Default;
            return inputManager;
        });
        public static InputManager Current => _current.Value;

        private HIDUsage[] _hidAware = HIDUsages.None;
        public HIDUsage[] HIDAware
        {
            get { return _hidAware; }
            set
            {
                if (_hidAware != value)
                {
                    _inputManager.SetHIDAware(value, value.Length);
                    _hidAware = value;
                }
            }
        }

        internal InputManager()
        {
            _inputManager = Platform.GetCurrentInputManager();
            _inputManager.SetCallback(new Callback(this));
        }

        internal void Initialize()
        {

        }
        
        private void OnMouseInput(IInputDevice device, ref HIDMouseInput input)
        {

        }

        private class Callback : IInputManagerCallback
        {
            private readonly WeakReference<InputManager> _inputManager;
            public Callback(InputManager inputManager)
            {
                _inputManager = new WeakReference<InputManager>(inputManager);
            }

            public void OnMouseInput([In] IInputDevice device, [In]ref HIDMouseInput input)
            {
                GetTarget()?.OnMouseInput(device, ref input);
            }

            private InputManager GetTarget()
            {
                InputManager obj;
                if (_inputManager.TryGetTarget(out obj))
                    return obj;
                return null;
            }
        }
    }
}
