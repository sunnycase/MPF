using MPF.Data;
using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Media3D
{
    public class Material : Animatable
    {
        public static readonly DependencyProperty<ShadersGroup> ShaderProperty = DependencyProperty.Register(nameof(Shader),
            typeof(Material), new PropertyMetadata<ShadersGroup>(null, OnShaderPropertyChanged));

        public ShadersGroup Shader
        {
            get => GetValue(ShaderProperty);
            set => this.SetLocalValue(ShaderProperty, value);
        }

        internal readonly Lazy<IResource> _materialRes;

        internal Material()
        {
            _materialRes = this.CreateResource(ResourceType.RT_Material);
            RegisterUpdateResource();
        }

        private static void OnShaderPropertyChanged(object sender, PropertyChangedEventArgs<ShadersGroup> e)
        {
            ((Material)sender).RegisterUpdateResource();
        }
    }

    public class Material<T> : Material where T : ShaderParameters
    {
        public static readonly DependencyProperty<T> ParametersProperty = DependencyProperty.Register(nameof(Parameters),
            typeof(Material<T>), new PropertyMetadata<T>(null, OnParametersPropertyChanged));

        public T Parameters
        {
            get => GetValue(ParametersProperty);
            set => this.SetLocalValue(ParametersProperty, value);
        }

        private static void OnParametersPropertyChanged(object sender, PropertyChangedEventArgs<T> e)
        {
            ((Material<T>)sender).RegisterUpdateResource();
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            
        }
    }
}
