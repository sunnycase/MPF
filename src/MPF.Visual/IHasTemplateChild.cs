using System;
using System.Collections.Generic;
using System.Text;

namespace MPF
{
    public interface IHasTemplateChild
    {
        object GetTemplateChild(FrameworkTemplate template);
    }
}
