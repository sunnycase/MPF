using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    internal class DependencyValueStorage : IDependencyValueStorage
    {
        private readonly ConcurrentDictionary<DependencyProperty, SortedList<float, IEffectiveValue>> _dict = new ConcurrentDictionary<DependencyProperty, SortedList<float, IEffectiveValue>>();

        public IEnumerable<DependencyProperty> Keys
        {
            get
            {
                foreach (var dp in _dict)
                {
                    var lst = dp.Value;
                    if (lst.Count != 0)
                        yield return dp.Key;
                }
            }
        }

        public event EventHandler<CurrentValueChangedEventArgs> CurrentValueChanged;

        public DependencyValueStorage()
        {

        }

        public IEffectiveValue AddOrUpdate<T>(IDependencyValueProvider provider, DependencyProperty<T> key, Func<DependencyProperty, IEffectiveValue<T>> addValueFactory, Func<DependencyProperty, IEffectiveValue<T>, IEffectiveValue<T>> updateValueFactory)
        {
            var storage = GetStorage(provider, key);
            var priority = provider.Priority;
            IEffectiveValue result;
            var oldIdx = storage.IndexOfKey(priority);
            if (oldIdx == -1)
            {
                var value = addValueFactory(key);
                storage.Add(priority, value);
                value.ValueChanged = (s, e) => OnEffectiveValueChanged(priority, key, e.OldValue, e.NewValue);
                result = value;
                var raiseChanged = storage.IndexOfKey(priority) == 0;
                if (raiseChanged)
                    OnCurrentValueChanged(key, false, null, true, value.Value);
            }
            else
            {
                var oldValue = (IEffectiveValue<T>)storage.Values[oldIdx];
                var newValue = updateValueFactory(key, oldValue);
                if (oldValue != newValue)
                {
                    oldValue.ValueChanged = null;
                    newValue.ValueChanged = (s, e) => OnEffectiveValueChanged(priority, key, e.OldValue, e.NewValue);
                    storage[priority] = newValue;
                    var raiseChanged = oldIdx == 0;
                    if (raiseChanged)
                        OnCurrentValueChanged(key, true, oldValue.Value, true, newValue.Value);
                }
                result = newValue;
            }

            return result;
        }

        public bool TryGetCurrentValue<T>(DependencyProperty<T> key, out T value)
        {
            SortedList<float, IEffectiveValue> list;
            if (_dict.TryGetValue(key, out list) && list.Count > 0)
            {
                value = ((IEffectiveValue<T>)list.Values[0]).Value;
                return true;
            }
            value = default(T);
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

        public bool TryGetCurrentEffectiveValue(DependencyProperty key, out IEffectiveValue value)
        {
            SortedList<float, IEffectiveValue> list;
            if (_dict.TryGetValue(key, out list) && list.Count > 0)
            {
                value = list.Values[0];
                return true;
            }
            value = null;
            return false;
        }

        private void OnCurrentValueChanged(DependencyProperty key, bool hasOldValue, object oldValue, bool hasNewValue, object newValue)
        {
            CurrentValueChanged?.Invoke(this, new CurrentValueChangedEventArgs(key, hasOldValue, oldValue, hasNewValue, newValue));
        }

        private void OnEffectiveValueCleared(int index, DependencyProperty key, object oldValue)
        {
            if (index == 0)
            {
                bool hasNewValue = false;
                object newValue = null;
                SortedList<float, IEffectiveValue> list;
                if (_dict.TryGetValue(key, out list) && list.Count > 0)
                {
                    hasNewValue = true;
                    newValue = ((dynamic)list.Values[0]).Value;
                }
                OnCurrentValueChanged(key, true, oldValue, hasNewValue, newValue);
            }
        }

        private void OnEffectiveValueChanged(float priority, DependencyProperty key, object oldValue, object newValue)
        {
            SortedList<float, IEffectiveValue> list;
            if (_dict.TryGetValue(key, out list) && list.IndexOfKey(priority) == 0)
                OnCurrentValueChanged(key, true, oldValue, true, newValue);
        }

        public bool TryGetValue<T>(IDependencyValueProvider provider, DependencyProperty<T> key, out IEffectiveValue<T> value)
        {
            var storage = GetStorage(provider, key);
            IEffectiveValue eValue;
            if (storage.TryGetValue(provider.Priority, out eValue))
            {
                value = (IEffectiveValue<T>)eValue;
                return true;
            }
            value = null;
            return false;
        }

        public bool TryRemove<T>(IDependencyValueProvider provider, DependencyProperty<T> key, out IEffectiveValue<T> value)
        {
            var storage = GetStorage(provider, key);
            var priority = provider.Priority;
            IEffectiveValue eValue;
            if (storage.TryGetValue(priority, out eValue))
            {
                value = (IEffectiveValue<T>)eValue;
                var index = storage.IndexOfKey(priority);
                storage.RemoveAt(index);
                OnEffectiveValueCleared(index, key, value.Value);
                return true;
            }
            value = null;
            return false;
        }

        private SortedList<float, IEffectiveValue> GetStorage(IDependencyValueProvider provider, DependencyProperty key)
        {
            return _dict.GetOrAdd(key, k => new SortedList<float, IEffectiveValue>());
        }
    }

    public class CurrentValueChangedEventArgs : EventArgs
    {
        public DependencyProperty Property { get; }
        public object OldValue { get; }
        public object NewValue { get; }
        public bool HasOldValue { get; }
        public bool HasNewValue { get; }

        public CurrentValueChangedEventArgs(DependencyProperty property, bool hasOldValue, object oldValue, bool hasNewValue, object newValue)
        {
            Property = property;
            HasOldValue = hasOldValue;
            OldValue = oldValue;
            HasNewValue = hasNewValue;
            NewValue = newValue;
        }
    }
}
