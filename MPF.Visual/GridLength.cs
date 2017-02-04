using System;
using System.Collections.Generic;
using System.Text;

namespace MPF
{
    public enum GridUnitType
    {
        Auto,
        Pixel,
        Star
    }

    public struct GridLength
    {
        public float Value { get; }
        public GridUnitType UnitType { get; }

        public static readonly GridLength Auto = new GridLength(1.0f, GridUnitType.Auto);

        public GridLength(float value, GridUnitType unitType = GridUnitType.Pixel)
        {
            Value = value;
            UnitType = unitType;
        }

        public static implicit operator GridLength(float value)
        {
            return new GridLength(value);
        }
    }
}
