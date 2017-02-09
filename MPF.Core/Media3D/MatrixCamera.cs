using MPF.Data;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;

namespace MPF.Media3D
{
    public class MatrixCamera : Camera
    {
        public static readonly DependencyProperty<Matrix4x4> ViewMatrixProperty = DependencyProperty.Register(nameof(ViewMatrix),
            typeof(MatrixCamera), new PropertyMetadata<Matrix4x4>(Matrix4x4.Identity, OnMatrixPropertyChanged));
        public static readonly DependencyProperty<Matrix4x4> ProjectionMatrixProperty = DependencyProperty.Register(nameof(ProjectionMatrix),
            typeof(MatrixCamera), new PropertyMetadata<Matrix4x4>(Matrix4x4.Identity, OnMatrixPropertyChanged));

        public Matrix4x4 ViewMatrix
        {
            get => GetValue(ViewMatrixProperty);
            set => this.SetLocalValue(ViewMatrixProperty, value);
        }

        public Matrix4x4 ProjectionMatrix
        {
            get => GetValue(ProjectionMatrixProperty);
            set => this.SetLocalValue(ProjectionMatrixProperty, value);
        }

        protected override Matrix4x4 GetViewMatrix() => ViewMatrix;
        protected override Matrix4x4 GetProjectionMatrix() => ProjectionMatrix;

        private static void OnMatrixPropertyChanged(object sender, PropertyChangedEventArgs<Matrix4x4> e)
        {
            ((MatrixCamera)sender).InvalidateMatrices();
        }
    }
}
