# Fork notice

**RevivalPad is a modified fork of AntiMicroX. Original AntiMicroX authors and
contributors retain copyright over their respective contributions. RevivalPad
modifications are distributed under the GNU General Public License version 3 or
later.**

This is a *modified version*. Problems you encounter in RevivalPad must be
reported to the RevivalPad issue tracker, **not** to the AntiMicroX maintainers.
0x1-1 is responsible only for the fork and its modifications, and did not
create the original AntiMicroX code.

## Fork base

| Field                    | Value                                                                 |
| ------------------------ | --------------------------------------------------------------------- |
| Upstream project         | AntiMicroX                                                            |
| Upstream repository      | `AntiMicroX/antimicrox` - <https://github.com/AntiMicroX/antimicrox>   |
| Upstream base version    | 3.6.1                                                                 |
| Upstream commit SHA      | `e0e11628aa6f20ddbce32caba496aa12ef8f84d3`                            |
| Fork created             | 2026-08-02                                                            |
| First RevivalPad version | 1.0.0                                                                 |

AntiMicroX is itself a continuation of `antimicro`, originally written by Travis
Nickles, which was in turn inspired by QJoyPad. That lineage is preserved in
`CHANGELOG.md`, in the About dialog's Credits tab, and in the per-file GPL
headers throughout `src/`.

## Licensing

RevivalPad is licensed **GPL-3.0-or-later**, unchanged from upstream.

* `LICENSE` is the original, unmodified GPL-3.0 text.
* Existing copyright notices, author names and GPL headers in the source files
  have been preserved verbatim.
* Files materially changed for this fork carry an **additional**
  `Copyright (C) 2026 0x1-1 (RevivalPad fork modifications)` line alongside -
  never instead of - the prior copyright holders.
* Historical AntiMicroX references inside copyright notices, changelog entries
  and credits have deliberately **not** been rewritten. Doing so would falsify
  the record.

### AntiMicroX references that intentionally remain

The rebrand deliberately left some AntiMicroX references in place. They fall
into two groups, and neither may be "cleaned up":

**Legal and historical attribution — must not be removed.**

* Per-file GPL headers throughout `src/` carrying the upstream authors'
  copyright notices.
* The AntiMicroX section of `CHANGELOG.md`, which is the upstream release
  history preserved verbatim under its own heading.
* The About dialog: original AntiMicro / AntiMicroX authors and links to the
  upstream repository and contributor list in the Credits tab, and Travis
  Nickles' original text in the "About Development" tab.
* The upstream identity block in `CMakeLists.txt` (`REVIVALPAD_UPSTREAM_*`),
  the single source of the attribution shown in the About dialog, the AppStream
  description and the Debian package description. (`other/revivalpad.1` is a
  static troff file and repeats the same attribution by hand.)
* The `AUTHORS` section of `other/revivalpad.1`.
* A provenance comment in `cmake/modules/FindSDL2.cmake` recording a local
  modification, and CI comments citing upstream issue
  `AntiMicroX/antimicrox#465`.
* Historical strings inside `share/revivalpad/translations/*.ts`. The catalogue
  namespace was renamed and the translations merged, not discarded; deleting
  the remaining entries would destroy contributed translation work.

**Backward compatibility — removing these would break existing users.**

* The AntiMicroX / `antimicroX` / `antimicro` configuration paths in
  `src/common.h`, used as *read-only* sources by the first-run import. See
  `src/settingsmigration.cpp`, whose behaviour is pinned by
  `tests/testsettingsmigration.cpp`.
* The migration dialog naming the product it imported from, so the user knows
  which configuration was copied.
* Factual notes about which upstream version changed the on-disk profile format
  (`src/gui/advancestickassignmentdialog.ui`, `src/gui/mainwindow.cpp`).
  Attributing those changes to a RevivalPad version would be wrong.
* The `.amgp` extension and the `application/x-amgp` media type, both pinned by
  `tests/testprofilecompatibility.cpp`. The Windows *ProgID* did change
  (`AntiMicro.amgp` -> `RevivalPad.amgp`) so the two applications do not fight
  over the association; the extension itself is untouched.
* The literal string `antimicro` inside the test suite, which exists precisely
  to assert its **absence** from the shipped identity.

Everything else — executable and package names, window titles, tray, About
header, CLI help, application id, D-Bus names, desktop entry, AppStream
component, icon names, configuration directory, settings filename, local socket
key, update endpoint, release artifact names and all image assets — carries no
AntiMicroX branding. `tests/testproductidentity.cpp` enforces this.

## Summary of RevivalPad modifications

RevivalPad 1.0.0 is a rebrand of AntiMicroX 3.6.1. No application functionality
was added, removed or redesigned.

* **Product identity** - display name, executable, window titles, tray, About
  dialog, CLI help, log output and error messages now say RevivalPad. All
  identity values are centralised in the CMake identity block and reach the C++
  code through the generated `config.h`.
* **Build system** - project, target, source-list variables, version macros,
  CPack package names and installer identity renamed. Version reset to 1.0.0.
* **Configuration namespace** - settings now live in
  `$XDG_CONFIG_HOME/revivalpad/revivalpad_settings.ini` (Linux) or
  `%LocalAppData%\revivalpad\revivalpad_settings.ini` (Windows), so RevivalPad
  can be installed alongside AntiMicroX.
* **First-run migration** - an existing AntiMicroX, `antimicroX` or `antimicro`
  configuration is *copied* into the RevivalPad location on first run. Originals
  are never moved, modified or deleted, and an existing RevivalPad configuration
  is never overwritten.
* **Runtime identity** - a distinct local socket key, so both applications can
  run at the same time.
* **Linux integration** - application id, desktop entry, AppStream component,
  MIME package, icon names, man page and Flatpak manifest moved to
  `io.github._0x1_1.revivalpad`.
* **D-Bus** - service and interface renamed. **This is a breaking integration
  change**; see below.
* **Translations** - catalogue namespace renamed from `antimicrox` to
  `revivalpad`. Existing translations were preserved and merged, not discarded.
* **Assets** - a new icon set drawn for RevivalPad. No AntiMicroX artwork was
  reused.
* **Update checker** - points at the RevivalPad repository. It never queries the
  AntiMicroX release endpoint, so an upstream release can never be offered as a
  RevivalPad update.
* **Documentation** - README, BUILDING, CONTRIBUTING, issue and PR templates
  rewritten. Upstream distribution and support claims that do not apply to this
  fork were removed rather than inherited.

## Compatibility

**Preserved:**

* The `.amgp` profile format is unchanged. Existing AntiMicroX profiles open in
  RevivalPad without conversion.
* The `application/x-amgp` MIME type is unchanged.
* Settings keys inside the `.ini` are unchanged, which is what makes the
  first-run import a straight file copy.

**Breaking:**

* **D-Bus.** RevivalPad claims `io.github._0x1_1.revivalpad` and exports
  `io.github._0x1_1.revivalpad.InputDevice`, not the upstream
  `io.github.antimicrox` names. Continuing to claim the upstream name would
  collide with an installed AntiMicroX instance. Scripts that drove AntiMicroX
  over D-Bus must be updated. Object paths (`/InputDevice/<n>`) are unchanged.
* **Windows file association ProgID.** `AntiMicro.amgp` became
  `RevivalPad.amgp`, so the two applications do not fight over the `.amgp`
  association. The file extension itself is unchanged.
* **Configuration path.** RevivalPad does not read or write the AntiMicroX
  configuration after the initial import.
