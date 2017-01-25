using MPF.Data;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF
{
    interface IDependencyPropertyHelper
    {
        object GetEffectiveValue(IEffectiveValue value);
        bool EqualsEffectiveValue(IEffectiveValue a, IEffectiveValue b);
        CurrentValueChangedEventArgs CreateCurrentValueChangedEventArgs(PropertyChangedEventArgs e);
    }


    class DependencyPropertyHelper<T> : IDependencyPropertyHelper
    {

        public object GetEffectiveValue(IEffectiveValue value)
        {
            return ((IEffectiveValue<T>)value).Value;
        }

        public bool EqualsEffectiveValue(IEffectiveValue a, IEffectiveValue b)
        {
            return EqualityComparer<T>.Default.Equals(((IEffectiveValue<T>)a).Value, ((IEffectiveValue<T>)b).Value);
        }

        public CurrentValueChangedEventArgs CreateCurrentValueChangedEventArgs(PropertyChangedEventArgs e)
        {
            var te = (PropertyChangedEventArgs<T>)e;
            return new CurrentValueChangedEventArgs(e.Property, true, te.OldValue, true, te.NewValue);
        }
    }
}
