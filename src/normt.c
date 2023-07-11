#include <math.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "normt.h"
#include "sum.h"
#include "simulate_abm_emxutil.h"
#include "randperm.h"
#include "round.h"
#include "simulate_abm_rtwutil.h"

void normt(emxArray_real_T *I_, double min_)
{
  double dI;
  int i;
  int loop_ub;
  int nx;
  emxArray_real_T *id;
  emxArray_int32_T *r19;
  emxArray_real_T *r20;
  emxArray_boolean_T *x;
  emxArray_int32_T *ii;
  emxArray_real_T *b_I_;
  int idx;
  boolean_T exitg1;
  dI = d_sum(I_);
  i = I_->size[0];
  emxEnsureCapacity_real_T(I_, i);
  loop_ub = I_->size[0];
  for (i = 0; i < loop_ub; i++) {
    I_->data[i] = I_->data[i] / dI * min_;
  }

  nx = I_->size[0];
  for (loop_ub = 0; loop_ub < nx; loop_ub++) {
    I_->data[loop_ub] = rt_roundd_snf(I_->data[loop_ub]);
  }

  loop_ub = I_->size[0];
  for (i = 0; i < loop_ub; i++) {
    if (I_->data[i] < 1.0) {
      I_->data[i] = 1.0;
    }
  }

  dI = d_sum(I_) - min_;
  emxInit_real_T(&id, 2);
  emxInit_int32_T(&r19, 2);
  emxInit_real_T(&r20, 1);
  emxInit_boolean_T(&x, 1);
  emxInit_int32_T(&ii, 1);
  emxInit_real_T(&b_I_, 2);
  while (dI != 0.0) {
    if (dI < 0.0) {
      randperm(I_->size[0], fabs(dI), id);
      i = r19->size[0] * r19->size[1];
      r19->size[0] = 1;
      r19->size[1] = id->size[1];
      emxEnsureCapacity_int32_T(r19, i);
      loop_ub = id->size[0] * id->size[1];
      for (i = 0; i < loop_ub; i++) {
        r19->data[i] = (int)id->data[i];
      }

      i = b_I_->size[0] * b_I_->size[1];
      b_I_->size[0] = 1;
      b_I_->size[1] = r19->size[1];
      emxEnsureCapacity_real_T(b_I_, i);
      loop_ub = r19->size[1];
      for (i = 0; i < loop_ub; i++) {
        b_I_->data[i] = I_->data[(int)id->data[i] - 1] + 1.0;
      }

      loop_ub = b_I_->size[1];
      for (i = 0; i < loop_ub; i++) {
        I_->data[r19->data[i] - 1] = b_I_->data[i];
      }
    } else {
      if (dI > 0.0) {
        i = x->size[0];
        x->size[0] = I_->size[0];
        emxEnsureCapacity_boolean_T(x, i);
        loop_ub = I_->size[0];
        for (i = 0; i < loop_ub; i++) {
          x->data[i] = (I_->data[i] > 1.0);
        }

        nx = x->size[0];
        idx = 0;
        i = ii->size[0];
        ii->size[0] = x->size[0];
        emxEnsureCapacity_int32_T(ii, i);
        loop_ub = 0;
        exitg1 = false;
        while ((!exitg1) && (loop_ub <= nx - 1)) {
          if (x->data[loop_ub]) {
            idx++;
            ii->data[idx - 1] = loop_ub + 1;
            if (idx >= nx) {
              exitg1 = true;
            } else {
              loop_ub++;
            }
          } else {
            loop_ub++;
          }
        }

        if (x->size[0] == 1) {
          if (idx == 0) {
            ii->size[0] = 0;
          }
        } else if (1 > idx) {
          ii->size[0] = 0;
        } else {
          i = ii->size[0];
          ii->size[0] = idx;
          emxEnsureCapacity_int32_T(ii, i);
        }

        randperm(ii->size[0], fmin(dI, ii->size[0]), id);
        i = r19->size[0] * r19->size[1];
        r19->size[0] = 1;
        r19->size[1] = id->size[1];
        emxEnsureCapacity_int32_T(r19, i);
        loop_ub = id->size[0] * id->size[1];
        for (i = 0; i < loop_ub; i++) {
          r19->data[i] = ii->data[(int)id->data[i] - 1];
        }

        i = r20->size[0];
        r20->size[0] = id->size[1];
        emxEnsureCapacity_real_T(r20, i);
        loop_ub = id->size[1];
        for (i = 0; i < loop_ub; i++) {
          r20->data[i] = I_->data[ii->data[(int)id->data[i] - 1] - 1];
        }

        loop_ub = r19->size[1];
        for (i = 0; i < loop_ub; i++) {
          I_->data[r19->data[i] - 1] = r20->data[i] - 1.0;
        }
      }
    }

    dI = d_sum(I_) - min_;
  }

  emxFree_real_T(&b_I_);
  emxFree_int32_T(&ii);
  emxFree_boolean_T(&x);
  emxFree_real_T(&r20);
  emxFree_int32_T(&r19);
  emxFree_real_T(&id);
}
