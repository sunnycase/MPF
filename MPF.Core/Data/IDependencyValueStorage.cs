using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    public interface IDependencyValueStorage
    {
        IEffectiveValue AddOrUpdate(IDependencyValueProvider provider, DependencyProperty key, Func<DependencyProperty, IEffectiveValue> addValueFactory, Func<DependencyProperty, IEffectiveValue, IEffectiveValue> updateValueFactory);
        bool TryGetValue(IDependencyValueProvider provider, DependencyProperty key, out IEffectiveValue value);
        bool TryRemove(IDependencyValueProvider provider, DependencyProperty key, out IEffectiveValue value);
    }
}
