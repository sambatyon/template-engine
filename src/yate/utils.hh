#pragma once

/// Small helper function template which works as syntax candy for
/// verification of element existance within a container.
/// The container needs to support the `.find()` method which limists
/// its use to `std::map`s and `std::set`s like containers.
///
/// @param container The container to be searched for an item.
/// @param elem The element to be searched for.
/// @returns `false` if `container.find(elem)` returns
///        `std::end(container)`.
template<template<typename...> class Container, typename Key, typename...Us>
inline
bool contains(const Container<Key, Us...> &container, const Key &elem) {
  return container.find(elem) != container.end();
}

/// Small helper function template used to verify if `prefix` is a
/// proper prefix of the `reference` container.
///
/// Note that both containers need to be of the same type and their
/// elements need to sopport the equality operator.
///
/// @param reference The container to be checked for a prefix.
/// @param prefix The expected prefix.
/// @return `true` if `prefix` is a proper prefix of `reference`.
template<typename Container>
inline
bool begins_with(const Container &reference, const Container &prefix) {
  return reference.size() >= prefix.size() &&
         equal(prefix.begin(), prefix.end(), reference.begin());
}
