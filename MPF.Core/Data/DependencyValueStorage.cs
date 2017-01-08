using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    internal class DependencyValueStorage : IDependencyValueStorage
    {
        private readonly ConcurrentDictionary<DependencyProperty, SortedList<float, IEffectiveValue>> _dict = new ConcurrentDictionary<DependencyProperty, SortedList<float, IEffectiveValue>>();

        public event EventHandler<EffectiveValueChangedEventArgs> EffectiveValueChanged;

        public DependencyValueStorage()
        {

        }

        public IEffectiveValue AddOrUpdate(IDependencyValueProvider provider, DependencyProperty key, Func<DependencyProperty, IEffectiveValue> addValueFactory, Func<DependencyProperty, IEffectiveValue, IEffectiveValue> updateValueFactory)
        {
            bool raiseChanged = false;
            var storage = GetStorage(provider, key);
            var priority = provider.Priority;
            IEffectiveValue result;

            var oldIdx = storage.IndexOfKey(priority);
            if (oldIdx == -1)
            {
                var value = addValueFactory(key);
                storage.Add(priority, value);
                value.ValueChanged = (s, e) => OnEffectiveValueChanged(priority, key);
                result = value;
                raiseChanged = storage.IndexOfKey(priority) == 0;
            }
            else
            {
                var oldValue = storage.Values[oldIdx];
                var newValue = updateValueFactory(key, oldValue);
                if(oldValue != newValue)
                {
                    oldValue.ValueChanged = null;
                    newValue.ValueChanged = (s, e) => OnEffectiveValueChanged(priority, key);
                    storage.Values[oldIdx] = newValue;
                    raiseChanged = oldIdx == 0;
                }
                result = newValue;
            }

            if (raiseChanged)
                OnEffectiveValueChanged(priority, key);
            return result;
        }

        private void OnEffectiveValueCleared(float priority, DependencyProperty key)
        {

        }

        private void OnEffectiveValueChanged(float priority, DependencyProperty key)
        {

        }

        public bool TryGetValue(IDependencyValueProvider provider, DependencyProperty key, out IEffectiveValue value)
        {
            var storage = GetStorage(provider, key);
            return storage.TryGetValue(provider.Priority, out value);
        }

        public bool TryRemove(IDependencyValueProvider provider, DependencyProperty key, out IEffectiveValue value)
        {
            var storage = GetStorage(provider, key);
            var priority = provider.Priority;
            if (storage.TryGetValue(priority, out value))
            {
                var index = storage.IndexOfKey(priority);
                storage.RemoveAt(index);
                OnEffectiveValueCleared(priority, key);
                return true;
            }
            return false;
        }

        private SortedList<float, IEffectiveValue> GetStorage(IDependencyValueProvider provider, DependencyProperty key)
        {
            return _dict.GetOrAdd(key, k => new SortedList<float, IEffectiveValue>());
        }
    }

    internal class EffectiveValueChangedEventArgs : EventArgs
    {
        public DependencyProperty Property { get; }

        public EffectiveValueChangedEventArgs(DependencyProperty property)
        {
            Property = property;
        }
    }
}
