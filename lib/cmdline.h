// Copyright 2019 YHSPY. All rights reserved.
#ifndef CMDLINE_H_
#define CMDLINE_H_

#include <functional>
#include <cassert>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace twvm {
namespace internal {
  
using ::std::function;
using ::std::string;
using ::std::vector;

class Options {
 public:
  using actionT = function<void(Options*, const string&)>;
  enum class Arguments { Zero, One, N, Optional };

  Options(const string&, const string&);
  Options& add(const string&, const string&, const string&, Arguments, const actionT&);
  Options& addPositional(const string&, Arguments, const actionT&);
  // entry point;
  void parse(int argc, const char *argv[]);

 private:
  struct Option {
    string longName;
    string shortName;
    string description;
    Arguments arguments;
    actionT action;
    size_t seen;
  };
  vector<Option> options;
  Arguments positional;
  string positionalName;
  actionT positionalAction;
};

struct CommandLine {
  static bool isDebugMode;
  static string executeModulePath;
};

}  // namespace internal
}  // namespace twvm

#endif  // CMDLINE_H_
