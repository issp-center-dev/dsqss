
#include "Graph_functions.h"

bool ascending(const GraphSpace::Vertex &_p1, const GraphSpace::Vertex &_p2) {
  return _p1.t < _p2.t;
}

GraphSpace::GraphSpace(Size *N, int m_nmax, Lattice *m_LT, Probability *m_P,
                       Parallel *m_PR, long long m_IMAX, long long m_WMAX,
                       std::string const &m_Eventfile_old, My_rdm *MR, int cb,
                       std::string const &m_outfile) {
  IMAX = m_IMAX;
  WMAX = m_WMAX;
  NMIN = G_NMIN;
  outfile = m_outfile;

  PR = m_PR;
  V = PR->V;
  B = PR->B;
  D = N->d;
  BetaOrg = N->B;
  BetaOldOrg = N->oldB;
  boxsize = IMAX / V;
  nmax = m_nmax;
  LT = m_LT;
  P = m_P;

  Bh = B / 2.0;

  if (D == 1)
    ES = 1;
  else if (D == 2)
    ES = PR->x;
  else
    ES = PR->x * PR->y;

  my_rank = PR->my_rank;
  newcalls(world, V);
  newcalls(worldB, V);
  newcalls(w, V);
  newcalls(vertex, 2);

  newcall_zero(Switch, 4, 4);
  newcall_zero(After, 4, 4);
  newcall_zero(op, 2, 2);

  newcall_zero(p0_box, V);
  newcall_zero(p1_box, V);
  newcall_zero(i0_box, V);
  newcall_zero(i1_box, V);

  newcall_zero(NewVertex_th0, LT->Pd);
  newcall_zero(NewVertex_th1, LT->Pd);

  int alpha = (int)(sqrt(B));
  if (alpha == 0) alpha = 1;

  newcall_zero(BoxSpace_t_th0, LT->Pd, alpha * ES * boxsize);
  newcall_zero(BoxSpace_p_th0, LT->Pd, alpha * ES * boxsize);
  newcall_zero(BoxSpace_py_th0, alpha * ES * boxsize);
  newcall_zero(BoxSpace_type_th0, alpha * ES * boxsize);

  newcall_zero(BoxSpace_t_th1, LT->Pd, alpha * ES * boxsize);
  newcall_zero(BoxSpace_p_th1, LT->Pd, alpha * ES * boxsize);
  newcall_zero(BoxSpace_py_th1, alpha * ES * boxsize);
  newcall_zero(BoxSpace_type_th1, alpha * ES * boxsize);

  newcall_zero(wf, LT->Pd, ES);
  newcall_zero(newt, LT->Pd, ES);
  newcall_zero(newt_th1, LT->Pd, ES);

  newcalls(INmax, V);
  newcall_zero(dtn, WMAX);

  newcall_zero(I, 4);
  newcall_zero(t, 4);

  initial_functions();

  initialev(m_Eventfile_old, MR, cb, PR->oldB);
}

GraphSpace::~GraphSpace() {
  delcall(world);
  delcall(worldB);
  delcall(w);
  delcall(vertex);

  delcall(W);

  delcall(Switch, 4);
  delcall(After, 4);
  delcall(op, 2);

  delcall(wf, LT->Pd);
  delcall(newt, LT->Pd);
  delcall(newt_th1, LT->Pd);

  delcall(p0_box);
  delcall(p1_box);
  delcall(i0_box);
  delcall(i1_box);

  delcall(NewVertex_th0);
  delcall(NewVertex_th1);

  delcall(BoxSpace_t_th0, LT->Pd);
  delcall(BoxSpace_p_th0, LT->Pd);
  delcall(BoxSpace_py_th0);
  delcall(BoxSpace_type_th0);

  delcall(BoxSpace_t_th1, LT->Pd);
  delcall(BoxSpace_p_th1, LT->Pd);
  delcall(BoxSpace_py_th1);
  delcall(BoxSpace_type_th1);

  delcall(INmax);
  delcall(dtn);

  delcall(I);
  delcall(t);
}

