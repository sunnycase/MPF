using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Input
{
    public class PointerRoutedEventArgs : RoutedEventArgs
    {
        public InputDevice InputDevice { get; }

        public PointerRoutedEventArgs(RoutedEvent routedEvent, object originalSource, InputDevice inputDevice) : base(routedEvent, originalSource)
        {
            InputDevice = inputDevice;
        }
    }

    public class PointerButtonRoutedEventArgs : PointerRoutedEventArgs
    {
        public ushort ChangedButton { get; }
        public PointerButtonState ButtonState { get; }

        public PointerButtonRoutedEventArgs(RoutedEvent routedEvent, object originalSource, InputDevice inputDevice, ushort changedButton, PointerButtonState buttonState) 
            : base(routedEvent, originalSource, inputDevice)
        {
            ChangedButton = changedButton;
            ButtonState = buttonState;
        }
    }
}
