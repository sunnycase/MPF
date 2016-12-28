using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    abstract class EffectiveValue
    {
        public event EventHandler ValueChanged;

        public abstract object GetValue(IServiceProvider serviceProvider);
    }
}
