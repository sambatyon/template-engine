# YATE: Yet Another Template Engine

YATE is a minimal template engine.

## Language

The language supproted by YATE is very simple. It has only one kind of
delimiter: `{{...}}`, outside these delimiters the text won't be interpreted
and will be copied verbatim to the output, except fro the string `{\{` which
is used to produce the output `{{`.

Inside the delimiters the following three constructions are supported:

1. `{{symbol}}` which is used to print the value of `symbol`.
1. `{{#loop array_symbol symbol}}` which is used to begin an iterable
   section. YATE will iterate over each element inside `array_symbol` and will
   set `symbol` to the value of the element currently begin used.
1. `{{/loop}}` which is used to leave the loops.

A simple example of the language is:

```text
First Line.
{{header}}
{{#loop somearray item}}This is a {{item}}.
{{/loop}}{{footer}}
Last Line.
```

Given the following input:

```text
"header": "Hello!"
"somearray": ["apple", "banana", "citrus"]
"footer": "That's it!"
```

Will produce:

```text
First Line.
Hello!
This is a apple.
This is a banana.
This is a citrus.
That's it!
Last Line.
```

## Compiling

The only dependency to build YATE is CMake and a compiler which support C++14,
the rest is built using only the STL. To compile create a build directory
inside the source code and call CMake with your favorite generator. Then just
build the code.

Example 1 Ninja:

```bash
cd ${YATE_SRC_ROOT}
mkdir build
cd build
cmake -GNinja -DCMAKE_BUILD_TYPE=Debug ..
ninja
ninja test
```

Example 2 Makefiles:

```bash
cd ${YATE_SRC_ROOT}
mkdir build
cd build
cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
make
make test
```

The special target `test` run the unit tests. However it is preferably to run
it outside as an standalone program (`template-engine-tests`) since ctest will
ignore the output of the tests and at the moment the test unit system is not
advanced enough to show as a failure when it actually fails.

## Code Structure

The `include` directory contains all the headers that a client from the library
would need.

The `src` directory contains the whole implementation of the template engine
under the `yate` subdirectory, while the `examples` subdirectory contains an
example of how to use the library.

The `tests` subdirectory contains the unit tests for the library. The unit tests
are rather comprehensive and it is recommended to look at them for an idea of
what the library can and cannot do, in particular `render_tests.cc`.

## API

## Design

## Known issues

Manipulating streams can get tricky, in this case the template engine works
somewhat funky when using stdin as the input stream and this is working
interactively. From my research it has to do with stdout and stdin sharing
the same buffer so it makes the stream jumps crazy.

Because we escape the string `{\{` to generate `{{` in the output, the string
`{\{` became ungeneratable.