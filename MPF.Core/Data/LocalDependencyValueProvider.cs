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

        public object CreateStorage()
        {
            return new ConcurrentDictionary<DependencyProperty, IEffectiveValue>();
        }

        public void SetValue<T>(DependencyObject d, DependencyProperty property, object storage, T value)
        {
            var myStorage = (ConcurrentDictionary<DependencyProperty, IEffectiveValue>)storage;
            myStorage.AddOrUpdate(property, o => new LocalEffectiveValue<T>(value), (k, o) => { ((LocalEffectiveValue<T>)o).Value = value; return o; });
        }

        public bool TryGetValue<T>(DependencyObject d, DependencyProperty property, object storage, out IEffectiveValue<T> value)
        {
            var myStorage = (ConcurrentDictionary<DependencyProperty, IEffectiveValue>)storage;
            IEffectiveValue oldValue;
            if (myStorage.TryGetValue(property, out oldValue))
            {
                value = (IEffectiveValue<T>)oldValue;
                return true;
            }
            value = null;
            return false;
        }

        class LocalEffectiveValue<T> : IEffectiveValue<T>
        {
            public EventHandler ValueChanged { get; set; }

            private T _value;
            public T Value
            {
                get { return _value; }
                set
                {
                    if (!EqualityComparer<T>.Default.Equals(_value, value))
                    {
                        _value = value;
                        ValueChanged?.Invoke(this, EventArgs.Empty);
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
            return d.TryGetValue(property, LocalDependencyValueProvider.Current, out value);
        }

        public static void SetLocalValue<T>(this DependencyObject d, DependencyProperty<T> property, T value)
        {
            var provider = LocalDependencyValueProvider.Current;
            provider.SetValue(d, property, d.GetValueStroage(provider), value);
        }
    }
}
