.. -*- coding: utf-8 -*-
.. highlight:: none

About DSQSS
---------------

Overview
****************
DSQSS is a program package for solving quantum many-body problems defined on lattices. It is based on the quantum Monte Carlo method in Feynman's path integral representation. It covers a broad range of problems written by flexible input files that define arbitrary unit cells in arbitrary dimensions, and arbitrary matrix elements of the interactions among arbitrary number of degrees of freedom.

For example, you can perform finite temperature calculation of XXZ spin model by specifying parameters such as dimension, size of lattice, anisotropic coupling constants, length of spin, strength of magnetic field, and temperature. You can calculate Bose-Hubbard model as well as quantum spin model. PMWA (Parallel Multi Worm Algorithm) suits for large-scale non-trivial parallel calculation by domain parallelization.

DSQSS consists of the following subpackages.

- serial (dla)

    #. Input files generator: ``dla_pre``
    #. Hamiltonian file generator: ``dla_hamgen``
    #. Lattice file generator: ``dla_latgen``
    #. Algorithm file generator:  ``dla_alg``
    #. QMC engines (directed loop algorithm): ``dla``

- non-trivial parallel (pmwa)

    #. Input files generator: ``pmwa_pre``
    #. Lattice file generator: ``lattgene_P``
    #. QMC engines (multiworm): ``pmwa_H`` ( XXZ spin model), ``pmwa_B`` (Hard core Boson)


Developers
****************
2018/10/19 

- Yasuyuki Kato (Univ. of Tokyo)
- Naoki Kawashima (ISSP)
- Kota Sakakura (NEC)
- Takafumi Suzuki (Hyogo Univ.)
- Kenji Harada (Kyoto Univ.)
- Akiko Masaki (Hitachi Research Laboratory)
- Yuichi Motoyama (ISSP)
- Kazuyoshi Yoshimi (ISSP)

Collaborators
****************
2018/10/19

- Tsuyoshi Okubo (Univ. of Tokyo)
- Takeo Kato (ISSP)

License
****************
- GNU General Public License (GPL)
- The users are kindly requested to cite the following paper in their publication, if any, based on the software.
  
.. topic:: Reference

    Y. Motoyama, K. Yoshimi, A. Masaki-Kato, T. Kato, and N. Kawashima, Comput. Phys. Commun. 264, 107944 (2021).

Acknowledgment
****************
Development of this software was and is being supported by K-computer project, post-K computer projects and “Project for advancement of software usability in materials science” by ISSP.

Contact
*********************
Write topics to GitHub's issues or send the e-mail to the following mailing list 
`dsqss-dev@issp.u-tokyo.ac.jp`
