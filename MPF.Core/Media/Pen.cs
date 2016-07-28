using MPF.Interop;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public sealed class Pen : Animatable, IResourceProvider
    {
        public static readonly DependencyProperty<float> ThicknessProperty = DependencyProperty.Register(nameof(Thickness), typeof(Pen),
            1.0f, OnThicknessPropertyChanged);
        public static readonly DependencyProperty<Brush> BrushProperty = DependencyProperty.Register<Brush>(nameof(Brush), typeof(Pen),
            null, OnBrushPropertyChanged);

        public float Thickness
        {
            get { return GetValue(ThicknessProperty); }
            set { SetValue(ThicknessProperty, value); }
        }

        public Brush Brush
        {
            get { return GetValue(BrushProperty); }
            set { SetValue(BrushProperty, value); }
        }

        private readonly IResource _penResource;

        IResource IResourceProvider.Resource => _penResource;

        public Pen()
        {
            _penResource = MediaResourceManager.Current.CreateResouce(ResourceType.RT_Pen);
            RegisterUpdateResource();
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            MediaResourceManager.Current.UpdatePen(_penResource, Thickness, Brush);
        }

        private static void OnThicknessPropertyChanged(object sender, PropertyChangedEventArgs<float> e)
        {
            ((Pen)sender).RegisterUpdateResource();
        }

        private static void OnBrushPropertyChanged(object sender, PropertyChangedEventArgs<Brush> e)
        {
            ((Pen)sender).UpdateOnBrushUpdateResource(e.OldValue, e.NewValue);
            ((Pen)sender).RegisterUpdateResource();
        }

        private void UpdateOnBrushUpdateResource(Brush oldValue, Brush newValue)
        {
            if (oldValue != null)
                oldValue.UpdateResource -= OnBrushUpdateResource;
            if (newValue != null)
                newValue.UpdateResource += OnBrushUpdateResource;
        }

        private void OnBrushUpdateResource(object sender, EventArgs e)
        {
            RegisterUpdateResource();
        }
    }
}
