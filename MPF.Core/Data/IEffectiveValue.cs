using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    public interface IEffectiveValue
    {
        EventHandler<EffectiveValueChangedEventArgs> ValueChanged { set; }
    }

    public interface IEffectiveValue<T> : IEffectiveValue
    {
        bool CanSetValue { get; }
        T Value { get; set; }
    }

    public class EffectiveValueChangedEventArgs : EventArgs
    {
        public object OldValue { get; }
        public object NewValue { get; }

        public EffectiveValueChangedEventArgs(object oldValue, object newValue)
        {
            OldValue = oldValue;
            NewValue = newValue;
        }
    }
}
