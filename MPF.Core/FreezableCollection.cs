using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace MPF
{
    public class FreezableCollection<T> : Freezable, IList<T>
    {
        private readonly List<T> _items;
        private static Lazy<FreezableCollection<T>> _empty = new Lazy<FreezableCollection<T>>(() =>
        {
            var empty = new FreezableCollection<T>();
            empty.Freeze();
            return empty;
        });
        public static FreezableCollection<T> Empty => _empty.Value;

        public T this[int index] { get => _items[index]; set => _items[index] = value; }

        public int Count => _items.Count;

        public bool IsReadOnly => ((IList<T>)_items).IsReadOnly;

        public FreezableCollection()
        {
            _items = new List<T>();
        }

        public FreezableCollection(IEnumerable<T> collection)
        {
            _items = new List<T>(collection);
        }

        public void Add(T item)
        {
            _items.Add(item);
        }

        public void Clear()
        {
            _items.Clear();
        }

        public bool Contains(T item)
        {
            return _items.Contains(item);
        }

        public void CopyTo(T[] array, int arrayIndex)
        {
            _items.CopyTo(array, arrayIndex);
        }

        public IEnumerator<T> GetEnumerator()
        {
            return ((IList<T>)_items).GetEnumerator();
        }

        public int IndexOf(T item)
        {
            return _items.IndexOf(item);
        }

        public void Insert(int index, T item)
        {
            _items.Insert(index, item);
        }

        public bool Remove(T item)
        {
            return _items.Remove(item);
        }

        public void RemoveAt(int index)
        {
            _items.RemoveAt(index);
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return ((IList<T>)_items).GetEnumerator();
        }
    }
}
