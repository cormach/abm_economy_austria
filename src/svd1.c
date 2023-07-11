#include <math.h>
#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "svd1.h"
#include "xrot.h"
#include "xrotg.h"
#include "sqrt.h"
#include "xswap.h"
#include "xaxpy.h"
#include "xdotc.h"
#include "xnrm2.h"

void b_svd(const double A_data[], const int A_size[2], double U_data[], int
           U_size[2], double s_data[], int s_size[1], double V[4])
{
  int m;
  int nctp1;
  double b_A_data[160];
  int n;
  int minnp;
  double b_s_data[2];
  double e[2];
  int nct;
  int q;
  int qp1;
  int qq;
  int nmq_tmp;
  boolean_T apply_transform;
  double nrm;
  int iter;
  int qjj;
  double rt;
  double snorm;
  boolean_T exitg1;
  double f;
  double scale;
  double sm;
  double b;
  double sqds;
  m = A_size[0];
  nctp1 = A_size[0] * A_size[1];
  if (0 <= nctp1 - 1) {
    memcpy(&b_A_data[0], &A_data[0], (unsigned int)(nctp1 * (int)sizeof(double)));
  }

  n = A_size[0];
  minnp = A_size[0];
  if (minnp >= 2) {
    minnp = 2;
  }

  nctp1 = A_size[0] + 1;
  if (nctp1 >= 2) {
    nctp1 = 2;
  }

  nctp1 = (signed char)nctp1;
  if (0 <= nctp1 - 1) {
    memset(&b_s_data[0], 0, (unsigned int)(nctp1 * (int)sizeof(double)));
  }

  e[1] = 0.0;
  U_size[0] = (signed char)A_size[0];
  U_size[1] = (signed char)minnp;
  nctp1 = (signed char)A_size[0] * (signed char)minnp;
  if (0 <= nctp1 - 1) {
    memset(&U_data[0], 0, (unsigned int)(nctp1 * (int)sizeof(double)));
  }

  V[1] = 0.0;
  V[2] = 0.0;
  if (A_size[0] == 0) {
    V[0] = 1.0;
    V[3] = 1.0;
  } else {
    if (A_size[0] > 1) {
      nct = A_size[0] - 1;
    } else {
      nct = 0;
    }

    if (nct >= 2) {
      nct = 2;
    }

    nctp1 = nct + 1;
    for (q = 0; q < nct; q++) {
      qp1 = q + 2;
      qq = (q + n * q) + 1;
      nmq_tmp = n - q;
      apply_transform = false;
      if (q + 1 <= nct) {
        nrm = xnrm2(nmq_tmp, b_A_data, qq);
        if (nrm > 0.0) {
          apply_transform = true;
          if (b_A_data[qq - 1] < 0.0) {
            nrm = -nrm;
          }

          b_s_data[q] = nrm;
          if (fabs(nrm) >= 1.0020841800044864E-292) {
            nrm = 1.0 / nrm;
            iter = (qq + nmq_tmp) - 1;
            for (qjj = qq; qjj <= iter; qjj++) {
              b_A_data[qjj - 1] *= nrm;
            }
          } else {
            iter = (qq + nmq_tmp) - 1;
            for (qjj = qq; qjj <= iter; qjj++) {
              b_A_data[qjj - 1] /= b_s_data[q];
            }
          }

          b_A_data[qq - 1]++;
          b_s_data[q] = -b_s_data[q];
        } else {
          b_s_data[q] = 0.0;
        }
      }

      for (iter = qp1; iter < 3; iter++) {
        qjj = q + n;
        if (apply_transform) {
          xaxpy(nmq_tmp, -(xdotc(nmq_tmp, b_A_data, qq, b_A_data, qjj + 1) /
                           b_A_data[q + m * q]), qq, b_A_data, qjj + 1);
        }

        e[1] = b_A_data[qjj];
      }

      if (q + 1 <= nct) {
        for (nmq_tmp = q + 1; nmq_tmp <= n; nmq_tmp++) {
          U_data[(nmq_tmp + U_size[0] * q) - 1] = b_A_data[(nmq_tmp + m * q) - 1];
        }
      }
    }

    if (2 < A_size[0] + 1) {
      m = 1;
    } else {
      m = A_size[0];
    }

    if (nct < 2) {
      b_s_data[nct] = b_A_data[nct + A_size[0] * nct];
    }

    if (A_size[0] < m + 1) {
      b_s_data[m] = 0.0;
    }

    e[0] = b_A_data[A_size[0]];
    e[m] = 0.0;
    if (nct + 1 <= minnp) {
      for (iter = nctp1; iter <= minnp; iter++) {
        for (nmq_tmp = 0; nmq_tmp < n; nmq_tmp++) {
          U_data[nmq_tmp + U_size[0] * (iter - 1)] = 0.0;
        }

        U_data[(iter + U_size[0] * (iter - 1)) - 1] = 1.0;
      }
    }

    for (q = nct; q >= 1; q--) {
      qp1 = q + 1;
      nmq_tmp = n - q;
      nctp1 = nmq_tmp + 1;
      qq = (q + n * (q - 1)) - 1;
      if (b_s_data[q - 1] != 0.0) {
        for (iter = qp1; iter <= minnp; iter++) {
          qjj = q + n;
          xaxpy(nmq_tmp + 1, -(xdotc(nctp1, U_data, qq + 1, U_data, qjj) /
                               U_data[qq]), qq + 1, U_data, qjj);
        }

        for (nmq_tmp = q; nmq_tmp <= n; nmq_tmp++) {
          nctp1 = (nmq_tmp + U_size[0] * (q - 1)) - 1;
          U_data[nctp1] = -U_data[nctp1];
        }

        U_data[qq]++;
        if (0 <= q - 2) {
          U_data[U_size[0] * (q - 1)] = 0.0;
        }
      } else {
        for (nmq_tmp = 0; nmq_tmp < n; nmq_tmp++) {
          U_data[nmq_tmp + U_size[0] * (q - 1)] = 0.0;
        }

        U_data[qq] = 1.0;
      }
    }

    V[2] = 0.0;
    V[3] = 1.0;
    V[1] = 0.0;
    V[0] = 1.0;
    for (q = 0; q <= m; q++) {
      if (b_s_data[q] != 0.0) {
        rt = fabs(b_s_data[q]);
        nrm = b_s_data[q] / rt;
        b_s_data[q] = rt;
        if (q + 1 < m + 1) {
          e[0] /= nrm;
        }

        if (q + 1 <= n) {
          nctp1 = n * q;
          iter = nctp1 + n;
          for (qjj = nctp1 + 1; qjj <= iter; qjj++) {
            U_data[qjj - 1] *= nrm;
          }
        }
      }

      if ((q + 1 < m + 1) && (e[0] != 0.0)) {
        rt = fabs(e[0]);
        nrm = rt / e[0];
        e[0] = rt;
        b_s_data[1] *= nrm;
        V[2] *= nrm;
        V[3] *= nrm;
      }
    }

    qp1 = m;
    iter = 0;
    snorm = 0.0;
    for (nmq_tmp = 0; nmq_tmp <= m; nmq_tmp++) {
      snorm = fmax(snorm, fmax(fabs(b_s_data[nmq_tmp]), fabs(e[nmq_tmp])));
    }

    while ((m + 1 > 0) && (iter < 75)) {
      nmq_tmp = m;
      exitg1 = false;
      while (!(exitg1 || (nmq_tmp == 0))) {
        nrm = fabs(e[0]);
        if ((nrm <= 2.2204460492503131E-16 * (fabs(b_s_data[0]) + fabs(b_s_data
               [1]))) || (nrm <= 1.0020841800044864E-292) || ((iter > 20) &&
             (nrm <= 2.2204460492503131E-16 * snorm))) {
          e[0] = 0.0;
          exitg1 = true;
        } else {
          nmq_tmp--;
        }
      }

      if (nmq_tmp == m) {
        nctp1 = 4;
      } else {
        qjj = m + 1;
        nctp1 = m + 1;
        exitg1 = false;
        while ((!exitg1) && (nctp1 >= nmq_tmp)) {
          qjj = nctp1;
          if (nctp1 == nmq_tmp) {
            exitg1 = true;
          } else {
            nrm = 0.0;
            if (nctp1 < m + 1) {
              nrm = fabs(e[0]);
            }

            if (nctp1 > nmq_tmp + 1) {
              nrm += fabs(e[0]);
            }

            rt = fabs(b_s_data[nctp1 - 1]);
            if ((rt <= 2.2204460492503131E-16 * nrm) || (rt <=
                 1.0020841800044864E-292)) {
              b_s_data[nctp1 - 1] = 0.0;
              exitg1 = true;
            } else {
              nctp1--;
            }
          }
        }

        if (qjj == nmq_tmp) {
          nctp1 = 3;
        } else if (qjj == m + 1) {
          nctp1 = 1;
        } else {
          nctp1 = 2;
          nmq_tmp = qjj;
        }
      }

      switch (nctp1) {
       case 1:
        f = e[0];
        e[0] = 0.0;
        for (qjj = m; qjj >= nmq_tmp + 1; qjj--) {
          xrotg(&b_s_data[0], &f, &b, &sm);
          xrot(V, 1 + (m << 1), b, sm);
        }
        break;

       case 2:
        f = e[nmq_tmp - 1];
        e[nmq_tmp - 1] = 0.0;
        for (qjj = nmq_tmp + 1; qjj <= m + 1; qjj++) {
          xrotg(&b_s_data[qjj - 1], &f, &b, &sm);
          nrm = e[qjj - 1];
          f = -sm * nrm;
          e[qjj - 1] = nrm * b;
          b_xrot(n, U_data, 1 + n * (qjj - 1), 1 + n * (nmq_tmp - 1), b, sm);
        }
        break;

       case 3:
        scale = fmax(fmax(fmax(fmax(fabs(b_s_data[m]), fabs(b_s_data[0])), fabs
          (e[0])), fabs(b_s_data[nmq_tmp])), fabs(e[nmq_tmp]));
        sm = b_s_data[m] / scale;
        nrm = b_s_data[0] / scale;
        rt = e[0] / scale;
        sqds = b_s_data[nmq_tmp] / scale;
        b = ((nrm + sm) * (nrm - sm) + rt * rt) / 2.0;
        nrm = sm * rt;
        nrm *= nrm;
        if ((b != 0.0) || (nrm != 0.0)) {
          rt = b * b + nrm;
          b_sqrt(&rt);
          if (b < 0.0) {
            rt = -rt;
          }

          rt = nrm / (b + rt);
        } else {
          rt = 0.0;
        }

        f = (sqds + sm) * (sqds - sm) + rt;
        nrm = sqds * (e[nmq_tmp] / scale);
        for (qjj = nmq_tmp + 1; qjj < 2; qjj++) {
          xrotg(&f, &nrm, &b, &sm);
          f = b * b_s_data[0] + sm * e[0];
          nrm = b * e[0] - sm * b_s_data[0];
          e[0] = nrm;
          rt = sm * b_s_data[1];
          b_s_data[1] *= b;
          xrot(V, 3, b, sm);
          b_s_data[0] = f;
          xrotg(&b_s_data[0], &rt, &b, &sm);
          f = b * nrm + sm * b_s_data[1];
          b_s_data[1] = -sm * nrm + b * b_s_data[1];
          nrm = sm * e[1];
          e[1] *= b;
          if (1 < n) {
            b_xrot(n, U_data, 1, 1 + n, b, sm);
          }
        }

        e[0] = f;
        iter++;
        break;

       default:
        if (b_s_data[nmq_tmp] < 0.0) {
          b_s_data[nmq_tmp] = -b_s_data[nmq_tmp];
          nctp1 = nmq_tmp << 1;
          iter = nctp1 + 2;
          for (qjj = nctp1 + 1; qjj <= iter; qjj++) {
            V[qjj - 1] = -V[qjj - 1];
          }
        }

        while ((nmq_tmp + 1 < qp1 + 1) && (b_s_data[0] < b_s_data[1])) {
          rt = b_s_data[0];
          b_s_data[0] = b_s_data[1];
          b_s_data[1] = rt;
          xswap(V);
          if (1 < n) {
            b_xswap(n, U_data, 1, 1 + n);
          }

          nmq_tmp = 1;
        }

        iter = 0;
        m--;
        break;
      }
    }
  }

  s_size[0] = minnp;
  if (0 <= minnp - 1) {
    memcpy(&s_data[0], &b_s_data[0], (unsigned int)(minnp * (int)sizeof(double)));
  }
}

