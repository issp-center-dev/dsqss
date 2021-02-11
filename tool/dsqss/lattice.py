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
from typing import List, MutableMapping, Tuple, Dict, Iterable

import sys
from itertools import chain

import toml

import numpy as np

from dsqss import util
from dsqss.util import tagged


class Site:
    """Site class

    Attributes:
        id: index of site
        stype: site type
        coord: coordination
        z: coordination number (the number of the nearest neighbor sites)
    """

    id: int
    stype: int
    coord: np.ndarray
    z: int

    def __init__(self, site_id: int, site_type: int, coordinate: np.ndarray) -> None:
        self.id = site_id
        self.stype = site_type
        self.coord = coordinate
        self.z = 0

    def __str__(self) -> str:
        s = f"{self.id} {self.stype}"
        for c in self.coord:
            s += f" {c}"
        return s


class Interaction:
    """Interaction class

    Attributes:
        id: index of interaction
        itype: interaction type
        itype_org:
        nbody: the number of involved sites
        sites: list of index of involved sites
        edge: on boundary (1) or not (0)
        direction: index of bond direction
    """

    id: int
    itype: int
    itype_org: int
    nbody: int
    sites: List[int]
    edge: int
    direction: int

    def __init__(
        self,
        int_id: int,
        int_type: int,
        nbody: int,
        site_indices: Iterable[int],
        edge: int,
        direction: int,
    ) -> None:
        self.id = int_id
        self.itype = int_type
        self.itype_org = self.itype
        self.nbody = nbody
        self.sites = list(site_indices)
        self.edge = edge
        self.dir = direction
        self.vtype = -1

    def __str__(self) -> str:
        s = f"{self.id} {self.itype} {self.nbody}"
        edgeflag = 1 if self.edge >= 0 else 0
        for site in self.sites:
            s += f" {site}"
        s += f" {edgeflag} {self.dir}"
        return s


class Vertex:
    """Vertex (sites and interaction)

    Attributes:
        v_id: index of vertex
        int_type: type of interaction
        site_types: type of sites
        zs: coodination numbers

    """

    v_id: int
    int_type: int
    site_types: List[int]
    zs: List[int]

    def __init__(
        self, v_id: int, int_type: int, site_types: Iterable[int], zs: Iterable[int]
    ) -> None:
        self.v_id = v_id
        self.int_type = int_type
        self.site_types = list(site_types)
        self.zs = list(zs)


