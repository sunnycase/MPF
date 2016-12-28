using System;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Text;

namespace MPF.Data
{
    public class PropertyPath
    {
        public string Path { get; }

        public PropertyPath(string path)
        {
            Path = path;
        }

        public Expression GetAccessor(Expression source)
        {
            var result = source;
            if(!string.IsNullOrEmpty(Path))
            {
                foreach (var property in Path.Split('.'))
                {
                    result = Expression.Convert(Expression.Property(result, property), typeof(object));
                }
            }
            return result;
        }

        public static implicit operator PropertyPath(string path)
        {
            return new PropertyPath(path);
        }
    }
}
