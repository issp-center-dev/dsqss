void GraphSpace::Worm_Update(My_rdm *MR) {
  long Nc = Ncyc;

  while (Nc--) {
    vector<Vertex *>::iterator it = WORM.begin();

    while (it != WORM.end()) {
      worm = *it;

      if (Next_event(MR)) { transition(MR); }

      ++it;
    }
  }

  //  WORM.clear();
}

bool GraphSpace::Next_event(My_rdm *MR) {
  bool dir;
  short type;

  dir  = worm->dir;
  type = worm->next[dir]->type;

  if (type == 0)
    // onsite vertex
    return update_selection(MR, dir);
  else if (type < 3 && type > 0)
    // kink or vertex
    return update_selection(MR, dir, type);
  else {
    // another worm or domain (wall)
    // I will be bounce
    double Ib = LengthofWalk(worm->next[dir]->t, worm->t, dir);
    double Ia = LengthofWalk(worm->next[dir]->t, worm->next[!dir]->t, dir);
    double la = Ia * MR->rdm();
    if (la < NMIN) la = NMIN;

    if (For_Nworm) Wlen += Ib + la;

    worm->t   = (dir) ? worm->next[1]->t - la : worm->next[0]->t + la;
    worm->dir = !dir;

    return false;
  }
}

void GraphSpace::transition(My_rdm *MR) {
  int a; // after
  int b; // before
  int ni, nj;  //,xl;
  double R, Pt;
  double dla;

  // dir == false : downward
  // dir == true : upward
  bool dir        = worm->dir;

  // oh == false : annihilation
  // oh == true : creation
  bool oh         = worm->type - 4;

  short next_type = vertex[0]->type;

  //a,b : worm position
  //0: below the next vertex
  //1: above the next vertex
  //2: above the next kink
  //3: below the next kink

  switch (next_type) {
  case 0:  //Onsite

    b = (oh == dir) ? 0 : 1;

    if (MR->rdm() < P->u[b][vertex[0]->p][vertex[0]->i]) {  //pass
      (oh == dir) ? vertex[0]->p++ : vertex[0]->p--;
      relink(dir, dir, 0);
    } else
      worm->dir = !dir;  //bounce

    dir = worm->dir;  //new direction

    dla     = I[dir] * MR->rdm();
    worm->t = tA + sgn[dir] * dla;

    if (For_Nworm) Wlen += dla;

    return;

  case 1:  //nearest-Vertex

    b = !dir;

    ni = worm->p;
    nj = vertex[1]->p;
    //    xl = LT->lx[worm->i%V];
    break;
  case 2:  //Kink

    ni = vertex[1]->p;
    //    xl = LT->lx[vertex[1]->i%V];
    b = 2 + dir;

    if (!dir)
      nj = worm->p;  //b=2;
    else
      nj = vertex[0]->p;  //b=3;

    break;
  }

  R  = MR->rdm();
  Pt = 0.0;

  for (a = 0; a < 4; a++) {
    Pt += P->t[oh][a][b][ni][nj][0];

    if (R < Pt) break;
  }

  t_update(MR, b, a, oh, dir);
}

void GraphSpace::t_update(My_rdm *MR, int b, int a, bool oh, bool dir) {
  int i = Switch[b][a];

  double dla, R;

  if (For_Nworm) Wlen += LengthofWalk(tA, worm->t, dir);

  dla = I[a] * MR->rdm();
  if (dla < NMIN) dla = NMIN;

  (this->*Transition[i])(oh, dir, b, a, dla);
}

void GraphSpace::bounce(bool oh, bool dir, int b, int a, double dla) {
  worm->dir = !dir;
  Newtime(a, dla);
}

void GraphSpace::pass(bool oh, bool dir, int b, int a, double dla) {
  (oh == dir) ? (vertex[0]->p++) : (vertex[0]->p--);
  relink(dir, dir, 0);
  Newtime(a, dla);
}

void GraphSpace::hop(bool oh, bool dir, int b, int a, double dla) {
  if (oh == dir) {
    vertex[dir]->p++;
  } else {
    vertex[dir]->p--;
  }

  if (b < 2)
    v_neibor();
  else
    k_neibor();

  relink(dir, dir, 1);
  Newtime(a, dla);
}

