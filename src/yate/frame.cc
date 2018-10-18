#include "frame.hh"

#include "utils.hh"

namespace yate {

Frame::Frame(std::shared_ptr<Frame> parent)
    : parent_(parent), nested_(), printable_values_(), iterable_values_() {
  if (parent != nullptr) {
    throw std::runtime_error("Initializing environment with no parent");
  }
}

Frame::Frame(
    std::unordered_map<std::string, std::string> printable_values,
    std::unordered_map<std::string, std::vector<std::string>> iterable_values)
    : parent_(),
      printable_values_(std::move(printable_values)),
      iterable_values_(std::move(iterable_values)) {}

std::string Frame::GetValue(const std::string& identifier) const {
  if (contains(printable_values_, identifier)) {
    return printable_values_.at(identifier);
  }
  auto parent = parent_.lock();
  if (parent == nullptr) {
    throw std::runtime_error("Unknown identifier '" + identifier + "'");
  }
  return parent->GetValue(identifier);
}

bool Frame::ContainsValue(const std::string& identifier) const {
  if (contains(printable_values_, identifier)) {
    return true;
  }
  auto parent = parent_.lock();
  return parent != nullptr && parent->ContainsValue(identifier);
}

bool Frame::PutValue(
    const std::string &identifier,
    const std::string &value) {
  if (contains(printable_values_, identifier)) {
    return false;
  }
  printable_values_[identifier] = value;
  return true;
}

const std::vector<std::string> &Frame::GetIterable(
    const std::string &identifier) const {
  if (contains(iterable_values_, identifier)) {
    return iterable_values_.at(identifier);
  }
  auto parent = parent_.lock();
  if (parent == nullptr) {
    throw std::runtime_error("Unknown identifier '" + identifier + "'");
  }
  return parent->GetIterable(identifier);
}

bool Frame::ContainsIterable(const std::string &identifier) const {
  if (contains(iterable_values_, identifier)) {
    return true;
  }
  auto parent = parent_.lock();
  return parent != nullptr && parent->ContainsIterable(identifier);
}

} // namespace yate
