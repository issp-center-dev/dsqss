#ifndef LATTICE_HPP
#define LATTICE_HPP

//######################################################################

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>

#include <stdma.h>
#include <systemparameter.h>

#include <xml.hpp>
//######################################################################

class Lattice {

private:

    //  Site* site; // the list of the poiters to sites
    //  Interaction* interaction; // the list of the pointers to interactions
    XML::Block X;
    //  Algorithm& ALG;

public:

    int D; // dimension
    int L[4];
    double BETA; // inverse temperature
    double oldBETA; //for annealing
    int NCELL; // total number of cells
    int NSITE; // total number of sites
    int NINT; // total number of interactions
    int NSTYPE; // number of site types
    int NITYPE; // number of interaction types
    int NLdiv;
    int NBdiv;
    int NFIELD;
    Size *N;

    ////domain///
    Parallel *PR;
    int V, Nx, Ny, Nz;
    double B;
    /////////////

    int **bd, bnum, lc, TB, *lx;
    double **bond_vec;

    bool **frame;
    int Pd;
    int *Fx;
    int **frame_lsite;
    int **frame_lnum;
    int **frame_rsite;
    int **frame_rnum;

    int rmax;

    Lattice(const char *FNAME);
    Lattice(std::string const& FNAME);

    ~Lattice();

    void dump();

    void show_param(std::ofstream &F);

    int countVertices();

    void make_Size(Size *N);
    void make_Parallel(Parallel *PR);
    void set_beta(double beta);
    void set_oldbeta(double oldbeta);

private:

    void read();

};


#endif
