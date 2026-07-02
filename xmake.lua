-- Improved Camera SF (ICSF) - xmake build configuration
-- Cross-compiled on Linux via clang-cl + lld-link + llvm-rc against the xwin
-- Windows SDK, built on libxse's CommonLibSF (runtime 1.16.244.0).
--
-- Linux cross-compile:
--   xmake f -p windows -a x64 --toolchain=clang-cl --sdk=$HOME/.vsxwin -m releasedbg -y
--   xmake build

-- Project-local module overrides: find_rc.lua makes xmake locate an rc compiler on
-- Linux (upstream find_rc is Windows-only). Must come before any target resolution.
add_moduledirs("modules")

-- CommonLibSF (+ nested commonlib-shared) built from source as a static-lib subproject.
-- The original repo shipped no build files and expected prebuilt *.lib. spdlog (v1.16.0,
-- std_format+wchar) is pulled by commonlib-shared via xrepo and propagates transitively.
includes("extern/commonlibsf")

set_project("ImprovedCameraSF")
set_version("1.0.0")
set_license("MPL-2.0")
set_languages("c++23")
set_warnings("allextra")
set_encodings("utf-8")

add_rules("mode.debug", "mode.release", "mode.releasedbg")

-- Third-party libraries used by the overlay/hooking/config layers, pulled from xrepo:
--   glm     - math used by the pseudo-camera transforms (header-only)
--   imgui   - core Dear ImGui (no graphics backend; the mod drives it itself)
--   minhook - inline hooking of the game's camera update functions
--   mini    - metayeti mINI, provides <mini/ini.h> for the .ini config
add_requires("glm", "imgui", "minhook", "mini")

target("ImprovedCameraSF")
    set_kind("shared")

    -- WIN32_LEAN_AND_MEAN / NOMINMAX / GLM_* are also set in PCH.h; keep the core ones
    -- here so every TU (incl. the PCH itself) agrees.
    add_defines("UNICODE", "_UNICODE", "NOMINMAX", "WIN32_LEAN_AND_MEAN")

    add_deps("commonlibsf")
    add_packages("glm", "imgui", "minhook", "mini")

    -- source files. spdlog_compat.cpp was a hand-rolled shim that redefines
    -- REX::Impl::Log/Fail for a manually built commonlib-shared.lib; the real
    -- commonlib-shared (built here from source) already provides those symbols, so
    -- compiling it would cause duplicate-symbol link errors.
    add_files("source/**.cpp")
    add_files("PCH/PCH.cpp")
    remove_files("source/spdlog_compat.cpp")
    add_headerfiles("include/**.h", "PCH/PCH.h")

    -- "." lets PCH.cpp resolve "PCH/PCH.h"; "include" resolves the plugin headers.
    add_includedirs(".", "include", "PCH")

    -- precompiled header
    set_pcxxheader("PCH/PCH.h")

    -- SFSEPlugin_Version / SFSEPlugin_Load come from CommonLibSF's SFSE_PLUGIN_* macros
    -- (already __declspec(dllexport)); pin them into the export table explicitly too.
    add_ldflags(
        "/EXPORT:SFSEPlugin_Version",
        "/EXPORT:SFSEPlugin_Load",
        { force = true }
    )

    -- Win32 libraries: D3D11/DXGI for the overlay, the rest are the usual SFSE set
    -- (some also arrive transitively from commonlib-shared).
    add_syslinks(
        "user32", "kernel32", "shell32", "version", "ws2_32", "advapi32",
        "bcrypt", "ole32", "oleaut32", "dxgi", "d3d11", "d3dcompiler", "dbghelp", "psapi"
    )
target_end()
