using MPF.Controls;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF
{
    public class DataTemplate : FrameworkTemplate
    {
        private readonly Func<ContentPresenter, FrameworkElement> _factory;

        public DataTemplate(Func<ContentPresenter, FrameworkElement> factory)
        {
            _factory = factory;
        }

        public override object LoadContent(object templatedParent)
        {
            return _factory((ContentPresenter)templatedParent);
        }
    }
}
