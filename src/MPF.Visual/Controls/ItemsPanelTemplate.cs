using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public class ItemsPanelTemplate : FrameworkTemplate
    {
        private readonly Func<ItemsPresenter, Panel> _factory;

        public ItemsPanelTemplate(Func<ItemsPresenter, Panel> factory)
        {
            _factory = factory;
        }

        public override object LoadContent(object templatedParent)
        {
            return _factory((ItemsPresenter)templatedParent);
        }
    }
}
