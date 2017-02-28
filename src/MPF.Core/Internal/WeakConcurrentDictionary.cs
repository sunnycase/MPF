using System;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Internal
{
    public class WeakConcurrentDictionary<TKey, TValue> where TValue : class
    {
        private readonly ConcurrentDictionary<TKey, WeakReference<TValue>> _entries;

        public WeakConcurrentDictionary()
        {
            _entries = new ConcurrentDictionary<TKey, WeakReference<TValue>>();
        }

        public WeakConcurrentDictionary(int capacity)
        {
            _entries = new ConcurrentDictionary<TKey, WeakReference<TValue>>(1, capacity);
        }

        public TValue GetOrAdd(TKey key, Func<TKey, TValue> valueFactory)
        {
            var weak = _entries.GetOrAdd(key, k => new WeakReference<TValue>(valueFactory(k)));
            TValue value;
            if (weak.TryGetTarget(out value))
                return value;
            value = valueFactory(key);
            weak.SetTarget(value);
            return value;
        }
    }
}
