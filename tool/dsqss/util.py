from copy import deepcopy

def ERROR(msg, to_be_continued=False):
    print('ERROR: {0}\n'.format(msg), file=sys.stderr)
    if not to_be_continued:
        sys.exit(1)

def extend_list(lst, N):
    last = lst[-1]
    for i in range(N-len(lst)):
        lst.append(deepcopy(last))
    return lst

def get_as_list(d, name, default=None, extendto=None):
    if default is None:
        v = d[name]
    else:
        v = d.get(name, default)
    if type(v) is not list:
        v = [v]
    if extendto is not None:
        extend_list(v, extendto)
    return v

