#pragma once

template<template<typename...> class Container, typename Key, typename...Us>
inline bool contains(const Container<Key, Us...> &container, const Key &elem) {
  return container.find(elem) != container.end();
}

template<typename Container>
bool begins_with(const Container &input, const Container &match) {
  return input.size() >= match.size() &&
         equal(match.begin(), match.end(), input.begin());
}
