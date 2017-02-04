using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace MPF.Controls
{
    public abstract class DefinationBase : DependencyObject
    {
        public event EventHandler Invalidated;

        protected abstract DependencyProperty<GridLength> SizeProperty { get; }

        protected void Invalidate()
        {
            Invalidated?.Invoke(this, EventArgs.Empty);
        }

        protected static void OnSizePropertyChanged(object sender, PropertyChangedEventArgs<GridLength> e)
        {

        }
    }

    public abstract class DefinationCollectionBase<T> : IList<T> where T : DefinationBase
    {
        private readonly Grid _grid;
        private readonly List<T> _definations = new List<T>();

        public T this[int index]
        {
            get => _definations[index];
            set
            {
                VerifyWriteAccess();

                var old = _definations[index];
                if(old != value)
                {
                    old.Invalidated -= Item_Invalidated;
                    _definations[index] = value;
                    value.Invalidated += Item_Invalidated;
                }
            }
        }

        public int Count => _definations.Count;

        public bool IsReadOnly => _grid._isMeasureInProgress;

        public DefinationCollectionBase(Grid grid)
        {
            _grid = grid ?? throw new ArgumentNullException(nameof(grid));
        }

        public void Add(T item)
        {
            VerifyWriteAccess();

            _definations.Add(item);
            item.Invalidated += Item_Invalidated;
            InvalidateParent();
        }

        public void Clear()
        {
            VerifyWriteAccess();

            foreach (var item in _definations)
                item.Invalidated -= Item_Invalidated;
            _definations.Clear();
        }

        public bool Contains(T item)
        {
            return _definations.Contains(item);
        }

        public void CopyTo(T[] array, int arrayIndex)
        {
            _definations.CopyTo(array, arrayIndex);
        }

        public IEnumerator<T> GetEnumerator()
        {
            return _definations.GetEnumerator();
        }

        public int IndexOf(T item)
        {
            return _definations.IndexOf(item);
        }

        public void Insert(int index, T item)
        {
            VerifyWriteAccess();

            _definations.Insert(index, item);
            item.Invalidated += Item_Invalidated;
            InvalidateParent();
        }

        public bool Remove(T item)
        {
            VerifyWriteAccess();

            if (_definations.Remove(item))
            {
                item.Invalidated -= Item_Invalidated;
                return true;
            }
            return false;
        }

        public void RemoveAt(int index)
        {
            VerifyWriteAccess();

            _definations[index].Invalidated -= Item_Invalidated;
            _definations.RemoveAt(index);
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return _definations.GetEnumerator();
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
