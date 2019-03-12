from __future__ import print_function

import codecs
import sys
from itertools import chain

import toml

import numpy as np

from .util import ERROR, WARN, coord2index, get_as_list, index2coord, tagged


class Site:
    def __init__(self, site_id, site_type, coordinate):
        self.id = site_id
        self.stype = site_type
        self.coord = coordinate
        self.z = 0

    def __str__(self):
        s = "{0} {1}".format(self.id, self.stype)
        for c in self.coord:
            s += " {0}".format(c)
        return s


class Interaction:
    def __init__(self, int_id, int_type, nbody, site_indices, edge_flag, direction):
        self.id = int_id
        self.itype = int_type
        self.nbody = nbody
        self.sites = site_indices
        self.edge = edge_flag
        self.dir = direction
        self.vtype = -1

    def __str__(self):
        s = "{0} {1} {2}".format(self.id, self.itype, self.nbody)
        edgeflag = 1 if self.edge >= 0 else 0
        for site in self.sites:
            s += " {0}".format(site)
        s += " {0} {1}".format(edgeflag, self.dir)
        return s


class Vertex:
    def __init__(self, v_id, int_type, site_types, zs):
        self.v_id = v_id
        self.int_type = int_type
        self.site_types = site_types
        self.zs = zs


class Lattice:
    def __init__(self, inp=None):
        if inp is not None:
            self.load(inp)

    def load(self, inp):
        if type(inp) is dict:
            self.load_dict(inp)
        elif inp.endswith(".toml"):
            self.load_dict(toml.load(inp))
        else:
            self.load_dat(inp)

    def load_dict(self, param):
        if "lattice" in param:
            return self.load_dict(param["lattice"])

        parameter = param["parameter"]
        self.name = parameter["name"]
        self.dim = parameter["dim"]
        self.size = parameter["L"]
        self.bc = get_as_list(parameter, "bc", default=True, extendto=self.dim)
        ncells = np.product(self.size)
        self.latvec = np.array(parameter["basis"], dtype=float).transpose()

        unitcell = param["unitcell"]
        nsites_cell = len(unitcell["sites"])
        nbonds_cell = len(unitcell["bonds"])
        self.nsites = ncells * nsites_cell
        self.nints = ncells * nbonds_cell

        localsitecoords = []
        for site in unitcell["sites"]:
            localsitecoords.append(np.array(site["coord"], dtype=float))

        directionmap = {}
        directions = []
        self.dirs = []
        for ib, bond in enumerate(unitcell["bonds"]):
            if "offset" in bond["source"]:
                if not all(bond["source"]["offset"] == 0):
                    ERROR("bonds['source'] has nonzero offset")

            offset = np.array(
                get_as_list(bond["target"], "offset", default=0.0, extendto=self.dim),
                dtype = float
            )
            offset += localsitecoords[bond["target"]["siteid"]]
            offset -= localsitecoords[bond["source"]["siteid"]]
            key = tuple(offset)
            if key not in directionmap:
                self.dirs.append(np.dot(self.latvec, offset))
                directionmap[key] = len(directionmap)
            directions.append(directionmap[key])
        self.ndir = len(self.dirs)

        def bondsites(center_cellcoord, bond):
            ret = []
            offset = center_cellcoord + np.array(
                get_as_list(bond["target"], "offset", default=0, extendto=self.dim),
            )
            edge = 0
            for dim in range(self.dim):
                if offset[dim] < 0:
                    if self.bc[dim]:
                        edge = 1
                    else:
                        return False, 0
                elif offset[dim] >= self.size[dim]:
                    if self.bc[dim]:
                        edge = 1
                    else:
                        return False, 0

            for site in (bond["source"], bond["target"]):
                cell_coord = center_cellcoord + np.array(
                    get_as_list(site, "offset", default=0, extendto=self.dim),
                )
                for dim in range(self.dim):
                    if cell_coord[dim] < 0:
                        cell_coord[dim] += self.size[dim]
                    elif cell_coord[dim] >= self.size[dim]:
                        cell_coord[dim] -= self.size[dim]
                icell = coord2index(cell_coord, self.size)
                sid = site["siteid"] + icell * nsites_cell
                ret.append(sid)
            if ret[0] == ret[1]:
                WARN("selfloop {0}=>{0} appears.".format(ret[0]))
            return ret, edge

        self.sites = []
        self.ints = []
        for icell in range(ncells):
            cell_coord = np.array(index2coord(icell, self.size))
            for lid, site in enumerate(unitcell["sites"]):
                sid = site["siteid"] + icell * nsites_cell
                coord = np.dot(self.latvec, cell_coord + np.array(site["coord"], dtype=float))
                S = Site(sid, site["type"], coord)
                self.sites.append(S)
            for ib, bond in enumerate(unitcell["bonds"]):
                bid = bond["bondid"] + icell * nbonds_cell
                nbody = 2
                sites, edge = bondsites(cell_coord, bond)
                if not sites:
                    continue
                INT = Interaction(bid, bond["type"], nbody, sites, edge, directions[ib])
                self.ints.append(INT)
        self.update()

    def save_dat(self, out):
        if type(out) is str:
            with codecs.open(out, "w", "utf-8") as f:
                self.save_dat(f)
                return

        out.write("name\n")
        out.write("{0}\n".format(self.name))
        out.write("\n")

        out.write("lattice\n")
        out.write("{0} # dim\n".format(self.dim))
        for x in self.size:
            out.write("{0} ".format(x))
        out.write("# size\n")
        for x in self.bc:
            out.write("{0} ".format(x))
        out.write("# 0:open boundary, 1:periodic boundary\n")
        for d in range(self.dim):
            out.write("{0} ".format(d))
            for x in self.latvec[:, d]:
                out.write("{0} ".format(x))
            out.write("# latvec_{0}\n".format(d))
        out.write("\n")

        out.write("directions\n")
        out.write("{0} # ndirections\n".format(self.ndir))
        out.write("# id, coords...\n")
        for i, dir in enumerate(self.dirs):
            out.write("{0} ".format(i))
            for x in dir:
                out.write("{0} ".format(x))
            out.write("\n")
        out.write("\n")

        out.write("sites\n")
        out.write("{0} # nsites\n".format(self.nsites))
        out.write("# id, type, coord...\n")
        for i, site in enumerate(self.sites):
            out.write("{0}\n".format(site))
        out.write("\n")

        out.write("interactions\n")
        out.write("{0} # nints\n".format(self.nints))
        out.write("# id, type, nbody, sites..., edge_flag, direction\n")
        for i, inter in enumerate(self.ints):
            out.write("{0}\n".format(inter))

    def load_dat(self, inp):
        if type(inp) is str:
            with open(inp) as f:
                self.load_dat(f)
                return

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
            elif state == "dim":
                self.dim = int(body)
                self.size = None
                self.bc = [None] * self.dim
                self.latvec = np.eye(self.dim)
                state = "size"
            elif state == "size":
                self.size = list(map(int, body.split()))
                if len(self.size) != self.dim:
                    if len(self.size) < self.dim:
                        ERROR("too few elements ({0})".format(body))
                    else:
                        ERROR("too many elements ({0})".format(body))
                state = "bc"
            elif state == "bc":
                elem = body.split()
                if len(elem) != self.dim:
                    if len(elem) < self.dim:
                        ERROR("too few elements ({0})".format(body))
                    else:
                        ERROR("too many elements ({0})".format(body))
                self.bc[:] = list(map(int, elem))
                state = "latvec"
            elif state == "latvec":
                self.load_latvec(body, count)
                count += 1
                if count == self.dim:
                    count = 0
                    break

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
            elif state == "name":
                self.name = body
                state = "waiting"
            elif state == "ndir":
                self.ndir = int(body)
                self.dirs = [None for i in range(self.ndir)]
                state = "dirs"
                count = 0
            elif state == "dirs":
                self.load_direction(body, count)
                count += 1
                if count == self.dim:
                    count = 0
                    state = "waiting"
            elif state == "nsites":
                self.nsites = int(body)
                state = "sites"
                self.sites = [None for i in range(self.nsites)]
            elif state == "sites":
                self.load_site(body, count)
                count += 1
                if count == self.nsites:
                    state = "waiting"
                    count = 0
            elif state == "nints":
                self.nints = int(body)
                state = "ints"
                self.ints = [None for i in range(self.nints)]
            elif state == "ints":
                self.load_int(body, count)
                count += 1
                if count == self.nints:
                    state = "waiting"
                    count = 0

        # end of for line in inp:

        self.check_all()
        self.update()

    def load_latvec(self, body, count):
        elem = body.split()
        if len(elem) != self.dim + 1:
            if len(elem) < self.dim + 1:
                ERROR("too few elements ({0})".format(body))
            else:
                ERROR("too many elements ({0})".format(body))
        self.latvec[:, int(elem[0])] = list(map(float, elem[1:]))

    def load_direction(self, body, count):
        elem = body.split()
        if len(elem) != self.dim + 1:
            if len(elem) < self.dim + 1:
                ERROR("too few elements ({0})".format(body))
            else:
                ERROR("too many elements ({0})".format(body))
        self.dirs[int(elem[0])] = list(map(float, elem[1:]))

    def load_site(self, body, count):
        elem = body.split()
        if len(elem) != self.dim + 3:
            if len(elem) < self.dim + 3:
                ERROR("too few elements ({0})".format(body))
            else:
                ERROR("too many elements ({0})".format(body))
        self.sites[int(elem[0])] = Site(
            site_id=int(elem[0]),
            site_type=int(elem[1]),
            coordinate=list(map(float, elem[2:])),
        )

    def load_int(self, body, count):
        elem = body.split()
        nbody = int(elem[2])
        if len(elem) != nbody + 5:
            if len(elem) < nbody + 5:
                ERROR("too few elements ({0})".format(body))
            else:
                ERROR("too many elements ({0})".format(body))
        self.ints[int(elem[0])] = Interaction(
            int_id=int(elem[0]),
            int_type=int(elem[1]),
            nbody=nbody,
            site_indices=list(map(int, elem[3 : (3 + nbody)])),
            edge_flag=int(elem[-2]),
            direction=int(elem[-1]),
        )

    def check_all(self):
        self.check_latvec()
        self.check_dirs()
        self.check_sites()
        self.check_ints()

    def check_latvec(self):
        invalid = False
        for i in range(self.dim):
            if self.latvec[i] is None:
                ERROR("lattice vector {0} is not defined".format(i), True)
                invalid = True
        if invalid:
            sys.exit(1)

    def check_dirs(self):
        invalid = False
        for i in range(self.ndir):
            if self.dirs[i] is None:
                ERROR("bond direction {0} is not defined".format(i), True)
                invalid = True
        if invalid:
            sys.exit(1)

    def check_sites(self):
        invalid = False
        for i in range(self.nsites):
            if self.sites[i] is None:
                ERROR("site {0} is not defined".format(i), True)
                invalid = True
        if invalid:
            sys.exit(1)

    def check_ints(self):
        invalid = False
        for i in range(self.nints):
            if self.ints[i] is None:
                ERROR("interaction {0} is not defined".format(i), True)
                invalid = True
        if invalid:
            sys.exit(1)

    def update(self):
        self.nstypes = 0
        for s in self.sites:
            s.z = 0
            self.nstypes = max(self.nstypes, s.stype)
        self.nstypes += 1

        self.nedges = 0
        for inter in self.ints:
            if inter.edge == 1:
                inter.edge = self.nedges
                self.nedges += 1
                pass
            else:
                inter.edge = -1
                pass
            for s in inter.sites:
                self.sites[s].z += 1

        self.nitypes = 0
        self.nvtypes = 0
        self.vertices = []
        vmap = {}
        for inter in self.ints:
            itype = inter.itype
            self.nitypes = max(self.nitypes, itype)
            stypes = tuple(self.sites[s].stype for s in inter.sites)
            zs = tuple(self.sites[s].z for s in inter.sites)
            if (itype, stypes, zs) in vmap:
                inter.itype = vmap[(itype, stypes, zs)]
            else:
                inter.itype = vmap[(itype, stypes, zs)] = self.nvtypes
                self.vertices.append(Vertex(self.nvtypes, itype, stypes, zs))
                self.nvtypes += 1
        self.nitypes += 1

    def write_xml(self, filename):
        with codecs.open(filename, "w", "utf_8") as f:
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

    def write_gnuplot(self, filename):
        with open(filename, 'w') as f:
            for st in range(self.nstypes):
                f.write('$SITES_{0} << EOD\n'.format(st))
                for site in self.sites:
                    if site.stype != st:
                        continue
                    v = site.coord
                    for x in site.coord:
                        f.write('{0} '.format(x))
                    f.write('{0}\n'.format(site.stype))
                f.write('EOD\n')

            for bt in range(self.nitypes):
                f.write('$BONDS_{0} << EOD\n'.format(bt))
                for bond in self.ints:
                    if bond.nbody != 2:
                        continue
                    if bond.itype != bt:
                        continue
                    v = np.array(self.sites[bond.sites[0]].coord, dtype=float)
                    for x in v:
                        f.write('{0} '.format(x))
                    f.write('\n')
                    v += np.array(self.dirs[bond.dir], dtype=float)
                    for x in v:
                        f.write('{0} '.format(x))
                    f.write('\n\n')
                f.write('EOD\n')

            f.write('plot ')
            for st in range(self.nstypes):
                f.write('$SITES_{0} w p pt {1} ps 2 t "" , \\\n'.format(st, st+4))
            for bt in range(self.nitypes):
                f.write('$BONDS_{0} w l lw 2 lt {1} t "" , \\\n'.format(bt, bt+1))
