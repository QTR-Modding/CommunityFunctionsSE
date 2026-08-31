# CommunityFunctionsSE

Community condition functions for Skyrim Special Edition.

## Installation (via vcpkg)

Add to your `vcpkg.json`:

```json
"dependencies": [
  "community-functions-se"
]
```

In your `CMakeLists.txt`:

```cmake
find_path(COMMUNITY_FUNCTIONS_SE_INCLUDE_DIRS "CommunityFunctionsSE/Functions.hpp")
target_include_directories(your_target PRIVATE ${COMMUNITY_FUNCTIONS_SE_INCLUDE_DIRS})
```

This is a header-only library; no linking is needed.

To use the port locally, copy `cmake/ports/community-functions-se` into your project's overlay ports.

## Functions

Community function IDs start at `1000`.

| ID | Function | Input | Author | PR |
|---:|---|---|---|---|
| 1000 | `WouldBeStealing` | Subject: Actor; Target: object reference | [@Quantumyilmaz](https://github.com/Quantumyilmaz) | [#1](https://github.com/QTR-Modding/CommunityFunctionsSE/pull/1) |
