using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ItemContainerGenerator
    {
        public virtual UIElement GenerateNext()
        {
            return new UIElement();
        }

        public virtual void LinkContainerToItem(UIElement container, object item)
        {
            container.SetLocalValue(FrameworkElement.DataContextProperty, item);
        }

        public virtual void UnlinkContainerFromItem(UIElement container, object item)
        {
            container.ClearLocalValue(FrameworkElement.DataContextProperty);
        }
    }
}
