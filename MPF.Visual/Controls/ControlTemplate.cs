using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public abstract class ControlTemplate : FrameworkTemplate
    {
        internal ControlTemplate()
        {

        }

        public static ControlTemplate Create<T>(Func<T, FrameworkElement> factory) where T : Control
        {
            return new ControlTemplate<T>(factory);
        }
    }

    internal class ControlTemplate<T> : ControlTemplate where T : Control
    {
        private readonly Func<T, FrameworkElement> _factory;

        public ControlTemplate(Func<T, FrameworkElement> factory)
        {
            _factory = factory;
        }

        public override object LoadContent(object templatedParent)
        {
            return _factory((T)templatedParent);
        }
    }
}
