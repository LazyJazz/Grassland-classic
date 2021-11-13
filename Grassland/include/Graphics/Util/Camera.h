#pragma once
#include "../../Math/Math.h"
#include "../../Geometry/Geometry.h"
#include "Transform.h"


namespace Grassland
{
	namespace Graphics
	{
		namespace Util
		{
			class Camera
			{
			public:
				virtual GRLRay GenRay(float x, float y, float z) const = 0;
				virtual GRLRayd GenRay(double x, double y, double z) const = 0;
			};

			template<typename __Ty>
			class Coord
			{
			public:
				Coord();
				void Translate(__Ty x, __Ty y, __Ty z);
				void Rotation(__Ty pitch, __Ty yaw, __Ty roll);
				void LookAt(Math::Vector<3, __Ty> from, Math::Vector<3, __Ty> look_at);
				Math::Matrix<4, 4, __Ty> GetCoord() const;
			protected:
				Math::Matrix<4, 4, __Ty> __coord;
			};

			template<typename __Ty>
			class CameraNormal:public Camera, public Coord<__Ty>
			{
			public:
				CameraNormal(__Ty afov, __Ty aspect, __Ty Near, __Ty Far);
				virtual GRLRay GenRay(float x, float y, float z) const override;
				virtual GRLRayd GenRay(double x, double y, double z) const override;

				Math::Matrix<4, 4, __Ty> GetShaderMatrix() const;

				/*
				* @param afov: Field of view angle, how wide you can see vertically. Should be in (0, 2 * pi).
				* 
				* @param aspect: Should be set as screen_width / screen_height to get correct photo.
				* 
				* @param near: The near distance for clipping.
				* 
				* @param far: The far distance for clipping.
				*/
				void SetFOV(__Ty afov, __Ty aspect, __Ty Near, __Ty Far);

			private:
				__Ty __afov;
				__Ty __aspect;
				__Ty __near;
				__Ty __far;
				Math::Matrix<4, 4, __Ty> __project;
			};
			template<typename __Ty>
			inline Coord<__Ty>::Coord()
			{
				Coord<__Ty>::__coord = Math::Matrix<4, 4, __Ty>(1.0);
			}

			template<typename __Ty>
			inline void Coord<__Ty>::Translate(__Ty x, __Ty y, __Ty z)
			{
				__coord = __coord * GRLTransformTranslate(x, y, z);
			}

			template<typename __Ty>
			inline void Coord<__Ty>::Rotation(__Ty pitch, __Ty yaw, __Ty roll)
			{
				__coord = __coord * GRLTransformRotation(pitch, yaw, roll);
			}
			template<typename __Ty>
			inline void Coord<__Ty>::LookAt(Math::Vector<3, __Ty> from, Math::Vector<3, __Ty> look_at)
			{
				__coord = GRLTransformLookAt(from, look_at);
			}
			template<typename __Ty>
			inline Math::Matrix<4, 4, __Ty> Coord<__Ty>::GetCoord() const
			{
				return __coord;
			}
			template<typename __Ty>
			inline CameraNormal<__Ty>::CameraNormal(__Ty afov, __Ty aspect, __Ty Near, __Ty Far)
			{
				SetFOV(afov, aspect, Near, Far);
			}

			template<typename __Ty>
			inline GRLRay CameraNormal<__Ty>::GenRay(float x, float y, float z) const
			{
				y *= __afov;
				x *= __afov * __aspect;
				z = 1.0;
				GRLVec3 d(x, y, z);
				d = d / d.norm();
				return GRLRay(GRLVec3(
					Coord<__Ty>::__coord[0][3],
					Coord<__Ty>::__coord[1][3],
					Coord<__Ty>::__coord[2][3]
				), d);
			}
			
			template<typename __Ty>
			inline GRLRayd CameraNormal<__Ty>::GenRay(double x, double y, double z) const
			{
				y *= __afov;
				x *= __afov * __aspect;
				z = 1.0;
				GRLVec3d d(x, y, z);
				d = d / d.norm();
				return GRLRayd(GRLVec3d(
					Coord<__Ty>::__coord[0][3],
					Coord<__Ty>::__coord[1][3],
					Coord<__Ty>::__coord[2][3]
				), d);
			}

			template<typename __Ty>
			inline Math::Matrix<4, 4, __Ty> CameraNormal<__Ty>::GetShaderMatrix() const
			{
				return __project * Coord<__Ty>::__coord.inverse();
			}
			template<typename __Ty>
			inline void CameraNormal<__Ty>::SetFOV(__Ty afov, __Ty aspect, __Ty Near, __Ty Far)
			{
				__afov = tan(afov * 0.5);
				__aspect = aspect;
				__near = Near;
				__far = Far;
				__project = GRLTransformProjection(afov, aspect, Near, Far);
			}
		}
	}
	typedef Graphics::Util::CameraNormal<float> GRLCameraNormal;
	typedef Graphics::Util::CameraNormal<double> GRLCameraNormald;
}

#include "Camera.inl"