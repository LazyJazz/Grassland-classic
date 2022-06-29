#include "String.h"

namespace Grassland {
int32_t GRLStringUTF8toUnicode(const char *utf8_str, wchar_t *unicode_str) {
  if (!utf8_str) {
    GRLSetErrorInfo("GRL String UTF8_to_Unicode, utf8_str is nullptr.");
    return -1;
  }
  int32_t ret = 0;
  int32_t len = strlen(utf8_str);

  if (unicode_str) {
    for (int i = 0; i < len; i++) {
      uint8_t c = utf8_str[i];
      if ((c & 0b11000000u) == 0b10000000u) {
        unicode_str[ret - 1] <<= 6;
        unicode_str[ret - 1] |= c & 0b00111111u;
      } else {
        uint8_t mask = 0b10000000u;
        while (mask & c) {
          c &= ~mask;
          mask >>= 1;
        }
        unicode_str[ret] = c;
        ret++;
      }
    }
    unicode_str[ret] = 0;
  } else {
    for (int i = 0; i < len; i++) {
      uint8_t c = utf8_str[i];
      if ((c & 0b11000000u) != 0b10000000u)
        ret++;
    }
  }
  return ret;
}

std::wstring GRLStringUTF8toUnicode(std::string utf8_str) {
  std::wstring result = L"";
  int32_t len = 0;
  for (uint8_t c : utf8_str) {
    if ((c & 0b11000000u) == 0b10000000u) {
      result[len - 1] <<= 6;
      result[len - 1] |= c & 0b00111111u;
    } else {
      uint8_t mask = 0b10000000u;
      while (mask & c) {
        c &= ~mask;
        mask >>= 1;
      }
      result += c;
      len++;
    }
  }
  return result;  // +L'\0\0';
}
std::string GRLStringUnicodetoUTF8(std::wstring unicode_str) {
  std::string res = "";
  for (wchar_t c : unicode_str) {
    if (c & 0b1111111110000000) {
      if (c & 0b1111100000000000) {
        res += (char)(((c >> 12) & 0b00001111) | 0b11100000);
        res += (char)(((c >> 6) & 0b00111111) | 0b10000000);
        res += (char)((c & 0b00111111) | 0b10000000);
      } else {
        res += (char)(((c >> 6) & 0b00011111) | 0b11000000);
        res += (char)((c & 0b00111111) | 0b10000000);
      }
    } else {
      res += (char)(c & 0b01111111);
    }
  }
  return res;  // +'\0';
}
}  // namespace Grassland
