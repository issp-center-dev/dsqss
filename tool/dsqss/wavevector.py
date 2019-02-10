import codecs
import numpy as np

from dsqss.util import tagged

class Wavevector:
    def __init__(self, lat):
        self.name = ''
        self.lat = lat

    def load(self, inp):
        if type(inp) is str:
            with open(inp) as f:
                self.load(f)
                return

        state = 'waiting'
        for line in inp:
            body = line.split('#')[0].strip()
            if len(body) == 0:
                continue
            if state == 'waiting':
                if body.lower() == 'ntau':
                    state = 'ntau'
                elif body.lower() == 'taucutoff':
                    state = 'taucutoff'
                elif body.lower() == 'kpoints':
                    state = 'nk'
            elif state == 'ntau':
                self.ntau = int(body)
                state = 'waiting'
            elif state == 'taucutoff':
                self.taucutoff = int(body)
                state = 'waiting'
            elif state == 'nk':
                self.nk = int(body)
                self.ks = np.zeros([self.lat.dim, self.nk], dtype=np.int)
                state = 'kpoints'
            elif state == 'kpoints':
                words = body.split()
                kid = int(words[0])
                self.ks[:,kid] = list(map(int, words[1:]))
        
    def write_xml(self, filename):
        with codecs.open(filename, 'w', 'utf-8') as f:
            f.write('<?xml version="1.0" encoding="UTF-8"?>\n')
            f.write('<StructureFactor>\n')
            f.write(tagged('Comment', self.name))
            f.write(tagged('Ntau', self.ntau))
            f.write(tagged('CutoffOfNtau', self.taucutoff))
            f.write(tagged('NumberOfElements', len(self.lat.sites) * self.nk))
            f.write(tagged('NumberOfInverseLattice', self.nk))

            f.write('<!-- <SF> [phase(cos)] [phase(sin)] [isite] [kindx] </SF> -->')
            for ik in range(self.nk):
                k = self.ks[:,ik]
                for site in self.lat.sites:
                    sid = site.id
                    coord = site.coord
                    phase = 0
                    for d in range(self.lat.dim):
                        phase += coord[d] * k[d] / float(self.lat.size[d])
                    c = np.cos(2*np.pi*phase)
                    s = np.sin(2*np.pi*phase)
                    f.write(tagged('SF', [c, s, sid, ik]))

            f.write('</StructureFactor>\n')
