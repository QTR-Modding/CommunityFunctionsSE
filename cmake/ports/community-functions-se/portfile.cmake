vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO QTR-Modding/CommunityFunctionsSE
    REF 8310a2b27a07d196cef47be1a94d0288dca26af8
    SHA512 9477ffbcedc6b64291917febc6b63c5305e22370c40ee6582bbd195d56ff7fc30dc89fa56c4d4ec2c5d43d2e9afd5a285d2ddba6c32a301ec94f4a7a3b914856
    HEAD_REF main
)

file(INSTALL "${SOURCE_PATH}/include/CommunityFunctionsSE" DESTINATION "${CURRENT_PACKAGES_DIR}/include")

set(VCPKG_POLICY_SKIP_COPYRIGHT_CHECK enabled)
