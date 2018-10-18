#pragma once

#include "token.hh"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace yate {

class Frame : public std::enable_shared_from_this<Frame> {
 public:
  explicit Frame(
      std::shared_ptr<Frame> parent,
      std::string frame_id,
      std::string array_id,
      std::string item_id);
  explicit Frame(
      std::unordered_map<std::string, std::string> printable_values,
      std::unordered_map<std::string, std::vector<std::string>>
          iterable_values);
  ~Frame() {}

  std::weak_ptr<Frame> parent() const { return parent_; }
  std::string id() const { return id_; }
  std::string array_id() const { return array_id_; }
  std::string item_id() const { return item_id_; }

  Token::Tag type() const { return type_; }

  void AddNestedFrame(std::shared_ptr<Frame> frame);

  std::string GetValue(const std::string& identifier) const;
  bool ContainsValue(const std::string& identifier) const;
  bool PutValue(const std::string &identifier, const std::string &value);

  std::vector<Token> &tokens() { return tokens_; }

  const std::vector<std::string> &GetIterable(
      const std::string &identifier) const;
  bool ContainsIterable(const std::string &identifier) const;

 private:
  std::weak_ptr<Frame> parent_;
  std::unordered_map<std::string, std::shared_ptr<Frame>> nested_;
  std::unordered_map<std::string, std::string> printable_values_;
  std::unordered_map<std::string, std::vector<std::string>> iterable_values_;
  std::vector<Token> tokens_;
  Token::Tag type_;
  const std::string id_;
  const std::string array_id_;
  const std::string item_id_;
};

} // namespace yate
