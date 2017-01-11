using System;
using System.Collections.Generic;
using System.Text;

namespace MPF
{
    public interface IHasTemplatedChild
    {
        object GetTemplatedChild(FrameworkTemplate template);
    }
}
