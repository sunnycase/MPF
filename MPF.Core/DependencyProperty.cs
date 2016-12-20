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

        public static DependencyProperty<T> Register<T>(string name, Type ownerType, PropertyMetadata<T> metadata = null)
        {
            if (name == null)
                throw new ArgumentNullException(nameof(name));
            if (ownerType == null)
                throw new ArgumentNullException(nameof(ownerType));

            return new DependencyProperty<T>(name, ownerType, metadata ?? new PropertyMetadata<T>(UnsetValue));
        }

        public static readonly UnsetValueType UnsetValue = default(UnsetValueType);

        public struct UnsetValueType
        {

        }
    }

    public sealed class DependencyProperty<T> : DependencyProperty
    {
        private readonly Type _ownerType;
        private PropertyMetadata<T> _baseMetadata;
        private readonly ConcurrentDictionary<Type, PropertyMetadata<T>> _metadatas = new ConcurrentDictionary<Type, PropertyMetadata<T>>();

        internal DependencyProperty(string name, Type ownerType, PropertyMetadata<T> metadata)
            : base(name, ownerType)
        {
            Contract.Assert(metadata != null);

            _ownerType = ownerType;
            _baseMetadata = metadata;
        }

        public void OverrideMetadata(Type type, PropertyMetadata<T> metadata)
        {
            if (type == null)
                throw new ArgumentNullException(nameof(type));
            if (metadata == null)
                throw new ArgumentNullException(nameof(metadata));

            if(!_ownerType.GetTypeInfo().IsAssignableFrom(type.GetTypeInfo()))
                throw new InvalidOperationException("The new type must be same as or derived from the owner type of this property.");

            if (type == _ownerType)
                _baseMetadata = MergeMetadata(_baseMetadata, metadata);
            else
                _metadatas.AddOrUpdate(type, metadata, (k, old) =>
                {
                    metadata = MergeMetadata(_baseMetadata, metadata);
                    metadata = MergeMetadata(old, metadata);
                    return metadata;
                });
        }

        public DependencyProperty<T> AddOwner(Type ownerType, PropertyMetadata<T> metadata)
        {
            if (ownerType == null)
                throw new ArgumentNullException(nameof(ownerType));
            if (metadata == null)
                throw new ArgumentNullException(nameof(metadata));

            if (_ownerType.GetTypeInfo().IsAssignableFrom(ownerType.GetTypeInfo()))
                throw new InvalidOperationException("The new type must not be same as or derived from the owner type of this property.");

            return new DependencyProperty<T>(Name, ownerType, metadata);
        }

        public bool TryGetDefaultValue(Type type, out T value)
        {
            return GetMetadata(type).TryGetDefaultValue(out value);
        }

        internal void RaisePropertyChanged(Type type, object sender, PropertyChangedEventArgs<T> e)
        {
            GetMetadata(type).RaisePropertyChanged(sender, e);
        }

        private PropertyMetadata<T> GetMetadata(Type type)
        {
            if (type == null)
                throw new ArgumentNullException(nameof(type));

            if (type != _ownerType)
            {
                PropertyMetadata<T> metadata;
                if (_metadatas.TryGetValue(type, out metadata))
                    return metadata;
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
    }
}
