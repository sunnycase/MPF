using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    public interface IEffectiveValue
    {
        EventHandler ValueChanged { set; }
    }

    public interface IEffectiveValue<T> : IEffectiveValue
    {
        T Value { get; }
    }
}
