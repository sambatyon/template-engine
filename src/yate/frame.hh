#pragma once

#include "token.hh"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace yate {

/// Helper class used for symbol resolution. Mostly based on how
/// compilers handle scopes although the use in a template engine is
/// much more reduced.
/// This class allows loops to redefine symbols which will only have
/// scope life withing the loop.
/// It supports also redefinition of iterable symbols.
class Frame : public std::enable_shared_from_this<Frame> {
 public:
  /// These constructor is used every time a new scope is necessary.
  /// This constructor does not set any symbols by default so any
  /// lookup will be deferred to `parent`.
  ///
  /// @param parent The frame to withc symbol lookup will be deferred
  ///        to if the symbol itself is not found it the created
  ///        frame.
  /// @param id The frame id.
  Frame(std::shared_ptr<Frame> parent, std::string id);

  /// Constructor used only for the root Frame, i.e. the initial top
  /// most Frame. It is initialized with the symbols given. Frames
  /// created with this constructor will always get the id "root".
  ///
  /// @param printable_values A map of strings indexed by strings. The
  ///        key is the identifier while the value is the one that
  ///        will be printed each time an identifier is used.
  /// @param iterable_values These are the values that will be printed
  ///        in loops. Also a map addressable by string keys.
  Frame(
      std::unordered_map<std::string, std::string> printable_values,
      std::unordered_map<std::string, std::vector<std::string>>
          iterable_values);
  ~Frame() {}

  // Not default constructable, nor copiable nor movable.
  Frame() = delete;
  Frame(const Frame &) = delete;
  Frame(const Frame &&) = delete;

  // Getters.
  std::shared_ptr<Frame> parent() const { return parent_; }
  std::string id() const { return id_; }

  /// Search and returns the value associated with the given
  /// symbol. If not found in the current `Frame` it will perform the
  /// lookup in the parent frame.  If the symbol is not found in the
  /// top-most Frame, a `std::runtime_error` is thrown.
  ///
  /// @param identifier The symbol name to be look for.
  /// @return The value associated with the given symbol.
  std::string GetValue(const std::string& identifier) const;

  /// Search if a value is stored for the given symbol in either the
  /// current frame; or, if not found there, on the parent frame.
  /// If not found in any frame up to the top-most one, false is
  /// returned.
  ///
  /// @param identifier The symbol name to be look for.
  /// @return true if the symbol is found either in the current frame
  ///        or in any frame up the stack.
  bool ContainsValue(const std::string& identifier) const;

  /// Associates an identifier with a given value. This value only
  /// persists inside the current frame and can shadow values set
  /// in ancestor frames.
  ///
  /// @param identifier The symbol to be associated with the given
  ///        value.
  /// @param value The value associated with the given symbol
  ///        identifier.
  void PutValue(const std::string &identifier, const std::string &value);

  /// Search and returns the vector associated with the given
  /// symbol. If not found in the current `Frame` it will perform the
  /// lookup in the parent frame.  If the symbol is not found in the
  /// top-most Frame, a `std::runtime_error` is thrown.
  ///
  /// @param identifier The symbol name to be look for.
  /// @return The vectorassociated with the given symbol.
  const std::vector<std::string> &GetIterable(
      const std::string &identifier) const;

  /// Search if an array is stored for the given symbol in either the
  /// current frame; or, if not found there, on the parent frame.
  /// If not found in any frame up to the top-most one, false is
  /// returned.
  ///
  /// @param identifier The symbol name to be look for.
  /// @return true if the symbol is found either in the current frame
  ///        or in any frame up the stack.
  bool ContainsIterable(const std::string &identifier) const;

 private:
  std::shared_ptr<Frame> parent_;
  std::unordered_map<std::string, std::string> printable_values_;
  std::unordered_map<std::string, std::vector<std::string>> iterable_values_;
  std::string id_;
};

} // namespace yate