void GraphSpace::initialev(std::string const &Eventfile_old, My_rdm *MR, int cb,
                           double oldB) {
  ev.resize(IMAX);
  ev.clear();

  WORM.resize(WMAX);
  WORM.clear();

  std::string rndfile("RND");
  rndfile += Eventfile_old;

  if (PR->FlgAnneal || PR->FlgRestart) {
    MR->ingen(rndfile);
  }

  for (int i = 0; i < V; i++) {
    world[i].type = worldB[i].type = -5;
    world[i].i = worldB[i].i = i;
    world[i].t = 0.0;
    worldB[i].t = B;
    world[i].next[1] = &(worldB[i]);
    world[i].next[0] = &(worldB[i]);
    worldB[i].next[1] = &(world[i]);
    worldB[i].next[0] = &(world[i]);
    w[i] = &(world[i]);
  }

  ifstream fin(Eventfile_old.c_str());
  Vertex new_event_L, new_event_R;
  Vertex *reuse_L, *reuse_R;

  int xl, xr, dummy;
  if (PR->FlgAnneal || PR->FlgRestart) {
    if (fin) {
      cout << "rank " << my_rank << ": checkpoint file" << Eventfile_old
           << " found, resume this calculation." << endl;

      double tmp_B_old;
      fin >> tmp_B_old;
      if (PR->FlgAnneal) {
        if (fabs(tmp_B_old - BetaOldOrg) < pow(10.0, -12)) {
          cout << "Annealing rate=" << B / oldB << endl;  // B: beta
        } else {
          if (PR->my_rank == 0) {
            cerr << "oldB = " << BetaOldOrg
                 << " is not same the beta =" << tmp_B_old
                 << " written in the input file" << endl;
          }
          fin.close();
          MPI_Finalize();
          exit(1);
        }
      } else {
        if (PR->FlgRestart) {
          if (fabs(tmp_B_old - BetaOrg) < pow(10.0, -12)) {
            cout << "Restart calculation starts." << endl;
          } else {
            if (PR->my_rank == 0) {
              cerr << "B = " << BetaOrg
                   << " is not same the beta =" << tmp_B_old
                   << " written in the input file" << endl;
            }
            fin.close();
            MPI_Finalize();
            exit(1);
          }
        }
      }

      fin >> Ncyc;
      for (int i = 0; i < V; i++) fin >> world[i].p >> worldB[i].p;

      while (!fin.eof()) {
        fin >> dummy >> new_event_L.t >> new_event_L.type >> new_event_L.p >>
            xl;
        if (new_event_L.type == 2)
          fin >> dummy >> new_event_R.t >> new_event_R.type >> new_event_R.p >>
              xr;

        if (PR->FlgAnneal) {
          new_event_L.t *= B / oldB;
          new_event_R.t *= B / oldB;
        }

        new_event_L.i = xl;
        connect_before(w[(xl % V)], &(new_event_L));
        ev.push_back(new_event_L);
        reuse_L = &(ev.back());
        connect_after(w, &(ev.back()), xl % V);

        if (new_event_L.type == 2) {
          new_event_R.i = xr;
          connect_before(w[(xr % V)], &(new_event_R));
          ev.push_back(new_event_R);
          reuse_R = &(ev.back());
          connect_after(w, &(ev.back()), xr % V);

          reuse_L->nleg = reuse_R;
          reuse_R->nleg = reuse_L;
        }
      }
    } else {
      if (PR->my_rank == 0) {
        cerr << "rank " << my_rank
             << ":no evfile found, start a new calculation" << endl;
      }
      MPI_Finalize();
      exit(1);
    }
  } else {
    Ncyc = 30;
    if (cb == 1)
      for (int i = 0; i < V; i++)
        world[i].p = worldB[i].p =
            ((i % PR->x + (int)(i / PR->x)) % 2 == 0) ? 1 : 0;
    else if (cb == 2)
      for (int i = 0; i < V; i++)
        world[i].p = worldB[i].p = (nmax + 1) * MR->rdm();
    else
      for (int i = 0; i < V; i++) world[i].p = worldB[i].p = 0;
  }

  fin.close();

  newcalls(W, 2);

  rtb_i = P->tb;
  rt_i = P->rtmax;

  rtb_tot = LT->TB * rtb_i;
  rt_tot = LT->TB * rt_i;

  rtot = rt_tot;

  Pv1 = rt_tot / rtot;

  //境界
  PBv1 = rt_tot / rtot;

  //###################################

  for (int a = 0; a < 4; a++) {
    for (int b = 0; b <= a; b++) {
      if (a == b) {
        Switch[a][b] = 0;       // bounce
        After[0][a] = b;        // bounce
      } else if (a + b == 3) {  // hop and turn

        Switch[a][b] = 3;
        Switch[b][a] = 3;
        After[3][b] = a;
        After[3][a] = b;
      } else if (a - b == 2) {  // hop

        Switch[a][b] = 2;
        Switch[b][a] = 2;
        After[2][b] = a;
        After[2][a] = b;
      } else {  // pass

        Switch[a][b] = 1;
        Switch[b][a] = 1;
        After[1][b] = a;
        After[1][a] = b;
      }
    }
  }

  op[0][0] = -1;
  op[0][1] = 1;
  op[1][0] = 0;
  op[1][1] = 0;

  sgn[0] = -1;
  sgn[1] = 1;
}

