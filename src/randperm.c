#include "rt_nonfinite.h"
#include <math.h>
#include "simulate_abm.h"
#include "randperm.h"
#include "simulate_abm_emxutil.h"
#include "rand.h"
#include "mod.h"

void b_randperm(double n, emxArray_real_T *p)
{
  emxArray_int32_T *idx;
  int b_n;
  int j;
  int i;
  emxArray_int32_T *iwork;
  int i2;
  int k;
  int pEnd;
  int b_p;
  int q;
  int qEnd;
  int kEnd;
  emxInit_int32_T(&idx, 2);
  d_rand(n, p);
  b_n = p->size[1] + 1;
  j = idx->size[0] * idx->size[1];
  idx->size[0] = 1;
  idx->size[1] = p->size[1];
  emxEnsureCapacity_int32_T(idx, j);
  i = p->size[1];
  for (j = 0; j < i; j++) {
    idx->data[j] = 0;
  }

  if (p->size[1] != 0) {
    emxInit_int32_T(&iwork, 1);
    j = iwork->size[0];
    iwork->size[0] = p->size[1];
    emxEnsureCapacity_int32_T(iwork, j);
    j = p->size[1] - 1;
    for (k = 1; k <= j; k += 2) {
      if ((p->data[k - 1] <= p->data[k]) || rtIsNaN(p->data[k])) {
        idx->data[k - 1] = k;
        idx->data[k] = k + 1;
      } else {
        idx->data[k - 1] = k + 1;
        idx->data[k] = k;
      }
    }

    if ((p->size[1] & 1) != 0) {
      idx->data[p->size[1] - 1] = p->size[1];
    }

    i = 2;
    while (i < b_n - 1) {
      i2 = i << 1;
      j = 1;
      for (pEnd = 1 + i; pEnd < b_n; pEnd = qEnd + i) {
        b_p = j;
        q = pEnd - 1;
        qEnd = j + i2;
        if (qEnd > b_n) {
          qEnd = b_n;
        }

        k = 0;
        kEnd = qEnd - j;
        while (k + 1 <= kEnd) {
          if ((p->data[idx->data[b_p - 1] - 1] <= p->data[idx->data[q] - 1]) ||
              rtIsNaN(p->data[idx->data[q] - 1])) {
            iwork->data[k] = idx->data[b_p - 1];
            b_p++;
            if (b_p == pEnd) {
              while (q + 1 < qEnd) {
                k++;
                iwork->data[k] = idx->data[q];
                q++;
              }
            }
          } else {
            iwork->data[k] = idx->data[q];
            q++;
            if (q + 1 == qEnd) {
              while (b_p < pEnd) {
                k++;
                iwork->data[k] = idx->data[b_p - 1];
                b_p++;
              }
            }
          }

          k++;
        }

        for (k = 0; k < kEnd; k++) {
          idx->data[(j + k) - 1] = iwork->data[k];
        }

        j = qEnd;
      }

      i = i2;
    }

    emxFree_int32_T(&iwork);
  }

  i = p->size[0];
  i2 = p->size[1];
  j = p->size[0] * p->size[1];
  p->size[0] = 1;
  p->size[1] = i2;
  emxEnsureCapacity_real_T(p, j);
  i *= i2;
  for (j = 0; j < i; j++) {
    p->data[j] = idx->data[j];
  }

  emxFree_int32_T(&idx);
}

