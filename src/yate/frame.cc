#include "frame.hh"

#include "utils.hh"

namespace yate {

Frame::Frame(std::shared_ptr<Frame> parent, std::string id)
    : parent_(parent),
      printable_values_(),
      iterable_values_(),
      id_(std::move(id)) {
  if (parent == nullptr) {
    throw std::runtime_error("Initializing environment with no parent");
  }
}

Frame::Frame(
    std::unordered_map<std::string, std::string> printable_values,
    std::unordered_map<std::string, std::vector<std::string>> iterable_values)
    : parent_(),
      printable_values_(std::move(printable_values)),
      iterable_values_(std::move(iterable_values)),
      id_("root") {
}

std::string Frame::GetValue(const std::string& identifier) const {
  if (contains(printable_values_, identifier)) {
    return printable_values_.at(identifier);
  }
  if (parent_ == nullptr) {
    throw std::runtime_error("Unknown identifier '" + identifier + "'");
  }
  return parent_->GetValue(identifier);
}

bool Frame::ContainsValue(const std::string& identifier) const {
  if (contains(printable_values_, identifier)) {
    return true;
  }
  return parent_ != nullptr && parent_->ContainsValue(identifier);
}

void Frame::PutValue(
    const std::string &identifier,
    const std::string &value) {
  printable_values_[identifier] = value;
}

const std::vector<std::string> &Frame::GetIterable(
    const std::string &identifier) const {
  if (contains(iterable_values_, identifier)) {
    return iterable_values_.at(identifier);
  }
  if (parent_ == nullptr) {
    throw std::runtime_error("Unknown identifier '" + identifier + "'");
  }
  return parent_->GetIterable(identifier);
}

bool Frame::ContainsIterable(const std::string &identifier) const {
  if (contains(iterable_values_, identifier)) {
    return true;
  }

  return parent_ != nullptr && parent_->ContainsIterable(identifier);
}

} // namespace yate
