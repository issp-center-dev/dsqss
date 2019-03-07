import codecs
from itertools import product

import numpy as np

from .util import tagged


class Displacement:
    def __init__(self, lat, distance_only=False, origin=None):
        self.lat = lat
        self.nr = 0
        self.origin = origin
        if origin is not None:
            sources = [origin]
            self.displacements = -np.ones(lat.nsites, dtype=int)
            self.nelems = lat.nsites
        else:
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
                if origin is not None:
                    self.displacements[t] = rdict[r]
                else:
                    self.displacements[s, t] = rdict[r]

    def write_xml(self, filename, lat):
        with codecs.open(filename, "w", "utf-8") as f:
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write("<Displacements>\n")
            f.write(tagged("Comment", self.lat.name))
            f.write(tagged("NumberOfKinds", self.nr))
            f.write(tagged("NumberOfSites", lat.nsites))
            f.write("\n")
            f.write("<!-- <R> [kind] [isite] [jsite] </R> -->\n")
            f.write("\n")

            if self.origin is not None:
                for t in range(self.lat.nsites):
                    f.write(tagged("R",
                                   (self.displacements[t], self.origin, t)))
            else:
                for s, t in product(range(self.lat.nsites), range(self.lat.nsites)):
                    f.write(tagged("R",
                                   (self.displacements[s, t], s, t)))

            f.write("</Displacements>\n")
