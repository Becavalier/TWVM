// Copyright 2019 YHSPY. All rights reserved.
#include <algorithm>
#include <iomanip>
#include <numeric>
#include "lib/utility.h"
#include "lib/include/macros.h"

namespace twvm {
namespace internal {

#define BUILDSTAMP ( __DATE__ " " __TIME__ )

using ::std::max_element;
using ::std::left;
using ::std::setw;
using ::std::setfill;
using ::std::transform;
using ::std::accumulate;
using ::std::ostream;

shared_ptr<Printer> Printer::singleIns = nullptr;

vector<string> Utility::splitStr(const string &str, char delimiter) {
  vector<string> tokens;
  string token;
  istringstream tokenStream(str);
  while (getline(tokenStream, token, delimiter)) {
    tokens.push_back(token);
  }
  return tokens;
}

void Printer::printTableView() {
  if (lines.size() > 0) {
    // calculate column width;
    vector<size_t> columnWidth;
    vector<vector<string>> columnContent;
    for (const auto &line : lines) {
      const auto snippets = Utility::splitStr(line, '|');
      columnContent.push_back(snippets);
      if (columnWidth.size() == 0) {
        columnWidth = vector<size_t>(snippets.size(), 0);
        transform(begin(snippets), end(snippets), begin(columnWidth),
          [](const string &snippet) -> auto {
            return snippet.length();
          });
      } else {
        for (size_t i = 0; i < snippets.size(); i++) {
          const auto len = snippets[i].length();
          columnWidth[i] = columnWidth[i] < len ? len : columnWidth[i];
        }
      }
    }
    // calculate line width;
    const auto columnWidthSize = columnWidth.size();
    const auto maxLengthPadding =
      accumulate(begin(columnWidth), end(columnWidth), 0) + columnWidthSize + 3;
    INTERNAL_DEBUG_PREFIX_OUTPUT() << setw(maxLengthPadding) << setfill('-') << left << '|' << endl;
    for (size_t i = 0; i < lines.size(); i++) {
      INTERNAL_DEBUG_PREFIX_OUTPUT() << "| ";
      for (size_t j = 0; j < columnWidthSize; j++) {
        cout << setw(columnWidth[j]) << setfill(' ') << columnContent[i][j];
        if (j != columnWidthSize - 1) {
          cout << '|';
        }
      }
      cout << " |" << endl;
    }
    INTERNAL_DEBUG_PREFIX_OUTPUT() << setw(maxLengthPadding) << setfill('-') << left << '|' << endl;
    lines.clear();
  }
}

void Utility::drawLogoGraphic(bool simplify) {
  cout << \
"\n//////    //    //\n\
  // //  ////  ////\n\
  //  ////  ////  //\n\
  //   //    //    //\n\n";
#if defined(BUILD_VERSION)
  cout << "  V." << BUILD_VERSION << "\n\n";
#endif
  if (!simplify) {
    cout << "  Built: " << BUILDSTAMP << "\n\n";
  }
}

}  // namespace internal
}  // namespace twvm
