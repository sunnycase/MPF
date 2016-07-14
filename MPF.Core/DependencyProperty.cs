using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

namespace MPF
{
    public abstract class DependencyProperty : IEquatable<DependencyProperty>
    {
        private static volatile int _nextAvailableGlobalId = 0;

        public string Name { get; }
        public Type OwnerType { get; }

        private readonly int _globalId;

        internal DependencyProperty(string name, Type ownerType)
        {
            Name = name;
            OwnerType = ownerType;
            _globalId = Interlocked.Increment(ref _nextAvailableGlobalId);
        }

        public override bool Equals(object obj)
        {
            if (obj is DependencyProperty)
                return Equals((DependencyProperty)obj);
            return false;
        }

        public bool Equals(DependencyProperty other)
        {
            if (other != null) return _globalId == other._globalId;
            return false;
        }

        public override int GetHashCode()
        {
            return _globalId.GetHashCode();
        }

        public static DependencyProperty<T> Register<T>(string name, Type ownerType, T defaultValue = default(T), EventHandler<PropertyChangedEventArgs<T>> propertyChangedHandler = null)
        {
            if (ownerType == null)
                throw new ArgumentNullException(nameof(ownerType));

            var property = new DependencyProperty<T>(name, ownerType, defaultValue);
            if (propertyChangedHandler != null)
                property.PropertyChanged += propertyChangedHandler;
            return property;
        }
    }

    public sealed class DependencyProperty<T> : DependencyProperty
    {
        private T _defaultValue;
        public T DefaultValue
        {
            get { return _defaultValue; }
            set
            {
                if (!EqualityComparer<T>.Default.Equals(_defaultValue, value))
                {
                    _defaultValue = value;
                    DefaultValueChanged?.Invoke(this, EventArgs.Empty);
                }
            }
        }

        public event EventHandler DefaultValueChanged;
        public event EventHandler<PropertyChangedEventArgs<T>> PropertyChanged;

        internal DependencyProperty(string name, Type ownerType, T defaultValue)
            : base(name, ownerType)
        {
            _defaultValue = defaultValue;
        }

        internal void InvokePropertyChangedHandlers(object sender, PropertyChangedEventArgs<T> e)
        {
            PropertyChanged?.Invoke(sender, e);
        }
    }
}
