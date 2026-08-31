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
find_path(COMMUNITY_FUNCTIONS_SE_INCLUDE_DIRS "CommunityFunctionsSE/API.hpp")
target_include_directories(your_target PRIVATE ${COMMUNITY_FUNCTIONS_SE_INCLUDE_DIRS})
```

This is a header-only library; no linking is needed.

To use the port locally, copy `cmake/ports/community-functions-se` into your project's overlay ports.

## Use a function

Include `CommunityFunctionsSE/API.hpp` and call `CommunityFunctionsSE::GetFunction(id)`.
It returns a pointer to the matching `RE::SCRIPT_FUNCTION`, or `nullptr` if the ID is not registered.

## Use a condition

```cpp
const auto condition = CommunityFunctionsSE::GetConditionFunction(
    CommunityFunctionsSE::WouldBeStealing::kID,
    CommunityFunctionsSE::Comparison::kEqualTo,
    1.0);

if (condition && condition->IsTrue(actor, object)) {
    // The actor would be stealing the object.
}
```

The comparison can be `kEqualTo`, `kNotEqualTo`, `kGreaterThan`, `kGreaterThanOrEqualTo`, `kLessThan`, or `kLessThanOrEqualTo`.

Extra function arguments follow the comparison value. Use `std::int32_t`, `float`, or a Form pointer as required by that function. `Target` parameters come from `IsTrue(subject, target)` and are not passed again.

`GetConditionFunction` returns `std::nullopt` for an unknown ID, a wrong argument count or type, or an unsupported callback. `IsTrue` returns `false` if the callback returns `false`, returns a non-finite number, loses a required reference, or fails the comparison.

This creates a C++ condition object. It does not create a native `RE::TESCondition` or plugin record.

## Functions

Community function IDs range from `1000` through `9999`. Gaps are allowed.
After an ID is merged, it remains assigned to that function. Compatible fixes may keep the ID; incompatible behavior needs a new one.

| ID | Function | Input | Author | PR |
|---:|---|---|---|---|
| 1000 | `WouldBeStealing` | Subject: Actor; Target: object reference | [@Quantumyilmaz](https://github.com/Quantumyilmaz) | [#1](https://github.com/QTR-Modding/CommunityFunctionsSE/pull/1) |

## Contributing a function

1. Choose an unused ID from `1000` through `9999`.
2. Add the implementation under `include/CommunityFunctionsSE/Functions/`. Author or team subfolders are allowed.
3. Export a `Registration`:

```cpp
inline constexpr std::uint32_t kID = 1234;

inline constexpr CommunityFunctionsSE::Registration registration{
    .id = kID,
    .function = &function,
    .conditionParameters = { CommunityFunctionsSE::ConditionParameter::kTarget }
};
```

Each callback parameter needs one matching binding:

| Binding | `SCRIPT_PARAM_TYPE` | Value |
|---|---|---|
| `kTarget` | `kObjectRef` | `IsTrue` target |
| `kReference` | `kObjectRef` | `RE::TESObjectREFR*` argument |
| `kForm` | `kForm` | `RE::TESForm*` argument |
| `kInteger` | `kInt` | `std::int32_t` argument |
| `kFloat` | `kFloat` | `float` argument |

Use `DecodeIntegerParameter` and `DecodeFloatParameter` inside callbacks. Explicit references are retained with an engine handle.

4. Include the header in `Functions.hpp` and add `MyFunction::registration` to `entries`.
5. Add the function, author, and pull-request link to the table above.
6. Compile `API.hpp` with CommonLibVR-MIT. Test a meaningful result and every failure path in Skyrim, then include the results in the pull request.
