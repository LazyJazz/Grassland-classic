#pragma once

#include <cctype>
#include <cstdint>
#include <cstdio>
#include <iomanip>
#include <iostream>

#define GRLSetErrorInfo puts

namespace Grassland {
typedef int32_t GRL_RESULT;
#define GRL_TRUE 1
#define GRL_FALSE 0

class GRLUUID {
 public:
  GRLUUID();
  GRLUUID(const char *uuid_str);
  bool operator==(const GRLUUID &__another_uuid) const;
  friend std::ostream &operator<<(std::ostream &os, const GRLUUID &uuid);

 private:
  union {
    uint8_t __uuid_bytes[16];
    uint32_t __uuid_int32[4];
    uint64_t __uuid_int64[2];
  };
};

template <typename GRLInterfaceType, typename keepTemplate>
GRLUUID GRLGetUUID(const GRLInterfaceType *);
template <typename GRLInterfaceType, typename keepTemplate>
GRLUUID GRLGetUUID(const GRLInterfaceType *) {
  return GRLUUID("00000000-0000-0000-0000-000000000000");
}

class GRLIBase;

#define GRLMakeObjectUUIDAssociate(ObjectName, GRLID_Name, UUID) \
  const GRLUUID GRLID_Name = GRLUUID(UUID);                      \
  template <typename keepTemplate>                               \
  GRLUUID GRLGetUUID(const ObjectName *) {                       \
    return GRLID_Name;                                           \
  }

#define GRLDeclareObject(ObjectSubname, UUID) \
  class GRL##ObjectSubname;                   \
  GRLMakeObjectUUIDAssociate(GRL##ObjectSubname, GRLID_##ObjectSubname, UUID);

#define GRLID_PPV_ARGS(x) GRLGetUUID<void>(*(x)), reinterpret_cast<void **>(x)

GRLMakeObjectUUIDAssociate(GRLIBase,
                           GRLID_IBase,
                           "53386c00-4617-4105-b7cb-caccfcf34848");

class GRLIBase {
 public:
  virtual GRL_RESULT AddRef() = 0;
  virtual GRL_RESULT Release() = 0;
  virtual GRL_RESULT QueryInterface(GRLUUID Uuid, void **ppObject) = 0;
};

template <typename __Ty>
class GRLPtr {
 public:
  GRLPtr();

  template <typename __Ty_other>
  GRLPtr(__Ty_other *pObject);

  template <typename __Ty_other>
  GRLPtr(GRLPtr<__Ty_other> &__Pnter_Source);

  GRLPtr(GRLPtr<__Ty> &__Pnter_Source) {
    __Pnter_Object = __Pnter_Source.__Pnter_Object;
    if (__Pnter_Object)
      __Pnter_Object->AddRef();
  }

  GRLPtr<__Ty> &operator=(const GRLPtr<__Ty> &__Pnter_Source) {
    Set(__Pnter_Source.__Pnter_Object);
    return *this;
  }

  ~GRLPtr();
  void Set(__Ty *pObject);
  void Reset();
  __Ty **GetAddressOf();
  __Ty *Get();
  __Ty **operator&();
  __Ty *operator->();

 private:
  __Ty *__Pnter_Object;
};

typedef uint32_t GRL_TYPE;
#define GRL_TYPE_INT8 0
#define GRL_TYPE_UNSIGNED_INT8 1
#define GRL_TYPE_INT32 2
#define GRL_TYPE_UNSIGNED_INT32 3
#define GRL_TYPE_INT64 4
#define GRL_TYPE_UNSIGNED_INT64 5
#define GRL_TYPE_FLOAT 6
#define GRL_TYPE_DOUBLE 7

typedef uint8_t Byte;
typedef float float32;
typedef double float64;
template <typename __Ty>
inline GRLPtr<__Ty>::GRLPtr() {
  __Pnter_Object = nullptr;
}
template <typename __Ty>
template <typename __Ty_other>
inline GRLPtr<__Ty>::GRLPtr(__Ty_other *pObject) {
  __Pnter_Object = pObject;
  if (pObject)
    __Pnter_Object->AddRef();
}
template <typename __Ty>
inline GRLPtr<__Ty>::~GRLPtr() {
  if (__Pnter_Object)
    __Pnter_Object->Release();
}
template <typename __Ty>
inline void GRLPtr<__Ty>::Set(__Ty *pObject) {
  if (pObject == __Pnter_Object)
    return;
  if (__Pnter_Object)
    __Pnter_Object->Release();
  __Pnter_Object = pObject;
  if (__Pnter_Object)
    __Pnter_Object->AddRef();
}
template <typename __Ty>
inline void GRLPtr<__Ty>::Reset() {
  Set(nullptr);
}
template <typename __Ty>
inline __Ty **GRLPtr<__Ty>::GetAddressOf() {
  return &__Pnter_Object;
}
template <typename __Ty>
inline __Ty *GRLPtr<__Ty>::Get() {
  return __Pnter_Object;
}
template <typename __Ty>
inline __Ty **GRLPtr<__Ty>::operator&() {
  return &__Pnter_Object;
}
template <typename __Ty>
inline __Ty *GRLPtr<__Ty>::operator->() {
  return __Pnter_Object;
}
template <typename __Ty>
template <typename __Ty_other>
inline GRLPtr<__Ty>::GRLPtr(GRLPtr<__Ty_other> &__Pnter_Source) {
  __Pnter_Object = __Pnter_Source.Get();
  if (__Pnter_Object) {
    __Pnter_Object->AddRef();
  }
}
}  // namespace Grassland
