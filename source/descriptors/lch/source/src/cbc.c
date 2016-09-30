#include "cbc.h"

#include "common.h"
#include "featurevector.h"
#include "set.h"
#include "sort.h"

#define MIN_AREA        0.1
#define MIN_DISTANCE      3
#define ALPHA         0.875

typedef struct _Edge {
  int p;
  int q;
  int d;
} Edge;

typedef struct _Node {
  int L, a, b;
  int h, v;
  int s;
} Node;

typedef struct _Graph {
  Node *nodes;
  Edge *edges;
  int nnodes;
  int nedges;
} Graph;

typedef struct _Region {
  Node *nodes;
  int n;
} Region;

int CompareEdge(const void *a, const void *b) {
  return (*(Edge *)a).d - (*(Edge *)b).d;
}

Edge *AllocEdgeArray(int n)
{
  Edge *v=NULL;
  v = (Edge *) calloc(n,sizeof(Edge));
  if (v==NULL)
    Error(MSG1,"AllocEdgeArray");
  return(v);
}

Node *AllocNodeArray(int n)
{
  Node *v=NULL;
  v = (Node *) calloc(n,sizeof(Node));
  if (v==NULL)
    Error(MSG1,"AllocNodeArray");
  return(v);
}

Node *ComputeNodes(CImage *clab)
{
  Node *n=NULL;
  int ncols, nrows;
  int x, y;
  int p;

  ncols = clab->C[0]->ncols; 
  nrows = clab->C[0]->nrows;

  n = AllocNodeArray(ncols * nrows);
  for (y=0; y<nrows; y++)
    for (x=0; x<ncols; x++) {
      p = x + ncols * y;
      n[p].L = clab->C[0]->val[p];
      n[p].a = clab->C[1]->val[p];
      n[p].b = clab->C[2]->val[p];
      n[p].h = x;
      n[p].v = y;
      n[p].s = 1;
  }

  return(n); 
}

Edge *ComputeEdges(CImage *clab)
{
  Edge *e=NULL;
  long x, y, p, q;
  int ncols, nrows;
  int L, a, b;
  int *tbrow;
  int i;
  
  tbrow = clab->C[0]->tbrow;
  ncols = clab->C[0]->ncols;
  nrows = clab->C[0]->nrows;
  
  e = AllocEdgeArray(2 * ncols * nrows - ncols - nrows);
  
  i = 0;
  for(y=0; y<nrows - 1; y++){
    for(x=0; x<ncols - 1; x++){
      p = x + tbrow[y];
      q = (x + 1) + tbrow[y];
      e[i].p = p;
      e[i].q = q;
      L = clab->C[0]->val[p] - clab->C[0]->val[q];
      a = clab->C[1]->val[p] - clab->C[1]->val[q];
      b = clab->C[2]->val[p] - clab->C[2]->val[q];
      e[i].d = L * L + a * a + b * b;
      i++;
      q = x + tbrow[y + 1];
      e[i].p = p;
      e[i].q = q;
      L = clab->C[0]->val[p] - clab->C[0]->val[q];
      a = clab->C[1]->val[p] - clab->C[1]->val[q];
      b = clab->C[2]->val[p] - clab->C[2]->val[q];
      e[i].d = L * L + a * a + b * b;
      i++;      
    }
  }

  x = ncols - 1;
  for (y=0; y<nrows - 1; y++){
    p = x + tbrow[y];
    q = x + tbrow[y + 1];
    e[i].p = p;
    e[i].q = q;
    L = clab->C[0]->val[p] - clab->C[0]->val[q];
    a = clab->C[1]->val[p] - clab->C[1]->val[q];
    b = clab->C[2]->val[p] - clab->C[2]->val[q];
    e[i].d = L * L + a * a + b * b;
    i++;  
  }

  y = nrows - 1;
  for (x=0; x<ncols - 1; x++){
    p = x + tbrow[y];
    q = (x + 1) + tbrow[y];
    e[i].p = p;
    e[i].q = q;
    L = clab->C[0]->val[p] - clab->C[0]->val[q];
    a = clab->C[1]->val[p] - clab->C[1]->val[q];
    b = clab->C[2]->val[p] - clab->C[2]->val[q];
    e[i].d = L * L + a * a + b * b;
    i++;  
  }

  return(e);
}

Graph *CreateGraph(CImage *cimg)
{
  Graph *g=NULL;
  CImage *cxyz, *clab;
  int ncols, nrows;

  ncols = cimg->C[0]->ncols;
  nrows = cimg->C[0]->nrows;

  cxyz = CImageRGBtoXYZ(cimg);
  clab = CImageXYZtoLAB(cxyz);
  DestroyCImage(&cxyz);

  g = (Graph *) calloc(1,sizeof(Graph));
  if (g != NULL) {
    g->nodes = ComputeNodes(clab);
    g->edges = ComputeEdges(clab);
    g->nnodes = ncols * nrows;
    g->nedges = 2 * ncols * nrows - ncols - nrows;
  } else {
    Error(MSG1,"CreateGraph");
  }
  DestroyCImage(&clab);
  return(g);
}

void DestroyGraph(Graph **g)
{
  Graph *aux;

  aux = *g;
  if (aux != NULL) {
    if (aux->nodes != NULL) free(aux->nodes);
    if (aux->edges != NULL) free(aux->edges);
    free(aux);
    *g = NULL;
  }
}

Region *CreateRegion(int n)
{
  Region *r=NULL;

  r = (Region *) calloc(1,sizeof(Region));
  if (r != NULL) {
    r->nodes = AllocNodeArray(n);
    r->n = n;
  } else {
    Error(MSG1,"CreateRegion");
  }
  return(r);
}

