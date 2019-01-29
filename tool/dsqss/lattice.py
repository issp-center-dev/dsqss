import codecs
from dsqss import parse_list

def index2coord(index, Ls):
    D = len(Ls)
    i = 0
    r = [0 for d in range(D)]
    while index > 0:
        r[i] = index%Ls[i]
        index //= Ls[i]
        i += 1
    return r

def coord2index(r, Ls):
    index = 0
    D = len(Ls)
    block = 1
    for x,L in zip(r,Ls):
        index += block*x
        block *= L
    return index

class Site:
    def __init__(self, stype, mtype):
        self.type = stype
        self.mtype = mtype

class Interaction:
    def __init__(self, itype, sites, edim, eid=-1):
        self.type = itype
        self.sites = sites
        self.edim = edim
        self.eid = eid

class Lattice:
    def __init__(self):
        self.name = ''
        self.dim = 0
        self.sites = []
        self.Ls = []
        self.nstypes = 0
        self.interactions = []
        self.nitypes = 0
        self.nboundary = 0
        self.latvecs = []
        self.beta = 1.0

    def write_xml(self, filename):
        with codecs.open(filename, 'w', 'utf_8') as f:
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write('<Lattice>\n')

            f.write('<Comment>\n')
            f.write(self.name + '\n')
            f.write('</Comment>\n')
            f.write('\n')

            f.write('<Dimension> {0} </Dimension>\n'.format(self.dim))
            f.write('<BondDimension> {0} </BondDimension>\n'.format(self.dim))
            f.write('<LinearSize> ')
            for L in self.Ls:
                f.write('{0} '.format(L))
            f.write('</LinearSize>\n')
            f.write('<Beta> {0} </Beta>\n'.format(self.beta))
            f.write('<NumberOfCells> {0} </NumberOfCells>\n'.format(len(self.sites)))
            f.write('<NumberOfSites> {0} </NumberOfSites>\n'.format(len(self.sites)))
            f.write('<NumberOfInteractions> {0} </NumberOfInteractions>\n'.format(len(self.interactions)))
            f.write('<NumberOfSiteTypes> {0} </NumberOfSiteTypes>\n'.format(self.nstypes))
            f.write('<NumberOfInteractionTypes> {0} </NumberOfInteractionTypes>\n'.format(self.nitypes))
            f.write('<NumberOfEdgeInteractions> {0} </NumberOfEdgeInteractions>\n'.format(self.nboundary))
            f.write('\n')

            f.write('<!-- <S> site_index site_type measure_type </S> -->\n')
            for i, site in enumerate(self.sites):
                f.write('<S> {0} {1} {2} </S>\n'.format(i, site.type, site.mtype))
            f.write('\n')

            f.write('<!-- <I> int_index int_type nbody site_index... edge_index direction_index </I> -->\n')
            for i, interaction in enumerate(self.interactions):
                f.write('<I> {0} '.format(i))
                f.write('{0} {1} '.format(interaction.type, len(interaction.sites)))
                for s in interaction.sites:
                    f.write('{0} '.format(s))
                f.write('{0} '.format(interaction.eid))
                f.write('{0} '.format(interaction.edim))
                f.write('</I>\n')
            f.write('\n')

            f.write('<!-- <V> direction_index direction... </V> -->\n')
            for i, bond in enumerate(self.latvecs):
                f.write('<V> {0} '.format(i))
                for b in bond:
                    f.write('{0} '.format(b))
                f.write('</V>\n')

            f.write('</Lattice>\n')

class HyperCubicLattice(Lattice):
    def __init__(self, param):
        self.dim = int(param['d'])
        self.name = '{0} dimensional hypercubic lattice'.format(self.dim)
        self.Ls = parse_list(param['l'], self.dim, int)
        self.sites = []
        self.interactions = []
        self.beta = float(param.get('beta', 1.0))
        bc = parse_list(param.get('boundarycondition', 1), self.dim, int)
        bondalt = False
        N = 1
        for L in self.Ls:
            N *= L
        P = [2]*self.dim
        nboundary = 0
        nstype = 0
        nitype = 0
        for i in range(N):
            ir = index2coord(i, self.Ls)
            parities = [x%2 for x in ir]
            p = sum(parities)%2
            if bondalt:
                stype = coord2index(parities,P)
            else:
                stype = 0
            nstype = max(nstype, stype)
            self.sites.append(Site(stype, p))

            for d in range(self.dim):
                jr = ir[:]
                jr[d] += 1
                if jr[d] == self.Ls[d]:
                    if bc[d]==0:
                        continue
                    jr[d] = 0
                    eid = nboundary
                    nboundary += 1
                else:
                    eid = -1
                j = coord2index(jr,self.Ls)
                
                if bondalt:
                    itype = stype*self.dim + d
                else:
                    itype=0
                nitype = max(nitype, itype)
                I = Interaction(itype=itype,
                                sites=[i,j],
                                edim=d,
                                eid=eid)
                self.interactions.append(I)
        self.latvecs = []
        for d in range(self.dim):
            latvec = [0]*self.dim
            latvec[d] = 1
            self.latvecs.append(latvec)
        self.nstypes = nstype+1
        self.nitypes = nitype+1
        self.nboundary = nboundary

def generate_lattice(params, outputfile='lattice.xml'):
    lat = HyperCubicLattice(params)
    lat.write_xml(outputfile)
