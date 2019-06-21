// Copyright (c) 2012-2016 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <clientversion.h>

#include <tinyformat.h>

#include <string>
#include <utilstrencodings.h>
#include <utilsplitstring.h>

/**
 * Name of client reported in the 'version' message. Report the same name
 * for both bitcoind and bitcoin-abc, to make it harder for attackers to
 * target servers or GUI users specifically.
 */
const std::string CLIENT_NAME("DeVault Core");

/**
 * Client version number
 */
#define CLIENT_VERSION_SUFFIX ""

/**
 * The following part of the code determines the CLIENT_BUILD variable.
 * Several mechanisms are used for this:
 * * first, if HAVE_BUILD_INFO is defined, include build.h, a file that is
 *   generated by the build environment, possibly containing the output
 *   of git-describe in a macro called BUILD_DESC
 * * secondly, if this is an exported version of the code, GIT_ARCHIVE will
 *   be defined (automatically using the export-subst git attribute), and
 *   GIT_COMMIT will contain the commit id.
 * * then, three options exist for determining CLIENT_BUILD:
 *   * if BUILD_DESC is defined, use that literally (output of git-describe)
 *   * if not, but GIT_COMMIT is defined, use
 *     v[maj].[min].[rev].[build]-g[commit]
 *   * otherwise, use v[maj].[min].[rev].[build]-unk
 * finally CLIENT_VERSION_SUFFIX is added
 */

//! First, include build.h if requested
#ifdef HAVE_BUILD_INFO
#include <build.h>
#endif

#define BUILD_DESC_WITH_SUFFIX(maj, min, rev, build, suffix)                   \
    "v" DO_STRINGIZE(maj) "." DO_STRINGIZE(min) "." DO_STRINGIZE(              \
        rev) "." DO_STRINGIZE(build) "-" DO_STRINGIZE(suffix)

#define BUILD_DESC_FROM_UNKNOWN(maj, min, rev, build)                          \
    "v" DO_STRINGIZE(maj) "." DO_STRINGIZE(min) "." DO_STRINGIZE(              \
        rev) "." DO_STRINGIZE(build) "-unk"

#ifndef BUILD_DESC
#ifdef BUILD_SUFFIX
#define BUILD_DESC                                                             \
    BUILD_DESC_WITH_SUFFIX(CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR,         \
                           CLIENT_VERSION_REVISION, CLIENT_VERSION_BUILD,      \
                           BUILD_SUFFIX)
#else
#define BUILD_DESC                                                             \
    BUILD_DESC_FROM_UNKNOWN(CLIENT_VERSION_MAJOR, CLIENT_VERSION_MINOR,        \
                            CLIENT_VERSION_REVISION, CLIENT_VERSION_BUILD)
#endif
#endif

const std::string CLIENT_BUILD(BUILD_DESC CLIENT_VERSION_SUFFIX);

static std::string FormatVersion(int nVersion) {
    if (nVersion % 100 == 0)
        return strprintf("%d.%d.%d", nVersion / 1000000,
                         (nVersion / 10000) % 100, (nVersion / 100) % 100);
    else
        return strprintf("%d.%d.%d.%d", nVersion / 1000000,
                         (nVersion / 10000) % 100, (nVersion / 100) % 100,
                         nVersion % 100);
}

std::string FormatFullVersion() {
    return CLIENT_BUILD;
}

/**
 * Format the subversion field according to BIP 14 spec
 * (https://github.com/bitcoin/bips/blob/master/bip-0014.mediawiki)
 */
std::string FormatSubVersion(const std::string &name, int nClientVersion,
                             const std::vector<std::string> &comments) {
    std::ostringstream ss;
    ss << "/";
    ss << name << ":" << FormatVersion(nClientVersion);
    if (!comments.empty()) {
        auto it(comments.begin());
        ss << "(" << *it;
        for (++it; it != comments.end(); ++it)
            ss << "; " << *it;
        ss << ")";
    }
    ss << "/";
    return ss.str();
}

/**
 * Split the Sub Version string such as /DeVault Core:1.0.1(EB32.0)/
 * in order to get integer version number
 */
int UnformatSubVersion(const std::string &name) {

    std::vector<std::string> split1;
    std::vector<std::string> split2;
    std::vector<std::string> ver_str;
    
    // throw away /DeVault Core: part
    Split(split1, name, ":");
    // throw away (.../ part
    if (split1.size() == 2) Split(split2, split1[1], "(");
    // Get individual numbers
    if (split2.size() == 2) Split(ver_str, split2[0], ".");

    if (ver_str.size() == 3) {
      int CLIENT_VER = 0;
      try {
        CLIENT_VER =  1000000 * std::stoi(ver_str[0]) + 10000 * std::stoi(ver_str[1]) + 100 * std::stoi(ver_str[2]);
        return CLIENT_VER;
      }
      catch (...) {
        return 0;
      }
    } else {
      return 0;
    }
}