// output of the configuration.
void GraphSpace::Output(std::string const &fname, My_rdm *MR) {
  std::ofstream file(fname.c_str());

  int EVmax = ev.size();

  file << BetaOrg << endl;
  file << Ncyc;

  for (int i = 0; i < V; i++) {
    file << endl;
    file << world[i].p << " " << worldB[i].p;
  }

  std::stable_sort(ev.begin(), ev.end(), ascending);
  file << setprecision(8);

  int i = 0;
  while (i < EVmax) {
    int type = ev.at(i).type;

    if (type == 2 || type == 4 || type == 5 || type % 2 == -1) {
      file << endl;
      file << i << " " << ev.at(i).t << " " << ev.at(i).type << " "
           << ev.at(i).p << " " << ev.at(i).i;
    }

    i++;
  }

  file.close();

  std::string rndfile("RND");
  rndfile += fname;
  MR->outgen(rndfile);
}

#include "worm.h"

bool GraphSpace::Diagonal_Update(My_rdm *MR, Quantities *QNT) {
  WORM.clear();

  W[0] = 0;  // initialization of the numberof worms

  Remove_Vertex();

  Assign_TwoSiteVertex(MR);

  Assign_OnsiteVertex(MR);

  worm_release(MR);

  //// Pararellization procedures ////
  SpatialDomainBoundary(MR);
  TemporalDomainBoundary(MR, QNT);
  ////////////////////////////////////

  Assign_Worm(MR);

  EVmax = ev.size();

  if (W[0] > 0)
    return true;
  else
    return false;
}

////////////////////////////////////////////////

void GraphSpace::Assign_OnsiteVertex(My_rdm *MR) {
  double Blim = B - NMIN;
  double targ_time, dt, R;
  int pl, xl;
  long Ih = IMAX / 2;

  for (int i = 0; i < V; i++) {
    w[i] = &(world[i]);

    targ_time = 0.0;

    while (1) {
      R = MR->rdm();
      double rhou = P->rumax[i];
      if (rhou != 0.0)
        dt = -log(1.0 - R) / P->rumax[i];
      else
        break;

      if (dt < NMIN) dt = NMIN;
      targ_time += dt;

      if (targ_time > Blim) break;

      Vnum++;
      if (Vnum == Ih) cout << PR->my_rank << "!!Vnum=" << Vnum << endl;

      xl = i;
      //##### old kinks #######
      while (w[xl]->next[1] != &(worldB[xl])) {
        if (targ_time < w[xl]->next[1]->t) break;
        w[xl] = w[xl]->next[1];
      }

      if (targ_time == w[xl]->t) continue;

      //##### new vertex #######
      pl = w[xl]->p;
      if (MR->rdm() < P->ru[pl][xl]) {
        insert(w[xl], 0, targ_time, xl, pl, 0);
        INmax[xl]++;
        Nu++;
      }
      ////////////////////////////////////////////////////////////////////////
    }
  }

  Nv += Nu;
}

void GraphSpace::Remove_Vertex() {
  Vnum = 0;

  for (int i = 0; i < V; i++) INmax[i] = 1;

  Nv = 0;
  Nk = 0;
  Nu = 0;
  for (int i = 0; i < V; i++) {
    for (Vertex *wx = &(world[i]); wx != &(worldB[i]); wx = wx->next[1]) {
      if (wx->type != 2 && wx->type != 4 && wx->type != 5 && wx->type % 2 != -1)
        release(wx);
      else if (wx->type == 2 || wx->type == -1 || wx->type == -3) {
        INmax[i]++;
        if (wx->type == 2) {
          Nv++;
          Nk++;
        }
        if (wx->type == -1) {
          Nv += 2;
          Nk += 2;
        }
      }
    }
  }

  Nv /= 2;
  Nk /= 2;

  for (int i = 0; i < V; i++) {
    w[i] = &(world[i]);
  }
}

