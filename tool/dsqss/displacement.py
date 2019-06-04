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

import codecs
from itertools import product

import numpy as np

from .util import tagged


class Displacement:
    def __init__(self, lat, distance_only=False):
        self.lat = lat
        self.nr = 0
        sources = range(lat.nsites)
        self.displacements = -np.ones((lat.nsites, lat.nsites), dtype=int)
        self.nelems = lat.nsites * lat.nsites
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
                    rdict[r] = self.nr
                    self.nr += 1
                self.displacements[s, t] = rdict[r]

    def write_xml(self, filename):
        lat = self.lat
        with codecs.open(filename, "w", "utf-8") as f:
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write("<Displacements>\n")
            f.write(tagged("Comment", lat.name))
            f.write(tagged("NumberOfKinds", self.nr))
            f.write(tagged("NumberOfSites", lat.nsites))
            f.write("\n")
            f.write("<!-- <R> [kind] [isite] [jsite] </R> -->\n")
            f.write("\n")

            for s, t in product(range(lat.nsites), range(lat.nsites)):
                f.write(tagged("R",
                               (self.displacements[s, t], s, t)))

            f.write("</Displacements>\n")
