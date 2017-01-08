using MPF.Data;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF
{
    public class DependencyObject
    {
        private static readonly SortedList<float, IDependencyValueProvider> _valueProviders = new SortedList<float, IDependencyValueProvider>();
        private readonly ConcurrentDictionary<IDependencyValueProvider, object> _valueProviderStroages = new ConcurrentDictionary<IDependencyValueProvider, object>();
        private readonly ConcurrentDictionary<DependencyProperty, SortedList<float, IEffectiveValue>> _effectiveValues = new ConcurrentDictionary<DependencyProperty, SortedList<float, IEffectiveValue>>();
        private readonly Type _realType;

        static DependencyObject()
        {
            RegisterValueProvider(LocalDependencyValueProvider.Current);
        }

        public DependencyObject()
        {
            _realType = this.GetType();
        }

        public static void RegisterValueProvider(IDependencyValueProvider provider)
        {
            _valueProviders.Add(provider.Priority, provider);
        }

        public object GetValueStroage(IDependencyValueProvider provider)
        {
            return _valueProviderStroages.GetOrAdd(provider, k => k.CreateStorage());
        }

        public T GetValue<T>(DependencyProperty<T> property)
        {
            T value;
            if (!_valueProviders.Any(o => TryGetValue(property, o.Value, out value)))
            {
                if (property.TryGetDefaultValue(_realType, out value))
                    return value;
            }
            return default(T);
        }

        public void SetValue<T>(DependencyProperty<T> property, T value)
        {

        }

        public bool TryGetValue<T>(DependencyProperty<T> property, IDependencyValueProvider provider, out T value)
        {
            var storage = GetValueStroage(provider);

            IEffectiveValue<T> oldValue;
            if(provider.TryGetValue(this, property, storage, out oldValue))
            {
                value = oldValue.Value;
                return true;
            }
            value = default(T);
            return false;
        }
    }

    public class PropertyChangedEventArgs<T> : EventArgs
    {
        public DependencyProperty<T> Property { get; }
        public T OldValue { get; }
        public T NewValue { get; }

        public PropertyChangedEventArgs(DependencyProperty<T> property, T oldValue, T newValue)
        {
            Property = property;
            OldValue = oldValue;
            NewValue = newValue;
        }
    }
}
