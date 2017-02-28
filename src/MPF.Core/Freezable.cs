using System;
using System.Collections.Generic;
using System.Text;

namespace MPF
{
    public class Freezable : DependencyObject
    {
        private bool _isFrozen;
        public bool IsFrozen
        {
            get
            {
                CheckAccess();
                return _isFrozen;
            }
        }

        public void Freeze()
        {
            if(!_isFrozen)
            {
                _isFrozen = true;
            }
        }

        protected void WritePremable()
        {
            if (IsFrozen)
                throw new InvalidOperationException("Frozen object can't be modified.");
        }
    }
}
