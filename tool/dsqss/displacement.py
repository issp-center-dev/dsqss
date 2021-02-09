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

import itertools

import numpy as np

import dsqss
import dsqss.util
import dsqss.lattice


class Displacement:
    latname: str
    nsites: int
    nkinds: int
    displacements: np.ndarray

    def __init__(self, lat: dsqss.lattice.Lattice, distance_only: bool = False):
        self.latname = lat.name
        self.nsites = lat.nsites
        self.nkinds = 0
        sources = range(lat.nsites)
        self.displacements = -np.ones((lat.nsites, lat.nsites), dtype=np.int64)
        rdict = {}
        for s in sources:
            sr = np.array(lat.sites[s].coord)
            for t in range(lat.nsites):
                dr = np.array(lat.sites[t].coord) - sr
                for d in range(lat.dim):
                    if lat.bc[d] == 1 and dr[d] <= -lat.size[d] // 2:
                        dr[d] += lat.size[d]
                    elif lat.bc[d] == 1 and dr[d] > lat.size[d] // 2:
                        dr[d] -= lat.size[d]
                if distance_only:
                    r = sum(map(lambda x: x * x, np.dot(lat.latvec, dr)))
                else:
                    r = tuple(dr)
                if r not in rdict:
                    rdict[r] = self.nkinds
                    self.nkinds += 1
                self.displacements[s, t] = rdict[r]

    def write_xml(self, filename: dsqss.util.Filename):
        tagged = dsqss.util.tagged
        with open(filename, "w", encoding="utf-8") as f:
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write("<Displacements>\n")
            f.write(tagged("Comment", self.latname))
            f.write(tagged("NumberOfKinds", self.nkinds))
            f.write(tagged("NumberOfSites", self.nsites))
            f.write("\n")
            f.write("<!-- <R> [kind] [isite] [jsite] </R> -->\n")
            f.write("\n")

            for s, t in itertools.product(range(self.nsites), range(self.nsites)):
                f.write(tagged("R", (self.displacements[s, t], s, t)))

            f.write("</Displacements>\n")
