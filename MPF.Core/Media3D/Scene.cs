using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace MPF.Media3D
{
    public class Scene : IList<Visual3D>
    {
        private readonly List<Visual3D> _items = new List<Visual3D>();

        public Visual3D this[int index] { get => _items[index]; set => _items[index] = value; }

        public int Count => _items.Count;

        public bool IsReadOnly => ((IList<Visual3D>)_items).IsReadOnly;

        public void Add(Visual3D item)
        {
            _items.Add(item);
        }

        public void Clear()
        {
            _items.Clear();
        }

        public bool Contains(Visual3D item)
        {
            return _items.Contains(item);
        }

        public void CopyTo(Visual3D[] array, int arrayIndex)
        {
            _items.CopyTo(array, arrayIndex);
        }

        public IEnumerator<Visual3D> GetEnumerator()
        {
            return ((IList<Visual3D>)_items).GetEnumerator();
        }

        public int IndexOf(Visual3D item)
        {
            return _items.IndexOf(item);
        }

        public void Insert(int index, Visual3D item)
        {
            _items.Insert(index, item);
        }

        public bool Remove(Visual3D item)
        {
            return _items.Remove(item);
        }

        public void RemoveAt(int index)
        {
            _items.RemoveAt(index);
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return ((IList<Visual3D>)_items).GetEnumerator();
        }
    }
}
