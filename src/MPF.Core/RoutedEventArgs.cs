using System;
using System.Collections.Generic;
using System.Text;

namespace MPF
{
    public class RoutedEventArgs : EventArgs
    {
        public RoutedEvent RoutedEvent { get; }
        public object OriginalSource { get; }
        public object Source { get; set; }
        public bool Handled { get; set; }

        public RoutedEventArgs(RoutedEvent routedEvent, object originalSource)
        {
            RoutedEvent = routedEvent;
            OriginalSource = originalSource;
        }
    }
}
