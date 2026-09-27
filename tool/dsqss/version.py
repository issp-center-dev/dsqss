# DSQSS (Discrete Space Quantum Systems Solver)
# Copyright (C) 2018- The University of Tokyo
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""Version information of DSQSS

The version number is set in the top-level CMakeLists.txt, and nowhere else.
CMake writes it to ``_version.py`` together with the commit hash when DSQSS is
built. Without ``_version.py``, that is, when the package is used in the
source tree, they are taken from CMakeLists.txt and the repository.
"""

import argparse
import os
import re
import subprocess
from typing import Optional

UNKNOWN = "unknown"
HASH_LENGTH = 8


def source_root() -> Optional[str]:
    """Top directory of the source tree of DSQSS, if the package is in it"""
    root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
    try:
        with open(os.path.join(root, "CMakeLists.txt"), encoding="utf_8") as f:
            if re.search(r"^\s*project\s*\(\s*DSQSS\b", f.read(), re.M) is None:
                return None
    except OSError:
        return None
    return root


def version_from_cmakelists(root: str) -> str:
    """Version number set in CMakeLists.txt, without the leading 'v'"""
    try:
        with open(os.path.join(root, "CMakeLists.txt"), encoding="utf_8") as f:
            m = re.search(
                r"^\s*set\s*\(\s*DSQSS_VERSION\s+\"?([^\s\"()]+)\"?\s*\)",
                f.read(),
                re.M,
            )
    except OSError:
        return UNKNOWN
    if m is None:
        return UNKNOWN
    return re.sub(r"^[vV]", "", m.group(1))


def _shorten(githash: str) -> str:
    githash = githash.strip()
    if re.fullmatch(r"[0-9a-f]{%d,}" % HASH_LENGTH, githash) is None:
        return UNKNOWN
    return githash[:HASH_LENGTH]


def _git(root: str, *args: str) -> Optional[str]:
    try:
        res = subprocess.run(
            ("git",) + args,
            cwd=root,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            universal_newlines=True,
        )
    except OSError:
        return None
    if res.returncode != 0:
        return None
    return res.stdout.strip()


def git_hash_from_source(root: str) -> str:
    """Commit hash of the source tree

    It is that of HEAD in a git repository, followed by "-dirty" if files
    under the version control have changes which are not committed.
    In a tarball, it is that of the commit which the tarball was made from,
    which "git archive" fills in .git_archival.txt.
    """
    if os.path.exists(os.path.join(root, ".git")):
        githash = _shorten(_git(root, "rev-parse", "HEAD") or "")
        if githash == UNKNOWN:
            return UNKNOWN
        if _git(root, "status", "--porcelain", "--untracked-files=no"):
            githash += "-dirty"
        return githash

    try:
        with open(os.path.join(root, ".git_archival.txt"), encoding="utf_8") as f:
            # "$Format:%H$" is left if it is not filled in
            return _shorten(f.readline())
    except OSError:
        return UNKNOWN


def _installed_version() -> str:
    try:
        from importlib.metadata import version
    except ImportError:
        return UNKNOWN
    try:
        return version("dsqss")
    except Exception:
        return UNKNOWN


try:
    from ._version import __version__, git_hash as _git_hash
except ImportError:
    _root = source_root()
    _git_hash = None
    if _root is not None:
        __version__ = version_from_cmakelists(_root)
    else:
        __version__ = _installed_version()
        _git_hash = UNKNOWN


def git_hash() -> str:
    """First 8 digits of the commit hash with '-dirty' if any, or 'unknown'"""
    global _git_hash
    if _git_hash is None:
        _git_hash = git_hash_from_source(_root)
    return _git_hash


def version_string() -> str:
    """Version with the commit hash, as 'v2.1.0 (7b79e710)'"""
    return "v{} ({})".format(__version__, git_hash())


class VersionAction(argparse.Action):
    """Action of argparse to show the version and exit

    The commit hash is looked up only when the option is given.
    """

    def __init__(
        self,
        option_strings,
        dest=argparse.SUPPRESS,
        default=argparse.SUPPRESS,
        help="show the version and exit",
    ):
        super().__init__(
            option_strings=option_strings,
            dest=dest,
            default=default,
            nargs=0,
            help=help,
        )

    def __call__(self, parser, namespace, values, option_string=None):
        print(version_string())
        parser.exit()
