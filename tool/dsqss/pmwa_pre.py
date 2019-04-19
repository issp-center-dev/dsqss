#!/usr/bin/env python

import argparse
import os.path
import subprocess
import sys
import warnings

from .read_keyvalues import read_keyvalues
from .util import ERROR


class param:
    def __init__(self):
        self.param_dict = {
            "beta": 10.0,
            "npre": 1000,
            "ntherm": 1000,
            "ndecor": 1000,
            "nmcs": 1000,
            "nset": 10,
            "seed": 31415,
            "nvermax": 10000000,
            "algfile": "algorithm.xml",
            "latfile": "lattice.xml",
            "outfile": "sample.log",
            "sfinpfile": "",
            "cfinpfile": "",
            "ckinpfile": "",
            "sfoutfile": "sf.dat",
            "cfoutfile": "cf.dat",
            "ckoutfile": "ck.dat",
        }

        self.param_dict_pmwa = {
            "runtype": 0,
            "cb": 0,
            "nwormax": 1000,
            "step_x": 1,
            "step_k": 1,
            # Hamiltonian
            "t": 0.0,
            "u": 0.0,
            "v": 0.0,
            "mu": 0.0,
            "g": 0.0,
        }

        self.info_dict = {
            # System
            "solver": "DLA",
            "model_type": "spin",
        }

        self.info_dict_dla = {
            # Hamiltonian
            "m": 1,
            "j": 1.0,
            "f": 0.0,
            "v": 0.0,
            "u": 0.0,
            # Lattice
            "lattice_type": "square",
            "d": 1,
            "l": "8",
            "beta": 10.0,
            # sf,cf
            "ntau": 100,
            "ntau_cutoff": 100,
            "ktype": 0,
        }

        self.info_dict_pmwa = {
            # Lattice
            "lattice_type": "square",
            "d": 1,
            "l": 8,
            "beta": 10.0,
            "nldiv": 1,
            "nbdiv": 1,
            "nfield": 0,  # to be deleted
            # additonal information
            "nmax": 1,  # fixed 1
        }


