from math import sqrt
import itertools
import codecs
from copy import deepcopy

import toml

import dsqss.lattice as lattice
from dsqss.util import tagged

def keystate(istate, fstate):
    if type(istate) is not list:
        istate = [istate]
    if type(fstate) is not list:
        fstate = [fstate]
    return (tuple(istate), tuple(fstate))

def append_matelem(matelems, state=None, istate=None, fstate=None, value=None, param=None):
    if param is None:
        if istate is not None:
            matelems[keystate(istate, fstate)] = value
        else:
            matelems[keystate(state, state)] = value
    else:
        if 'istate' in param:
            matelems[keystate(param['istate'], param['fstate'])] = param['value']
        else:
            matelems[keystate(param['state'], param['state'])] = param['value']
    return matelems

def matelems_todict(matelems, keepzero=False):
    return [{'istate' : list(key[0]),
             'fstate' : list(key[1]),
             'value'  : matelems[key]} for key in matelems if keepzero or matelems[key]!=0.0]

class Site(object):
    def __init__(self, param=None, id=None, N=None, elements=None, sources=None):
        if param is not None:
            self.id = param['id']
            self.N = param['N']
            self.elements = {}
            for x in param['elements']:
                append_matelem(self.elements, param=x)
            self.sources = {}
            for x in param['sources']:
                append_matelem(self.sources, param=x)
            '''
            self.elements = [MatElem(param=x) for x in param['elements']]
            self.sources = [MatElem(param=x) for x in param['sources']]
            '''
        else:
            self.id = id
            self.N = N
            self.elements = elements
            self.sources = sources

    def to_dict(self):
        return {'id' : self.id, 'N' : self.N,
                'elements' : matelems_todict(self.elements),
                'sources' : matelems_todict(self.sources),
                }
        '''
        return {'id' : self.id, 'N' : self.N,
                'elements' : list(map(lambda x: x.to_dict(), self.elements)),
                'sources' : list(map(lambda x: x.to_dict(), self.sources)),
                }
                '''

class Interaction(object):
    def __init__(self, param=None, id=None, nbody=None, Ns=None, elements=None):
        if param is not None:
            self.id = param['id']
            self.nbody = param['nbody']
            self.Ns = param['N']
            self.elements = {}
            for x in param['elements']:
                append_matelem(self.elements, param=x)
        else:
            self.id = id
            self.nbody = nbody
            self.Ns = Ns
            self.elements = elements

    def to_dict(self):
        return {'id' : self.id, 'nbody' : self.nbody, 'N' : self.Ns,
                'elements' : matelems_todict(self.elements),
                }

class IndeedInteraction(object):
    def __init__(self, sites, ints, v):
        inter = ints[v.int_type]
        zs = v.zs

        self.itype = v.v_id
        self.stypes = v.site_types
        self.nbody = len(self.stypes)
        self.elements = deepcopy(inter.elements)

        site_elems = [deepcopy(sites[stype].elements) for stype in self.stypes]

        for state in itertools.product(*map(range, inter.Ns)):
            val = 0.0
            for st, els, z in zip(state, site_elems, zs):
                val += els.get(st, 0.0)/z
            key = (state, state)
            if key in self.elements:
                self.elements[key] += val
            else:
                self.elements[key] = val

class Hamiltonian(object):
    def __init__(self, ham_dict):
        if type(ham_dict) == str:
            ham_dict = toml.load(ham_dict)
        self.name = ham_dict.get('name', '')

        self.nstypes = len(ham_dict['sites'])
        self.sites = [None for i in range(self.nstypes)]
        self.nitypes = len(ham_dict['interactions'])
        self.interactions = [None for i in range(self.nitypes)]

        for site in ham_dict['sites']:
            S = Site(site)
            self.sites[S.id] = S
        for inter in ham_dict['interactions']:
            I = Interaction(inter)
            self.interactions[I.id] = I

        self.nxmax = 0
        for site in self.sites:
            self.nxmax = max(site.N, self.nxmax)

    def to_dict(self):
        return {'name' : self.name,
                'sites' : list(map(lambda x: x.to_dict(), self.sites)),
                'interactions' : list(map(lambda x: x.to_dict(), self.interactions)),
                }

    def write_toml(self, filename):
        with codecs.open(filename, 'w', 'utf_8') as f:
            toml.dump(self.to_dict(), f)

class GraphedHamiltonian(Hamiltonian):
    def __init__(self, ham, lat):
        if not isinstance(ham, Hamiltonian):
            super(GraphedHamiltonian, self).__init__(ham)
        else:
            super(GraphedHamiltonian, self).__init__(ham.to_dict())
        self.load_lattice(lat)

    def load_lattice(self, lat):
        if type(lat) == str:
            lat = lattice.Lattice(lat)
        self.indeed_interactions = [IndeedInteraction(self.sites, self.interactions, v) for v in lat.vertices]
        self.nitypes = len(self.indeed_interactions)

    def write_xml(self, filename):
        with codecs.open(filename, 'w', 'utf_8') as f:

            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write('<Hamiltonian>\n')
            indent = '  '
            level = 1
            
            f.write(indent*level + '<General>\n')
            level += 1
            f.write(indent*level + tagged('Comment', self.name))
            f.write(indent*level + tagged('NSTYPE', self.nstypes))
            f.write(indent*level + tagged('NITYPE', self.nitypes))
            f.write(indent*level + tagged('NXMAX', self.nxmax))
            level -= 1
            f.write(indent*level + '</General>\n')
            f.write('\n')

            for site in self.sites:
                f.write(indent*level + '<Site>\n')
                level += 1
                f.write(indent*level + tagged('STYPE', site.id))
                f.write(indent*level + tagged('TTYPE', 0))
                f.write(indent*level + tagged('NX', site.N))
                level -= 1
                f.write(indent*level + '</Site>\n')
                f.write('\n')

            for site in self.sites:
                f.write(indent*level + '<Source>\n')
                level += 1
                f.write(indent*level + tagged('STYPE', site.id))
                f.write(indent*level + tagged('TTYPE', 0))
                for elem in site.sources:
                    if site.sources[elem] == 0.0:
                        continue
                    f.write(indent*level + tagged('Weight', [elem[0][0],
                                                             elem[1][0],
                                                             abs(site.sources[elem])]))
                level -= 1
                f.write(indent*level + '</Source>\n')
                f.write('\n')

            for interaction in self.indeed_interactions:
                nbody = interaction.nbody
                f.write(indent*level + '<Interaction>\n')
                level += 1
                f.write(indent*level + tagged('ITYPE', interaction.itype))
                f.write(indent*level + tagged('NBODY', nbody))
                f.write(indent*level + tagged('STYPE', interaction.stypes))

                for elem in interaction.elements:
                    v = -interaction.elements[elem]
                    if v==0.0:
                        continue
                    f.write(indent*level + '<Weight> ')
                    for i,j in zip(elem[0], elem[1]):
                        f.write('{0} {1} '.format(i,j))
                    if elem[0] != elem[1]:
                        v = abs(v)
                    f.write('{0:0< 18} '.format(v))
                    f.write('</Weight>\n')

                level -= 1
                f.write(indent*level + '</Interaction>\n')
                f.write('\n')

            level -= 1
            f.write('</Hamiltonian>\n')