//#############################################################################################
void GraphSpace::Assign_TwoSiteVertex(My_rdm *MR) {
  double R;
  MPI_Status status;

  int k, kmax;
  double dt;

  int bond, site, xnum;
  int pr, pl, xl, xr, d;

  double targ_time;
  double Blim = B - NMIN;

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //// For MPI information!!
  //////////////////////////////////////////////////////////////////////////////////////////////////

  for (int d = 0; d < LT->Pd; d++) {
    kmax = 0;
    for (int i = 0; i < LT->Fx[d]; i++) {
      k = 0;
      for (Vertex *wl = &(world[LT->frame_rsite[d][i]]);
           wl != &(worldB[LT->frame_rsite[d][i]]); wl = wl->next[1]) {
        BoxSpace_t_th0[d][f(d, i, k)] = wl->t;
        BoxSpace_p_th0[d][f(d, i, k)] = wl->p;
        k++;
      }

      BoxSpace_t_th0[d][f(d, i, k)] = worldB[LT->frame_rsite[d][i]].t;
      BoxSpace_p_th0[d][f(d, i, k)] = worldB[LT->frame_rsite[d][i]].p;
      k++;

      kmax = max(k, kmax);
    }

    NewVertex_th0[d] = kmax * LT->Fx[d];
    MPI_Sendrecv(&(NewVertex_th0[d]), 1, MPI_INT, PR->left[d], 0,
                 &(NewVertex_th1[d]), 1, MPI_INT, PR->right[d], 0,
                 MPI_COMM_WORLD, &status);
  }

  for (int d = 0; d < LT->Pd; d++) {
    MPI_Sendrecv(&(BoxSpace_t_th0[d][0]), NewVertex_th0[d], MPI_DOUBLE,
                 PR->left[d], 0, &(BoxSpace_t_th1[d][0]), NewVertex_th1[d],
                 MPI_DOUBLE, PR->right[d], 0, MPI_COMM_WORLD, &status);
    MPI_Sendrecv(&(BoxSpace_p_th0[d][0]), NewVertex_th0[d], MPI_SHORT,
                 PR->left[d], 0, &(BoxSpace_p_th1[d][0]), NewVertex_th1[d],
                 MPI_SHORT, PR->right[d], 0, MPI_COMM_WORLD, &status);
  }
  ///////////////////////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////////////////////////////

  k = 0;

  long Ih = IMAX / 2;
  targ_time = 0.0;

  for (int d = 0; d < D; d++)
    for (int i = 0; i < ES; i++) wf[d][i] = newt[d][i] = 0;

  int ics = 0;

  while (1) {
    R = MR->rdm();
    dt = -log(1.0 - R) / rtot;
    if (dt < NMIN) dt = NMIN;
    targ_time += dt;

    if (targ_time > Blim) break;

    Vnum++;

    if (Vnum == Ih) cout << PR->my_rank << "!!Vnum=" << Vnum << endl;

    R = MR->rdm();
    if (Pv1 > R) {
      site = (int)(V * MR->rdm());
      d = (int)(LT->Pd * MR->rdm());  //(LT->bnum*MR->rdm());
      xl = site;
      xr = LT->bd[site][d];
    } else {
      if (D != 2) {
        cout << "error in DG" << endl;
        exit(1);
      }
    }

    //##### onsite vertex and old kinks #######
    while (w[xl]->next[1] != &(worldB[xl])) {
      if (targ_time < w[xl]->next[1]->t) break;
      w[xl] = w[xl]->next[1];
    }

    if (targ_time == w[xl]->t) continue;

    ////////////////////////////////////////////////////////////////////////

    if (LT->frame[d][xl]) {  // interbond

      xnum = LT->frame_rnum[d][xr];
      while (BoxSpace_t_th1[d][f(d, xnum, wf[d][xnum] + 1)] < targ_time) {
        wf[d][xnum]++;
      }

      pl = w[xl]->p;  // same as previous one since this is diagonal term
      pr = BoxSpace_p_th1[d][f(d, xnum, wf[d][xnum])];

      if (MR->rdm() < P->at[pl][pr][0] / rt_i) {
        insert(w[xl], -2, targ_time, xl, pl, d);
        INmax[xl]++;
        BoxSpace_t_th0[d][f(d, xnum, newt[d][xnum])] = targ_time;
        newt[d][xnum]++;
        Nv++;
      }

    } else {  // innerbond

      while (w[xr]->next[1] != &(worldB[xr])) {
        if (targ_time < w[xr]->next[1]->t) break;
        w[xr] = w[xr]->next[1];
      }
      if (targ_time == w[xr]->t) continue;

      pl = w[xl]->p;  // same as previous one since this is diagonal term
      pr = w[xr]->p;
      if (MR->rdm() < P->at[pl][pr][0] / rt_i) {
        insert(w[xl], w[xr], 1, targ_time, xl, xr, pl, pr, d);
        Nv++;
        INmax[xl]++;
        INmax[xr]++;
      }
    }

    ////////////////////////////////////////////////////////////////////////

    ics++;
  }

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //// For MPI information!!
  //////////////////////////////////////////////////////////////////////////////////////////////////

  int ntmax;
  for (int d = 0; d < LT->Pd; d++) {
    MPI_Sendrecv(newt[d], LT->Fx[d], MPI_INT, PR->right[d], 0, newt_th1[d],
                 LT->Fx[d], MPI_INT, PR->left[d], 0, MPI_COMM_WORLD, &status);

    ntmax = 0;
    for (int i = 0; i < LT->Fx[d]; i++) ntmax = max(ntmax, newt[d][i]);
    NewVertex_th0[d] = ntmax * LT->Fx[d];

    ntmax = 0;
    for (int i = 0; i < LT->Fx[d]; i++) ntmax = max(ntmax, newt_th1[d][i]);
    NewVertex_th1[d] = ntmax * LT->Fx[d];

    MPI_Sendrecv(&(BoxSpace_t_th0[d][0]), NewVertex_th0[d], MPI_DOUBLE,
                 PR->right[d], 0, &(BoxSpace_t_th1[d][0]), NewVertex_th1[d],
                 MPI_DOUBLE, PR->left[d], 0, MPI_COMM_WORLD, &status);
    MPI_Barrier(MPI_COMM_WORLD);

    for (int i = 0; i < LT->Fx[d]; i++) {
      xr = LT->frame_rsite[d][i];
      w[xr] = &(world[xr]);
      k = 0;

      while (k < newt_th1[d][i]) {
        if (BoxSpace_t_th1[d][f(d, i, k)] < w[xr]->next[1]->t) {
          insert(w[xr], -4, BoxSpace_t_th1[d][f(d, i, k)], xr, w[xr]->p, d);
          INmax[xr]++;
          w[xr] = w[xr]->next[1];
          k++;
        } else
          w[xr] = w[xr]->next[1];
      }
    }
  }
  //////////////////////////////////////////////////
  //////////////////////////////////////////////////
}

