import json
import os
import sys

import numpy as np
from common import cleanup, geninp, read_result, run, p_value

if len(sys.argv) < 3:
    print("usage: python test.py <BINDIR> <jsonfile>")
    sys.exit(1)

test_Green = False

BINDIR = sys.argv[1]
JSONFILE = sys.argv[2]

alpha = 0.001
SEED = 19937


def evaluate(ref, nset, alpha, name=""):
    if name != "":
        name = "_{0}".format(name)

    res = read_result("res{0}.dat".format(name))
    SF = read_result("sf{0}.dat".format(name))
    res["SF"] = SF

    if test_Green:
        CF = read_result("cf{0}.dat".format(name))
        res["CF"] = CF
        CK = read_result("ck{0}.dat".format(name))
        res["CK"] = CK

    failed = 0
    for name in ref:
        if name not in res:
            continue
        if name not in ["SF", "CF", "CK"]:
            X, E = res[name]
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
        else:
            y = np.array(ref[name])
            nk, ntau = y.shape
            for ik in range(nk):
                for it in range(ntau):
                    X, E = res[name]["C{0}t{1}".format(ik, it)]
                    Y = y[ik, it]
                    p = p_value(X, E, nset, Y)
                    if p < alpha:
                        print(
                            "NG: {0}[C{1}t{2}]: X={3}, E={4}, Y={5}, t={6}, p={7}".format(
                                name, ik, it, X, E, Y, (X - Y) / E, p
                            )
                        )
                        failed += 1
                    else:
                        print(
                            "OK: {0}[C{1}t{2}]: X={3}, E={4}, Y={5}, t={6}, p={7}".format(
                                name, ik, it, X, E, Y, (X - Y) / E, p
                            )
                        )
    if failed > 0:
        sys.exit(1)


with open(JSONFILE) as f:
    data = json.load(f)

ID = ".".join(os.path.basename(JSONFILE).split(".")[0:-1])

param = data["Parameter"]
ref = data["Result"]

exename = 'dla'

# if param["hamiltonian"]["model"] == "boson":
#     exename = "dla_B"
# else:
#     exename = "dla_H"

nset = param["parameter"]["nset"]
cleanup(ID)
geninp(param, SEED, name=ID)
run(exename, BINDIR=BINDIR, name=ID)
evaluate(ref, nset, alpha, name=ID)
sys.exit(0)
