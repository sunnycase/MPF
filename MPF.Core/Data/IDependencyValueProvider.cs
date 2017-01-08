using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    public interface IDependencyValueProvider
    {
        float Priority { get; }
        bool TryGetValue<T>(DependencyObject d, DependencyProperty property, object storage, out IEffectiveValue<T> value);
        object CreateStorage();
    }
}
