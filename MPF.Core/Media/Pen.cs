using MPF.Data;
using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace MPF.Media
{
    public sealed class Pen : Animatable, IResourceProvider
    {
        public static readonly DependencyProperty<float> ThicknessProperty = DependencyProperty.Register(nameof(Thickness), typeof(Pen),
            new PropertyMetadata<float>(1.0f, OnThicknessPropertyChanged));
        public static readonly DependencyProperty<Brush> BrushProperty = DependencyProperty.Register(nameof(Brush), typeof(Pen),
            new PropertyMetadata<Brush>(DependencyProperty.UnsetValue, OnBrushPropertyChanged));

        public float Thickness
        {
            get { return GetValue(ThicknessProperty); }
            set { this.SetLocalValue(ThicknessProperty, value); }
        }

        public Brush Brush
        {
            get { return GetValue(BrushProperty); }
            set { this.SetLocalValue(BrushProperty, value); }
        }

        private float _thickness;
        private Brush _brush;

        private readonly Lazy<IResource> _penResource;

        IResource IResourceProvider.Resource => _penResource.Value;

        public Pen()
        {
            _penResource = this.CreateResource(ResourceType.RT_Pen);
            OnThicknessChanged(Thickness);
            OnBrushChanged(Brush);
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            MediaResourceManager.Current.UpdatePen(_penResource.Value, _thickness, _brush);
        }

        private void OnBrushChanged(Brush brush)
        {
            _brush = brush;
            RegisterUpdateResource();
        }

        private void OnThicknessChanged(float thickness)
        {
            _thickness = thickness;
            RegisterUpdateResource();
        }

        private static void OnThicknessPropertyChanged(object sender, PropertyChangedEventArgs<float> e)
        {
            ((Pen)sender).OnThicknessChanged(e.NewValue);
        }

        private static void OnBrushPropertyChanged(object sender, PropertyChangedEventArgs<Brush> e)
        {
            ((Pen)sender).UpdateOnBrushUpdateResource(e.OldValue, e.NewValue);
            ((Pen)sender).OnBrushChanged(e.NewValue);
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
