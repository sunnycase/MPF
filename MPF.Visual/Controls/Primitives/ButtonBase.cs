using MPF.Input;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;

namespace MPF.Controls.Primitives
{
    public class ButtonBase : ContentControl
    {
        public static readonly RoutedEvent<RoutedEventArgs> ClickEvent = RoutedEvent.Register<RoutedEventArgs>(nameof(Click),
            typeof(ButtonBase), RoutingStrategy.Bubble);

        public event EventHandler<RoutedEventArgs> Click
        {
            add { AddHandler(ClickEvent, value); }
            remove { RemoveHandler(ClickEvent, value); }
        }

        static ButtonBase()
        {
            PointerPressedEvent.RegisterClassEventHandler(typeof(ButtonBase), OnPointerPressedEvent);
            PointerReleasedEvent.RegisterClassEventHandler(typeof(ButtonBase), OnPointerReleasedEvent);
        }

        private static void OnPointerReleasedEvent(object sender, PointerButtonRoutedEventArgs e)
        {
            ((ButtonBase)sender).OnPointerReleased(e);
        }

        private static void OnPointerPressedEvent(object sender, PointerButtonRoutedEventArgs e)
        {
            ((ButtonBase)sender).OnPointerPressed(e);
        }

        private bool _button1Pressed = false;

        private void OnPointerReleased(PointerButtonRoutedEventArgs e)
        {
            if(e.ChangedButton == 1 && _button1Pressed)
            {
                _button1Pressed = false;
                RaiseEvent(new RoutedEventArgs(ClickEvent, e.OriginalSource));
            }
        }

        private void OnPointerPressed(PointerButtonRoutedEventArgs e)
        {
            if (e.ChangedButton == 1)
                _button1Pressed = true;
        }
    }
}
