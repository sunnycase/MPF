using MPF.Data;
using MPF.Interop;
using MPF.Media;
using System;
using System.Collections.Generic;
using System.Numerics;
using System.Text;

namespace MPF.Media3D
{
    public abstract class Camera : Animatable, IResourceProvider
    {
        private Matrix4x4 _viewMatrix;
        private Matrix4x4 _projectionMatrix;
        private readonly Lazy<IResource> _cameraResource;

        public Camera()
        {
            _cameraResource = this.CreateResource(ResourceType.RT_Camera);
            InvalidateMatrices();
        }

        protected void InvalidateMatrices()
        {
            _viewMatrix = GetViewMatrix();
            _projectionMatrix = GetProjectionMatrix();

            RegisterUpdateResource();
        }

        internal override void OnUpdateResource(object sender, EventArgs e)
        {
            base.OnUpdateResource(sender, e);
            MediaResourceManager.Current.UpdateCamera(_cameraResource.Value, ref _viewMatrix, ref _projectionMatrix);
        }

        protected abstract Matrix4x4 GetViewMatrix();
        protected abstract Matrix4x4 GetProjectionMatrix();

        IResource IResourceProvider.Resource => _cameraResource.Value;

        private static void OnMatrixPropertyChanged(object sender, PropertyChangedEventArgs<Matrix4x4> e)
        {
            ((Camera)sender).InvalidateMatrices();
        }
    }
}
