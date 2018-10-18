#pragma once

#include "token.hh"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace yate {

class Environment : public std::enable_shared_from_this<Environment> {
 public:
  explicit Environment(std::shared_ptr<Environment> parent);
  explicit Environment(
      std::unordered_map<std::string, std::string> printable_values,
      std::unordered_map<std::string, std::vector<std::string>>
          iterable_values);
  ~Environment() {}

  std::string GetValue(const std::string& identifier) const;
  bool ContainsValue(const std::string& identifier) const;
  bool PutValue(const std::string &identifier, const std::string &value);

  std::vector<Token> &tokens() { return tokens_; }

  const std::vector<std::string> &GetIterable(
      const std::string &identifier) const;
  bool ContainsIterable(const std::string &identifier) const;

 private:
  std::weak_ptr<Environment> parent_;
  std::unordered_map<std::string, std::shared_ptr<Environment>> nested_;
  std::unordered_map<std::string, std::string> printable_values_;
  std::unordered_map<std::string, std::vector<std::string>> iterable_values_;
  std::vector<Token> tokens_;
};

} // namespace yate