class info:
    def __init__(self, args):
        self.args = args
        self.dict_lattice = {"square": "lattgene_C", "triangular": "lattgene_T"}
        self.dict_hamgen = {"spin": "hamgen_H", "boson": "hamgen_B"}
        self.dict_solver = {"DLA": "dla_", "PMWA": "pmwa_"}
        self.prm = param()
        self.gendir = args.gendir
        if self.gendir is None:
            self.gendir = os.path.dirname(os.path.abspath(sys.argv[0]))
            print("use file generators in {0}".format(self.gendir))
        self._get_info(args.input)
        self._check_condition()

    def get_solver(self):
        if self.prm.info_dict["model_type"] == "spin":
            model_label = "H"
        else:
            model_label = "B"
        return self.dict_solver[self.prm.info_dict["solver"]] + model_label

    def _check_condition(self):
        if self.prm.info_dict["solver"] == "PMWA":
            if self.prm.info_dict["lattice_type"] != "square":
                raise RuntimeError("For PMWA, square type lattice is only used.")

    def _get_lattgene(self):
        lattgene_name = self.dict_lattice[self.prm.info_dict["lattice_type"]]
        if self.prm.info_dict["solver"] == "PMWA":
            lattgene_name = "lattgene_P"
        return os.path.join(self.gendir, lattgene_name)

    def _get_info(self, inputfile):
        tmp_dict = read_keyvalues(inputfile)
        """
        if inputfile is sys.stdin:
            print('Waiting for standard input...')
        data_list = inputfile.readlines()
        tmp_dict={}
        # get key and value
        for data in data_list:
            data.strip()
            d_re = re.search("(.*)#(.*)", data)
            if d_re is not None:
                data = d_re.group(1)
            pattern = "(.*)=(.*)"
            if re.search(pattern,data) is not None:
                d_re = re.search(pattern,data)
                key = d_re.group(1).strip().lower()
                tmp_dict[key] = d_re.group(2).strip()
        """

        if tmp_dict["solver"] == "DLA":
            ERROR("Please use dla_pre instead of pmwa_pre")
        else:
            self.prm.info_dict.update(self.prm.info_dict_pmwa)
            self.prm.param_dict.update(self.prm.param_dict_pmwa)
            # if oldbeta exits, add param_dict
            if tmp_dict.get("oldbeta") is not None:
                self.prm.param_dict["oldbeta"] = float(tmp_dict.get("oldbeta"))

            # modify parameters for spin model
            if ("model_type" in tmp_dict) is False:
                tmp_dict["model_type"] = self.prm.info_dict["model_type"]

            if tmp_dict["model_type"] == "spin":
                # assume that the lattice is bipartite
                # this makes that t is always positive (by unitary transform)
                if tmp_dict.get("jxy") is not None:
                    tmp_dict["jxy"] = abs(float(tmp_dict["jxy"]))
                self._replace_keyword(tmp_dict, "t", "jxy")
                self._replace_keyword(tmp_dict, "v", "jz")
                self._replace_keyword(tmp_dict, "mu", "h")
                self._replace_keyword(tmp_dict, "g", "gamma")
                tmp_dict["g"] = float(tmp_dict["g"]) * 2.0
            else:
                self._replace_keyword(tmp_dict, "g", "gamma")

        self.param_list = list(self.prm.param_dict)
        self.param_list.sort()

        for key in tmp_dict.keys():
            invalid = True
            if key in self.prm.info_dict:
                self.prm.info_dict[key] = tmp_dict[key]
                invalid = False
            if key in self.prm.param_dict:
                self.prm.param_dict[key] = tmp_dict[key]
                invalid = False
            if invalid:
                warnings.warn(
                    'Unknown keyword " ' + key + '" is found (ignored).', Warning
                )

    def _replace_keyword(self, _tmp_dict, origin, new):
        if _tmp_dict.get(new) is not None:
            _tmp_dict[origin] = _tmp_dict[new]
            del _tmp_dict[new]

    def _get_latticeinfo(self):
        linfo = self.prm.info_dict["l"]
        linfo = linfo.split(",")
        if self.prm.info_dict["lattice_type"] == "triangular":
            LatticeInfo = []
        else:
            LatticeInfo = [self.prm.info_dict["d"]]
        for lattice in linfo:
            LatticeInfo.append(lattice)
        return LatticeInfo

    def make_xml(self):
        LatticeInfo = self._get_latticeinfo()
        genpath = self._get_lattgene()
        if self.prm.info_dict["solver"] == "PMWA":
            if not os.path.exists(genpath):
                raise RuntimeError(
                    "{0} is not found: use correct --generator_dir.".format(genpath)
                )
            cmd = [genpath, "-o", self.prm.param_dict["latfile"]]
            for l in LatticeInfo:
                cmd.append(str(l))
            cmd.append(str(self.prm.info_dict["beta"]))

            cmd += [
                str(self.prm.info_dict["nldiv"]),
                str(self.prm.info_dict["nbdiv"]),
                str(self.prm.info_dict["nfield"]),
            ]
            print("\n " + " ".join(cmd))
            subprocess.call(cmd)
        else:
            dsqss.generate_lattice(self.prm.info_dict, self.prm.param_dict["latfile"])

        # Genelate Hamiltonian and Algorithm xml files (only used for DLA)
        if self.prm.info_dict["solver"] == "DLA":
            hfile = self.args.hfile
            dsqss.generate_hamiltonian(self.prm.info_dict, hfile)

            """
            model_type = self.prm.info_dict["model_type"]
            genpath = os.path.join(self.gendir, self.dict_hamgen[model_type])
            if not os.path.exists(genpath):
                raise RuntimeError('{0} is not found: use correct --generator_dir.'.format(genpath))
            if model_type == "spin":
                cmd = [genpath,
                       '-o', hfile,
                       str(self.prm.info_dict["m"]),
                       str(self.prm.info_dict["j"]),
                       str(self.prm.info_dict["f"])]
            elif model_type == "boson":
                cmd = [genpath,
                       '-o', hfile,
                       str(self.prm.info_dict["m"]),
                       str(self.prm.info_dict["j"]),
                       str(self.prm.info_dict["v"]),
                       str(self.prm.info_dict["u"]),
                       str(self.prm.info_dict["f"])]
            print("\n " + ' '.join(cmd))
            subprocess.call(cmd)
            genpath = os.path.join(self.gendir, 'dla_alg')
            if not os.path.exists(genpath):
                raise RuntimeError('{0} is not found: use correct --generator_dir.'.format(genpath))
            cmd = [genpath, hfile, self.prm.param_dict["algfile"]]
            print("\n" + ' '.join(cmd))
            subprocess.call(cmd)
            """

            if len(self.prm.param_dict["sfinpfile"]) > 0:
                genpath = os.path.join(self.gendir, "sfgene")
                if not os.path.exists(genpath):
                    raise RuntimeError(
                        "{0} is not found: use correct --generator_dir.".format(genpath)
                    )
                cmd = [genpath, "-o", self.prm.param_dict["sfinpfile"]]
                for l in LatticeInfo:
                    cmd.append(str(l))
                cmd.append(str(self.prm.info_dict["ntau"]))
                cmd.append(str(self.prm.info_dict["ntau_cutoff"]))
                cmd.append(str(self.prm.info_dict["ktype"]))
                print("\n" + " ".join(cmd))
                subprocess.call(cmd)
            if len(self.prm.param_dict["cfinpfile"]) > 0:
                genpath = os.path.join(self.gendir, "cfgene")
                if not os.path.exists(genpath):
                    raise RuntimeError(
                        "{0} is not found: use correct --generator_dir.".format(genpath)
                    )
                cmd = [genpath, "-o", self.prm.param_dict["cfinpfile"]]
                for l in LatticeInfo:
                    cmd.append(str(l))
                cmd.append(str(self.prm.info_dict["ntau"]))
                print("\n " + " ".join(cmd))
                subprocess.call(cmd)
            if len(self.prm.param_dict["ckinpfile"]) > 0:
                genpath = os.path.join(self.gendir, "sfgene")
                if not os.path.exists(genpath):
                    raise RuntimeError(
                        "{0} is not found: use correct --generator_dir.".format(genpath)
                    )
                cmd = [genpath, "-o", self.prm.param_dict["ckinpfile"]]
                for l in LatticeInfo:
                    cmd.append(str(l))
                cmd.append(str(self.prm.info_dict["ntau"]))
                cmd.append(str(self.prm.info_dict["ntau_cutoff"]))
                cmd.append(str(self.prm.info_dict["ktype"]))
                print("\n " + " ".join(cmd))
                subprocess.call(cmd)

    def print_init_param(self):
        with open(self.args.pfile, mode="w") as f:
            for param in self.param_list:
                f.write(param + " = " + str(self.prm.param_dict[param]) + "\n")


def main():
    parser = argparse.ArgumentParser(
        description="Generate input files for dsqss", add_help=True
    )

    parser.add_argument(
        "-g",
        "--generator-dir",
        dest="gendir",
        help="Path to directory including generators (lattgene_C, hamgen_B, etc)",
    )
    parser.add_argument(
        "-p", "--paramfile", dest="pfile", default="param.in", help="Parameter file"
    )
    parser.add_argument(
        "--hamfile", dest="hfile", default="hamiltonian.xml", help="Hamiltonian file"
    )
    parser.add_argument("input", type=argparse.FileType("r"), help="Input file")
    parser.add_argument(
        "-s",
        "--stdin",
        dest="stdin",
        action="store_true",
        help="Read from standard input (overrides --input)",
    )

    args = parser.parse_args()
    if args.stdin:
        args.input = sys.stdin
    if args.input is None:
        raise RuntimeError(
            "Neither --input (read from file) nor --stdin (read from stdin) option is used."
        )
    calcinfo = info(args)
    calcinfo.make_xml()
    # Make initial parameter
    calcinfo.print_init_param()
    solver_name = calcinfo.get_solver()
    print("Please type: " + solver_name + " " + args.pfile)


if __name__ == "__main__":
    main()
