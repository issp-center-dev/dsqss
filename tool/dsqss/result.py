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

class Result:
    def __init__(self, mean, err):
        self.mean = mean
        self.err = err

    def to_str(self, delim="+/-"):
        return "{}{}{}".format(self.mean, delim, self.err)


class Results:
    def __init__(self, resfile):
        self.result = {}
        with open(resfile) as f:
            for line in f:
                if not line.startswith("R"):
                    continue
                words = line.split()
                R = Result(float(words[3]), float(words[4]))
                self.result[words[1]] = R

    def to_str(self, names, delim=" ", delim_data=" "):
        if type(names) is str:
            return self.result[names].to_str(delim)
        ss = [self.result[name].to_str(delim) for name in names]
        return delim_data.join(ss)
