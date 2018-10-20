#pragma once

#include <iosfwd>
#include <string>
#include <unordered_map>
#include <vector>

namespace yate {

/// Interprest a template stored in an input stream and generates a
/// rendered results which is copied in the output stream. It uses
/// the values and arrays parameters to resolve the symbols which
/// appear in the template.
///
/// @param values These are strings that can directly be copied into
///        the output stream.
/// @param arrays These are the symbols which store vector which can
///        be used inside loops.
/// @param input The stream from which the template will be read.
/// @param output The stream where the rendered output will be stored.
void Render(
    const std::unordered_map<std::string, std::string> &values,
    const std::unordered_map<std::string, std::vector<std::string>> &arrays,
    std::istream &input,
    std::ostream &output);

} // namespace yate
