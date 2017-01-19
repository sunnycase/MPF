using MPF.Data;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Reflection;
using System.Text;

namespace MPF
{
    public class Style
    {
        public Type TargetType { get; }

        private bool _isSealed = false;
        private readonly Dictionary<DependencyProperty, IEffectiveValueProvider> _values = new Dictionary<DependencyProperty, IEffectiveValueProvider>();

        public Style(Type targetType)
        {
            TargetType = targetType;
        }

        public void SetValue<T>(DependencyProperty<T> property, IEffectiveValueProvider<T> value)
        {
            CheckSealed();
            _values.Add(property, value);
        }

        public void Apply(DependencyObject d)
        {
            if (!TargetType.GetTypeInfo().IsAssignableFrom(d.GetType().GetTypeInfo()))
                throw new InvalidOperationException($"This style cannot be applied to {d.GetType()}, Target type is {TargetType}.");

            Seal();
            d.ValueStorage[StyleDependencyValueStorage.Key] = new StyleDependencyValueStorage(this, d);
        }

        public static void Clear(DependencyObject d)
        {
            d.ValueStorage[StyleDependencyValueStorage.Key] = null;
        }

        private void Seal()
        {
            _isSealed = true;
        }

        private void CheckSealed()
        {
            if (_isSealed)
                throw new InvalidOperationException("Style is already sealed.");
        }

        class StyleDependencyValueStorage : IDependencyValueStorage
        {
            public static readonly DependencyValueStorageKey Key = new DependencyValueStorageKey(2.0f, "Style");

            public IEnumerable<DependencyProperty> Keys
            {
                get
                {
                    _style.Seal();
                    return _style._values.Keys;
                }
            }

            public event EventHandler<CurrentValueChangedEventArgs> CurrentValueChanged;

            private readonly Style _style;
            private readonly DependencyObject _d;
            private readonly ConcurrentDictionary<DependencyProperty, IEffectiveValue> _values = new ConcurrentDictionary<DependencyProperty, IEffectiveValue>();

            public StyleDependencyValueStorage(Style style, DependencyObject d)
            {
                _style = style;
                _d = d;
            }

            public bool TryGetCurrentEffectiveValue(DependencyProperty key, out IEffectiveValue value)
            {
                if (_values.TryGetValue(key, out value))
                    return true;

                _style.Seal();
                IEffectiveValueProvider provider;
                if (_style._values.TryGetValue(key, out provider))
                {
                    value = _values.GetOrAdd(key, k =>
                    {
                        var effectiveValue = provider.ProviderValue(_d);
                        effectiveValue.ValueChanged = (s, e) => OnEffectiveValueChanged(key, e.OldValue, e.NewValue);
                        return effectiveValue;
                    });
                    return true;
                }
                value = null;
                return false;
            }

            public bool TryGetCurrentEffectiveValue<T>(DependencyProperty<T> key, out IEffectiveValue<T> value)
            {
                IEffectiveValue eValue;
                if (TryGetCurrentEffectiveValue(key, out eValue))
                {
                    value = (IEffectiveValue<T>)eValue;
                    return true;
                }
                value = null;
                return false;
            }

            public bool TryGetCurrentValue<T>(DependencyProperty<T> key, out T value)
            {
                IEffectiveValue<T> eValue;
                if (TryGetCurrentEffectiveValue(key, out eValue))
                {
                    value = eValue.Value;
                    return true;
                }
                value = default(T);
                return false;
            }

            private void OnEffectiveValueChanged(DependencyProperty key, object oldValue, object newValue)
            {
                OnCurrentValueChanged(key, true, oldValue, true, newValue);
            }

            private void OnCurrentValueChanged(DependencyProperty key, bool hasOldValue, object oldValue, bool hasNewValue, object newValue)
            {
                CurrentValueChanged?.Invoke(this, new CurrentValueChangedEventArgs(key, hasOldValue, oldValue, hasNewValue, newValue));
            }

            public IEffectiveValue AddOrUpdate<T>(IDependencyValueProvider provider, DependencyProperty<T> key, Func<DependencyProperty, IEffectiveValue<T>> addValueFactory, Func<DependencyProperty, IEffectiveValue<T>, IEffectiveValue<T>> updateValueFactory)
            {
                throw new NotSupportedException();
            }

            public bool TryGetValue<T>(IDependencyValueProvider provider, DependencyProperty<T> key, out IEffectiveValue<T> value)
            {
                throw new NotSupportedException();
            }

            public bool TryRemove<T>(IDependencyValueProvider provider, DependencyProperty<T> key, out IEffectiveValue<T> value)
            {
                throw new NotSupportedException();
            }
        }
    }
}
