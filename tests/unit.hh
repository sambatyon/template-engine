#pragma once

#include <regex>
#include <string>

#define TEST_EXPECT(expectation)                                          \
  do {                                                                    \
    if (!(expectation)) {                                                 \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__ \
                << "]: " << #expectation << '\n';                         \
    }                                                                     \
  } while (false)

#define TEST_EXPECT_EQ(expression, expected)                              \
  do {                                                                    \
    auto result = (expression);                                           \
    auto expectation = (expected);                                        \
    if (result != expectation) {                                          \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__ \
                << "] expected " << expectation << " but got " << result  \
                << '\n';                                                  \
    }                                                                     \
  } while (false)

#define TEST_EXPECT_NEQ(expression, expected)                                 \
  do {                                                                        \
    auto result = (expression);                                               \
    auto expectation = (expected);                                            \
    if (result == expectation) {                                              \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__     \
                << "] expression " << #expression << " evalued to " << result \
                << '\n';                                                      \
    }                                                                         \
  } while (false)

#define TEST_EXPECT_EXCEPTION1(expression)                                \
  do {                                                                    \
    bool __exception_occurred__ = false;                                  \
    try {                                                                 \
      (expression);                                                       \
    } catch (...) {                                                       \
      __exception_occurred__ = true;                                      \
    }                                                                     \
    if (!__exception_occurred__) {                                        \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__ \
                << "] expression " << #expression                         \
                << " was supposed to throw exception\n";                  \
    }                                                                     \
  } while (false)

#define TEST_EXPECT_EXCEPTION2(expression, exception_type)                \
  do {                                                                    \
    bool __exception_occurred__ = false;                                  \
    try {                                                                 \
      (expression);                                                       \
    } catch (const exception_type &e) {                                   \
      __exception_occurred__ = true;                                      \
    } catch (...) {                                                       \
      __exception_occurred__ = true;                                      \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__ \
                << "] expression " << #expression                         \
                << " throw unexpected exception.\n";                      \
    }                                                                     \
    if (!__exception_occurred__) {                                        \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__ \
                << "] expression " << #expression                         \
                << " was supposed to throw exception " << #exception_type \
                << '\n';                                                  \
    }                                                                     \
  } while (false)

#define TEST_EXPECT_EXCEPTION3(expression, exception_type, message)         \
  do {                                                                      \
    bool __exception_occurred__ = false;                                    \
    try {                                                                   \
      (expression);                                                         \
    } catch (const exception_type &e) {                                     \
      __exception_occurred__ = true;                                        \
      std::string msg = (message);                                          \
      if (e.what() != msg) {                                                \
        std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__ \
                  << "] exception message \"" << e.what()                   \
                  << "\" is not the same expected message \"" << msg        \
                  << "\"\n";                                                \
      }                                                                     \
    } catch (...) {                                                         \
      __exception_occurred__ = true;                                        \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__   \
                << "] expression " << #expression                           \
                << " throw unexpected exception.\n";                        \
    }                                                                       \
    if (!__exception_occurred__) {                                          \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__   \
                << "] expression " << #expression                           \
                << " was supposed to throw exception " << #exception_type   \
                << '\n';                                                    \
    }                                                                       \
  } while (false)

#define TEST_EXPECT_EXCEPTION_REGEX(expression, exception_type, message)    \
  do {                                                                      \
    bool __exception_occurred__ = false;                                    \
    try {                                                                   \
      (expression);                                                         \
    } catch (const exception_type &e) {                                     \
      __exception_occurred__ = true;                                        \
      std::regex rgx(message);                                              \
      if (std::regex_match(e.what(), rgx)) {                                \
        std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__ \
                  << "] exception message \"" << e.what()                   \
                  << "\" does not match expected message \"" << message     \
                  << "\"\n";                                                \
      }                                                                     \
    } catch (...) {                                                         \
      __exception_occurred__ = true;                                        \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__   \
                << "] expression " << #expression                           \
                << " throw unexpected exception.\n";                        \
    }                                                                       \
    if (!__exception_occurred__) {                                          \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__   \
                << "] expression " << #expression                           \
                << " was supposed to throw exception " << #exception_type   \
                << '\n';                                                    \
    }                                                                       \
  } while (false)

#define GET_MACRO(_1, _2, _3, NAME, ...) NAME

#define TEST_EXPECT_EXCEPTION(...)                                       \
  GET_MACRO(__VA_ARGS__, TEST_EXPECT_EXCEPTION3, TEST_EXPECT_EXCEPTION2) \
  (__VA_ARGS__)

#define TEST_ASSERT_EQ(expression, expected)                              \
  do {                                                                    \
    auto result = (expression);                                           \
    auto expectation = (expected);                                        \
    if (result != expectation) {                                          \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__ \
                << "] expected " << expectation << " but got " << result  \
                << '\n';                                                  \
      return 1;                                                           \
    }                                                                     \
  } while (false)

#define TEST_ASSET_NEQ(expression, expected)                                  \
  do {                                                                        \
    auto result = (expression);                                               \
    auto expectation = (expected);                                            \
    if (result == expectation) {                                              \
      std::cerr << __func__ << " failed [" << __FILE__ << ":" << __LINE__     \
                << "] expression " << #expression << " evalued to " << result \
                << '\n';                                                      \
      return 1;                                                               \
    }                                                                         \
  } while (false)