void randperm(double n, double k, emxArray_real_T *p)
{
  int i18;
  int loop_ub;
  emxArray_real_T *hashTbl;
  double t;
  int m;
  double j;
  double selectedLoc;
  double newEntry;
  int j_tmp;
  emxArray_real_T *link;
  double nleftm1;
  emxArray_real_T *val;
  emxArray_real_T *loc;
  boolean_T exitg1;
  i18 = p->size[0] * p->size[1];
  p->size[0] = 1;
  loop_ub = (int)k;
  p->size[1] = loop_ub;
  emxEnsureCapacity_real_T(p, i18);
  for (i18 = 0; i18 < loop_ub; i18++) {
    p->data[i18] = 0.0;
  }

  if (!(k == 0.0)) {
    if (k >= n) {
      p->data[0] = 1.0;
      i18 = (int)(n - 1.0);
      for (loop_ub = 0; loop_ub < i18; loop_ub++) {
        j = c_rand() * ((1.0 + (double)loop_ub) + 1.0);
        j = floor(j);
        j_tmp = (int)(j + 1.0) - 1;
        p->data[1 + loop_ub] = p->data[j_tmp];
        p->data[j_tmp] = (1.0 + (double)loop_ub) + 1.0;
      }
    } else if (k >= n / 4.0) {
      t = 0.0;
      i18 = (int)((k - 1.0) + 1.0);
      for (m = 0; m < i18; m++) {
        selectedLoc = k - (double)m;
        newEntry = n - t;
        nleftm1 = selectedLoc / newEntry;
        j = c_rand();
        while (j > nleftm1) {
          t++;
          newEntry--;
          nleftm1 += (1.0 - nleftm1) * (selectedLoc / newEntry);
        }

        t++;
        j = c_rand() * ((double)m + 1.0);
        j = floor(j);
        j_tmp = (int)(j + 1.0) - 1;
        p->data[m] = p->data[j_tmp];
        p->data[j_tmp] = t;
      }
    } else {
      emxInit_real_T(&hashTbl, 1);
      i18 = hashTbl->size[0];
      hashTbl->size[0] = loop_ub;
      emxEnsureCapacity_real_T(hashTbl, i18);
      for (i18 = 0; i18 < loop_ub; i18++) {
        hashTbl->data[i18] = 0.0;
      }

      emxInit_real_T(&link, 1);
      i18 = link->size[0];
      link->size[0] = loop_ub;
      emxEnsureCapacity_real_T(link, i18);
      for (i18 = 0; i18 < loop_ub; i18++) {
        link->data[i18] = 0.0;
      }

      emxInit_real_T(&val, 1);
      i18 = val->size[0];
      val->size[0] = loop_ub;
      emxEnsureCapacity_real_T(val, i18);
      for (i18 = 0; i18 < loop_ub; i18++) {
        val->data[i18] = 0.0;
      }

      emxInit_real_T(&loc, 1);
      i18 = loc->size[0];
      loc->size[0] = loop_ub;
      emxEnsureCapacity_real_T(loc, i18);
      for (i18 = 0; i18 < loop_ub; i18++) {
        loc->data[i18] = 0.0;
      }

      newEntry = 1.0;
      for (m = 0; m < loop_ub; m++) {
        nleftm1 = n - (1.0 + (double)m);
        selectedLoc = c_rand() * (nleftm1 + 1.0);
        selectedLoc = floor(selectedLoc);
        j_tmp = (int)(1.0 + b_mod(selectedLoc, k)) - 1;
        j = hashTbl->data[j_tmp];
        exitg1 = false;
        while ((!exitg1) && (j > 0.0)) {
          i18 = (int)j - 1;
          if (loc->data[i18] != selectedLoc) {
            j = link->data[i18];
          } else {
            exitg1 = true;
          }
        }

        if (j > 0.0) {
          p->data[m] = val->data[(int)j - 1] + 1.0;
        } else {
          p->data[m] = selectedLoc + 1.0;
          j = newEntry;
          newEntry++;
          i18 = (int)j - 1;
          loc->data[i18] = selectedLoc;
          link->data[i18] = hashTbl->data[j_tmp];
          hashTbl->data[j_tmp] = j;
        }

        if (1.0 + (double)m < k) {
          selectedLoc = hashTbl->data[(int)(1.0 + b_mod(nleftm1, k)) - 1];
          exitg1 = false;
          while ((!exitg1) && (selectedLoc > 0.0)) {
            i18 = (int)selectedLoc - 1;
            if (loc->data[i18] != nleftm1) {
              selectedLoc = link->data[i18];
            } else {
              exitg1 = true;
            }
          }

          if (selectedLoc > 0.0) {
            val->data[(int)j - 1] = val->data[(int)selectedLoc - 1];
          } else {
            val->data[(int)j - 1] = nleftm1;
          }
        }
      }

      emxFree_real_T(&loc);
      emxFree_real_T(&val);
      emxFree_real_T(&link);
      emxFree_real_T(&hashTbl);
    }
  }
}