void DestroyRegion(Region **r)
{
  Region *aux;

  aux = *r;
  if (aux != NULL) {
    if (aux->nodes != NULL) free(aux->nodes);
    free(aux);
    *r = NULL;
  }
}

Region *MakeRegions(Graph *g, Image *mask)
{
  Region *r=NULL;
  int L, a, b, h, v, s;
  bool stop;
  Set *set;
  int p, q;
  int i, n;

  set = CreateSet(g->nnodes);
  n   = g->nnodes;
  for (i=0;i<g->nnodes;i++)
    if (mask->val[i])
      MakeSet(set, i);
    else
      n--;

  QuickSort(g->edges, g->nedges, sizeof(Edge), CompareEdge);

  stop = false;
  for (i=0;i<g->nedges && !stop;i++) {
    if (mask->val[g->edges[i].p] && mask->val[g->edges[i].q]) {
      p = FindSet(set,g->edges[i].p);
      q = FindSet(set,g->edges[i].q);
      if (p != q) {
        L = g->nodes[p].L / g->nodes[p].s - g->nodes[q].L / g->nodes[q].s;
        a = g->nodes[p].a / g->nodes[p].s - g->nodes[q].a / g->nodes[q].s;
        b = g->nodes[p].b / g->nodes[p].s - g->nodes[q].b / g->nodes[q].s;
        if (100 * (L * L + a * a + b * b) < MIN_DISTANCE * 195075) {
          L = g->nodes[p].L + g->nodes[q].L;
          a = g->nodes[p].a + g->nodes[q].a;
          b = g->nodes[p].b + g->nodes[q].b;
          h = g->nodes[p].h + g->nodes[q].h;
          v = g->nodes[p].v + g->nodes[q].v;
          s = g->nodes[p].s + g->nodes[q].s;
          Union(set,p,q);
          p = FindSet(set,q);
          g->nodes[p].L = L;
          g->nodes[p].a = a;
          g->nodes[p].b = b;
          g->nodes[p].h = h;
          g->nodes[p].v = v;
          g->nodes[p].s = s;
          n--;
        }
        else
          stop = true;
      }
    }
  }

  for (; i<g->nedges; i++) {
    if (mask->val[g->edges[i].p] && mask->val[g->edges[i].q]) {
      p = FindSet(set,g->edges[i].p);
      q = FindSet(set,g->edges[i].q);
      if (p != q) {
        if (100 * g->nodes[p].s < MIN_AREA * g->nnodes ||
            100 * g->nodes[q].s < MIN_AREA * g->nnodes) {
          L = g->nodes[p].L + g->nodes[q].L;
          a = g->nodes[p].a + g->nodes[q].a;
          b = g->nodes[p].b + g->nodes[q].b;
          h = g->nodes[p].h + g->nodes[q].h;
          v = g->nodes[p].v + g->nodes[q].v;
          s = g->nodes[p].s + g->nodes[q].s;
          Union(set,p,q);
          p = FindSet(set,q);
          g->nodes[p].L = L;
          g->nodes[p].a = a;
          g->nodes[p].b = b;
          g->nodes[p].h = h;
          g->nodes[p].v = v;
          g->nodes[p].s = s;
          n--;
        }
      }
    }
  }

  r = CreateRegion(n);
  i = 0;
  for (p=0; p<set->n; p++)
    if (mask->val[p] && p == FindSet(set,p))
      r->nodes[i++] = g->nodes[p];
  DestroySet(&set);

  return(r);
}

Region *ComputeRegions(CImage *cimg, Image *mask)
{
  Region *r=NULL;
  Graph *g;

  g = CreateGraph(cimg);
  r = MakeRegions(g, mask);
  DestroyGraph(&g);

  return(r);
}

Ap_FeatureVector1D *CBC(CImage *cimg, Image *mask, int *nfeatures)
{
  Ap_FeatureVector1D *ap_fv=NULL;
  int ncols, nrows, npxls;
  Region *r;
  int i, s;

  ncols = cimg->C[0]->ncols;
  nrows = cimg->C[0]->nrows;
  npxls = ncols * nrows;

  r = ComputeRegions(cimg, mask);

  ap_fv = (Ap_FeatureVector1D *)calloc(r->n,sizeof(Ap_FeatureVector1D));
  if (ap_fv == NULL) {
    Error(MSG1,"AllocAp_FeatureVector1DArray");
  }

  for (i=0; i<r->n; i++) {
    ap_fv[i] = CreateFeatureVector1D(6);
    s = r->nodes[i].s;
    ap_fv[i]->X[0] = r->nodes[i].L / (double) s;
    ap_fv[i]->X[1] = r->nodes[i].a / (double) s;
    ap_fv[i]->X[2] = r->nodes[i].b / (double) s;
    ap_fv[i]->X[3] = (r->nodes[i].h / (double) s) / (double)ncols;
    ap_fv[i]->X[4] = (r->nodes[i].v / (double) s) / (double)nrows;
    ap_fv[i]->X[5] = s / (double) npxls;
  }
  *nfeatures = r->n;
  DestroyRegion(&r);

  return(ap_fv);
}

double CBCRegionDistance(FeatureVector1D *fv1, FeatureVector1D *fv2)
{
  double L, a, b, h, v;

  L = fv1->X[0] - fv2->X[0];
  a = fv1->X[1] - fv2->X[1];
  b = fv1->X[2] - fv2->X[2];
  h = fv1->X[3] - fv2->X[3];
  v = fv1->X[4] - fv2->X[4];

  return (ALPHA * (L * L + a * a + b * b) + (1.0 - ALPHA) * (h * h + v * v));
}

