# AGENTS.md

Operational guidance for coding agents working in this repository.

## 1) Repository At A Glance

- Project type: C++ Geode mod (`xdBot` / `zilko.xdbot`).
- Build system: CMake + Geode SDK integration (`setup_geode_mod(...)`).
- Primary source roots:
  - `src/` gameplay hooks, macro logic, UI, renderer, utilities.
  - `resources/` runtime assets packaged with the mod.
- Metadata and settings:
  - `mod.json` (Geode mod metadata, settings schema, dependencies).
  - `CMakeLists.txt` (C++20 target, source list, Geode SDK dependency).
- CI workflows:
  - `.github/workflows/multi-platform.yml` (Windows + Android32 + Android64).
  - `.github/workflows/android.yml` and `.github/workflows/android copy.yml` (Android variants).

## 2) Required Environment

- `GEODE_SDK` environment variable must be set.
  - `CMakeLists.txt` hard-fails when `GEODE_SDK` is missing.
- C++ standard is fixed to C++20.
- Platform-aware code paths exist:
  - `#ifdef GEODE_IS_WINDOWS`
  - `#ifdef GEODE_IS_ANDROID`
  - Keep platform guards intact when modifying behavior.

## 3) Build Commands

### Recommended local build (manual CMake)

Use out-of-source build directories.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build -j
```

Release-style build:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Platform-specific build directories (mirrors `.gitignore` conventions)

```bash
cmake -S . -B build-android32 -DCMAKE_BUILD_TYPE=Release
cmake --build build-android32 -j

cmake -S . -B build-android64 -DCMAKE_BUILD_TYPE=Release
cmake --build build-android64 -j
```

### CI-equivalent build behavior

CI uses `geode-sdk/build-geode-mod@main` and matrix targets:

- Windows: `RelWithDebInfo`
- Android32: `Release`
- Android64: `Release`

When validating locally for parity, prefer `RelWithDebInfo` on Windows and `Release` on Android targets.

## 4) Test Commands

### Current state

- No first-class test suite is committed in this repository.
- No `tests/` directory or explicit CTest registration was found.

### If/when CTest tests exist

Run all tests:

```bash
ctest --test-dir build --output-on-failure
```

Run a single test (important pattern for agents):

```bash
ctest --test-dir build -R "<test-name-regex>" --output-on-failure
```

Run one exact test name:

```bash
ctest --test-dir build -R "^<exact-test-name>$" --output-on-failure
```

### Validation expectation in this repo today

- Treat a clean compile as the primary automated validation signal.
- For behavior changes, prefer minimal, focused runtime checks in Geometry Dash + Geode.

## 5) Lint / Format Commands

### Current state

- No repository-owned lint config was found (`.clang-format`, `.editorconfig`, clang-tidy config not present).
- No dedicated lint target or script is defined.

### Practical linting approach

- Use compiler warnings as baseline signal during build.
- If you introduce new tooling locally, do not commit tool-driven mass reformat unless requested.
- Keep diffs narrow and style-consistent with surrounding files.

## 6) Cursor / Copilot Rules Status

Checked locations:

- `.cursor/rules/`
- `.cursorrules`
- `.github/copilot-instructions.md`

Result in this repository snapshot:

- No Cursor rule files found.
- No Copilot instruction file found.

If these files are added later, they become first-priority agent instructions and should be reflected in this document.

## 7) Code Style Guidelines (Derived From Existing Code)

### 7.1 Includes and dependencies

- Prefer project headers first, then Geode/framework headers, then STL headers.
- Keep include paths consistent with existing style (`"../includes.hpp"`, `"ui/..."`, etc.).
- Avoid adding heavy headers in widely included headers unless required.
- Preserve platform-specific includes inside `#ifdef GEODE_IS_WINDOWS` / `#ifdef GEODE_IS_ANDROID` blocks.

### 7.2 Formatting and layout

- Follow nearby file style instead of enforcing a new global style.
- Existing code commonly uses 4-space indentation and compact control flow.
- Keep brace style and spacing consistent within the file you edit.
- Prefer early returns to reduce nesting.
- Avoid unrelated whitespace churn.

### 7.3 Types and APIs

- Use explicit, fixed-size integer types when crossing settings/API boundaries (example: `int64_t` for Geode setting callbacks).
- Use `auto& g = Global::get();` pattern for repeated global-state access.
- Use `std::filesystem::path` for file paths.
- Use `static_cast<...>` for numeric/object conversions when needed.
- Keep C++20 compatibility; do not introduce features requiring newer standards.

### 7.4 Naming conventions

- Types/classes: `PascalCase` (`Global`, `Renderer`, `ButtonEditLayer`).
- Functions/methods: `camelCase` (`updateKeybinds`, `handleRecording`, `togglePlaying`).
- Variables: generally `camelCase`.
- Constants often use `camelCase`/descriptive identifiers rather than ALL_CAPS.
- Maintain naming already used in each subsystem; avoid renaming without strong reason.

### 7.5 State management patterns

- Central mutable runtime state is stored in `Global` singleton.
- When mutating gameplay-critical fields, keep side effects synchronized with UI updates:
  - `Interface::updateLabels()`
  - `Interface::updateButtons()`
- Respect existing state machine values:
  - `state::none`
  - `state::recording`
  - `state::playing`

### 7.6 Error handling and logging

- Favor defensive checks and early return for null game objects (`if (!PlayLayer::get()) return ...`).
- For filesystem operations, use `std::error_code` when failure is non-fatal.
- Surface user-facing failures with `FLAlertLayer::create(...)->show()` where appropriate.
- Keep internal diagnostics with `log::debug`, `log::warn`, `log::info`.
- Do not throw exceptions across game-hook boundaries.

### 7.7 Hooking and threading safety

- Hooks use Geode `$modify(...)` and `$execute` conventions; preserve signatures exactly.
- For cross-thread UI/game interactions, dispatch back to main thread with `Loader::get()->queueInMainThread(...)`.
- In rendering/audio paths, preserve existing synchronization primitives (`std::mutex`, lock/unlock discipline).

### 7.8 Platform compatibility

- Keep Windows-specific APIs guarded (`WideCharToMultiByte`, Win32 file handles, ffmpeg exe path logic).
- Keep Android paths guarded and avoid introducing Windows-only assumptions.
- Validate behavior under both branches when touching shared logic.

### 7.9 File and setting conventions

- New user-facing settings belong in `mod.json` with explicit type/default/constraints.
- Saved values/settings keys are stringly typed; reuse existing keys exactly to avoid migration bugs.
- Resource additions should be mirrored in `mod.json` `resources` section when needed.

## 8) Change Scope Guidance For Agents

- Prefer small, surgical patches over broad refactors.
- Do not rewrite large legacy sections unless required to fix a concrete issue.
- Keep compatibility with existing macro formats and runtime behavior.
- When adding features, follow current patterns in `src/ui/`, `src/hacks/`, and `src/renderer/`.

## 9) Quick Pre-PR Checklist

- Build succeeds in at least one relevant configuration.
- No platform guard regressions introduced.
- No accidental formatting-only churn.
- No new warnings/errors from modified code paths.
- If changing settings/resources, update `mod.json` accordingly.
