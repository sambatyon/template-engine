# YATE: Yet Another Template Engine

YATE is a minimal template engine.

## Design

The driving force behind the design of YATE is my lack of know lack of knowledge
of template languages but some hobbyist experience writing parsers; as such the
engine is designed pretty much like a parser with two stages: a lexer, or
tokenizer, and a interpreter, which also works as a parser. There are also two
other helper classes: The Token one as well as a Frame.

- [**_Lexer_**](./src/yate/lexer.hh): Is the only class that deals directly with
  the input stream, Its main purpose is to generate a stream of tokens which can
  be consumed by the renderer. The renderer can consume this tokens through the
  `Lexer::Scan()` method, which will consume characters of the stream until it
  can create a well formed token. If it cannot generate a token, it throws a
  `std::runtime_error`. When all the input is consumed, an special token `EOF`
  is emitted, indicating the parser that it should stop consuming the input.
  Repeated calls after `EOF` has been emitted will only result in more `EOF`
  tokens being emitted.

  The lexer works in two modes, the literate mode where the only input to be
  interpret is the special string `{{` at which point a token of type `NOOP` is
  emitted, this token is also emitted if the end of file is reached. After `{{`
  is encounter, the lexer switches to the second mode: script mode, in this mode
  spaces are discarded and all input is interpret to generate one of the
  different tokes. A full list can be found in [token.hh](./src/yate/token.hh).

  Another role of the lexer is to keep track of the position in the stream which
  allows to perform jumps front and back in the stream when dealing with loops.
  This functionality can be access through the `Lexer::CurrentStreamPos()` and
  `Lexer::SetStreamPos()` methods.

- [**_Renderer_**](./src/yate/renderer.hh): Is the class on charge of consuming
  the tokens, interpret them and generate copy the output into the out stream.
  Unlike a traditional parser which would generate an AST as a result, this one
  omits that middle step and gives a processed output.

  Apart from token manipulation, other tasks performed by `Renderer` are: Verify
  that the syntax is correct and keep track of the symbols that are going to be
  written in the output. Syntax verification is performed as an inherent step of
  the parsing step, while symbol resolution is performed with a stack of
  [`Frame`](./src/yate/frame.hh)s which stores symbols associated with the
  current scope and resolve to scopes up the stack if symbols are not found.

  The most important part to understand of the `Renderer` is how it handles
  loops. Whenever it finds the `#loop` keyword it extracts the array associated
  with the loop and the symbol name to be used for each element of the array.
  Creates a new `Frame` and stores the first value of the array into the frame
  with the symbol given. It continues parsing the input until the `/loop`
  keyword is found, it then stores the location of the next token in the stream
  and moves the stream location in the lexer back to the loop begin location.
  It does this repeatedly until all elements have been processed. It then jumps
  past the loop end by moving the stream past the end of the loop and deletes
  the current topmost `Frame`.

  Perhaps the one thing where the rendered can clearly be improved is by not
  copies of the input parameters since the current design can lead to big memory
  allocations and de-allocations. I noticed too late to fix the issue.

The approach of jumping all over the stream makes the code in the `Renderer`
much simpler, however it causes issues when the input stream is an interactive
TTY. One solution would be to buffer read the input, although classes of the
`std::istream` family are already buffered. This is an issue that could be
better addressed.

### others

1. A common patter found in the code base which may come as somewhat weird is
   the way some classes constructors take copied parameters which are later
   moved into its attributes (see [frame.hh](./src/yate/frame.hh)), for example:

   ```c++
   class Foo {
    public:
     Foo(Bar bar)

    private:
     Bar bar_;
   }

   Foo::Foo(Bar bar) : bar_(std::move(bar)) {}
   ```

   The pattern was taken from Nicolai Josuttis talk
   [The nightmare of move semantics for trivial classes](https://youtu.be/PNRju6_yn3o?t=8830).

## API

The usage of the library is very simple. There is only one include header
[yate.hh](./include/yate/yate.hh) which defines a single function
`yate::Render()`. This function takes four parameters as input:

1. `values` of type `std::unordered_map<std::string, std::string>` which are
   those values which are directly printable.
1. `arrays` of type `std::unordered_map<std::string, std::vector<std::string>>`
   which are those values which are used only inside loops. They cannot be
   printed directly. This different use cases from `values` allows to reuse
   the same name for an array and a value and no check is done to prevent this
   from happening.
1. `input`, a reference to an object of type `std::istream` which is the stream
   from where the template would be read.
1. `output`, a reference to an object of type `std::ostream` which is the stream
   where the processed template will be streamed.

An example of the intended used of the library can be found in
[example_main.cc](./src/example/example_main.cc).

## Language

The language supported by YATE is very simple. It has only one kind of
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

On Windows with Visual Studio 15:

```bat
cd %YATE_SRC_ROOT%
mkdir build
cd build
cmake -G"Visual Studio 15 2017 Win64" ..
REM Building can be done inside Visual Studio itself.
```

The code was tested in Ubuntu Linux 18.04, macOS 10 Mojave and MS Windows 10
with Visual Studio 15 Community Edition. The latter required changes to the
generated library (static library instead of shared) due to windows way of
exporting symbols.

The special target `test` run the unit tests. However it is preferably to run
it outside as an standalone program (`template-engine-tests`) since ctest will
ignore the output of the tests and at the moment the test unit system is not
advanced enough to show as a failure when it actually fails.

## Code Structure

- [`include`](./include/) directory contains all the headers that a client from
  the library would need.

- [`src`](./src/) contains a couple of subdirectories:

  - [`yate`](./src/yate/) contains the whole implementation of the template
    engine. Most of the code is located here.
  - [`example`](./src/example) subdirectory contains an example of how to use
    the library from and end user perspective.

- [`tests`](./tests/) subdirectory contains the unit tests for the library.
  The unit tests are rather comprehensive and it is recommended to look at
  them for an idea of what the library can and cannot do,
  in particular [`render_tests.cc`](./tests/render_tests.cc).

## Known issues

1. Manipulating streams can get tricky, in this case the template engine works
   somewhat funky when using stdin as the input stream and this is working
   interactively. From my research it has to do with stdout and stdin sharing
   the same buffer so it makes the stream jumps crazy.

1. Because we escape the string `{\{` to generate `{{` in the output, the string
   `{\{` became un-generable.

1. Arrays and values can share the same name. Since I implemented only the loop
   statement, I'm not so sure if this should be consider a bug or a feature. If
   we consider it a bug, a simple check in `Frame` constructor should be enough.