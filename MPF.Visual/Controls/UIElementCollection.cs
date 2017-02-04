using MPF.Media;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class UIElementCollection : VisualCollectionBase<UIElement>
    {
        protected new Panel VisualParent => (Panel)base.VisualParent;
        public override bool IsReadOnly => VisualParent.IsItemsHost;

        public UIElementCollection(Panel panel, object cookie)
            :base(panel, cookie)
        {

        }

        protected override void InvalidateParent()
        {
            VisualParent.InvalidateMeasure();
        }
    }
}
