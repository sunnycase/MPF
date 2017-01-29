using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;

namespace MPF.Data
{
    public class DependencyValueStorageChain
    {
        public event EventHandler<CurrentValueChangedEventArgs> CurrentValueChanged;

        private static readonly DependencyValueStorageKey DefaultKey = new DependencyValueStorageKey(1.0f, "Default");

        public IDependencyValueStorage Default { get; } = new DependencyValueStorage();
        private readonly object _locker = new object();
        private readonly SortedDictionary<DependencyValueStorageKey, IDependencyValueStorage> _storages = new SortedDictionary<DependencyValueStorageKey, IDependencyValueStorage>();

        public DependencyValueStorageChain()
        {
            _storages.Add(DefaultKey, Default);
            Default.CurrentValueChanged += Storage_CurrentValueChanged;
        }

        public IDependencyValueStorage this[DependencyValueStorageKey key]
        {
            get
            {
                lock(_locker)
                {
                    IDependencyValueStorage storage;
                    if (_storages.TryGetValue(key, out storage))
                        return storage;
                    return null;
                }
            }
            set
            {
                if (key == DefaultKey)
                    throw new InvalidOperationException("Default storage is readonly.");

                lock (_locker)
                {
                    IDependencyValueStorage storage;
                    if (_storages.TryGetValue(key, out storage))
                    {
                        if (storage != value)
                        {
                            storage.CurrentValueChanged -= Storage_CurrentValueChanged;
                            if (value != null)
                            {
                                UpdateStorage(key, storage, value);
                                value.CurrentValueChanged += Storage_CurrentValueChanged;
                            }
                            else
                                RemoveStorage(key, storage);
                        }
                    }
                    else
                    {
                        if (value != null)
                        {
                            AddStorage(key, value);
                            value.CurrentValueChanged += Storage_CurrentValueChanged;
                        }
                    }
                }
            }
        }

        private void AddStorage(DependencyValueStorageKey key, IDependencyValueStorage storage)
        {
            var dps = storage.Keys.ToList();
            UpdateStorage(dps, () => _storages.Add(key, storage));
        }

        private void RemoveStorage(DependencyValueStorageKey key, IDependencyValueStorage storage)
        {
            var dps = storage.Keys.ToList();
            UpdateStorage(dps, () => _storages.Remove(key));
        }

        private void UpdateStorage(DependencyValueStorageKey key, IDependencyValueStorage old, IDependencyValueStorage value)
        {
            var dps = value.Keys.Union(old.Keys).ToList();
            UpdateStorage(dps, () => _storages[key] = value);
        }

        private void UpdateStorage(List<DependencyProperty> dps, Action action)
        {
            var oldValues = new Dictionary<DependencyProperty, IEffectiveValue>();
            foreach (var dp in dps)
            {
                IEffectiveValue oldEValue;
                if (TryGetCurrentEffectiveValue(dp, out oldEValue))
                    oldValues.Add(dp, oldEValue);
            }
            action();

            foreach (var dp in dps)
            {
                IEffectiveValue newEValue;
                if (TryGetCurrentEffectiveValue(dp, out newEValue))
                {
                    IEffectiveValue oldEValue;
                    if (oldValues.TryGetValue(dp, out oldEValue))
                    {
                        if (!dp.Helper.EqualsEffectiveValue(oldEValue, newEValue))
                            RaiseCurrentValueChanged(dp, true, dp.Helper.GetEffectiveValue(oldEValue), true, dp.Helper.GetEffectiveValue(newEValue));
                    }
                    else
                        RaiseCurrentValueChanged(dp, false, null, true, dp.Helper.GetEffectiveValue(newEValue));
                }
                else
                {
                    IEffectiveValue oldEValue;
                    if (oldValues.TryGetValue(dp, out oldEValue))
                        RaiseCurrentValueChanged(dp, true, dp.Helper.GetEffectiveValue(oldEValue), false, null);
                }
            }
        }

        public bool TryGetCurrentEffectiveValue<T>(DependencyProperty<T> key, out IEffectiveValue<T> value)
        {
            foreach (var storage in _storages.Values)
                if (storage.TryGetCurrentEffectiveValue(key, out value))
                    return true;
            value = null;
            return false;
        }

        public bool TryGetCurrentValue<T>(DependencyProperty<T> key, out T value)
        {
            foreach (var storage in _storages.Values)
                if (storage.TryGetCurrentValue(key, out value))
                    return true;
            value = default(T);
            return false;
        }

        internal bool TryGetCurrentEffectiveValue(DependencyProperty key, out IEffectiveValue value)
        {
            foreach (var storage in _storages.Values)
                if (storage.TryGetCurrentEffectiveValue(key, out value))
                    return true;
            value = null;
            return false;
        }

        private bool TryGetCurrentEffectiveValueExcept(DependencyProperty key, IDependencyValueStorage except, out IEffectiveValue value)
        {
            foreach (var storage in _storages.Values)
            {
                if (storage == except) continue;
                if (storage.TryGetCurrentEffectiveValue(key, out value))
                    return true;
            }
            value = null;
            return false;
        }

        private IDependencyValueStorage GetCurrentValueStorage(DependencyProperty key)
        {
            foreach (var storage in _storages.Values)
                if (storage.Keys.Contains(key))
                    return storage;
            return null;
        }

        private void RaiseCurrentValueChanged(DependencyProperty key, bool hasOldValue, object oldValue, bool hasNewValue, object newValue)
        {
            CurrentValueChanged?.Invoke(this, new CurrentValueChangedEventArgs(key, hasOldValue, oldValue, hasNewValue, newValue));
        }

        private void Storage_CurrentValueChanged(object sender, CurrentValueChangedEventArgs e)
        {
            var storage = (IDependencyValueStorage)sender;
            var dp = e.Property;
            if (GetCurrentValueStorage(dp) == storage)
            {
                bool hasOldValue = false;
                object oldValue = null;

                if (e.HasOldValue)
                {
                    hasOldValue = true;
                    oldValue = e.OldValue;
                }
                else
                {
                    IEffectiveValue oldEValue;
                    if (TryGetCurrentEffectiveValueExcept(dp, storage, out oldEValue))
                    {
                        hasOldValue = true;
                        oldValue = dp.Helper.GetEffectiveValue(oldEValue);
                    }
                }

                bool hasNewValue = false;
                object newValue = null;

                if (e.HasNewValue)
                {
                    hasNewValue = true;
                    newValue = e.NewValue;
                }
                else
                {
                    IEffectiveValue newEValue;
                    if (TryGetCurrentEffectiveValueExcept(dp, storage, out newEValue))
                    {
                        hasNewValue = true;
                        newValue = dp.Helper.GetEffectiveValue(newEValue);
                    }
                }
                RaiseCurrentValueChanged(dp, hasOldValue, oldValue, hasNewValue, newValue);
            }
        }
    }
}
