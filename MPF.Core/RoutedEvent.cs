using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Reflection;
using System.Text;
using System.Threading;

namespace MPF
{
    public abstract class RoutedEvent
    {
        private static int _nextAvailableGlobalId = 0;
        private static readonly ConcurrentDictionary<FromNameKey, RoutedEvent> _fromNameMaps = new ConcurrentDictionary<FromNameKey, RoutedEvent>();

        public string Name { get; }
        public Type OwnerType { get; }
        public abstract Type EventArgsType { get; }
        public RoutingStrategy RoutingStrategy { get; }

        private readonly int _globalId;

        internal RoutedEvent(string name, Type ownerType, RoutingStrategy routingStrategy)
        {
            Name = name;
            OwnerType = ownerType;
            RoutingStrategy = routingStrategy;
            AddFromeNameKey(name, ownerType);
            _globalId = Interlocked.Increment(ref _nextAvailableGlobalId);
        }

        public override bool Equals(object obj)
        {
            if (obj is RoutedEvent)
                return Equals((RoutedEvent)obj);
            return false;
        }

        public bool Equals(RoutedEvent other)
        {
            if (other != null) return _globalId == other._globalId;
            return false;
        }

        public override int GetHashCode()
        {
            return _globalId.GetHashCode();
        }

        public static RoutedEvent<T> Register<T>(string name, Type ownerType, RoutingStrategy routingStrategy) where T : RoutedEventArgs
        {
            if (name == null)
                throw new ArgumentNullException(nameof(name));
            if (ownerType == null)
                throw new ArgumentNullException(nameof(ownerType));

            return new RoutedEvent<T>(name, ownerType, routingStrategy);
        }

        public static RoutedEvent FromName(string name, Type ownerType)
        {
            if (name == null)
                throw new ArgumentNullException(nameof(name));
            if (ownerType == null)
                throw new ArgumentNullException(nameof(ownerType));

            RoutedEvent property = null;
            while (property == null && ownerType != null)
            {
                if (!_fromNameMaps.TryGetValue(new FromNameKey(name, ownerType), out property))
                    ownerType = ownerType.GetTypeInfo().BaseType;
            }
            return property != null ? property : throw new InvalidOperationException($"Property {ownerType.Name}.{name} not found.");
        }

        protected void AddFromeNameKey(string name, Type ownerType)
        {
            if (!_fromNameMaps.TryAdd(new FromNameKey(name, ownerType), this))
                throw new ArgumentException($"Property {ownerType.Name}.{name} is already registered.");
        }

        private struct FromNameKey : IEquatable<FromNameKey>
        {
            public string Name { get; }
            public Type OwnerType { get; }

            private readonly int _hashCode;

            public FromNameKey(string name, Type ownerType)
            {
                Name = name;
                OwnerType = ownerType;
                _hashCode = name.GetHashCode() ^ ownerType.GetHashCode();
            }

            public bool Equals(FromNameKey other)
            {
                return Name == other.Name && OwnerType == other.OwnerType;
            }

            public override int GetHashCode()
            {
                return _hashCode;
            }

            public override bool Equals(object obj)
            {
                if (obj is FromNameKey)
                    return Equals((FromNameKey)obj);
                return false;
            }
        }
    }

    public sealed class RoutedEvent<TArgs> : RoutedEvent where TArgs : RoutedEventArgs
    {
        public override Type EventArgsType => typeof(TArgs);

        internal RoutedEvent(string name, Type ownerType, RoutingStrategy routingStrategy)
            : base(name, ownerType, routingStrategy)
        {

        }
    }
}