void GraphSpace::release(Vertex *wx) {
  wx->next[1]->next[0] = wx->next[0];
  wx->next[0]->next[1] = wx->next[1];

  wx->type = 6;

  VertexPool.push(wx);
}

void GraphSpace::insert(Vertex *vl, Vertex *vr, short new_type, double new_time,
                        int xl, int xr, int pl, int pr, int d) {
  // Left
  if (VertexPool.empty())
    insert_NewEvent(vl, new_type, new_time, xl, pl, d);
  else
    Renew_Vertex(vl, new_type, new_time, xl, pl, d);

  // Right
  if (VertexPool.empty())
    insert_NewEvent(vr, new_type, new_time, xr, pr, d);
  else
    Renew_Vertex(vr, new_type, new_time, xr, pr, d);

  vl->next[1]->nleg = vr->next[1];
  vr->next[1]->nleg = vl->next[1];
}

void GraphSpace::insert(Vertex *v, short new_type, double new_time, int x,
                        int p, int d) {
  if (VertexPool.empty())
    insert_NewEvent(v, new_type, new_time, x, p, d);
  else
    Renew_Vertex(v, new_type, new_time, x, p, d);
}

void GraphSpace::insert_NewEvent(Vertex *v, int new_type, double new_time,
                                 int xx, int px, int d) {
  Vertex new_event;

  new_event.type = new_type;
  new_event.t = new_time;
  new_event.i = xx + d * V;
  new_event.p = px;

  ev.push_back(new_event);

  relink(v, &(ev.back()), v->next[1]);
}

void GraphSpace::Renew_Vertex(Vertex *v, int new_type, double new_time, int xx,
                              int px, int d) {
  Vertex *reuse;

  reuse = VertexPool.top();
  VertexPool.pop();

  reuse->type = new_type;
  reuse->t = new_time;
  reuse->i = xx + d * V;
  reuse->p = px;

  relink(v, reuse, v->next[1]);
}

void GraphSpace::relink(Vertex *v1, Vertex *v2, Vertex *v3) {
  v1->next[1] = v2;
  v2->next[0] = v1;
  v2->next[1] = v3;
  v3->next[0] = v2;
}

void GraphSpace::connect_after(Vertex **wl, Vertex *el, int x) {
  wl[x]->next[1]->next[0] = el;
  wl[x]->next[1] = el;
  wl[x] = el;
}

void GraphSpace::connect_before(Vertex *wl, Vertex *new_event) {
  new_event->next[1] = wl->next[1];
  new_event->next[0] = wl;
}

