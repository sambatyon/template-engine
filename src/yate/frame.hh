#pragma once

#include "token.hh"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace yate {

class Frame : public std::enable_shared_from_this<Frame> {
 public:
  explicit Frame(std::shared_ptr<Frame> parent);
  explicit Frame(
      std::unordered_map<std::string, std::string> printable_values,
      std::unordered_map<std::string, std::vector<std::string>>
          iterable_values);
  ~Frame() {}

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
};

} // namespace yate
