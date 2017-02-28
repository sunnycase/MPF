using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public abstract class Animatable : Freezable
    {
        internal event EventHandler UpdateResource;

        public Animatable()
        {

        }

        bool _updateRegistered = false;
        internal void RegisterUpdateResource()
        {
            if (!_updateRegistered)
            {
                _updateRegistered = true;
                DeviceContext.Current.UpdateResource += OnUpdateResource;
                UpdateResource?.Invoke(this, EventArgs.Empty);
            }
        }

        internal virtual void OnUpdateResource(object sender, EventArgs e)
        {
            _updateRegistered = false;
        }
    }
}