// check parity and remove worm
void GraphSpace::worm_release(My_rdm *MR) {
  int parity;
  Vertex *dw;

  for (int site = 0; site < V; site++) {
    parity = 0;
    dw = &(world[site]);

    if (INmax[site] == 1) {
      // No vertex exists (only worm)

      Bh = B * MR->rdm();
      if (Bh < NMIN)
        Bh = NMIN;
      else if (B - Bh < NMIN)
        Bh = B - NMIN;

      do {
        dw = dw->next[1];
      } while (dw != &(worldB[site]) && dw->t < Bh);

      insert(dw->next[0], 6, Bh, site, dw->next[0]->p, 0);

      INmax[site]++;
      dw = &(world[site]);
    }

    for (int i = 0; i < INmax[site]; i++) {
      dw = dw->next[1];

      while (dw->type == 4 || dw->type == 5) {
        // release old worm
        parity++;
        dw = dw->next[1];
        release(dw->next[0]);
      }

      // index of the below segment
      dw->dir = parity % 2;

      parity = 0;
    }
  }
}

//#################################################################################################

void GraphSpace::Assign_Worm(My_rdm *MR) {
  Vertex *dw;
  int j;  // the number of worms in a segment
  bool n0;

  for (int site = 0; site < V; site++) {
    for (Vertex *wl = world[site].next[1]; wl != &(world[site]);
         wl = wl->next[1]) {  // until worldB

      j = 0;
      dw = wl->next[0];
      dtn[0] = dw->t;
      n0 = dw->p;

      parity_check(MR, (wl->dir) % 2, dtn, j, wl->t, dw->t);

      for (int l = 0; l < j; l++) {  // insert j worms

        insert(dw, 4 + n0, dtn[l], site, !n0, 0);
        WORM.push_back(wl->next[0]);
        wl->next[0]->dir = (int)(2 * MR->rdm());
        dw = wl->next[0];
        n0 = !n0;
      }

      if (dw->type == 6) {
        dw->next[0]->next[1] = dw->next[1];
        dw->next[1]->next[0] = dw->next[0];
      }
    }
  }

  EVmax = ev.size();
  W[0] = WORM.size();
}

void GraphSpace::parity_check(My_rdm *MR, int py, double *x, int &j,
                              double topt, double bottomt) {
  // j: # of worms in a segment
  // py: parity
  double Il = topt - bottomt;
  double dt, dtmin = NMIN;
  double Ia = Il * P->rh_even;

  j = (Ia == 0.0) ? 0 : NumberOfVertex(MR, Ia, py);

  if (j != 0) {
    if (Il <= (double)(j + 1) * NMIN) {
      if (PR->my_rank == 0)
        cout << "Il is too small: Il=" << Il << ", topt=" << topt
             << ", bottomt=" << bottomt << endl;
      j = py % 2;
      if (Il <= (double)(j + 1) * NMIN) {
        if (PR->my_rank == 0) cout << "///Il has error" << endl;
      }
    }

    for (int i = 0; i < j; i++) {
      dt = Il * MR->rdm();
      if (dt <= dtmin) {
        dt = dtmin;
        dtmin = dtmin + NMIN;
      }
      x[i] = bottomt + dt;
    }

    sort(x, x + j);
  }
}

int GraphSpace::NumberOfVertex(My_rdm *MR, double m, int py) {
  int n = py, fn = 1;
  double Pn = 0.0;
  double pos;

  double R = MR->rdm();
  double POW, EXP;
  POW = (n) ? m : 1.0;
  int FAC = 1;
  EXP = (this->*fmath[py])(m);

  while (1) {
    FAC *= fn;  // factorial( n );
    pos = POW / (double)FAC;

    Pn += pos;

    if (R < Pn / EXP) {
      return n;
    } else {
      n += 2;
      fn = n;
      POW *= m * m;
    }
  }
}

