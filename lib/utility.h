// Copyright 2019 YHSPY. All rights reserved.
#ifndef LIB_UTILITY_H_
#define LIB_UTILITY_H_

#include <ios>
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <limits>
#include "lib/config.h"
#include "lib/common/constants.h"
#include "lib/common/errors.h"

#define OUTPUT_PREFIX "twvm: "
// ANSI escape code (Colors);
#define COLOR_CTL_NORMAL "\x1b[37;40m"
#define INTERNAL_DEBUG_PREFIX_OUTPUT() \
  cout << OUTPUT_PREFIX << "\x1b[36;40m" << "info: " << COLOR_CTL_NORMAL
#define INTERNAL_WARNING_PREFIX_OUTPUT() \
  cout << OUTPUT_PREFIX << "\x1b[93;40m" << "warning: " << COLOR_CTL_NORMAL
#define INTERNAL_ERROR_PREFIX_OUTPUT() \
  cout << OUTPUT_PREFIX << "\x1b[91;40m" << "error: " << COLOR_CTL_NORMAL
#define INTERNAL_SAY_PREFIX_OUTPUT() \
  cout << OUTPUT_PREFIX << "\x1b[92;40m" << "out: " << COLOR_CTL_NORMAL

using std::runtime_error;
using std::string;
using std::vector;
using std::shared_ptr;
using std::make_shared;
using std::cout;
using std::endl;
using std::stringstream;
using std::istringstream;
using std::memcpy;
using std::hex;
using std::showbase;
using std::to_string;
using std::isnan;
using std::signbit;
using std::numeric_limits;

// singleton instance;
class Printer {
 private:
  vector<string> lines;
  stringstream ss;
  static shared_ptr<Printer> singleIns;

 public:
  static Printer& instance() {
    if (!singleIns) {
      singleIns = make_shared<Printer>();
    }
    return *singleIns;
  }
  template <typename T>
  Printer& operator << (const T &x) {
    ss << x;
    return *this;
  }
  inline Printer& useHexFormat() {
    ss << hex << showbase;
    return *this;
  }
  inline void debug() {
    if (!Config::isDebugMode) {
      ss.str(string());
      return;
    }
    INTERNAL_DEBUG_PREFIX_OUTPUT() << ss.str();
    ss.str(string());
  }
  inline void say() {
    INTERNAL_SAY_PREFIX_OUTPUT() << ss.str();
    ss.str(string());
  }
  inline void warn() {
    INTERNAL_WARNING_PREFIX_OUTPUT() << ss.str();
    ss.str(string());
  }
  inline void error(Errors code, bool throwException = true) {
    ss << errorMapper[code] << '\n';
    INTERNAL_ERROR_PREFIX_OUTPUT() << ss.str();
    ss.str(string());
    if (throwException) {
      throw runtime_error('(' + to_string(static_cast<uint8_t>(code)) + ')');
    }
  }
  inline void feedLine(const string &line) {
    lines.push_back(line);
  }
  inline void makeLine() {
    lines.push_back(ss.str());
    ss.str(string());
  }
  inline void makeLine(stringstream &ss) {
    lines.push_back(ss.str());
    ss.str(string());
  }
  void printTableView();
};

class Utility {
 public:
  static vector<string> splitStr(const string&, char);

  template <typename T>
  static inline void writeUnalignedValue(uintptr_t p, T value) {
    memcpy(reinterpret_cast<void*>(p), &value, sizeof(T));
  }

  template <typename T>
  static inline T readUnalignedValue(uintptr_t p) {
    T r;
    memcpy(&r, reinterpret_cast<void*>(p), sizeof(T));
    return r;
  }

  template <typename T>
  static inline void savePtrIntoBytes(vector<uint8_t> *v, T *ptr) {
    const auto ptrVal = reinterpret_cast<uintptr_t>(ptr);
    for (auto i = 0; i < PTR_SIZE; i++) {
      v->push_back(ptrVal >> (PTR_SIZE * i) & 0x000000ff);
    }
  }

  template <typename T>
  static inline T max(T x, T y) {
    if (isnan(x)) { return x; }
    if (isnan(y)) { return y; }
    if (signbit(x) < signbit(y)) { return x; }
    return x > y ? x : y;
  }

  template <typename T>
  static inline T min(T x, T y) {
    if (isnan(x)) { return x; }
    if (isnan(y)) { return y; }
    if (signbit(x) < signbit(y)) { return y; }
    return x > y ? y : x;
  }

  static inline float double64ToFloat32(double x) {
    using limits = numeric_limits<float>;
    static const double kRoundingThreshold = 3.4028235677973362e+38;
    if (x > limits::max()) {
      if (x <= kRoundingThreshold) {
        return limits::max();
      } else {
        return limits::infinity(); 
      }
    } else if (x < limits::lowest()) {
      if (x >= -kRoundingThreshold) {
        return limits::lowest();
      } else {
        return -limits::infinity();
      }
    }
    return static_cast<float>(x);
  };
  static void drawLogoGraphic(bool = true);
};

#endif  // LIB_UTILITY_H_
