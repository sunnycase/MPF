using MPF.Data;
using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Diagnostics.Contracts;
using System.Linq;
using System.Reflection;
using System.Threading;
using System.Threading.Tasks;

namespace MPF
{
    public abstract class DependencyProperty : IEquatable<DependencyProperty>
    {
        private static int _nextAvailableGlobalId = 0;
        private static readonly ConcurrentDictionary<FromNameKey, DependencyProperty> _fromNameMaps = new ConcurrentDictionary<FromNameKey, DependencyProperty>();

        public string Name { get; }
        public Type OwnerType { get; }
        public abstract Type PropertyType { get; }

        private readonly int _globalId;

        internal DependencyProperty(string name, Type ownerType)
        {
            Name = name;
            OwnerType = ownerType;
            AddFromeNameKey(name, ownerType);
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

        public static DependencyProperty<T> Register<T>(string name, Type ownerType, PropertyMetadata<T> metadata = null)
        {
            if (name == null)
                throw new ArgumentNullException(nameof(name));
            if (ownerType == null)
                throw new ArgumentNullException(nameof(ownerType));

            return new DependencyProperty<T>(name, ownerType, metadata ?? new PropertyMetadata<T>(UnsetValue));
        }

        public static DependencyProperty FromName(string name, Type ownerType)
        {
            if (name == null)
                throw new ArgumentNullException(nameof(name));
            if (ownerType == null)
                throw new ArgumentNullException(nameof(ownerType));

            DependencyProperty property = null;
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

        internal abstract object GetEffectiveValueHelper(IEffectiveValue value);
        internal abstract bool EqualsEffectiveValueHelper(IEffectiveValue a, IEffectiveValue b);

        public static readonly UnsetValueType UnsetValue = default(UnsetValueType);

        public struct UnsetValueType
        {

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

    public sealed class DependencyProperty<T> : DependencyProperty
    {
        private PropertyMetadata<T> _baseMetadata;
        private readonly ConcurrentDictionary<Type, PropertyMetadata<T>> _metadatas = new ConcurrentDictionary<Type, PropertyMetadata<T>>();

        public override Type PropertyType => typeof(T);

        internal DependencyProperty(string name, Type ownerType, PropertyMetadata<T> metadata)
            : base(name, ownerType)
        {
            Contract.Assert(metadata != null);
            
            _baseMetadata = metadata;
        }

        public void OverrideMetadata(Type type, PropertyMetadata<T> metadata)
        {
            if (type == null)
                throw new ArgumentNullException(nameof(type));
            if (metadata == null)
                throw new ArgumentNullException(nameof(metadata));

            metadata = MergeMetadata(_baseMetadata, metadata);
            if (type == OwnerType)
                _baseMetadata = metadata;
            else
            {
                _metadatas.AddOrUpdate(type, metadata, (k, old) =>
                {
                    metadata = MergeMetadata(old, metadata);
                    return metadata;
                });
            }
        }

        public DependencyProperty<T> AddOwner(Type ownerType, PropertyMetadata<T> metadata = null)
        {
            if (ownerType == null)
                throw new ArgumentNullException(nameof(ownerType));

            AddFromeNameKey(Name, ownerType);

            if (metadata != null)
                OverrideMetadata(ownerType, metadata);
            return this;
        }

        public bool TryGetDefaultValue(DependencyObject d, Type type, out T value)
        {
            return GetMetadata(type).TryGetDefaultValue(d, this, out value);
        }

        internal void RaisePropertyChanged(Type type, object sender, PropertyChangedEventArgs<T> e)
        {
            GetMetadata(type).RaisePropertyChanged(sender, e);
        }

        private PropertyMetadata<T> GetMetadata(Type type)
        {
            if (type == null)
                throw new ArgumentNullException(nameof(type));

            if (type != OwnerType)
            {
                PropertyMetadata<T> metadata;
                while (type != null)
                {
                    if (_metadatas.TryGetValue(type, out metadata))
                        return metadata;
                    type = type.GetTypeInfo().BaseType;
                }
            }
            return _baseMetadata;
        }

        private PropertyMetadata<T> MergeMetadata(PropertyMetadata<T> oldMetadata, PropertyMetadata<T> newMetadata)
        {
            if (!oldMetadata.GetType().GetTypeInfo().IsAssignableFrom(newMetadata.GetType().GetTypeInfo()))
                throw new InvalidOperationException("The type of new metadata must be derived from the type of old metadata.");
            newMetadata.Merge(oldMetadata);
            return newMetadata;
        }

        internal override object GetEffectiveValueHelper(IEffectiveValue value)
        {
            return ((IEffectiveValue<T>)value).Value;
        }

        internal override bool EqualsEffectiveValueHelper(IEffectiveValue a, IEffectiveValue b)
        {
            return EqualityComparer<T>.Default.Equals(((IEffectiveValue<T>)a).Value, ((IEffectiveValue<T>)b).Value);
        }
    }
}
