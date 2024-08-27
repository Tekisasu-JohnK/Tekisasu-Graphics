# Licensing

By submitting a pull request, you represent that you have the right to
license your contribution to the Laf project owners and the community,
and agree by submitting the patch that your contributions are licensed
under the [MIT license](https://raw.githubusercontent.com/aseprite/laf/main/LICENSE.txt)
terms.

# Code submission policy

We have some rules for commits that are contributed:

* Split your changes in the most atomic commits possible: one commit
  for feature, or fix.
* Rebase your commits to the `main` branch (or `beta` if you are
  targeting the beta version).
* Wrap your commit messages at 72 characters.
* The first line of the commit message is the subject line.
* Write the subject line in the imperative mood, e.g. "Fix something",
  not "Fixed something".
* For platform-specific commits start the subject line using
  `[win]`, `[osx]`, or `[x11]` prefixes.
* Check the spelling of your code, comments and commit messages.
* We're using some C++17 features, targeting macOS 10.9 mainly as the
  oldest platform (and the one limiting us to newer C++ standards).
