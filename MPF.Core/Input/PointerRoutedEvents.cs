using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Input
{
    public class PointerRoutedEventArgs : RoutedEventArgs
    {
        public PointerRoutedEventArgs(RoutedEvent routedEvent, object originalSource) : base(routedEvent, originalSource)
        {
        }
    }
}
