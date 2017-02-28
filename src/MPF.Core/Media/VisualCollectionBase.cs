using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;

namespace MPF.Media
{
    public class VisualCollectionBase<T> : IList<T> where T : Visual
    {
        protected Visual VisualParent { get; }
        private readonly List<T> _elements = new List<T>();

        public T this[int index]
        {
            get => _elements[index];
            set
            {
                VerifyWriteAccess(null);

                var old = _elements[index];
                if (old != value)
                {
                    RemoveChild(old);
                    _elements[index] = value;
                    AddChild(value);
                    InvalidateParent();
                }
            }
        }

        public int Count => _elements.Count;

        private readonly object _cookie;
        public virtual bool IsReadOnly => false;

        public VisualCollectionBase(Visual visualParent, object cookie)
        {
            VisualParent = visualParent ?? throw new ArgumentNullException(nameof(visualParent));
            _cookie = cookie;
        }

        public void Add(T item)
        {
            Add(item, null);
        }

        public void Add(T item, object cookie)
        {
            VerifyWriteAccess(cookie);

            _elements.Add(item);
            AddChild(item);
            InvalidateParent();
        }

        public void Clear()
        {
            Clear(null);
        }

        public void Clear(object cookie)
        {
            VerifyWriteAccess(cookie);

            foreach (var item in _elements)
                RemoveChild(item);
            _elements.Clear();
            InvalidateParent();
        }

        public bool Contains(T item)
        {
            return _elements.Contains(item);
        }

        public void CopyTo(T[] array, int arrayIndex)
        {
            _elements.CopyTo(array, arrayIndex);
        }

        public IEnumerator<T> GetEnumerator()
        {
            return _elements.GetEnumerator();
        }

        public int IndexOf(T item)
        {
            return _elements.IndexOf(item);
        }

        public void Insert(int index, T item)
        {
            Insert(index, item, null);
        }

        public void Insert(int index, T item, object cookie)
        {
            VerifyWriteAccess(cookie);

            _elements.Insert(index, item);
            AddChild(item);
            InvalidateParent();
        }

        public bool Remove(T item)
        {
            return Remove(item, null);
        }

        public bool Remove(T item, object cookie)
        {
            VerifyWriteAccess(cookie);

            if(_elements.Remove(item))
            {
                RemoveChild(item);
                InvalidateParent();
                return true;
            }
            return false;
        }

        public void RemoveAt(int index)
        {
            RemoveAt(index, null);
        }

        public void RemoveAt(int index, object cookie)
        {
            VerifyWriteAccess(cookie);

            var item = _elements[index];
            _elements.RemoveAt(index);
            RemoveChild(item);
            InvalidateParent();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return _elements.GetEnumerator();
        }

        private void VerifyWriteAccess(object cookie)
        {
            if (IsReadOnly && _cookie != cookie)
                throw new InvalidOperationException("ItemsHost's children is readonly.");
        }

        protected virtual void AddChild(T child)
        {
            VisualParent.AddVisualChild(child);
        }

        protected virtual void RemoveChild(T child)
        {
            VisualParent.RemoveVisualChild(child);
        }

        protected virtual void InvalidateParent()
        {

        }
    }
}
