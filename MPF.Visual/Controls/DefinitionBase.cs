using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public abstract class DefinitionBase : DependencyObject
    {
        public event EventHandler Invalidated;

        protected abstract DependencyProperty<GridLength> SizeProperty { get; }
        protected abstract DependencyProperty<float> MinSizeProperty { get; }
        protected abstract DependencyProperty<float> MaxSizeProperty { get; }

        internal GridLength UserSize => GetValue(SizeProperty);
        internal float UserMinSize => GetValue(MinSizeProperty);
        internal float UserMaxSize => GetValue(MaxSizeProperty);

        protected void Invalidate()
        {
            Invalidated?.Invoke(this, EventArgs.Empty);
        }

        protected static void OnSizePropertyChanged(object sender, PropertyChangedEventArgs<GridLength> e)
        {

        }

        protected static void OnMinMaxSizePropertyChanged(object sender, PropertyChangedEventArgs<float> e)
        {

        }
    }

    public abstract class DefinitionCollectionBase<T> : IList<T>, IReadOnlyList<T> where T : DefinitionBase
    {
        private readonly Grid _grid;
        private readonly List<T> _definitions = new List<T>();

        public T this[int index]
        {
            get => _definitions[index];
            set
            {
                VerifyWriteAccess();

                var old = _definitions[index];
                if(old != value)
                {
                    old.Invalidated -= Item_Invalidated;
                    _definitions[index] = value;
                    value.Invalidated += Item_Invalidated;
                }
            }
        }

        public int Count => _definitions.Count;

        public bool IsReadOnly => _grid._isMeasureInProgress;

        public DefinitionCollectionBase(Grid grid)
        {
            _grid = grid ?? throw new ArgumentNullException(nameof(grid));
        }

        public void Add(T item)
        {
            VerifyWriteAccess();

            _definitions.Add(item);
            _grid.AddDefination(item);
            item.Invalidated += Item_Invalidated;
            InvalidateParent();
        }

        public void Clear()
        {
            VerifyWriteAccess();

            foreach (var item in _definitions)
            {
                item.Invalidated -= Item_Invalidated;
                _grid.RemoveDefination(item);
            }
            _definitions.Clear();
        }

        public bool Contains(T item)
        {
            return _definitions.Contains(item);
        }

        public void CopyTo(T[] array, int arrayIndex)
        {
            _definitions.CopyTo(array, arrayIndex);
        }

        public IEnumerator<T> GetEnumerator()
        {
            return _definitions.GetEnumerator();
        }

        public int IndexOf(T item)
        {
            return _definitions.IndexOf(item);
        }

        public void Insert(int index, T item)
        {
            VerifyWriteAccess();

            _definitions.Insert(index, item);
            _grid.AddDefination(item);
            item.Invalidated += Item_Invalidated;
            InvalidateParent();
        }

        public bool Remove(T item)
        {
            VerifyWriteAccess();

            if (_definitions.Remove(item))
            {
                item.Invalidated -= Item_Invalidated;
                _grid.RemoveDefination(item);
                return true;
            }
            return false;
        }

        public void RemoveAt(int index)
        {
            VerifyWriteAccess();

            var item = _definitions[index];
            _grid.RemoveDefination(item);
            item.Invalidated -= Item_Invalidated;
            _definitions.RemoveAt(index);
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return _definitions.GetEnumerator();
        }

        private void Item_Invalidated(object sender, EventArgs e)
        {
            InvalidateParent();
        }

        private void InvalidateParent()
        {
            _grid.InvalidateCells();
        }

        private void VerifyWriteAccess()
        {
            if (IsReadOnly)
                throw new InvalidOperationException("Definations is readonly.");
        }
    }
}
