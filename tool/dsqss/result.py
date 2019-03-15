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
