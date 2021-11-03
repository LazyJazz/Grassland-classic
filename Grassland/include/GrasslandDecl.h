#pragma once

#include <cstdint>

namespace Grassland
{
	typedef int32_t GRL_RESULT;
#define GRL_TRUE 1
#define GRL_FALSE 0

	class GRLIBase
	{
	public:
		virtual GRL_RESULT AddRef() = 0;
		virtual GRL_RESULT Release() = 0;
	};

	template<typename __Ty>
	class GRLPtr
	{
	public:
		GRLPtr();

		template<typename __Ty_other>
		GRLPtr(__Ty_other* pObject);
		
		template<typename __Ty_other>
		GRLPtr(GRLPtr<__Ty_other>& __Pnter_Source);

		GRLPtr(GRLPtr<__Ty>& __Pnter_Source)
		{
			__Pnter_Object = __Pnter_Source.__Pnter_Object;
			if (__Pnter_Object)
				__Pnter_Object->AddRef();
		}//*/

		GRLPtr<__Ty>& operator = (const GRLPtr<__Ty>& __Pnter_Source)
		{
			Set(__Pnter_Source.__Pnter_Object);
			return *this;
		}

		~GRLPtr();
		void Set(__Ty* pObject);
		void Reset();
		__Ty** GetAddressOf();
		__Ty* Get();
		__Ty** operator & ();
		__Ty* operator-> ();
	private:
		__Ty* __Pnter_Object;
	};

	enum GRL_TYPE
	{
		GRL_TYPE_INT8 = 0,
		GRL_TYPE_UNSIGNED_INT8,
		GRL_TYPE_INT32,
		GRL_TYPE_UNSIGNED_INT32,
		GRL_TYPE_INT64,
		GRL_TYPE_UNSIGNED_INT64,
		GRL_TYPE_FLOAT,
		GRL_TYPE_DOUBLE
	};

	typedef uint8_t Byte;
	typedef float float32;
	typedef double float64;
	template<typename __Ty>
	inline GRLPtr<__Ty>::GRLPtr()
	{
		__Pnter_Object = nullptr;
	}
	template<typename __Ty>
	template<typename __Ty_other>
	inline GRLPtr<__Ty>::GRLPtr(__Ty_other* pObject)
	{
		__Pnter_Object = pObject;
		if (pObject)
			__Pnter_Object->AddRef();
	}
	template<typename __Ty>
	inline GRLPtr<__Ty>::~GRLPtr()
	{
		__Pnter_Object->Release();
	}
	template<typename __Ty>
	inline void GRLPtr<__Ty>::Set(__Ty* pObject)
	{
		if (pObject == __Pnter_Object) return;
		if (__Pnter_Object) __Pnter_Object->Release();
		__Pnter_Object = pObject;
		if (__Pnter_Object) __Pnter_Object->AddRef();
	}
	template<typename __Ty>
	inline void GRLPtr<__Ty>::Reset()
	{
		Set(nullptr);
	}
	template<typename __Ty>
	inline __Ty** GRLPtr<__Ty>::GetAddressOf()
	{
		return &__Pnter_Object;
	}
	template<typename __Ty>
	inline __Ty* GRLPtr<__Ty>::Get()
	{
		return __Pnter_Object;
	}
	template<typename __Ty>
	inline __Ty** GRLPtr<__Ty>::operator&()
	{
		return &__Pnter_Object;
	}
	template<typename __Ty>
	inline __Ty* GRLPtr<__Ty>::operator->()
	{
		return __Pnter_Object;
	}
	template<typename __Ty>
	template<typename __Ty_other>
	inline GRLPtr<__Ty>::GRLPtr(GRLPtr<__Ty_other>& __Pnter_Source)
	{
		__Pnter_Object = __Pnter_Source.Get();
		if (__Pnter_Object) {
			__Pnter_Object->AddRef();
		}

	}
}