void GraphSpace::turn(bool oh, bool dir, int b, int a, double dla) {
  if (!dir) {  // b=1,2 are downward
    vertex[!oh]->p--;
    vertex[oh]->p++;
  }

  if (b < 2)
    v_neibor_dir();
  else
    k_neibor_dir();

  relink(dir, !dir, 1);
  Newtime(a, dla);
}

void GraphSpace::k_neibor() {
  vertex[0]->type = vertex[1]->type = 1;
  worm->i                           = vertex[1]->i % V;
}

void GraphSpace::k_neibor_dir() {  // w/ changing direction

  worm->dir = !(worm->dir);
  k_neibor();
}

void GraphSpace::v_neibor() {
  vertex[0]->type = vertex[1]->type = 2;
  worm->i                           = vertex[1]->i % V;
}

void GraphSpace::v_neibor_dir() {
  worm->dir = !(worm->dir);
  v_neibor();
}

void GraphSpace::Newtime(int a, double dla) {
  double ta, tb;

  bool dir = worm->dir;  //new direction

  tb = tA;
  ta = dla + dir * tA + (!dir) * t[a];

  worm->t = ta;

  if (For_Nworm) Wlen += LengthofWalk(ta, tb, dir);
}

inline double GraphSpace::LengthofWalk(double next_t, double curr_t, bool dir) {
  return (1.0 - 2.0 * dir) * (curr_t - next_t);
}

void GraphSpace::relink(bool b_dir, bool a_dir, bool aside) {
  vertex[aside]->next[a_dir]->next[!a_dir] = worm;       //1
  worm->next[!b_dir]->next[b_dir]          = vertex[0];  //2

  vertex[0]->next[!b_dir] = worm->next[!b_dir];          //3
  worm->next[a_dir]       = vertex[aside]->next[a_dir];  //4

  vertex[aside]->next[a_dir] = worm;           //5
  worm->next[!a_dir]         = vertex[aside];  //6
}

bool GraphSpace::update_selection(My_rdm *MR, bool dir, short type) {
  double Imin;

  vertex[0] = worm->next[dir];
  vertex[1] = worm->next[dir]->nleg;

  tA = vertex[0]->t;  // time of the next vertex
  double tc = worm->next[!dir]->t;  // time of the previous vertex
  double Ic = (1.0 - 2.0 * dir) * (tc - tA);

  // ratate vertex 
  
  // cnum(type,dir):
  // cnum(1,0) = 1
  // cnum(1,1) = 0
  // cnum(2,0) = 2
  // cnum(2,1) = 3
  int cnum = type + (1.0 - 2.0 * (type % 2)) * dir;

  t[0] = vertex[type - 1]->next[0]->t;
  t[1] = vertex[type - 1]->next[1]->t;
  t[2] = vertex[type % 2]->next[1]->t;
  t[3] = vertex[type % 2]->next[0]->t;

  t[cnum] = tc;

  I[0] = tA - t[0];
  I[1] = t[1] - tA;
  I[2] = t[2] - tA;
  I[3] = tA - t[3];

  Imin = min(min(I[0], I[1]), min(I[2], I[3]));

  bool scat = (Imin >= 2.0 * NMIN) && (Imin / Ic > MR->rdm());
  if (!scat) {
    // はねかえる
    if (For_Nworm) Wlen += LengthofWalk(tA, worm->t, dir);
    worm->dir = !(worm->dir);
  }

  return scat;
}

bool GraphSpace::update_selection(My_rdm *MR, bool dir) {
  double Imin, Ic;

  double tc;

  vertex[0] = worm->next[dir];

  tA = vertex[0]->t;  // time of the next vertex

  tc = worm->next[!dir]->t;  // time of the previous vertex

  Ic = (1.0 - 2.0 * dir) * (tc - tA);

  t[0] = vertex[0]->next[0]->t;
  t[1] = vertex[0]->next[1]->t;

  t[!dir] = tc;

  I[0] = tA - t[0];
  I[1] = t[1] - tA;

  Imin = min(I[0], I[1]);
  bool scat;

  scat = (Imin >= 2.0 * NMIN) ? (Imin / Ic > MR->rdm()) : false;

  if (!scat) {
    if (For_Nworm) Wlen += LengthofWalk(tA, worm->t, dir);
    worm->dir = !(worm->dir);
  }

  return scat;
}
