#pragma once

#pragma comment(lib, "Version.lib")

#define MAKE_STR_HELPER(a_str) #a_str
#define MAKE_STR(a_str) MAKE_STR_HELPER(a_str)

#define VERSION_PRODUCTNAME_DESCRIPTION_STR "Improved Camera SF" " v" VERSION_STR
#define VERSION_MAJOR                       1
#define VERSION_MINOR                       0
#define VERSION_REVISION                    0
#define VERSION_BUILD                       1
#define VERSION_STR                         MAKE_STR(VERSION_MAJOR) "." MAKE_STR(VERSION_MINOR) "." MAKE_STR(VERSION_REVISION) "." MAKE_STR(VERSION_BUILD)

#define VERSION_PRODUCTNAME_STR             "ImprovedCameraSF"
#define VERSION_YEARS_STR                   "(C) 2025-2026"
#define VERSION_AUTHOR_STR                  "IC Team"
#define VERSION_LICENSE_STR                 "(MPL 2.0)"
#define VERSION_COPYRIGHT_STR               VERSION_YEARS_STR " " VERSION_AUTHOR_STR " " VERSION_LICENSE_STR

#define VERSION_VER REL::Version(VERSION_MAJOR, VERSION_MINOR, VERSION_REVISION, VERSION_BUILD)
