import subprocess

from dsqss.dla_pre import dla_pre
from dsqss.result import Results

V = 3
L = [8, 8]
beta = 10.0

lattice = {"lattice": "hypercubic", "dim": 2, "L": L}
hamiltonian = {"model": "boson", "t": 1, "V": V, "M": 1}
parameter = {"beta": beta, "nset": 4, "ntherm": 100, "ndecor": 100, "nmcs": 100}

name = "amzu"
mus = [-4.0, -2.0, 0.0, 2.0, 2.5, 3.0, 6.0, 9.0, 9.5, 10.0, 12.0, 14.0]

output = open("{}.dat".format(name), "w")
for i, mu in enumerate(mus):
    ofile = "res_{}.dat".format(i)
    pfile = "param_{}.in".format(i)
    hamiltonian["mu"] = mu
    parameter["outfile"] = ofile
    dla_pre(
        {"parameter": parameter, "hamiltonian": hamiltonian, "lattice": lattice}, pfile
    )
    cmd = ["dla", pfile]
    subprocess.call(cmd)
    res = Results(ofile)
    output.write("{} {}\n".format(mu, res.to_str(name)))
output.close()
