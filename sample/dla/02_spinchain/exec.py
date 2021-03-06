import subprocess

from dsqss.dla_pre import dla_pre
from dsqss.result import Results

L = 32

lattice = {"lattice": "hypercubic", "dim": 1, "L": L}
hamiltonian = {"model": "spin", "Jz": -1, "Jxy": -1}
parameter = {"nset": 5, "ntherm": 1000, "ndecor": 1000, "nmcs": 1000,
             "wvfile": "wv.xml", "dispfile": "disp.xml"}

name = "xmzu"
Ms = [1, 2]
Ts = [0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0, 1.25, 1.5, 1.75, 2.0]

for M in Ms:
    output = open("{0}_{1}.dat".format(name, M), "w")
    for i, T in enumerate(Ts):
        ofile = "res_{}_{}.dat".format(M, i)
        pfile = "param_{}_{}.in".format(M, i)
        sfoutfile = "sf_{}_{}.dat".format(M, i)
        cfoutfile = "cf_{}_{}.dat".format(M, i)
        ckoutfile = "ck_{}_{}.dat".format(M, i)
        hamiltonian["M"] = M
        parameter["beta"] = 1.0 / T
        parameter["outfile"] = ofile
        parameter["sfoutfile"] = sfoutfile
        parameter["cfoutfile"] = cfoutfile
        parameter["ckoutfile"] = ckoutfile
        dla_pre(
            {"parameter": parameter, "hamiltonian": hamiltonian, "lattice": lattice},
            pfile,
        )
        cmd = ["dla", "param_{0}_{1}.in".format(M, i)]
        subprocess.call(cmd)
        res = Results(ofile)
        output.write("{} {}\n".format(T, res.to_str(name)))
    output.close()
