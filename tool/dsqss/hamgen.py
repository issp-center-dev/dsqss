from math import sqrt
import itertools
import codecs

import toml

import dsqss.latgen as latgen

class MatElem:
    def __init__(self, istate=None, fstate=None, value=None, param=None):
        if param is None:
            self.istate = istate
            self.fstate = fstate
            self.value = value
        else:
            if 'istate' in param:
                self.istate = param['istate']
                self.fstate = param['fstate']
            else:
                self.istate = self.fstate = param['state']
            self.value = param['value']

class Site:
    def __init__(self, param):
        self.id = param['id']
        self.N = param['N']
        self.elements = [MatElem(param=x) for x in param['elements']]
        self.sources = [MatElem(param=x) for x in param['sources']]

class Interaction:
    def __init__(self, param):
        self.id = param['id']
        self.nbody = param['nbody']
        self.Ns = param['N']
        self.elements = [MatElem(param=x) for x in param['elements']]


class IndeedInteraction:
    def __init__(self, sites, ints, v):
        inter = ints[v.int_type]
        zs = v.zs

        self.itype = v.v_id
        self.stypes = v.site_types
        self.nbody = len(self.stypes)
        elements = {(tuple(elem.istate), tuple(elem.fstate)) : elem.value
                      for elem in inter.elements}

        site_elems = []
        for stype in self.stypes:
            site_elems.append({ siteelem.istate : siteelem.value
                                for siteelem in sites[stype].elements})

        for state in itertools.product(*map(range, inter.Ns)):
            val = 0.0
            for st, els, z in zip(state, site_elems, zs):
                val += els.get(st, 0.0)/z
            key = (state, state)
            if key in elements:
                elements[key] += val
            else:
                elements[key] = val

        self.elements = [MatElem(istate=st[0], fstate=st[1], value=elements[st])
                         for st in elements]



class Hamiltonian:
    def __init__(self, ham_dict, lat):
        if type(ham_dict) == str:
            ham_dict = toml.load(ham_dict)
        if type(lat) == str:
            lat = latgen.Lattice(lat)

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

        self.indeed_interactions = [IndeedInteraction(self.sites, self.interactions, v) for v in lat.vertices]

    def write_xml(self, filename):
        with codecs.open(filename, 'w', 'utf_8') as f:
            nxmax = 0
            for site in self.sites:
                nxmax = max(site.N, nxmax)

            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write('<Hamiltonian>\n')
            indent = '  '
            level = 1
            
            f.write(indent*level + '<General>\n')
            level += 1
            f.write(indent*level + '<Comment>' + self.name + '</Comment>\n')
            f.write(indent*level + '<NSTYPE> {0} </NSTYPE>\n'.format(len(self.sites)))
            f.write(indent*level + '<NITYPE> {0} </NITYPE>\n'.format(len(self.indeed_interactions)))
            f.write(indent*level + '<NXMAX> {0} </NXMAX>\n'.format(nxmax))
            level -= 1
            f.write(indent*level + '</General>\n')
            f.write('\n')

            for site in self.sites:
                f.write(indent*level + '<Site>\n')
                level += 1
                f.write(indent*level + '<STYPE> {0} </STYPE>\n'.format(site.id))
                f.write(indent*level + '<TTYPE> {0} </TTYPE>\n'.format(0))
                f.write(indent*level + '<NX> {0} </NX>\n'.format(site.N))
                level -= 1
                f.write(indent*level + '</Site>\n')
                f.write('\n')

            for site in self.sites:
                f.write(indent*level + '<Source>\n')
                level += 1
                f.write(indent*level + '<STYPE> {0} </STYPE>\n'.format(site.id))
                f.write(indent*level + '<TTYPE> {0} </TTYPE>\n'.format(0))
                for elem in site.sources:
                    if elem.value == 0.0:
                        continue
                    f.write(indent*level + '<Weight> {0} {1} '.format(elem.istate, elem.fstate))
                    f.write('{0:0< 18} '.format(elem.value))
                    f.write('</Weight>\n')
                level -= 1
                f.write(indent*level + '</Source>\n')
                f.write('\n')

            for interaction in self.indeed_interactions:
                nbody = interaction.nbody
                f.write(indent*level + '<Interaction>\n')
                level += 1
                f.write(indent*level + '<ITYPE> {0} </ITYPE>\n'.format(interaction.itype))
                f.write(indent*level + '<NBODY> {0} </NBODY>\n'.format(nbody))
                f.write(indent*level + '<STYPE> ')
                for st in interaction.stypes:
                    f.write('{0} '.format(st))
                f.write('</STYPE>\n')

                for elem in interaction.elements:
                    f.write(indent*level + '<Weight> ')
                    for i,j in zip(elem.istate, elem.fstate):
                        f.write('{0} {1} '.format(i,j))
                    f.write('{0:0< 18} '.format(elem.value))
                    f.write('</Weight>\n')

                level -= 1
                f.write(indent*level + '</Interaction>\n')
                f.write('\n')

            level -= 1
            f.write('</Hamiltonian>\n')

