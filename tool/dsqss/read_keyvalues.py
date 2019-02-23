import re
import sys


def read_keyvalues(inputfile):
    """
    read and eval `inputfile` and return a dictionary
    """
    if type(inputfile) is str:
        with open(inputfile) as f:
            return read_keyvalues(f)
    if inputfile is sys.stdin:
        print("Waiting for standard input...")
    data_list = inputfile.readlines()
    res = {}
    # get key and value
    for data in data_list:
        data.strip()
        d_re = re.search("(.*)#(.*)", data)
        if d_re is not None:
            data = d_re.group(1)
        pattern = "(.*)=(.*)"
        if re.search(pattern, data) is not None:
            d_re = re.search(pattern, data)
            key = d_re.group(1).strip().lower()
            val = d_re.group(2).strip().split(",")
            if len(val) == 1:
                val = val[0]
            res[key] = val
    return res


def parse_list(lst, N, typ=None):
    if type(lst) is not list:
        ret = [lst]
    else:
        ret = lst[:]
    last = ret[-1]
    for n in range(N - len(ret)):
        ret.append(last)
    if typ is not None:
        ret = list(map(typ, ret))
    return ret
