# C++ Coding Style

This topic contains the C++ formatting and coding guidelines for MAVSDK.

::: info
These guidelines are not written in stone!
[Start a discussion](../../index.md#getting-help) if you have suggestions for improvement \(the project will consider anything other than "matters of personal taste"\).
:::

## Formatting and White Space

All **.cpp** and **.h** files should be formatted according to the `.clang-format` style.

### Fix style

To automatically fix the formatting, run the command:

```
./tools/fix_style.sh .
```

If you don't have clang-format installed or in the correct version, you can use a docker image or install it as explained below:

### Use clang-format in docker

You can just use the pre-built docker image:

```
tools/run-docker.sh tools/fix_style.sh .
```

## General Guidelines

The following general guidelines should be used for all code:

* C++17 is encouraged to allow developers to use C++17 features and the C++ STL:
  Examples:

  * [`std::function`](http://en.cppreference.com/w/cpp/utility/functional/function) and [lambda expressions](http://en.cppreference.com/w/cpp/language/lambda)
  * [`std::vector`](http://en.cppreference.com/w/cpp/container/vector), [`std::map`](http://www.cplusplus.com/reference/map/map/)
  * [`std::thread`](http://www.cplusplus.com/reference/thread/thread/), [`std::mutex`](http://en.cppreference.com/w/cpp/thread/mutex)

* `using namespace std` is discouraged \([read why](https://stackoverflow.com/questions/1452721/why-is-using-namespace-std-considered-bad-practice)\).
  If needed specific declarations can be used in the source files such as `using std::this_thread::sleep_for` to reduce verbosity.

* The usage of namespacing wherever possible is encouraged \(e.g. `enum class` is to be used over `enum`\).
* Filename extensions should be `.h` for header files and `.cpp` for source files \(for consistency\).
* Variable and method names should err on the side of verbosity instead of being quick to type and read. Abbreviations are only to be used for small scopes and should not be exposed in public APIs.
* All variables that have a physical unit should have the unit in the variable name \(e.g. `_m` for meters, `_m_s` for meters/second\).
* Variable and method names should be `snake_case` and class/struct/enum names `CamelCase`. Private variables should start with an underscore, e.g.: `_variable_name`.
* Try to exit functions early instead of nesting ifs \([read why](https://softwareengineering.stackexchange.com/questions/18454/should-i-return-from-a-function-early-or-use-an-if-statement)\).
* We don't use exceptions but use error codes. There are pros and cons for exceptions but given that the public API should be as simple as possible, it makes sense to refrain from exceptions altogether.
* The use of `std::bind` is discouraged. The aim is to move towards just using lambdas for that purpose instead.
