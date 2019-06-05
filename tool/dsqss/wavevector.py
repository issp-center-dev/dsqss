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
import scipy.special as spsp

from .util import ERROR, get_as_list, tagged, dictkey_tolower


class Wavevector:
    def __init__(self):
        pass

    def generate(self, param, size):
        self.dim = len(size)
        steps = get_as_list(param, "ksteps", default=0, extendto=self.dim)
        for d in range(self.dim):
            if steps[d] == 0:
                steps[d] = size[d]//2
        ks = []
        self.nk = 1
        for d in range(self.dim):
            ks.append(list(range(0, size[d] // 2 + 1, steps[d])))
            self.nk *= len(ks[-1])
        self.ks = np.zeros([self.dim, self.nk], dtype=np.int)
        for ik, k in enumerate(product(*ks)):
            self.ks[:, ik] = list(k)

    def load(self, inp):
        if type(inp) is str:
            with open(inp) as f:
                self.load(f)
                return

        self.dim = 0
        state = "waiting"
        for line in inp:
            body = line.split("#")[0].strip()
            if len(body) == 0:
                continue
            if state == "waiting":
                if body.lower() == "dim":
                    state = "dim"
                else:
                    continue
            elif state == "dim":
                self.dim = int(body)
                break
        if not self.dim > 0:
            ERROR("invalid dimension")

        state = "waiting"
        for line in inp:
            body = line.split("#")[0].strip()
            if len(body) == 0:
                continue
            if state == "waiting":
                if body.lower() == "dim":
                    continue
                elif body.lower() == "kpoints":
                    state = "nk"
            elif state == "nk":
                self.nk = int(body)
                self.ks = np.zeros([self.dim, self.nk], dtype=np.int)
                state = "kpoints"
            elif state == "kpoints":
                words = body.split()
                kid = int(words[0])
                self.ks[:, kid] = list(map(int, words[1:]))

    def save(self, out):
        if type(out) is str:
            with codecs.open(out, "w", "utf-8") as f:
                self.save(f)
                return

        out.write("dim\n{0}\n".format(self.dim))
        out.write("\n")

        out.write("kpoints\n")
        out.write("{0}\n".format(self.nk))
        for ik in range(self.nk):
            out.write(str(ik))
            for k in self.ks[:, ik]:
                out.write(" {0}".format(k))
            out.write("\n")

    def write_xml(self, filename, lat):
        if self.dim != lat.dim:
            ERROR("dimension mismatch between wavevector and lattice")
        with codecs.open(filename, "w", "utf-8") as f:
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write("<WaveVector>\n")
            f.write(tagged("Comment", lat.name))
            f.write(tagged("NumberOfSites", len(lat.sites)))
            f.write(tagged("NumberOfWaveVectors", self.nk))

            f.write("<!-- <RK> [phase(cos)] [phase(sin)] [isite] [kindx] </RK> -->\n")
            for ik in range(self.nk):
                k = self.ks[:, ik]
                for site in lat.sites:
                    sid = site.id
                    coord = site.coord
                    phase = 0
                    for d in range(self.dim):
                        phase += coord[d] * k[d] / float(lat.size[d])
                    c = spsp.cosdg(360 * phase)
                    s = spsp.sindg(360 * phase)
                    f.write(tagged("RK", [c, s, sid, ik]))

            f.write("</WaveVector>\n")


def main():
    import argparse
    import toml

    parser = argparse.ArgumentParser(
        description="Generate wavevector file for DSQSS/DLA",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        add_help=True,
    )

    parser.add_argument("input", help="Input filename")
    parser.add_argument(
        "-o", "--output", dest="out", default="kpoints.dat", help="Output filename"
    )
    parser.add_argument(
        "-s",
        "--size",
        dest="size",
        default=None,
        help='system size (specified by space separated integers like "4 4"). '
        + "This option ignores the setting in [lattice] section of input TOML file",
    )

    args = parser.parse_args()

    Ls = None
    inp = toml.load(args.input)
    dictkey_tolower(inp)
    if "lattice" in inp:
        dim = inp["lattice"]["dim"]
        Ls = get_as_list(inp["lattice"], "l", extendto=dim)
    if "kpoints" in inp:
        inp = inp["kpoints"]
    if args.size is not None:
        Ls = list(map(int, args.size.split()))
    if Ls is None:
        ERROR("system size is not specified")
    wv = Wavevector()
    wv.generate(inp, Ls)
    wv.save(args.out)


if __name__ == "__main__":
    main()
