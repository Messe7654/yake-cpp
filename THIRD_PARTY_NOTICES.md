# Third-party notices

This project downloads its compiled and test dependencies with CMake
`FetchContent`. Their source code is not stored in this repository.

## Snowball English stop-word list

The English stop-word list in [`resources/stopwords/en.txt`](resources/stopwords/en.txt)
is derived from the [Snowball project's English stop-word list](https://snowballstem.org/algorithms/english/stop.txt).
Comments and disabled entries in the Snowball-format source have been omitted,
and the active entries have been sorted. Snowball is copyright © 2001 Dr Martin
Porter and copyright © 2002 Richard Boulton, and is distributed under the BSD
3-Clause license.

The complete BSD 3-Clause license is reproduced in
[`LICENSES/snowball.txt`](LICENSES/snowball.txt).

## utf8proc 2.11.3

utf8proc is copyright © 2014–2021 Steven G. Johnson, Jiahao Chen, Tony Kelman,
Jonas Fonseca, and other contributors, with portions copyright © 2009, 2013
Public Software Group e. V. It is distributed under the MIT license. Its
generated Unicode data is also subject to the Unicode Data Files and Software
License.

The complete MIT and Unicode notices are reproduced in
[`LICENSES/utf8proc.txt`](LICENSES/utf8proc.txt).

## GoogleTest 1.15.2

GoogleTest is copyright © 2008 Google Inc. and is distributed under the
BSD 3-Clause license. It is used only when building the test suite.

The complete BSD 3-Clause license is reproduced in
[`LICENSES/googletest.txt`](LICENSES/googletest.txt).
