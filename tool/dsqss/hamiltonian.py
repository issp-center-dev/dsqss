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

import typing
from typing import (
    Union,
    Iterable,
    List,
    Tuple,
    Optional,
    Sequence,
    MutableMapping,
    Dict,
    NamedTuple,
)

import itertools
from copy import deepcopy

import toml

import dsqss
import dsqss.lattice
import dsqss.util


State = Union[int, Iterable[int]]


class States(NamedTuple):
    initial: Tuple[int, ...]
    final: Tuple[int, ...]


MatElems = Dict[States, float]


def keystate(istate: State, fstate: State) -> States:
    ret: List[Tuple[int, ...]] = []
    for st in (istate, fstate):
        if isinstance(st, int):
            ret.append((st,))
        else:
            ret.append(tuple(st))
    return States(initial=ret[0], final=ret[1])


def append_matelem(
    matelems: MatElems,
    state: Optional[State] = None,
    istate: Optional[State] = None,
    fstate: Optional[State] = None,
    value: Optional[float] = None,
    param: Optional[MutableMapping] = None,
) -> MatElems:
    if param is None:
        if value is None:
            raise RuntimeError("value is required")
        if istate is not None and fstate is not None:
            matelems[keystate(istate, fstate)] = float(value)
        elif state is not None:
            matelems[keystate(state, state)] = float(value)
        else:
            raise RuntimeError("(istate and fstate) or state is required")
    else:
        if "value" not in param:
            raise RuntimeError("value is required")
        if "istate" in param and "fstate" in param:
            matelems[keystate(param["istate"], param["fstate"])] = float(param["value"])
        elif "state" in param:
            matelems[keystate(param["state"], param["state"])] = float(param["value"])
        else:
            raise RuntimeError("(istate and fstate) or state is required")
    return matelems


def matelems_todict(matelems: MatElems, keepzero: bool = False) -> List[Dict]:
    return [
        {"istate": list(key[0]), "fstate": list(key[1]), "value": value}
        for key, value in matelems.items()
        if keepzero or value != 0.0
    ]


class Site(object):
    id: int
    N: int
    values: List
    elements: MatElems
    sources: MatElems

    def __init__(
        self,
        param: Optional[MutableMapping] = None,
        id=None,
        N=None,
        values=None,
        elements=None,
        sources=None,
    ):
        if param is not None:
            self.id = param["type"]
            self.N = param["N"]
            self.values = param["values"]
            self.elements = {}
            for x in param["elements"]:
                append_matelem(self.elements, param=x)
            self.sources = {}
            for x in param["sources"]:
                append_matelem(self.sources, param=x)
        else:
            self.id = id
            self.N = N
            self.values = values
            self.elements = elements
            self.sources = sources

    def to_dict(self) -> Dict:
        return {
            "type": self.id,
            "N": self.N,
            "values": self.values,
            "elements": matelems_todict(self.elements),
            "sources": matelems_todict(self.sources),
        }


class Interaction(object):
    id: int
    nbody: int
    Ns: List[int]
    elements: MatElems

    def __init__(
        self,
        param: Optional[MutableMapping] = None,
        id=None,
        nbody=None,
        Ns=None,
        elements=None,
    ):
        if param is not None:
            self.id = param["type"]
            self.nbody = param["nbody"]
            self.Ns = param["N"]
            self.elements = {}
            for x in param["elements"]:
                append_matelem(self.elements, param=x)
        else:
            self.id = id
            self.nbody = nbody
            self.Ns = Ns
            self.elements = elements

    def to_dict(self) -> Dict:
        return {
            "type": self.id,
            "nbody": self.nbody,
            "N": self.Ns,
            "elements": matelems_todict(self.elements),
        }


class IndeedInteraction(object):
    itype: int
    stypes: List[int]
    nbody: int
    elements: MatElems

    def __init__(
        self,
        sites: Sequence[Site],
        ints: Sequence[Interaction],
        v: dsqss.lattice.Vertex,
    ):
        inter = ints[v.int_type]
        zs = v.zs

        self.itype = v.v_id
        self.stypes = v.site_types
        self.nbody = len(self.stypes)
        self.elements = deepcopy(inter.elements)

        site_elems = [deepcopy(sites[stype].elements) for stype in self.stypes]

        for state in itertools.product(*map(range, inter.Ns)):
            val = 0.0
            for st, els, z in zip(state, site_elems, zs):
                val += els.get(keystate(st, st), 0.0) / z
            key = keystate(state, state)
            if key in self.elements:
                self.elements[key] += val
            else:
                self.elements[key] = val