void d_svd(const double A_data[], const int A_size[2], double U_data[], int
           U_size[2], double s_data[], int s_size[1], double V_data[], int
           V_size[2])
{
  int A_size_idx_0;
  int nctp1;
  double b_A_data[400];
  int n;
  int p;
  int nmq;
  int minnp;
  int qp1q;
  double b_s_data[5];
  double e_data[5];
  double work_data[80];
  int nrt;
  int ii;
  int nct;
  int i10;
  int q;
  int qp1;
  int m;
  int qq;
  boolean_T apply_transform;
  double nrm;
  int k;
  int qjj;
  double snorm;
  double r;
  boolean_T exitg1;
  double scale;
  double sm;
  double sqds;
  double b;
  A_size_idx_0 = A_size[0];
  nctp1 = A_size[0] * A_size[1];
  if (0 <= nctp1 - 1) {
    memcpy(&b_A_data[0], &A_data[0], (unsigned int)(nctp1 * (int)sizeof(double)));
  }

  n = A_size[0];
  p = A_size[1];
  nmq = A_size[0];
  minnp = A_size[1];
  if (nmq < minnp) {
    minnp = nmq;
  }

  nmq = A_size[0] + 1;
  qp1q = A_size[1];
  if (nmq < qp1q) {
    qp1q = nmq;
  }

  nctp1 = (signed char)qp1q;
  if (0 <= nctp1 - 1) {
    memset(&b_s_data[0], 0, (unsigned int)(nctp1 * (int)sizeof(double)));
  }

  nctp1 = (signed char)A_size[1];
  if (0 <= nctp1 - 1) {
    memset(&e_data[0], 0, (unsigned int)(nctp1 * (int)sizeof(double)));
  }

  nctp1 = (signed char)A_size[0];
  if (0 <= nctp1 - 1) {
    memset(&work_data[0], 0, (unsigned int)(nctp1 * (int)sizeof(double)));
  }

  U_size[0] = (signed char)A_size[0];
  U_size[1] = (signed char)minnp;
  nctp1 = (signed char)A_size[0] * (signed char)minnp;
  if (0 <= nctp1 - 1) {
    memset(&U_data[0], 0, (unsigned int)(nctp1 * (int)sizeof(double)));
  }

  V_size[0] = (signed char)A_size[1];
  V_size[1] = (signed char)A_size[1];
  nctp1 = (signed char)A_size[1] * (signed char)A_size[1];
  if (0 <= nctp1 - 1) {
    memset(&V_data[0], 0, (unsigned int)(nctp1 * (int)sizeof(double)));
  }

  if (A_size[1] == 0) {
    nmq = A_size[0];
    if (nmq >= minnp) {
      nmq = minnp;
    }

    for (ii = 0; ii < nmq; ii++) {
      U_data[ii + U_size[0] * ii] = 1.0;
    }
  } else {
    if (A_size[1] > 2) {
      nmq = A_size[1] - 2;
    } else {
      nmq = 0;
    }

    nrt = A_size[0];
    if (nmq < nrt) {
      nrt = nmq;
    }

    nmq = A_size[0] - 1;
    nct = A_size[1];
    if (nmq < nct) {
      nct = nmq;
    }

    nctp1 = nct + 1;
    if (nct > nrt) {
      i10 = nct;
    } else {
      i10 = nrt;
    }

    for (q = 0; q < i10; q++) {
      qp1 = q + 2;
      qq = (q + n * q) + 1;
      nmq = n - q;
      apply_transform = false;
      if (q + 1 <= nct) {
        nrm = b_xnrm2(nmq, b_A_data, qq);
        if (nrm > 0.0) {
          apply_transform = true;
          if (b_A_data[qq - 1] < 0.0) {
            nrm = -nrm;
          }

          b_s_data[q] = nrm;
          if (fabs(nrm) >= 1.0020841800044864E-292) {
            nrm = 1.0 / nrm;
            qp1q = (qq + nmq) - 1;
            for (k = qq; k <= qp1q; k++) {
              b_A_data[k - 1] *= nrm;
            }
          } else {
            qp1q = (qq + nmq) - 1;
            for (k = qq; k <= qp1q; k++) {
              b_A_data[k - 1] /= b_s_data[q];
            }
          }

          b_A_data[qq - 1]++;
          b_s_data[q] = -b_s_data[q];
        } else {
          b_s_data[q] = 0.0;
        }
      }

      for (k = qp1; k <= p; k++) {
        qjj = q + n * (k - 1);
        if (apply_transform) {
          xaxpy(nmq, -(xdotc(nmq, b_A_data, qq, b_A_data, qjj + 1) / b_A_data[q
                       + A_size_idx_0 * q]), qq, b_A_data, qjj + 1);
        }

        e_data[k - 1] = b_A_data[qjj];
      }

      if (q + 1 <= nct) {
        for (ii = q + 1; ii <= n; ii++) {
          U_data[(ii + U_size[0] * q) - 1] = b_A_data[(ii + A_size_idx_0 * q) -
            1];
        }
      }

      if (q + 1 <= nrt) {
        qjj = p - q;
        nrm = c_xnrm2(qjj - 1, e_data, q + 2);
        if (nrm == 0.0) {
          e_data[q] = 0.0;
        } else {
          if (e_data[q + 1] < 0.0) {
            e_data[q] = -nrm;
          } else {
            e_data[q] = nrm;
          }

          nrm = e_data[q];
          if (fabs(e_data[q]) >= 1.0020841800044864E-292) {
            nrm = 1.0 / e_data[q];
            qp1q = q + qjj;
            for (k = qp1; k <= qp1q; k++) {
              e_data[k - 1] *= nrm;
            }
          } else {
            qp1q = q + qjj;
            for (k = qp1; k <= qp1q; k++) {
              e_data[k - 1] /= nrm;
            }
          }

          e_data[q + 1]++;
          e_data[q] = -e_data[q];
          if (q + 2 <= n) {
            if (qp1 <= n) {
              memset(&work_data[qp1 + -1], 0, (unsigned int)(((n - qp1) + 1) *
                      (int)sizeof(double)));
            }

            for (k = qp1; k <= p; k++) {
              b_xaxpy(nmq - 1, e_data[k - 1], b_A_data, (q + n * (k - 1)) + 2,
                      work_data, q + 2);
            }

            for (k = qp1; k <= p; k++) {
              b_xaxpy(nmq - 1, -e_data[k - 1] / e_data[q + 1], work_data, q + 2,
                      b_A_data, (q + n * (k - 1)) + 2);
            }
          }
        }

        for (ii = qp1; ii <= p; ii++) {
          V_data[(ii + V_size[0] * q) - 1] = e_data[ii - 1];
        }
      }
    }

    if (A_size[1] < A_size[0] + 1) {
      m = A_size[1] - 1;
    } else {
      m = A_size[0];
    }

    if (nct < A_size[1]) {
      b_s_data[nct] = b_A_data[nct + A_size[0] * nct];
    }

    if (A_size[0] < m + 1) {
      b_s_data[m] = 0.0;
    }

    if (nrt + 1 < m + 1) {
      e_data[nrt] = b_A_data[nrt + A_size[0] * m];
    }

    e_data[m] = 0.0;
    if (nct + 1 <= minnp) {
      for (k = nctp1; k <= minnp; k++) {
        for (ii = 0; ii < n; ii++) {
          U_data[ii + U_size[0] * (k - 1)] = 0.0;
        }

        U_data[(k + U_size[0] * (k - 1)) - 1] = 1.0;
      }
    }

    for (q = nct; q >= 1; q--) {
      qp1 = q + 1;
      qp1q = n - q;
      nmq = qp1q + 1;
      qq = (q + n * (q - 1)) - 1;
      if (b_s_data[q - 1] != 0.0) {
        for (k = qp1; k <= minnp; k++) {
          qjj = q + n * (k - 1);
          xaxpy(qp1q + 1, -(xdotc(nmq, U_data, qq + 1, U_data, qjj) / U_data[qq]),
                qq + 1, U_data, qjj);
        }

        for (ii = q; ii <= n; ii++) {
          nmq = (ii + U_size[0] * (q - 1)) - 1;
          U_data[nmq] = -U_data[nmq];
        }

        U_data[qq]++;
        for (ii = 0; ii <= q - 2; ii++) {
          U_data[ii + U_size[0] * (q - 1)] = 0.0;
        }
      } else {
        for (ii = 0; ii < n; ii++) {
          U_data[ii + U_size[0] * (q - 1)] = 0.0;
        }

        U_data[qq] = 1.0;
      }
    }

    for (q = p; q >= 1; q--) {
      if ((q <= nrt) && (e_data[q - 1] != 0.0)) {
        qp1 = q + 1;
        qjj = p - q;
        qp1q = (q + p * (q - 1)) + 1;
        for (k = qp1; k <= p; k++) {
          nmq = (q + p * (k - 1)) + 1;
          A_size_idx_0 = V_size[0];
          nct = V_size[1];
          nctp1 = V_size[0] * V_size[1];
          if (0 <= nctp1 - 1) {
            memcpy(&b_A_data[0], &V_data[0], (unsigned int)(nctp1 * (int)sizeof
                    (double)));
          }

          xaxpy(qjj, -(xdotc(qjj, V_data, qp1q, V_data, nmq) / V_data[qp1q - 1]),
                qp1q, b_A_data, nmq);
          V_size[0] = A_size_idx_0;
          V_size[1] = nct;
          nctp1 = A_size_idx_0 * nct;
          if (0 <= nctp1 - 1) {
            memcpy(&V_data[0], &b_A_data[0], (unsigned int)(nctp1 * (int)sizeof
                    (double)));
          }
        }
      }

      for (ii = 0; ii < p; ii++) {
        V_data[ii + V_size[0] * (q - 1)] = 0.0;
      }

      V_data[(q + V_size[0] * (q - 1)) - 1] = 1.0;
    }

    qq = m;
    qjj = 0;
    snorm = 0.0;
    for (q = 0; q <= m; q++) {
      if (b_s_data[q] != 0.0) {
        nrm = fabs(b_s_data[q]);
        r = b_s_data[q] / nrm;
        b_s_data[q] = nrm;
        if (q + 1 < m + 1) {
          e_data[q] /= r;
        }

        if (q + 1 <= n) {
          nmq = 1 + n * q;
          i10 = (nmq + n) - 1;
          for (k = nmq; k <= i10; k++) {
            U_data[k - 1] *= r;
          }
        }
      }

      if ((q + 1 < m + 1) && (e_data[q] != 0.0)) {
        nrm = fabs(e_data[q]);
        r = nrm / e_data[q];
        e_data[q] = nrm;
        b_s_data[q + 1] *= r;
        nmq = 1 + p * (q + 1);
        A_size_idx_0 = V_size[0];
        nct = V_size[1];
        nctp1 = V_size[0] * V_size[1];
        if (0 <= nctp1 - 1) {
          memcpy(&b_A_data[0], &V_data[0], (unsigned int)(nctp1 * (int)sizeof
                  (double)));
        }

        i10 = (nmq + p) - 1;
        for (k = nmq; k <= i10; k++) {
          b_A_data[k - 1] *= r;
        }

        V_size[0] = A_size_idx_0;
        V_size[1] = nct;
        nctp1 = A_size_idx_0 * nct;
        if (0 <= nctp1 - 1) {
          memcpy(&V_data[0], &b_A_data[0], (unsigned int)(nctp1 * (int)sizeof
                  (double)));
        }
      }

      snorm = fmax(snorm, fmax(fabs(b_s_data[q]), fabs(e_data[q])));
    }

    while ((m + 1 > 0) && (qjj < 75)) {
      ii = m;
      exitg1 = false;
      while (!(exitg1 || (ii == 0))) {
        nrm = fabs(e_data[ii - 1]);
        if ((nrm <= 2.2204460492503131E-16 * (fabs(b_s_data[ii - 1]) + fabs
              (b_s_data[ii]))) || (nrm <= 1.0020841800044864E-292) || ((qjj > 20)
             && (nrm <= 2.2204460492503131E-16 * snorm))) {
          e_data[ii - 1] = 0.0;
          exitg1 = true;
        } else {
          ii--;
        }
      }

      if (ii == m) {
        qp1q = 4;
      } else {
        nmq = m + 1;
        qp1q = m + 1;
        exitg1 = false;
        while ((!exitg1) && (qp1q >= ii)) {
          nmq = qp1q;
          if (qp1q == ii) {
            exitg1 = true;
          } else {
            nrm = 0.0;
            if (qp1q < m + 1) {
              nrm = fabs(e_data[qp1q - 1]);
            }

            if (qp1q > ii + 1) {
              nrm += fabs(e_data[qp1q - 2]);
            }

            r = fabs(b_s_data[qp1q - 1]);
            if ((r <= 2.2204460492503131E-16 * nrm) || (r <=
                 1.0020841800044864E-292)) {
              b_s_data[qp1q - 1] = 0.0;
              exitg1 = true;
            } else {
              qp1q--;
            }
          }
        }

        if (nmq == ii) {
          qp1q = 3;
        } else if (nmq == m + 1) {
          qp1q = 1;
        } else {
          qp1q = 2;
          ii = nmq;
        }
      }

      switch (qp1q) {
       case 1:
        r = e_data[m - 1];
        e_data[m - 1] = 0.0;
        for (k = m; k >= ii + 1; k--) {
          xrotg(&b_s_data[k - 1], &r, &sm, &scale);
          if (k > ii + 1) {
            b = e_data[k - 2];
            r = -scale * b;
            e_data[k - 2] = b * sm;
          }

          A_size_idx_0 = V_size[0];
          nct = V_size[1];
          qp1q = V_size[0] * V_size[1];
          if (0 <= qp1q - 1) {
            memcpy(&b_A_data[0], &V_data[0], (unsigned int)(qp1q * (int)sizeof
                    (double)));
          }

          b_xrot(p, b_A_data, 1 + p * (k - 1), 1 + p * m, sm, scale);
          V_size[0] = A_size_idx_0;
          V_size[1] = nct;
          nctp1 = A_size_idx_0 * nct;
          if (0 <= nctp1 - 1) {
            memcpy(&V_data[0], &b_A_data[0], (unsigned int)(nctp1 * (int)sizeof
                    (double)));
          }
        }
        break;

       case 2:
        r = e_data[ii - 1];
        e_data[ii - 1] = 0.0;
        for (k = ii + 1; k <= m + 1; k++) {
          xrotg(&b_s_data[k - 1], &r, &sm, &scale);
          r = -scale * e_data[k - 1];
          e_data[k - 1] *= sm;
          b_xrot(n, U_data, 1 + n * (k - 1), 1 + n * (ii - 1), sm, scale);
        }
        break;

       case 3:
        scale = fmax(fmax(fmax(fmax(fabs(b_s_data[m]), fabs(b_s_data[m - 1])),
          fabs(e_data[m - 1])), fabs(b_s_data[ii])), fabs(e_data[ii]));
        sm = b_s_data[m] / scale;
        nrm = b_s_data[m - 1] / scale;
        r = e_data[m - 1] / scale;
        sqds = b_s_data[ii] / scale;
        b = ((nrm + sm) * (nrm - sm) + r * r) / 2.0;
        nrm = sm * r;
        nrm *= nrm;
        if ((b != 0.0) || (nrm != 0.0)) {
          r = b * b + nrm;
          b_sqrt(&r);
          if (b < 0.0) {
            r = -r;
          }

          r = nrm / (b + r);
        } else {
          r = 0.0;
        }

        r += (sqds + sm) * (sqds - sm);
        nrm = sqds * (e_data[ii] / scale);
        for (k = ii + 1; k <= m; k++) {
          xrotg(&r, &nrm, &sm, &scale);
          if (k > ii + 1) {
            e_data[k - 2] = r;
          }

          b = e_data[k - 1];
          nrm = b_s_data[k - 1];
          e_data[k - 1] = sm * b - scale * nrm;
          r = scale * b_s_data[k];
          b_s_data[k] *= sm;
          A_size_idx_0 = V_size[0];
          nct = V_size[1];
          qp1q = V_size[0] * V_size[1];
          if (0 <= qp1q - 1) {
            memcpy(&b_A_data[0], &V_data[0], (unsigned int)(qp1q * (int)sizeof
                    (double)));
          }

          b_xrot(p, b_A_data, 1 + p * (k - 1), 1 + p * k, sm, scale);
          V_size[0] = A_size_idx_0;
          V_size[1] = nct;
          nctp1 = A_size_idx_0 * nct;
          if (0 <= nctp1 - 1) {
            memcpy(&V_data[0], &b_A_data[0], (unsigned int)(nctp1 * (int)sizeof
                    (double)));
          }

          b_s_data[k - 1] = sm * nrm + scale * b;
          xrotg(&b_s_data[k - 1], &r, &sm, &scale);
          b = e_data[k - 1];
          r = sm * b + scale * b_s_data[k];
          b_s_data[k] = -scale * b + sm * b_s_data[k];
          nrm = scale * e_data[k];
          e_data[k] *= sm;
          if (k < n) {
            b_xrot(n, U_data, 1 + n * (k - 1), 1 + n * k, sm, scale);
          }
        }

        e_data[m - 1] = r;
        qjj++;
        break;

       default:
        if (b_s_data[ii] < 0.0) {
          b_s_data[ii] = -b_s_data[ii];
          nmq = 1 + p * ii;
          nctp1 = V_size[0] * V_size[1];
          if (0 <= nctp1 - 1) {
            memcpy(&b_A_data[0], &V_data[0], (unsigned int)(nctp1 * (int)sizeof
                    (double)));
          }

          i10 = (nmq + p) - 1;
          for (k = nmq; k <= i10; k++) {
            b_A_data[k - 1] = -b_A_data[k - 1];
          }

          qp1q = V_size[0] * V_size[1];
          if (0 <= qp1q - 1) {
            memcpy(&V_data[0], &b_A_data[0], (unsigned int)(qp1q * (int)sizeof
                    (double)));
          }
        }

        qp1 = ii + 1;
        while ((ii + 1 < qq + 1) && (b_s_data[ii] < b_s_data[qp1])) {
          nrm = b_s_data[ii];
          b_s_data[ii] = b_s_data[qp1];
          b_s_data[qp1] = nrm;
          if (ii + 1 < p) {
            A_size_idx_0 = V_size[0];
            nct = V_size[1];
            nctp1 = V_size[0] * V_size[1];
            if (0 <= nctp1 - 1) {
              memcpy(&b_A_data[0], &V_data[0], (unsigned int)(nctp1 * (int)
                      sizeof(double)));
            }

            b_xswap(p, b_A_data, 1 + p * ii, 1 + p * (ii + 1));
            V_size[0] = A_size_idx_0;
            V_size[1] = nct;
            nctp1 = A_size_idx_0 * nct;
            if (0 <= nctp1 - 1) {
              memcpy(&V_data[0], &b_A_data[0], (unsigned int)(nctp1 * (int)
                      sizeof(double)));
            }
          }

          if (ii + 1 < n) {
            b_xswap(n, U_data, 1 + n * ii, 1 + n * (ii + 1));
          }

          ii = qp1;
          qp1++;
        }

        qjj = 0;
        m--;
        break;
      }
    }
  }

  s_size[0] = minnp;
  if (0 <= minnp - 1) {
    memcpy(&s_data[0], &b_s_data[0], (unsigned int)(minnp * (int)sizeof(double)));
  }
}
