#include "frame.hh"

#include "utils.hh"

namespace yate {

Frame::Frame(
    std::shared_ptr<Frame> parent,
    std::string frame_id,
    std::string array_id,
    std::string item_id)
    : parent_(parent),
      nested_(),
      printable_values_(),
      iterable_values_(),
      type_(Token::Tag::eLoopBegin),
      id_(std::move(frame_id)),
      array_id_(std::move(array_id)),
      item_id_(std::move(item_id)) {
  if (parent == nullptr) {
    throw std::runtime_error("Initializing environment with no parent");
  }
}

Frame::Frame(
    std::unordered_map<std::string, std::string> printable_values,
    std::unordered_map<std::string, std::vector<std::string>> iterable_values)
    : parent_(),
      nested_(),
      printable_values_(std::move(printable_values)),
      iterable_values_(std::move(iterable_values)),
      type_(Token::Tag::eNoOp), // Root frame gets NoOp type.
      id_("root"),
      array_id_(),
      item_id_() {}

void Frame::AddNestedFrame(std::shared_ptr<Frame> frame) {
  if (contains(nested_, frame->id())) {
    throw std::runtime_error("Got invalid frame name");
  }
  nested_.insert({frame->id(), frame});
}

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