//#############################################################################################
void GraphSpace::SpatialDomainBoundary(My_rdm *MR) {
  int a, b, left, right, rnum;
  double I1, I2, I3, I4;
  bool py1, py2, py3, py4, next = false;
  double PP[6], R;

  int inmax, k;

  MPI_Status status;

  for (int d = 0; d < LT->Pd; d++) {
    inmax = 0;

    for (int i = 0; i < LT->Fx[d]; i++) {
      right = LT->frame_rsite[d][i];
      k = 0;
      for (Vertex *wl = world[right].next[1]; wl != &(world[right]);
           wl = wl->next[1]) {  // until worldB

        if (((wl->i) / V == d && wl->type <= -3 && wl->type >= -4) || next) {
          BoxSpace_t_th0[d][f(d, i, k)] = wl->t - wl->next[0]->t;
          BoxSpace_py_th0[f(d, i, k)] = (short)wl->dir;
          if ((wl->i) / V != d || wl->type > -3 || wl->type < -4) {
            BoxSpace_type_th0[f(d, i, k)] = 7;
            next = false;
          } else {
            BoxSpace_type_th0[f(d, i, k)] = wl->type;
            next = true;
          }
          BoxSpace_p_th0[d][f(d, i, k)] = wl->p;
          k++;
        }
      }

      inmax = max(k, inmax);
    }

    NewVertex_th0[d] = inmax * LT->Fx[d];

    MPI_Sendrecv(&(NewVertex_th0[d]), 1, MPI_INT, PR->left[d], 0,
                 &(NewVertex_th1[d]), 1, MPI_INT, PR->right[d], 0,
                 MPI_COMM_WORLD, &status);
    MPI_Sendrecv(&(BoxSpace_py_th0[0]), NewVertex_th0[d], MPI_SHORT,
                 PR->left[d], 3, &(BoxSpace_py_th1[0]), NewVertex_th1[d],
                 MPI_SHORT, PR->right[d], 3, MPI_COMM_WORLD, &status);
    MPI_Sendrecv(&(BoxSpace_p_th0[d][0]), NewVertex_th0[d], MPI_SHORT,
                 PR->left[d], 1, &(BoxSpace_p_th1[d][0]), NewVertex_th1[d],
                 MPI_SHORT, PR->right[d], 1, MPI_COMM_WORLD, &status);
    MPI_Sendrecv(&(BoxSpace_type_th0[0]), NewVertex_th0[d], MPI_SHORT,
                 PR->left[d], 4, &(BoxSpace_type_th1[0]), NewVertex_th1[d],
                 MPI_SHORT, PR->right[d], 4, MPI_COMM_WORLD, &status);
    MPI_Sendrecv(&(BoxSpace_t_th0[d][0]), NewVertex_th0[d], MPI_DOUBLE,
                 PR->left[d], 2, &(BoxSpace_t_th1[d][0]), NewVertex_th1[d],
                 MPI_DOUBLE, PR->right[d], 2, MPI_COMM_WORLD, &status);

    for (int i = 0; i < LT->Fx[d]; i++) {
      rnum = 0;
      left = LT->frame_lsite[d][i];

      for (Vertex *wl = world[left].next[1]; wl != &(worldB[left]);
           wl = wl->next[1]) {
        if ((wl->i) / V == d &&
            (wl->type == -1 || wl->type == -2)) {  // judge by the left

          while (BoxSpace_type_th1[f(d, i, rnum)] == 7) {
            rnum++;
          }

          b = (wl->type == -2) ? BoxSpace_p_th1[d][f(d, i, rnum)] + 2 * wl->p
                               : wl->p + 4;

          I1 = wl->t - wl->next[0]->t;
          I2 = wl->next[1]->t - wl->t;
          I3 = BoxSpace_t_th1[d][f(d, i, rnum + 1)];
          I4 = BoxSpace_t_th1[d][f(d, i, rnum)];
          I1 = tanh(I1 * P->rh_even);
          I2 = tanh(I2 * P->rh_even);
          I3 = tanh(I3 * P->rh_even);
          I4 = tanh(I4 * P->rh_even);

          py1 = wl->dir;
          py2 = wl->next[1]->dir;
          py3 = (bool)BoxSpace_py_th1[f(d, i, rnum + 1)];
          py4 = (bool)BoxSpace_py_th1[f(d, i, rnum)];

          //	 int xl=LT->lx[left];

          PP[0] = (this->*Form[b][0])(I1, I2, I3, I4, py1, py2, py3, py4) *
                  P->FracW[b][0][0];

          for (a = 1; a < 6; a++) {
            PP[a] = (this->*Form[b][a])(I1, I2, I3, I4, py1, py2, py3, py4) *
                    P->FracW[b][a][0];
            PP[a] += PP[a - 1];
          }

          R = MR->rdm() * PP[5];
          for (a = 0; a < 6; a++)
            if (R < PP[a]) break;

          if (a != b) {
            (this->*Parity_Update[b][a])(wl, d, i, rnum);
            (this->*Event_Update[a])(wl, d, i, rnum);
          }

          rnum++;
        }
      }
    }

    MPI_Sendrecv(&(BoxSpace_p_th1[d][0]), NewVertex_th1[d], MPI_SHORT,
                 PR->right[d], 0, &(BoxSpace_p_th0[d][0]), NewVertex_th0[d],
                 MPI_SHORT, PR->left[d], 0, MPI_COMM_WORLD, &status);
    MPI_Sendrecv(&(BoxSpace_py_th1[0]), NewVertex_th1[d], MPI_SHORT,
                 PR->right[d], 0, &(BoxSpace_py_th0[0]), NewVertex_th0[d],
                 MPI_SHORT, PR->left[d], 0, MPI_COMM_WORLD, &status);
    MPI_Sendrecv(&(BoxSpace_type_th1[0]), NewVertex_th1[d], MPI_SHORT,
                 PR->right[d], 0, &(BoxSpace_type_th0[0]), NewVertex_th0[d],
                 MPI_SHORT, PR->left[d], 0, MPI_COMM_WORLD, &status);

    next = false;

    for (int i = 0; i < LT->Fx[d]; i++) {
      right = LT->frame_rsite[d][i];
      k = 0;
      for (Vertex *wl = world[right].next[1]; wl != &(world[right]);
           wl = wl->next[1]) {  // until worldB

        if (((wl->i) / V == d && wl->type <= -3 && wl->type >= -4) || next) {
          wl->dir = BoxSpace_py_th0[f(d, i, k)];
          if (BoxSpace_type_th0[f(d, i, k)] != 7) {
            wl->type = BoxSpace_type_th0[f(d, i, k)];
            wl->p = BoxSpace_p_th0[d][f(d, i, k)];
            next = true;
          } else
            next = false;
          k++;
        }
      }
    }
  }
}

