from __future__ import print_function

import codecs
import sys
from itertools import chain

import numpy as np

from .util import ERROR, tagged


class Site:
    def __init__(self, site_id, site_type, measure_type, coordinate):
        self.id = site_id
        self.stype = site_type
        self.mtype = measure_type
        self.coord = coordinate
        self.z = 0

    def __str__(self):
        s = "{0} {1} {2}".format(self.id, self.stype, self.mtype)
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
    def __init__(self, inp):
        self.load(inp)

    def save(self, out):
        if type(out) is str:
            with codecs.open(out, "w", "utf-8") as f:
                self.save(f)
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
        for i, dir in enumerate(self.directions):
            out.write("{0} ".format(i))
            for x in dir:
                out.write("{0} ".format(x))
            out.write("\n")
        out.write("\n")

        out.write("sites\n")
        out.write("{0} # nsites\n".format(self.nsites))
        out.write("# id, type, mtype, coord...\n")
        for i, site in enumerate(self.sites):
            out.write("{0}\n".format(site))
        out.write("\n")

        out.write("interactions\n")
        out.write("{0} # nints\n".format(self.nints))
        out.write("# id, type, nbody, sites..., edge_flag, direction\n")
        for i, inter in enumerate(self.ints):
            out.write("{0}\n".format(inter))

    def load(self, inp):
        if type(inp) is str:
            with open(inp) as f:
                self.load(f)
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
            measure_type=int(elem[2]),
            coordinate=list(map(float, elem[3:])),
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

        self.nvtypes = 0
        self.vertices = []
        vmap = {}
        for inter in self.ints:
            itype = inter.itype
            stypes = tuple(self.sites[s].stype for s in inter.sites)
            zs = tuple(self.sites[s].z for s in inter.sites)
            if (itype, stypes, zs) in vmap:
                inter.itype = vmap[(itype, stypes, zs)]
            else:
                inter.itype = vmap[(itype, stypes, zs)] = self.nvtypes
                self.vertices.append(Vertex(self.nvtypes, itype, stypes, zs))
                self.nvtypes += 1

    def write_xml(self, filename):
        with codecs.open(filename, "w", "utf_8") as f:
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write("<LATTICE>\n")

            f.write(tagged("Comment", self.name))

            f.write(tagged("Dimension", self.dim))
            f.write(tagged("BondDimension", self.dim))
            f.write(tagged("LinearSize", self.size))
            f.write(tagged("BoundaryCondition", self.bc))
            f.write(tagged("NumberOfSites", self.nsites))
            f.write(tagged("NumberOfInteractions", self.nints))
            f.write(tagged("NumberOfSiteTypes", self.nstypes))
            f.write(tagged("NumberOfInteractionTypes", self.nvtypes))
            f.write(tagged("NumberOfEdgeInteractions", self.nedges))
            f.write("\n")

            f.write("<!-- <S> site_index site_type measure_type </S> -->\n")
            for i, site in enumerate(self.sites):
                f.write(tagged("S", (i, site.stype, site.mtype)))
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

            f.write("<!-- <V> direction_index direction... </V> -->\n")
            for i, bond in enumerate(self.latvec):
                f.write(tagged("V", chain([i], bond)))

            f.write("</LATTICE>\n")
