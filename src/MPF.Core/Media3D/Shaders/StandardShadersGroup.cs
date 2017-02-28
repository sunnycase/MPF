using MPF.Data;
using MPF.Media;
using MPF.Properties;
using System;
using System.Collections.Generic;
using System.Text;

namespace MPF.Media3D.Shaders
{
    public sealed class StandardShaderParameters : ShaderParameters
    {
        public static readonly DependencyProperty<Brush> MainTextureProperty = DependencyProperty.Register(
            nameof(MainTexture), typeof(StandardShaderParameters), new PropertyMetadata<Brush>(null));

        public Brush MainTexture
        {
            get => GetValue(MainTextureProperty);
            set => this.SetLocalValue(MainTextureProperty, value);
        }

        protected override void OnWriteParameters(ShaderParametersWriteContext context)
        {
            var mainTexture = MainTexture;
            if (mainTexture != null)
                context.Write(MainTexture);
        }
    }

    public sealed class StandardShadersGroup : ShadersGroup<StandardShaderParameters>
    {
        protected override (byte[] vertexShader, byte[] pixelShader) GetShaderBytesCode()
        {
            return (Resource.D3D11_3D_Fill_VertexShader, Resource.D3D11_3D_Fill_PixelShader);
        }
    }
}
