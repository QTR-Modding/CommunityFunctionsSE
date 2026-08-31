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

## Usage

Include `CommunityFunctionsSE/Functions.hpp` and call `CommunityFunctionsSE::GetFunction(id)`.
It returns a pointer to the matching `RE::SCRIPT_FUNCTION`, or `nullptr` when the ID is not registered.

## Functions

Community function IDs range from `1000` through `9999`. Gaps are allowed.
After an ID is merged, it remains assigned to that function. Compatible fixes may keep the ID; incompatible behavior needs a new one.

| ID | Function | Input | Author | PR |
|---:|---|---|---|---|
| 1000 | `WouldBeStealing` | Subject: Actor; Target: object reference | [@Quantumyilmaz](https://github.com/Quantumyilmaz) | [#1](https://github.com/QTR-Modding/CommunityFunctionsSE/pull/1) |

## Contributing a function

1. Choose an unused ID from `1000` through `9999`.
2. Add the implementation under `include/CommunityFunctionsSE/Functions/`. Author or team subfolders are allowed.
3. Export its registration:

```cpp
inline constexpr std::uint32_t kID = 1234;

inline constexpr CommunityFunctionsSE::RegistrationV1 registration{
    .id = kID,
    .function = &function
};
```

4. In `Functions.hpp`, include the new header and add `Normalize(MyFunction::registration)` to `entries`.
5. Add the function, author, and pull-request link to the table above.
6. Compile the umbrella header with CommonLibVR-MIT. Test a meaningful result and every implemented failure path in Skyrim, then include those results in the pull request.
