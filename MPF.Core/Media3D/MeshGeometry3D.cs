using MPF.Data;
using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Runtime.InteropServices;
using System.Text;

namespace MPF.Media3D
{
    public class MeshGeometry3D : Geometry3D
    {
        public static readonly DependencyProperty<FreezableCollection<Point3D>> PositionsProperty = DependencyProperty.Register(nameof(Positions),
            typeof(MeshGeometry3D), new PropertyMetadata<FreezableCollection<Point3D>>(FreezableCollection<Point3D>.Empty, OnPositionsPropertyChanged));
        public static readonly DependencyProperty<FreezableCollection<Vector3>> NormalsProperty = DependencyProperty.Register(nameof(Normals),
            typeof(MeshGeometry3D), new PropertyMetadata<FreezableCollection<Vector3>>(FreezableCollection<Vector3>.Empty, OnNormalsPropertyChanged));
        public static readonly DependencyProperty<FreezableCollection<Point>> TextureCoordinatesProperty = DependencyProperty.Register(nameof(TextureCoordinates),
            typeof(MeshGeometry3D), new PropertyMetadata<FreezableCollection<Point>>(FreezableCollection<Point>.Empty, OnTextureCoordinatesPropertyChanged));
        public static readonly DependencyProperty<FreezableCollection<uint>> IndicesProperty = DependencyProperty.Register(nameof(Indices),
            typeof(MeshGeometry3D), new PropertyMetadata<FreezableCollection<uint>>(FreezableCollection<uint>.Empty, OnIndicesPropertyChanged));

        public FreezableCollection<Point3D> Positions
        {
            get => GetValue(PositionsProperty);
            set => this.SetLocalValue(PositionsProperty, value);
        }

        public FreezableCollection<Vector3> Normals
        {
            get => GetValue(NormalsProperty);
            set => this.SetLocalValue(NormalsProperty, value);
        }

        public FreezableCollection<Point> TextureCoordinates
        {
            get => GetValue(TextureCoordinatesProperty);
            set => this.SetLocalValue(TextureCoordinatesProperty, value);
        }

        public FreezableCollection<uint> Indices
        {
            get => GetValue(IndicesProperty);
            set => this.SetLocalValue(IndicesProperty, value);
        }

        private readonly Lazy<IResource> _geometryRes;

        public MeshGeometry3D()
        {
            _geometryRes = this.CreateResource(ResourceType.RT_MeshGeometry3D);
            RegisterUpdateResource();
        }

        private static void OnPositionsPropertyChanged(object sender, PropertyChangedEventArgs<FreezableCollection<Point3D>> e)
        {
            ((MeshGeometry3D)sender).RegisterUpdateResource();
        }

        private static void OnNormalsPropertyChanged(object sender, PropertyChangedEventArgs<FreezableCollection<Vector3>> e)
        {
            ((MeshGeometry3D)sender).RegisterUpdateResource();
        }

        private static void OnTextureCoordinatesPropertyChanged(object sender, PropertyChangedEventArgs<FreezableCollection<Point>> e)
        {
            ((MeshGeometry3D)sender).RegisterUpdateResource();
        }

        private static void OnIndicesPropertyChanged(object sender, PropertyChangedEventArgs<FreezableCollection<uint>> e)
        {
            ((MeshGeometry3D)sender).RegisterUpdateResource();
        }

        internal unsafe override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);

            var positions = Positions.ToArray();
            var normals = Normals.ToArray();
            var texCoords = TextureCoordinates.ToArray();
            var indices = Indices.ToArray();

            fixed (Point3D* positionsPtr = positions)
            fixed (Vector3* normalsPtr = normals)
            fixed (Point* texCoordsPtr = texCoords)
            fixed (uint* indicesPtr = indices)
            {
                var data = new MeshGeometry3DData
                {
                    Positions = positionsPtr,
                    PositionsCount = (uint)positions.Length,
                    Normals = normalsPtr,
                    NormalsCount = (uint)normals.Length,
                    TextureCoordinates = texCoordsPtr,
                    TextureCoordinatesCount = (uint)texCoords.Length,
                    Indices = indicesPtr,
                    IndicesCount = (uint)indices.Length
                };
                MediaResourceManager.Current.UpdateMeshGeometry3D(_geometryRes.Value, ref data);
            }
        }

        internal override IResource GetResourceOverride() => _geometryRes.Value;
    }
}