class Lattice:
    name: str
    dim: int
    size: List[int]
    bc: List[bool]  # periodic if True, open if False
    latvec: np.ndarray
    ndir: int
    dirs: List[np.ndarray]
    sites: List[Site]
    ints: List[Interaction]
    nstypes: int
    nedges: int
    nitypes: int
    nvtypes: int
    vertices: List[Vertex]

    def __init__(self, inp=None) -> None:
        if inp is not None:
            self.load(inp)

    def load(self, inp) -> None:
        if isinstance(inp, dict):
            self.load_dict(inp)
        elif isinstance(inp, str) and inp.endswith(".toml"):
            self.load_dict(toml.load(inp))
        else:
            self.load_dat(inp)

    def load_dict(self, param: MutableMapping) -> None:
        if "lattice" in param and not isinstance(param["lattice"], str):
            self.load_dict(param["lattice"])
            return

        parameter = param["parameter"]
        self.name = parameter["name"]
        self.dim = parameter["dim"]
        self.size = util.get_as_list(parameter, "L", extendto=self.dim)
        self.bc = util.get_as_list(parameter, "bc", default=True, extendto=self.dim)
        self.latvec = np.array(parameter["basis"], dtype=float).transpose()

        unitcell = param["unitcell"]
        nsites_cell = len(unitcell["sites"])

        localsitecoords: List[np.ndarray] = []
        for site in unitcell["sites"]:
            localsitecoords.append(np.array(site["coord"], dtype=float))

        directionmap: Dict[Tuple, int] = {}
        directions: List[int] = []
        self.dirs = []
        for ib, bond in enumerate(unitcell["bonds"]):
            if "offset" in bond["source"]:
                if not all(bond["source"]["offset"] == 0):
                    util.ERROR("bonds['source'] has nonzero offset")

            offset = np.array(
                util.get_as_list(
                    bond["target"], "offset", default=0.0, extendto=self.dim
                ),
                dtype=float,
            )
            offset += localsitecoords[bond["target"]["siteid"]]
            offset -= localsitecoords[bond["source"]["siteid"]]
            key = tuple(offset)
            if key not in directionmap:
                self.dirs.append(offset)
                directionmap[key] = len(directionmap)
            directions.append(directionmap[key])
        self.ndir = len(self.dirs)

        self.sites = []
        self.ints = []
        ncells = np.product(self.size)
        bid = 0
        for icell in range(ncells):
            cell_coord = np.array(util.index2coord(icell, self.size))
            for lid, site in enumerate(unitcell["sites"]):
                sid = site["siteid"] + icell * nsites_cell
                coord = cell_coord + np.array(site["coord"], dtype=float)
                S = Site(sid, site["type"], coord)
                self.sites.append(S)
            for ib, bond in enumerate(unitcell["bonds"]):
                nbody = 2
                sites, edge = self._bondsites(cell_coord, bond, nsites_cell)
                if len(sites) == 0:
                    continue
                INT = Interaction(bid, bond["type"], nbody, sites, edge, directions[ib])
                self.ints.append(INT)
                bid += 1
        self.nsites = len(self.sites)
        self.nints = len(self.ints)
        self.check_all()
        self._update()

    def _bondsites(
        self,
        center_cellcoord: np.ndarray,
        bond: Dict,
        nsites_cell: int,
    ) -> Tuple[List[int], int]:
        offset = center_cellcoord + np.array(
            util.get_as_list(bond["target"], "offset", default=0, extendto=self.dim)
        )
        edge = 0
        for dim in range(self.dim):
            if offset[dim] < 0:
                if self.bc[dim]:
                    edge = 1
                else:
                    return [], 0
            elif offset[dim] >= self.size[dim]:
                if self.bc[dim]:
                    edge = 1
                else:
                    return [], 0
        ret = []
        for site in (bond["source"], bond["target"]):
            cell_coord = center_cellcoord + np.array(
                util.get_as_list(site, "offset", default=0, extendto=self.dim)
            )
            for dim in range(self.dim):
                if cell_coord[dim] < 0:
                    cell_coord[dim] += self.size[dim]
                elif cell_coord[dim] >= self.size[dim]:
                    cell_coord[dim] -= self.size[dim]
            icell = util.coord2index(cell_coord, self.size)
            sid = site["siteid"] + icell * nsites_cell
            ret.append(sid)
        if ret[0] == ret[1]:
            util.WARN("selfloop {0}=>{0} appears.".format(ret[0]))
        return ret, edge

    def save_dat(self, filename: util.Filename) -> None:
        out = open(filename, "w", encoding="utf-8")

        out.write("name\n")
        out.write(f"{self.name}\n")
        out.write("\n")

        out.write("lattice\n")
        out.write(f"{self.dim} # dim\n")
        for x in self.size:
            out.write(f"{x} ")
        out.write("# size\n")
        for x in self.bc:
            out.write(f"{int(x)} ")
        out.write("# 0:open boundary, 1:periodic boundary\n")
        for d in range(self.dim):
            out.write(f"{d} ")
            for x in self.latvec[:, d]:
                out.write(f"{x} ")
            out.write(f"# latvec_{d}\n")
        out.write("\n")

        out.write("directions\n")
        out.write(f"{self.ndir} # ndirections\n")
        out.write("# id, coords...\n")
        for i, dir in enumerate(self.dirs):
            out.write(f"{i} ")
            for x in dir:
                out.write(f"{x} ")
            out.write("\n")
        out.write("\n")

        out.write("sites\n")
        out.write(f"{self.nsites} # nsites\n")
        out.write("# id, type, coord...\n")
        for site in self.sites:
            out.write(f"{site}\n")
        out.write("\n")

        out.write("interactions\n")
        out.write(f"{self.nints} # nints\n")
        out.write("# id, type, nbody, sites..., edge_flag, direction\n")
        for inter in self.ints:
            out.write(f"{inter}\n")
        out.close()
        # end of save_dat

    def load_dat(self, filename: util.Filename) -> None:
        inp = open(filename)

        state = "waiting"
        count = 0
        self.dim = 0
        self.nsites = 0
        self.nints = 0

        # search for 'lattice' because others requires dimension
        for line in inp:
            body = line.split("#")[0].strip()
            if len(body) == 0:
                continue
            if state == "waiting" and body.lower() == "lattice":
                state = "dim"
                continue
            elif state == "dim":
                self.dim = int(body)
                self.latvec = np.eye(self.dim)
                state = "size"
                continue
            elif state == "size":
                elem = body.split()
                if len(elem) != self.dim:
                    if len(elem) < self.dim:
                        util.ERROR(f"too few elements ({body})")
                    else:
                        util.ERROR(f"too many elements ({body})")
                self.size = list(map(int, elem))
                state = "bc"
                continue
            elif state == "bc":
                elem = body.split()
                if len(elem) != self.dim:
                    if len(elem) < self.dim:
                        util.ERROR(f"too few elements ({body})")
                    else:
                        util.ERROR(f"too many elements ({body})")
                self.bc = list(map(bool, elem))
                state = "latvec"
                continue
            elif state == "latvec":
                self._load_latvec(body, count)
                count += 1
                if count == self.dim:
                    count = 0
                    break
        # end of for line in inp:

        # working vars
        dirs = []
        sites = []
        ints = []

        # search for others
        inp.seek(0)
        count = 0
        state = "waiting"
        for line in inp:
            body = line.split("#")[0].strip()
            if len(body) == 0:
                continue

            if state == "waiting":
                if body.lower() == "name":
                    state = "name"
                elif body.lower() == "directions":
                    state = "ndir"
                elif body.lower() == "sites":
                    state = "nsites"
                elif body.lower() == "interactions":
                    state = "nints"
                continue
            elif state == "name":
                self.name = body
                state = "waiting"
                continue
            elif state == "ndir":
                self.ndir = int(body)
                dirs = [None for i in range(self.ndir)]
                state = "dirs"
                count = 0
                continue
            elif state == "dirs":
                self._load_direction(body)
                count += 1
                if count == self.ndir:
                    self.dirs = typing.cast(List[np.ndarray], dirs)
                    count = 0
                    state = "waiting"
                continue
            elif state == "nsites":
                self.nsites = int(body)
                state = "sites"
                sites = [None for i in range(self.nsites)]
                continue
            elif state == "sites":
                self._load_site(body)
                count += 1
                if count == self.nsites:
                    self.sites = typing.cast(List[Site], sites)
                    state = "waiting"
                    count = 0
                continue
            elif state == "nints":
                self.nints = int(body)
                state = "ints"
                ints = [None for i in range(self.nints)]
                continue
            elif state == "ints":
                self._load_int(body)
                count += 1
                if count == self.nints:
                    self.ints = typing.cast(List[Interaction], ints)
                    state = "waiting"
                    count = 0
        # end of for line in inp:
        inp.close()
        self.check_all()
        self._update()

    def _load_latvec(self, body, count):
        elem = body.split()
        if len(elem) != self.dim + 1:
            if len(elem) < self.dim + 1:
                util.ERROR("too few elements ({0})".format(body))
            else:
                util.ERROR("too many elements ({0})".format(body))
        self.latvec[:, int(elem[0])] = list(map(float, elem[1:]))

    def _load_direction(self, body: str) -> None:
        elem = body.split()
        if len(elem) != self.dim + 1:
            if len(elem) < self.dim + 1:
                util.ERROR(f"too few elements ({body})")
            else:
                util.ERROR(f"too many elements ({body})")
        self.dirs[int(elem[0])] = np.array(list(map(float, elem[1:])))

    def _load_site(self, body: str) -> None:
        elem = body.split()
        if len(elem) != self.dim + 2:
            if len(elem) < self.dim + 2:
                util.ERROR(f"too few elements ({body})")
            else:
                util.ERROR(f"too many elements ({body})")
        self.sites[int(elem[0])] = Site(
            site_id=int(elem[0]),
            site_type=int(elem[1]),
            coordinate=np.array(list(map(float, elem[2:]))),
        )

    def _load_int(self, body: str) -> None:
        elem = body.split()
        nbody = int(elem[2])
        if len(elem) != nbody + 5:
            if len(elem) < nbody + 5:
                util.ERROR(f"too few elements ({body})")
            else:
                util.ERROR(f"too many elements ({body})")
        self.ints[int(elem[0])] = Interaction(
            int_id=int(elem[0]),
            int_type=int(elem[1]),
            nbody=nbody,
            site_indices=list(map(int, elem[3 : (3 + nbody)])),
            edge=int(elem[-2]),
            direction=int(elem[-1]),
        )

    def check_all(self):
        num_errors = 0
        num_errors += self._check_latvec()
        num_errors += self._check_dirs()
        num_errors += self._check_sites()
        num_errors += self._check_ints()
        if num_errors > 0:
            sys.exit(1)

    def _check_latvec(self) -> int:
        num_errors = 0
        for i in range(self.dim):
            if self.latvec[i] is None:
                util.ERROR(f"lattice vector {i} is not defined", to_be_continued=True)
                num_errors += 1
        return num_errors

    def _check_dirs(self) -> int:
        num_errors = 0
        for i in range(self.ndir):
            if self.dirs[i] is None:
                util.ERROR(f"bond direction {i} is not defined", to_be_continued=True)
                num_errors += 1
        return num_errors

    def _check_sites(self) -> int:
        num_errors = 0
        for i in range(self.nsites):
            if self.sites[i] is None:
                util.ERROR(f"site {i} is not defined", to_be_continued=True)
                num_errors += 1
        return num_errors

    def _check_ints(self) -> int:
        num_errors = 0
        for i in range(self.nints):
            if self.ints[i] is None:
                util.ERROR(f"interaction {i} is not defined", to_be_continued=True)
                num_errors += 1
        return num_errors

    def _update(self) -> None:
        """ generate vertices"""

        self.nstypes = 0
        for site in self.sites:
            site.z = 0
            self.nstypes = max(self.nstypes, site.stype)
        self.nstypes += 1

        self.nedges = 0
        for inter in self.ints:
            if inter.edge == 1:
                inter.edge = self.nedges
                self.nedges += 1
            else:
                inter.edge = -1
            for s in inter.sites:
                self.sites[s].z += 1

        self.nitypes = 0
        self.nvtypes = 0
        self.vertices = []
        vmap: Dict = {}
        for inter in self.ints:
            itype = inter.itype
            self.nitypes = max(self.nitypes, itype)
            stypes = tuple(self.sites[s].stype for s in inter.sites)
            zs = tuple(self.sites[s].z for s in inter.sites)
            if (itype, stypes, zs) in vmap:
                inter.itype = vmap[(itype, stypes, zs)]
            else:
                inter.itype = vmap[(itype, stypes, zs)] = self.nvtypes
                self.vertices.append(
                    Vertex(self.nvtypes, itype, list(stypes), list(zs))
                )
                self.nvtypes += 1
        self.nitypes += 1

    def write_xml(self, filename: util.Filename) -> None:
        with open(filename, "w", encoding="utf_8") as f:
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write("<LATTICE>\n")

            f.write(tagged("Comment", self.name))

            f.write(tagged("Dimension", self.dim))
            f.write(tagged("LinearSize", self.size))
            f.write(tagged("BoundaryCondition", self.bc))
            f.write(tagged("NumberOfSites", self.nsites))
            f.write(tagged("NumberOfInteractions", self.nints))
            f.write(tagged("NumberOfSiteTypes", self.nstypes))
            f.write(tagged("NumberOfBondDirections", len(self.dirs)))
            f.write(tagged("NumberOfInteractionTypes", self.nvtypes))
            f.write(tagged("NumberOfEdgeInteractions", self.nedges))
            f.write("\n")

            for d in range(self.dim):
                f.write(tagged("Basis", self.latvec[:, d]))
            f.write("\n")

            f.write("<!-- <S> site_index site_type </S> -->\n")
            for i, site in enumerate(self.sites):
                f.write(tagged("S", (i, site.stype)))
            f.write("\n")

            f.write("<!-- <SiteCoordinate> site_index coord... </SiteCoordinate> -->\n")
            for i, site in enumerate(self.sites):
                f.write(tagged("SiteCoordinate", chain([i], site.coord)))
            f.write("\n")

            f.write(
                (
                    "<!-- <I> int_index int_type nbody site_index... "
                    "edge_index direction_index </I> -->\n"
                )
            )
            for i, interaction in enumerate(self.ints):
                f.write(
                    tagged(
                        "I",
                        chain(
                            [i, interaction.itype, len(interaction.sites)],
                            interaction.sites,
                            [interaction.edge, interaction.dir],
                        ),
                    )
                )
            f.write("\n")

            f.write("<!-- <Direction> direction_index direction... </Direction> -->\n")
            for i, bond in enumerate(self.dirs):
                f.write(tagged("Direction", chain([i], bond)))

            f.write("</LATTICE>\n")
            # end of write_xml

    def write_gnuplot(self, filename: util.Filename) -> None:
        with open(filename, "w") as f:
            for st in range(self.nstypes):
                f.write(f"$SITES_{st} << EOD\n")
                for site in self.sites:
                    if site.stype != st:
                        continue
                    v = np.dot(self.latvec, site.coord)
                    for x in v:
                        f.write(f"{x} ")
                    f.write(f"{site.stype}\n")
                f.write("EOD\n")

            for bt in range(self.nitypes):
                f.write(f"$BONDS_{bt} << EOD\n")
                for bond in self.ints:
                    if bond.nbody != 2:
                        continue
                    if bond.itype_org != bt:
                        continue
                    v = np.dot(
                        self.latvec,
                        np.array(self.sites[bond.sites[0]].coord, dtype=float),
                    )
                    for x in v:
                        f.write(f"{x} ")
                    f.write("\n")
                    v += np.dot(self.latvec, np.array(self.dirs[bond.dir], dtype=float))
                    for x in v:
                        f.write(f"{x} ")
                    f.write("\n\n")
                f.write("EOD\n")

            f.write("plot ")
            for st in range(self.nstypes):
                f.write(f'$SITES_{st} w p pt {st+4} ps 2 t "" , \\\n')
            for bt in range(self.nitypes):
                f.write(f'$BONDS_{bt} w l lw 2 lt {bt+1} t "" , \\\n')
            f.write("\n")
            f.write("pause -1\n")