void GraphSpace::TemporalDomainBoundary(My_rdm *MR, Quantities *QNT) {
  // Here, P0 (P1) means the upper (lower) side interval than the temporal
  // domain boundary.

  double R, P_i;
  bool py0, py1;
  double Om0, Om1;
  double I0;  // the length of the interval P0
  double I1;  // the length of the interval P1

  double exp0, exp1;

  // MPI communication here. ( send information to the upper domain and receive
  // from the lower domain. )
  OldBox();

  //###### "the domain boundary (world)" is updated in P0 ######
  for (int site = 0; site < V; site++) {
    py0 = world[site].next[1]->dir;            // the parity of P0
    py1 = (p0_box[site] + world[site].p) % 2;  // the parity of P1.

    I0 = world[site].next[1]->t;
    I1 = i0_box[site];
    exp0 = exp(I0 * P->rh_even);
    exp1 = exp(I1 * P->rh_even);

    //######################## calculation of <Q>, <QQ>
    //####################################
    CondensateFraction(site, QNT, py0, py1, exp0, exp1);
    //############################################################

    p0_box[site] = py1;

    Om0 = (this->*fexp[!py0])(exp0) / (this->*fexp[py0])(exp0);
    Om1 = (this->*fexp[!py1])(exp1) / (this->*fexp[py1])(exp1);

    R = MR->rdm();
    P_i = Om0 * Om1;

    if (P_i > R) {  // update the temporal domain boundary.

      world[site].next[1]->dir = !(py0);
      p0_box[site] = !(py1);
      py0 = world[site].p;
      world[site].p = !py0;
    }
  }

  //############################################################
  NewBox();
}

void GraphSpace::OldBox() {
  for (int site = 0; site < V; site++) {
    p1_box[site] = worldB[site].next[0]->p;
    i1_box[site] = B - worldB[site].next[0]->t;
  }

  MPI_Status status;

  MPI_Sendrecv(&(p1_box[0]), V, MPI_SHORT, PR->upper, 0, &(p0_box[0]), V,
               MPI_SHORT, PR->lower, 0, MPI_COMM_WORLD, &status);
  MPI_Sendrecv(&(i1_box[0]), V, MPI_DOUBLE, PR->upper, 0, &(i0_box[0]), V,
               MPI_DOUBLE, PR->lower, 0, MPI_COMM_WORLD, &status);
}

void GraphSpace::NewBox() {
  MPI_Status status;

  MPI_Sendrecv(&(p0_box[0]), V, MPI_SHORT, PR->lower, 0, &(p1_box[0]), V,
               MPI_SHORT, PR->upper, 0, MPI_COMM_WORLD, &status);

  for (int site = 0; site < V; site++) worldB[site].dir = p1_box[site];
}

void GraphSpace::CondensateFraction(int site, Quantities *QNT, bool py0,
                                    bool py1, double exp0, double exp1) {
  // 0:upper, 1:lower

  bool cf;
  double Ra, Rc, Rca, Rac, norm;
  bool PY = (py0 + py1) % 2;

  norm = (this->*fexp[PY])(exp0 * exp1);

  cf = (PY + p0_box[site]) % 2;

  Ra = (this->*fexp[!(p0_box[site])])(exp1) * (this->*fexp[cf])(exp0);
  Rc = (this->*fexp[p0_box[site]])(exp1) * (this->*fexp[!cf])(exp0);
  Rca = (this->*fexp[!(p0_box[site])])(exp1) * (this->*fexp[!cf])(exp0);
  Rac = (this->*fexp[p0_box[site]])(exp1) * (this->*fexp[cf])(exp0);

  QNT->an[site] = Ra / norm;
  QNT->cr[site] = Rc / norm;
  QNT->ca[site] = Rca / norm;
  QNT->ac[site] = Rac / norm;
}
