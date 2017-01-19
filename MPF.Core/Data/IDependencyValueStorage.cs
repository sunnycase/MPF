using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Data
{
    public class DependencyValueStorageKey : IEquatable<DependencyValueStorageKey>, IComparable<DependencyValueStorageKey>
    {
        public float Priority { get; }
        public string Name { get; }

        public DependencyValueStorageKey(float priority, string name)
        {
            Priority = priority;
            Name = name;
        }

        public bool Equals(DependencyValueStorageKey other)
        {
            if (other == null) return false;
            return Priority == other.Priority && Name == other.Name;
        }

        public int CompareTo(DependencyValueStorageKey other)
        {
            return Comparer<float?>.Default.Compare(Priority, other?.Priority);
        }

        public override bool Equals(object obj)
        {
            if (obj is DependencyValueStorageKey)
                return Equals((DependencyValueStorageKey)obj);
            return false;
        }

        public override int GetHashCode()
        {
            return Priority.GetHashCode() ^ Name?.GetHashCode() ?? 0;
        }
    }

    public interface IDependencyValueStorage
    {
        event EventHandler<CurrentValueChangedEventArgs> CurrentValueChanged;

        IEffectiveValue AddOrUpdate<T>(IDependencyValueProvider provider, DependencyProperty<T> key, Func<DependencyProperty, IEffectiveValue<T>> addValueFactory, Func<DependencyProperty, IEffectiveValue<T>, IEffectiveValue<T>> updateValueFactory);
        bool TryGetValue<T>(IDependencyValueProvider provider, DependencyProperty<T> key, out IEffectiveValue<T> value);
        bool TryRemove<T>(IDependencyValueProvider provider, DependencyProperty<T> key, out IEffectiveValue<T> value);

        bool TryGetCurrentValue<T>(DependencyProperty<T> key, out T value);
        bool TryGetCurrentEffectiveValue<T>(DependencyProperty<T> key, out IEffectiveValue<T> value);
        bool TryGetCurrentEffectiveValue(DependencyProperty key, out IEffectiveValue value);

        IEnumerable<DependencyProperty> Keys { get; }
    }
}