class Hamiltonian:
    name: str
    nstypes: int
    sites: List[Site]
    nitypes: int
    interactions: List[Interaction]

    def __init__(self, ham_dict: Union[str, MutableMapping]) -> None:
        if isinstance(ham_dict, str):
            ham_dict = toml.load(ham_dict)
        self.name = ham_dict.get("name", "")

        self.nstypes = len(ham_dict["sites"])
        sites: List[Optional[Site]] = [None for i in range(self.nstypes)]
        for site in ham_dict["sites"]:
            S = Site(site)
            sites[S.id] = S
        for site in sites:
            if site is None:
                raise RuntimeError("")
        self.sites = typing.cast(List[Site], sites)

        self.nitypes = len(ham_dict["interactions"])
        interactions: List[Optional[Interaction]] = [None for i in range(self.nitypes)]
        for inter in ham_dict["interactions"]:
            Int = Interaction(inter)
            interactions[Int.id] = Int

        for inter in interactions:
            if inter is None:
                raise RuntimeError("")
        self.interactions = typing.cast(List[Interaction], interactions)

        self.nxmax = 0
        for site in self.sites:
            self.nxmax = max(site.N, self.nxmax)

    def to_dict(self) -> Dict:
        return {
            "name": self.name,
            "sites": list(map(lambda x: x.to_dict(), self.sites)),
            "interactions": list(map(lambda x: x.to_dict(), self.interactions)),
        }

    def write_toml(self, filename: dsqss.util.Filename) -> None:
        with open(filename, "w", encoding="utf_8") as f:
            toml.dump(self.to_dict(), f)


class GraphedHamiltonian(Hamiltonian):
    indeed_interactions: List[IndeedInteraction]

    def __init__(self, ham, lat) -> None:
        if isinstance(ham, Hamiltonian):
            super().__init__(ham.to_dict())
        else:
            super().__init__(ham)
        self.load_lattice(lat)

    def load_lattice(self, lat) -> None:
        if type(lat) == str:
            lat = dsqss.lattice.Lattice(lat)
        self.indeed_interactions = [
            IndeedInteraction(self.sites, self.interactions, v) for v in lat.vertices
        ]
        self.nitypes = len(self.indeed_interactions)

    def write_xml(self, filename: dsqss.util.Filename) -> None:
        tagged = dsqss.util.tagged
        with open(filename, "w", encoding="utf_8") as f:
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write("<Hamiltonian>\n")
            indent = "  "
            level = 1

            f.write(indent * level + "<General>\n")
            level += 1
            f.write(indent * level + tagged("Comment", self.name))
            f.write(indent * level + tagged("NSTYPE", self.nstypes))
            f.write(indent * level + tagged("NITYPE", self.nitypes))
            f.write(indent * level + tagged("NXMAX", self.nxmax))
            level -= 1
            f.write(indent * level + "</General>\n")
            f.write("\n")

            for site in self.sites:
                f.write(indent * level + "<Site>\n")
                level += 1
                f.write(indent * level + tagged("STYPE", site.id))
                f.write(indent * level + tagged("TTYPE", 0))
                f.write(indent * level + tagged("NX", site.N))
                level -= 1
                f.write(indent * level + "</Site>\n")
                f.write("\n")

            for site in self.sites:
                f.write(indent * level + "<Source>\n")
                level += 1
                f.write(indent * level + tagged("STYPE", site.id))
                f.write(indent * level + tagged("TTYPE", 0))
                for elem in site.sources:
                    if site.sources[elem] == 0.0:
                        continue
                    f.write(
                        indent * level
                        + tagged(
                            "Weight", [elem[0][0], elem[1][0], abs(site.sources[elem])]
                        )
                    )
                level -= 1
                f.write(indent * level + "</Source>\n")
                f.write("\n")

            for interaction in self.indeed_interactions:
                nbody = interaction.nbody
                f.write(indent * level + "<Interaction>\n")
                level += 1
                f.write(indent * level + tagged("ITYPE", interaction.itype))
                f.write(indent * level + tagged("NBODY", nbody))
                f.write(indent * level + tagged("STYPE", interaction.stypes))

                for elem, v in interaction.elements.items():
                    v = -v
                    if v == 0.0:
                        continue
                    f.write(indent * level + "<Weight> ")
                    for i, j in zip(elem[0], elem[1]):
                        f.write("{0} {1} ".format(i, j))
                    if elem[0] != elem[1]:
                        v = abs(v)
                    f.write("{0:0< 18} ".format(v))
                    f.write("</Weight>\n")

                level -= 1
                f.write(indent * level + "</Interaction>\n")
                f.write("\n")

            level -= 1
            f.write("</Hamiltonian>\n")
