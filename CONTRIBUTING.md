# Contributing to RevivalPad

We welcome and appreciate all contributions to this open source project!

## How to contribute

Before contributing, please take a moment to review the following guidelines:

1. Check the [issue tracker](https://github.com/0x1-1/revivalpad/issues) to see if the issue or feature you're interested in has already been reported or discussed.
2. If you would like to suggest a new feature, please [start a new issue](https://github.com/0x1-1/revivalpad/issues/new/choose) to discuss the proposal.
3. If you would like to contribute code, follow the existing style in the file you are editing
   (`.clang-format` is authoritative) and make sure your code is well-documented, preferably in
   doxygen-compatible format.
4. Submit a pull request to the main repository for review. In case of preparing bigger changes into the codebase it is recommended to firstly open `Draft Pull Request` to show what you are going to do.

You can generate API documentation locally with `-DBUILD_DOCS=ON` and Doxygen installed.

### Rebranding scope

RevivalPad is a fork, and it takes upstream attribution seriously. When editing this codebase:

- Do **not** rewrite historical AntiMicroX references in copyright headers, changelog entries or
  the About dialog's credits. They are accurate and must stay.
- Do **not** hardcode product names, URLs or the application id in source files. Add them to the
  identity block in `CMakeLists.txt` and consume them through the generated `config.h`.
- Any intentionally remaining AntiMicroX string must be justified in
  [`FORK_NOTICE.md`](./FORK_NOTICE.md), which records the legal, historical and
  compatibility reasons the existing ones are kept.

## Translation

RevivalPad inherits AntiMicroX's translation catalogues, preserved intact and renamed into the
`revivalpad` namespace. They live in `share/revivalpad/translations/`.

RevivalPad does not currently use a hosted translation platform. Submit translation updates as
pull requests against the `.ts` files. The product name should remain `RevivalPad` in every
language unless a language genuinely requires grammatical treatment of it.


To refresh the catalogues after changing source strings:

```bash
cmake -S . -B build -DUPDATE_TRANSLATIONS=ON
cmake --build build --target updateqm
```

This merges new and changed source strings into the existing catalogues without discarding
translated content.

## License

By contributing to this project, you agree to license your contributions under the GPL-3.0 license.
