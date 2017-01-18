using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using MPF.Media;

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

        private void OnMouseInput(INativeWindow window, IInputDevice device, ref HIDMouseInput input)
        {
            var mouseDevice = (MouseDevice)InputDevice.GetOrAddDevice(device);
            var coreWindow = window == null ? null : CoreWindow.FindWindow(window);
            var rootVisual = coreWindow?.RootVisual;
            var hitUIElement = rootVisual?.InputHitTest(new Point(input.CursorX, input.CursorY));
            if (hitUIElement != null)
            {
                if (input.ChangedButton == 0)
                    UIElement.RaiseEvent(new PointerRoutedEventArgs(UIElement.PointerMoveEvent, hitUIElement, mouseDevice));
                else if (input.ChangedButton == 6) ;
                else
                {
                    if (input.ChangedButtonState == PointerButtonState.Pressed)
                        UIElement.RaiseEvent(new PointerButtonRoutedEventArgs(UIElement.PointerPressedEvent, hitUIElement, mouseDevice,
                            input.ChangedButton, input.ChangedButtonState));
                    else if (input.ChangedButtonState == PointerButtonState.Released)
                        UIElement.RaiseEvent(new PointerButtonRoutedEventArgs(UIElement.PointerReleasedEvent, hitUIElement, mouseDevice,
                            input.ChangedButton, input.ChangedButtonState));
                }
            }
        }

        private class Callback : IInputManagerCallback
        {
            private readonly WeakReference<InputManager> _inputManager;
            public Callback(InputManager inputManager)
            {
                _inputManager = new WeakReference<InputManager>(inputManager);
            }

            public void OnMouseInput([In]INativeWindow window, [In] IInputDevice device, [In]ref HIDMouseInput input)
            {
                GetTarget()?.OnMouseInput(window, device, ref input);
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
