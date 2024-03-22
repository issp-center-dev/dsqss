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

from typing import TextIO, Union, List, MutableMapping, Optional

import os
import sys
from copy import deepcopy

Filename = Union[str, bytes, os.PathLike]


def LOG_IMPL(
    msg: str, typ: str, to_be_continued: bool, linebreak: bool, file: TextIO
) -> None:
    print(f"{typ}: {msg}\n", file=file)
    if linebreak:
        print("\n", file=file)
    if not to_be_continued:
        sys.exit(1)


def INFO(msg: str, linebreak: bool = True, file: TextIO = sys.stderr) -> None:
    LOG_IMPL(msg, "INFO", linebreak=linebreak, to_be_continued=True, file=file)


def WARN(msg: str, linebreak: bool = True, file: TextIO = sys.stderr) -> None:
    LOG_IMPL(msg, "WARN", linebreak=linebreak, to_be_continued=True, file=file)


def ERROR(
    msg: str,
    to_be_continued: bool = False,
    linebreak: bool = True,
    file: TextIO = sys.stderr,
):
    LOG_IMPL(
        msg, "ERROR", linebreak=linebreak, to_be_continued=to_be_continued, file=file
    )
    pass


def extend_list(lst: List, N: int) -> List:
    last = lst[-1]
    for i in range(N - len(lst)):
        lst.append(deepcopy(last))
    return lst


def get_as_list(
    d: MutableMapping, name, default=None, extendto: Optional[int] = None
) -> List:
    if default is None:
        v = d[name]
    else:
        v = d.get(name, default)
    if not isinstance(v, list):
        v = [v]
    if extendto is not None:
        extend_list(v, extendto)
    return v


def index2coord(index: int, size: List) -> List:
    D = len(size)
    i = 0
    r = [0 for d in range(D)]
    while index > 0:
        r[i] = index % size[i]
        index //= size[i]
        i += 1
    return r


def coord2index(r: List, size: List) -> int:
    index = 0
    block = 1
    for x, L in zip(r, size):
        index += block * x
        block *= L
    return index


def tagged(tagname: str, values) -> str:
    if not hasattr(type(values), "__iter__") or isinstance(values, str):
        values = [values]
    s = "<{0}>".format(tagname)
    for v in values:
        if type(v) is float:
            s += " {0:0< 18}".format(v)
        else:
            s += " {0}".format(v)
    s += " </{0}>\n".format(tagname)
    return s
