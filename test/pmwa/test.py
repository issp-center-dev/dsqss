import json
import os
import shutil
import subprocess as sub
import sys

import numpy as np
import scipy.stats as stats

if len(sys.argv) < 3:
    print("usage: python test.py <path/to/bin> <jsonfile>")
    sys.exit(1)

BINDIR = sys.argv[1]
JSONFILE = sys.argv[2]

alpha = 0.01
nset = 10
SEED = 19937


def p_value(X, E, N, Y):
    t = (X - Y) / E
    return 2 * stats.t.cdf(-np.abs(t), N - 1)


def cleanup(name=""):
    if name != "":
        name = "_{0}".format(name)
    for prefix, suffix in [
        ("lattice", ".xml"),
        ("res", ".dat.0"),
        ("evout_res", ".dat.0_rank0.dat"),
        ("RNDevout_res", ".dat.0_rank0.dat"),
        ("qmc_", ".inp"),
    ]:
        fname = "{0}{1}{2}".format(prefix, name, suffix)
        if os.path.exists(fname):
            os.remove(fname)


def genXML(param, name=""):
    if name != "":
        name = "_{0}".format(name)
    L = param["L"]
    D = param["D"]
    # beta = param['beta']
    beta = 1.0

    cmd = ["{0}/lattgene_P".format(BINDIR), str(D)]
    cmd.extend(map(str, L))
    cmd.extend([str(beta), "1", "1", "0"])

    retval = sub.call(cmd)
    if retval != 0:
        sys.exit(retval)
    shutil.move("lattice.xml", "lattice{0}.xml".format(name))


def geninp(param, seed, nset=10, name=""):
    beta = param["beta"]
    if name != "":
        name = "_{0}".format(name)
    if param["Model"] == "boson":
        D = param["D"]
        t = param["t"]
        V = param["V"]
        U = param["U"]
        mu = param["mu"]
        G = param["G"]
        M = param["M"]
    else:
        D = param["D"]
        t = param["Jxy"]
        V = -param["Jz"]
        U = 0.0
        mu = param["h"]
        G = param["G"]
        M = int(2 * param["S"])
    with open("qmc{0}.inp".format(name), "w") as f:
        f.write("runtype = 0\n")
        f.write("nset = {0}\n".format(nset))
        f.write("ntherm = 1000\n")
        f.write("ndecor = 1000\n")
        f.write("nmcs  = 1000\n")
        f.write("npre = 1000\n")
        f.write("nvermax = 10000000\n")
        f.write("nwormax = 1000\n")
        f.write("seed = {0}\n".format(seed))
        f.write("latfile = lattice{0}.xml\n".format(name))
        f.write("outfile = res{0}.dat\n".format(name))
        f.write("beta = {0}\n".format(beta))
        f.write("oldbeta = {0}\n".format(beta))
        f.write("CB = 0\n")
        f.write("G = {0}\n".format(G))
        f.write("t = {0}\n".format(t))
        f.write("V = {0}\n".format(V))
        f.write("u = {0}\n".format(U))
        f.write("mu = {0}\n".format(mu))
        f.write("Wc = 0\n")
        f.write("PS = 1\n")
        f.write("NMAX = {0}\n".format(M))


def run(exename, name=""):
    if name != "":
        name = "_{0}".format(name)
    retval = sub.call(["{0}/{1}".format(BINDIR, exename), "qmc{0}.inp".format(name)])
    if retval != 0:
        sys.exit(retval)


def read_result(filename):
    res = {}
    with open(filename) as f:
        for line in f:
            if not line.startswith("R"):
                continue
            words = line.split()
            res[words[1]] = tuple(map(float, words[3:5]))
    return res


def evaluate(ref, nset, alpha, name=""):
    if name != "":
        name = "_{0}".format(name)
    res = read_result("res{0}.dat.0".format(name))
    failed = 0
    for name in ref:
        X, E = res[name]
        if E == 0.0:
            E = 1.0e-15
        Y = ref[name]
        p = p_value(X, E, nset, Y)
        if p < alpha:
            print(
                "NG: {0}: X={1}, E={2}, Y={3}, t={4}, p={5}".format(
                    name, X, E, Y, (X - Y) / E, p
                )
            )
            failed += 1
        else:
            print(
                "OK: {0}: X={1}, E={2}, Y={3}, t={4}, p={5}".format(
                    name, X, E, Y, (X - Y) / E, p
                )
            )
    if failed > 0:
        sys.exit(1)


with open(JSONFILE) as f:
    data = json.load(f)

ID = ".".join(os.path.basename(JSONFILE).split(".")[0:-1])

param = data["Parameter"]
ref = data["Result"]

if param["Model"] == "boson":
    exename = "pmwa_B"
else:
    exename = "pmwa_H"


cleanup(name=ID)
genXML(param, ID)
geninp(param, SEED, nset, ID)
run(exename, ID)
evaluate(ref, nset, alpha, ID)
sys.exit(0)
