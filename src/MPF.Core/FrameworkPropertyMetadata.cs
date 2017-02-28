using MPF.Data;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF
{
    [Flags]
    public enum FrameworkPropertyMetadataOptions
    {
        None = 0x0,
        Inherits = 0x1
    }

    public class FrameworkPropertyMetadata<T> : UIPropertyMetadata<T>
    {
        private FrameworkPropertyMetadataOptions _fmkOptions;

        public FrameworkPropertyMetadata(T defaultValue, FrameworkPropertyMetadataOptions fmOptions = FrameworkPropertyMetadataOptions.None, UIPropertyMetadataOptions uiOptions = UIPropertyMetadataOptions.None, EventHandler<PropertyChangedEventArgs<T>> propertyChangedHandler = null)
            : base(defaultValue, uiOptions, propertyChangedHandler)
        {
            _fmkOptions = fmOptions;
        }

        public FrameworkPropertyMetadata(DependencyProperty.UnsetValueType unsetValue, FrameworkPropertyMetadataOptions fmOptions = FrameworkPropertyMetadataOptions.None, UIPropertyMetadataOptions uiOptions = UIPropertyMetadataOptions.None, EventHandler<PropertyChangedEventArgs<T>> propertyChangedHandler = null)
            : base(unsetValue, uiOptions, propertyChangedHandler)
        {
            _fmkOptions = fmOptions;
        }

        protected override void MergeOverride(PropertyMetadata<T> old)
        {
            base.MergeOverride(old);
            _fmkOptions |= ((FrameworkPropertyMetadata<T>)old)._fmkOptions;
        }

        public override bool TryGetNonDefaultValue(DependencyObject d, DependencyProperty<T> property, out T value)
        {
            if (_fmkOptions.HasFlag(FrameworkPropertyMetadataOptions.Inherits))
            {
                var visual = d as Visual;
                if (visual != null)
                {
                    var parent = VisualTreeHelper.GetParent(visual);

                    var storage = (InheritDependencyValueStorage)visual.ValueStorage[InheritDependencyValueStorage.Key];
                    if (storage == null)
                    {
                        storage = new InheritDependencyValueStorage(parent);
                        visual.ValueStorage[InheritDependencyValueStorage.Key] = storage;
                        visual.ParentChanged += Visual_ParentChanged;
                    }
                    storage.TryAddInheritProperty(property);
                    if (parent != null)
                    {
                        value = parent.GetValue(property);
                        return true;
                    }
                }
            }
            return base.TryGetNonDefaultValue(d, property, out value);
        }

        private static void Visual_ParentChanged(object sender, EventArgs e)
        {
            var visual = (Visual)sender;
            var parent = VisualTreeHelper.GetParent(visual);
            var storage = visual.ValueStorage[InheritDependencyValueStorage.Key];
            visual.ValueStorage[InheritDependencyValueStorage.Key] = new InheritDependencyValueStorage(parent, storage.Keys);
        }
    }

    class InheritDependencyValueStorage : IDependencyValueStorage
    {
        public static readonly DependencyValueStorageKey Key = new DependencyValueStorageKey(3.0f, "Inherit");

        public IEnumerable<DependencyProperty> Keys => _inheritProperties;

        public event EventHandler<CurrentValueChangedEventArgs> CurrentValueChanged;

        private readonly DependencyObject _parent;
        private readonly HashSet<DependencyProperty> _inheritProperties = new HashSet<DependencyProperty>();

        public InheritDependencyValueStorage(DependencyObject parent)
        {
            _parent = parent;
            if (_parent != null)
                _parent.RegisterAnyPropertyChangedHandler(OnParentPropertyChanged);
        }

        public InheritDependencyValueStorage(DependencyObject parent, IEnumerable<DependencyProperty> keys)
            : this(parent)
        {
            foreach (var key in keys)
                TryAddInheritProperty(key);
        }

        public void TryAddInheritProperty<T>(DependencyProperty<T> property)
        {
            if (_inheritProperties.Add(property) && _parent != null)
            {
                var value = _parent.GetValue(property);
                CurrentValueChanged?.Invoke(this, new CurrentValueChangedEventArgs(property, false, null, true, value));
            }
        }

        private void TryAddInheritProperty(DependencyProperty property)
        {
            if (_parent != null && _inheritProperties.Add(property))
            {
                IEffectiveValue e;
                if (_parent.ValueStorage.TryGetCurrentEffectiveValue(property, out e))
                    CurrentValueChanged?.Invoke(this, new CurrentValueChangedEventArgs(property, false, null, true, property.Helper.GetEffectiveValue(e)));
            }
        }

        private void OnParentPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (_inheritProperties.Contains(e.Property))
                CurrentValueChanged?.Invoke(this, e.Property.Helper.CreateCurrentValueChangedEventArgs(e));
        }

        public bool TryGetCurrentEffectiveValue(DependencyProperty key, out IEffectiveValue value)
        {
            if (_parent != null && _inheritProperties.Contains(key))
                return _parent.ValueStorage.TryGetCurrentEffectiveValue(key, out value);
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
