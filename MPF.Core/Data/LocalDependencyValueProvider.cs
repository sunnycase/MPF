using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    public class LocalDependencyValueProvider : IDependencyValueProvider
    {
        public static LocalDependencyValueProvider Current { get; } = new LocalDependencyValueProvider();
        public float Priority => 1.0f;

        public void SetValue<T>(DependencyProperty<T> property, IDependencyValueStorage storage, T value)
        {
            storage.AddOrUpdate(this, property, o => new LocalEffectiveValue<T>(value), (k, o) => { ((LocalEffectiveValue<T>)o).Value = value; return o; });
        }

        public bool TryGetValue<T>(DependencyProperty<T> property, IDependencyValueStorage storage, out T value)
        {
            IEffectiveValue<T> eValue;
            if (storage.TryGetValue(this, property, out eValue))
            {
                value = eValue.Value;
                return true;
            }
            value = default(T);
            return false;
        }

        public void ClearValue<T>(DependencyProperty<T> property, IDependencyValueStorage storage)
        {
            IEffectiveValue<T> eValue;
            storage.TryRemove(this, property, out eValue);
        }

        class LocalEffectiveValue<T> : IEffectiveValue<T>
        {
            public EventHandler<EffectiveValueChangedEventArgs> ValueChanged { get; set; }

            public bool CanSetValue => true;

            private T _value;
            public T Value
            {
                get { return _value; }
                set
                {
                    if (!EqualityComparer<T>.Default.Equals(_value, value))
                    {
                        var oldValue = _value;
                        _value = value;
                        ValueChanged?.Invoke(this, new EffectiveValueChangedEventArgs(oldValue, value));
                    }
                }
            }


            public LocalEffectiveValue(T value)
            {
                Value = value;
            }
        }
    }

    public static class LocalDependencyValueExtensions
    {
        public static bool TryGetLocalValue<T>(this DependencyObject d, DependencyProperty<T> property, out T value)
        {
            return LocalDependencyValueProvider.Current.TryGetValue(property, d.ValueStorage, out value);
        }

        public static void SetLocalValue<T>(this DependencyObject d, DependencyProperty<T> property, T value)
        {
            LocalDependencyValueProvider.Current.SetValue(property, d.ValueStorage, value);
        }

        public static void ClearLocalValue<T>(this DependencyObject d, DependencyProperty<T> property)
        {
            LocalDependencyValueProvider.Current.ClearValue(property, d.ValueStorage);
        }
    }
}
