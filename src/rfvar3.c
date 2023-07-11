#include <string.h>
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "rfvar3.h"
#include "diag.h"
#include "sum.h"
#include "svd.h"
#include "simulate_abm_emxutil.h"

void b_rfvar3(const double ydata_data[], const int ydata_size[1], const double
              xdata_data[], const int xdata_size[2], double *var_By, double
              var_Bx_data[], int var_Bx_size[2], double var_u_data[], int
              var_u_size[1], double var_xxi_data[], int var_xxi_size[2], double *
              var_snglty)
{
  int y_size_idx_1;
  int m;
  int inner;
  double smpl_data[80];
  double y_data[80];
  int n;
  double X_data[80];
  int i8;
  int tmp_data[80];
  double varargin_2_data[320];
  cell_wrap_1 reshapes[2];
  signed char sizes_idx_1;
  double result_data[405];
  double b_X_data[400];
  int loop_ub_tmp;
  int result_size[2];
  double b_result_data[400];
  double vl_data[400];
  int vl_size[2];
  double d_data[25];
  int d_size[2];
  double vr_data[340];
  int vr_size[2];
  double di_data[5];
  int di_size[1];
  int b_di_size[1];
  double dfx;
  boolean_T b_di_data[5];
  int loop_ub;
  int b_loop_ub;
  static double a_data[6400];
  int k;
  int aoffset;
  double b_tmp_data[5];
  int i;
  int b_tmp_size_idx_0;
  int b_tmp_size_idx_1;
  double b_b_tmp_data[340];
  int j;
  int coffset;
  int boffset;
  int var_xxi_data_tmp;
  static double b_var_xxi_data[4624];
  emxArray_real_T *y;
  if (ydata_size[0] < 2) {
    y_size_idx_1 = 0;
  } else {
    y_size_idx_1 = ydata_size[0] - 1;
    m = ydata_size[0] - 2;
    for (inner = 0; inner <= m; inner++) {
      y_data[inner] = 2.0 + (double)inner;
    }
  }

  if (0 <= y_size_idx_1 - 1) {
    memcpy(&smpl_data[0], &y_data[0], (unsigned int)(y_size_idx_1 * (int)sizeof
            (double)));
    memset(&X_data[0], 0, (unsigned int)(y_size_idx_1 * (int)sizeof(double)));
  }

  for (n = 0; n < y_size_idx_1; n++) {
    X_data[n] = ydata_data[(int)(smpl_data[n] - 1.0) - 1];
  }

  m = xdata_size[1];
  for (inner = 0; inner < m; inner++) {
    for (i8 = 0; i8 < y_size_idx_1; i8++) {
      varargin_2_data[i8 + y_size_idx_1 * inner] = xdata_data[((int)smpl_data[i8]
        + xdata_size[0] * inner) - 1];
    }
  }

  for (inner = 0; inner < y_size_idx_1; inner++) {
    tmp_data[inner] = inner;
  }

  for (inner = 0; inner < y_size_idx_1; inner++) {
    reshapes[0].f1.data[inner] = X_data[tmp_data[inner]];
  }

  if ((y_size_idx_1 == 0) || (xdata_size[1] != 0)) {
    sizes_idx_1 = (signed char)xdata_size[1];
  } else {
    sizes_idx_1 = 0;
  }

  for (inner = 0; inner < 1; inner++) {
    for (i8 = 0; i8 < y_size_idx_1; i8++) {
      result_data[i8] = reshapes[0].f1.data[i8];
    }
  }

  m = sizes_idx_1;
  for (inner = 0; inner < m; inner++) {
    for (i8 = 0; i8 < y_size_idx_1; i8++) {
      result_data[i8 + y_size_idx_1 * (inner + 1)] = varargin_2_data[i8 +
        y_size_idx_1 * inner];
    }
  }

  m = y_size_idx_1 - 1;
  if (0 <= m) {
    memcpy(&y_data[0], &X_data[0], (unsigned int)((m + 1) * (int)sizeof(double)));
  }

  if (0 <= y_size_idx_1 - 1) {
    memcpy(&X_data[0], &y_data[0], (unsigned int)(y_size_idx_1 * (int)sizeof
            (double)));
  }

  for (inner = 0; inner < 1; inner++) {
    for (i8 = 0; i8 < y_size_idx_1; i8++) {
      b_X_data[i8] = X_data[i8];
    }
  }

  m = sizes_idx_1;
  for (inner = 0; inner < m; inner++) {
    for (i8 = 0; i8 < y_size_idx_1; i8++) {
      b_X_data[i8 + y_size_idx_1 * (inner + 1)] = varargin_2_data[i8 +
        y_size_idx_1 * inner];
    }
  }

  var_u_size[0] = y_size_idx_1;
  for (inner = 0; inner < y_size_idx_1; inner++) {
    var_u_data[inner] = ydata_data[(int)smpl_data[inner] - 1];
  }

  loop_ub_tmp = 1 + sizes_idx_1;
  result_size[0] = y_size_idx_1;
  result_size[1] = loop_ub_tmp;
  for (inner = 0; inner < loop_ub_tmp; inner++) {
    for (i8 = 0; i8 < y_size_idx_1; i8++) {
      b_result_data[i8 + y_size_idx_1 * inner] = result_data[i8 + y_size_idx_1 *
        inner];
    }
  }

  c_svd(b_result_data, result_size, vl_data, vl_size, d_data, d_size, vr_data,
        vr_size);
  c_diag(d_data, d_size, di_data, di_size);
  b_di_size[0] = di_size[0];
  m = di_size[0];
  for (inner = 0; inner < m; inner++) {
    b_di_data[inner] = (di_data[inner] > 2.2204460492503131E-14);
  }

  dfx = c_sum(b_di_data, b_di_size);
  *var_snglty = (1.0 + (double)sizes_idx_1) - dfx;
  if (1.0 > dfx) {
    m = 0;
    loop_ub = 0;
    b_loop_ub = -1;
  } else {
    m = (int)dfx;
    loop_ub = (int)dfx;
    b_loop_ub = (int)dfx - 1;
  }

  n = vl_size[0];
  for (inner = 0; inner < n; inner++) {
    for (i8 = 0; i8 < loop_ub; i8++) {
      a_data[i8 + loop_ub * inner] = vl_data[inner + vl_size[0] * i8];
    }
  }

  if ((vl_size[0] == 1) || (y_size_idx_1 == 1)) {
    for (inner = 0; inner < loop_ub; inner++) {
      X_data[inner] = 0.0;
      for (i8 = 0; i8 < n; i8++) {
        X_data[inner] += a_data[inner + loop_ub * i8] * var_u_data[i8];
      }
    }
  } else {
    inner = vl_size[0];
    if (0 <= loop_ub - 1) {
      memset(&X_data[0], 0, (unsigned int)(loop_ub * (int)sizeof(double)));
    }

    for (k = 0; k < inner; k++) {
      aoffset = k * loop_ub;
      for (i = 0; i < loop_ub; i++) {
        X_data[i] += ydata_data[(int)smpl_data[k] - 1] * a_data[aoffset + i];
      }
    }
  }

  for (inner = 0; inner < m; inner++) {
    b_tmp_data[inner] = 1.0 / di_data[inner];
  }

  di_size[0] = m;
  if (0 <= m - 1) {
    memcpy(&di_data[0], &b_tmp_data[0], (unsigned int)(m * (int)sizeof(double)));
  }

  b_diag(di_data, di_size, d_data, d_size);
  b_tmp_size_idx_0 = d_size[0];
  b_tmp_size_idx_1 = d_size[1];
  m = d_size[0] * d_size[1];
  if (0 <= m - 1) {
    memcpy(&b_b_tmp_data[0], &d_data[0], (unsigned int)(m * (int)sizeof(double)));
  }

  if ((b_loop_ub + 1 == 1) || (d_size[0] == 1)) {
    m = vr_size[0];
    var_xxi_size[0] = vr_size[0];
    var_xxi_size[1] = d_size[1];
    for (inner = 0; inner < m; inner++) {
      for (i8 = 0; i8 < b_tmp_size_idx_1; i8++) {
        var_xxi_data[inner + m * i8] = 0.0;
        for (n = 0; n <= b_loop_ub; n++) {
          var_xxi_data_tmp = inner + var_xxi_size[0] * i8;
          var_xxi_data[var_xxi_data_tmp] += vr_data[inner + vr_size[0] * n] *
            b_b_tmp_data[n + b_tmp_size_idx_0 * i8];
        }
      }
    }
  } else {
    inner = vr_size[0];
    n = d_size[1];
    var_xxi_size[0] = vr_size[0];
    var_xxi_size[1] = d_size[1];
    for (j = 0; j < n; j++) {
      coffset = j * inner;
      boffset = j * (b_loop_ub + 1);
      if (0 <= inner - 1) {
        memset(&var_xxi_data[coffset], 0, (unsigned int)(inner * (int)sizeof
                (double)));
      }

      for (k = 0; k <= b_loop_ub; k++) {
        aoffset = k * inner;
        dfx = b_b_tmp_data[boffset + k];
        for (i = 0; i < inner; i++) {
          i8 = aoffset + i;
          var_xxi_data_tmp = coffset + i;
          var_xxi_data[var_xxi_data_tmp] += dfx * vr_data[i8 % vr_size[0] +
            vr_size[0] * (i8 / vr_size[0])];
        }
      }
    }
  }

  if ((var_xxi_size[1] == 1) || (loop_ub == 1)) {
    n = var_xxi_size[0];
    m = var_xxi_size[0];
    for (inner = 0; inner < m; inner++) {
      smpl_data[inner] = 0.0;
      loop_ub = var_xxi_size[1];
      for (i8 = 0; i8 < loop_ub; i8++) {
        smpl_data[inner] += var_xxi_data[inner + var_xxi_size[0] * i8] *
          X_data[i8];
      }
    }
  } else {
    m = var_xxi_size[0];
    inner = var_xxi_size[1];
    n = var_xxi_size[0];
    if (0 <= var_xxi_size[0] - 1) {
      memset(&smpl_data[0], 0, (unsigned int)(var_xxi_size[0] * (int)sizeof
              (double)));
    }

    for (k = 0; k < inner; k++) {
      aoffset = k * m;
      for (i = 0; i < m; i++) {
        smpl_data[i] += X_data[k] * var_xxi_data[aoffset + i];
      }
    }
  }

  for (inner = 0; inner < loop_ub_tmp; inner++) {
    for (i8 = 0; i8 < y_size_idx_1; i8++) {
      a_data[i8 + y_size_idx_1 * inner] = result_data[i8 + y_size_idx_1 * inner];
    }
  }

  if ((loop_ub_tmp == 1) || (n == 1)) {
    for (inner = 0; inner < y_size_idx_1; inner++) {
      X_data[inner] = 0.0;
      for (i8 = 0; i8 < loop_ub_tmp; i8++) {
        X_data[inner] += b_X_data[inner + y_size_idx_1 * i8] * smpl_data[i8];
      }
    }
  } else {
    if (0 <= y_size_idx_1 - 1) {
      memset(&X_data[0], 0, (unsigned int)(y_size_idx_1 * (int)sizeof(double)));
    }

    for (k = 0; k < loop_ub_tmp; k++) {
      aoffset = k * y_size_idx_1;
      for (i = 0; i < y_size_idx_1; i++) {
        X_data[i] += smpl_data[k] * a_data[aoffset + i];
      }
    }
  }

  for (inner = 0; inner < y_size_idx_1; inner++) {
    var_u_data[inner] -= X_data[inner];
  }

  if ((b_loop_ub + 1 == 1) || (d_size[0] == 1)) {
    m = vr_size[0];
    var_xxi_size[0] = vr_size[0];
    var_xxi_size[1] = d_size[1];
    for (inner = 0; inner < m; inner++) {
      for (i8 = 0; i8 < b_tmp_size_idx_1; i8++) {
        var_xxi_data[inner + m * i8] = 0.0;
        for (n = 0; n <= b_loop_ub; n++) {
          var_xxi_data_tmp = inner + var_xxi_size[0] * i8;
          var_xxi_data[var_xxi_data_tmp] += vr_data[inner + vr_size[0] * n] *
            b_b_tmp_data[n + b_tmp_size_idx_0 * i8];
        }
      }
    }
  } else {
    inner = vr_size[0];
    n = d_size[1];
    var_xxi_size[0] = vr_size[0];
    var_xxi_size[1] = d_size[1];
    for (j = 0; j < n; j++) {
      coffset = j * inner;
      boffset = j * (b_loop_ub + 1);
      if (0 <= inner - 1) {
        memset(&var_xxi_data[coffset], 0, (unsigned int)(inner * (int)sizeof
                (double)));
      }

      for (k = 0; k <= b_loop_ub; k++) {
        aoffset = k * inner;
        dfx = b_b_tmp_data[boffset + k];
        for (i = 0; i < inner; i++) {
          i8 = aoffset + i;
          var_xxi_data_tmp = coffset + i;
          var_xxi_data[var_xxi_data_tmp] += dfx * vr_data[i8 % vr_size[0] +
            vr_size[0] * (i8 / vr_size[0])];
        }
      }
    }
  }

  loop_ub_tmp = var_xxi_size[0] * var_xxi_size[1];
  if (0 <= loop_ub_tmp - 1) {
    memcpy(&vr_data[0], &var_xxi_data[0], (unsigned int)(loop_ub_tmp * (int)
            sizeof(double)));
  }

  m = var_xxi_size[0];
  for (inner = 0; inner < m; inner++) {
    loop_ub = var_xxi_size[1];
    for (i8 = 0; i8 < loop_ub; i8++) {
      b_var_xxi_data[i8 + var_xxi_size[1] * inner] = var_xxi_data[inner +
        var_xxi_size[0] * i8];
    }
  }

  b_tmp_size_idx_1 = var_xxi_size[0];
  if (0 <= loop_ub_tmp - 1) {
    memcpy(&b_b_tmp_data[0], &b_var_xxi_data[0], (unsigned int)(loop_ub_tmp *
            (int)sizeof(double)));
  }

  if (var_xxi_size[1] == 1) {
    m = var_xxi_size[0];
    for (inner = 0; inner < m; inner++) {
      for (i8 = 0; i8 < b_tmp_size_idx_1; i8++) {
        var_xxi_data_tmp = inner + var_xxi_size[0] * i8;
        b_var_xxi_data[var_xxi_data_tmp] = 0.0;
        for (n = 0; n < 1; n++) {
          b_var_xxi_data[var_xxi_data_tmp] += var_xxi_data[inner] *
            b_b_tmp_data[i8];
        }
      }
    }

    var_xxi_size[1] = var_xxi_size[0];
    m = var_xxi_size[0] * var_xxi_size[0];
    if (0 <= m - 1) {
      memcpy(&var_xxi_data[0], &b_var_xxi_data[0], (unsigned int)(m * (int)
              sizeof(double)));
    }
  } else {
    m = var_xxi_size[0];
    inner = var_xxi_size[1];
    n = var_xxi_size[0];
    var_xxi_size[1] = var_xxi_size[0];
    for (j = 0; j < n; j++) {
      coffset = j * m;
      boffset = j * inner;
      if (0 <= m - 1) {
        memset(&var_xxi_data[coffset], 0, (unsigned int)(m * (int)sizeof(double)));
      }

      for (k = 0; k < inner; k++) {
        aoffset = k * m;
        dfx = b_b_tmp_data[boffset + k];
        for (i = 0; i < m; i++) {
          var_xxi_data_tmp = coffset + i;
          var_xxi_data[var_xxi_data_tmp] += dfx * vr_data[aoffset + i];
        }
      }
    }
  }

  if (xdata_size[1] == 0) {
    var_Bx_size[0] = 0;
    var_Bx_size[1] = 0;
  } else {
    emxInit_real_T(&y, 2);
    inner = y->size[0] * y->size[1];
    y->size[0] = 1;
    m = (int)((double)xdata_size[1] - 1.0);
    y->size[1] = m + 1;
    emxEnsureCapacity_real_T(y, inner);
    for (inner = 0; inner <= m; inner++) {
      y->data[inner] = 1.0 + (double)inner;
    }

    var_Bx_size[0] = 1;
    var_Bx_size[1] = y->size[1];
    m = y->size[1];
    for (inner = 0; inner < m; inner++) {
      var_Bx_data[inner] = smpl_data[(int)(1.0 + y->data[inner]) - 1];
    }

    emxFree_real_T(&y);
  }

  *var_By = smpl_data[0];
}

void rfvar3(const double ydata_data[], const int ydata_size[1], const double
            xdata_data[], const int xdata_size[1], double *var_By, double
            var_Bx_data[], int var_Bx_size[2], double var_u_data[], int
            var_u_size[1], double var_xxi[4], double *var_snglty)
{
  int y_size_idx_1;
  int coffset;
  int aoffset;
  double smpl_data[80];
  double y_data[80];
  int inner;
  double X_data[80];
  double b_X_data[160];
  int X_size[2];
  double c_X_data[160];
  double vl_data[400];
  int vl_size[2];
  double d_data[10];
  int d_size[2];
  double di_data[2];
  int di_size[1];
  int b_di_size[1];
  double dfx;
  boolean_T b_di_data[2];
  int loop_ub;
  int b_loop_ub;
  int i;
  double a_data[6400];
  int k;
  int tmp_size[1];
  double tmp_data[5];
  double b_tmp_data[25];
  int b_tmp_size_idx_0;
  double b_b_tmp_data[4];
  int xxi_size_idx_1;
  int boffset;
  double xxi_data[4];
  double B_idx_1;
  double b_data[4];
  if (ydata_size[0] < 2) {
    y_size_idx_1 = 0;
  } else {
    y_size_idx_1 = ydata_size[0] - 1;
    coffset = ydata_size[0] - 2;
    for (aoffset = 0; aoffset <= coffset; aoffset++) {
      y_data[aoffset] = 2.0 + (double)aoffset;
    }
  }

  if (0 <= y_size_idx_1 - 1) {
    memcpy(&smpl_data[0], &y_data[0], (unsigned int)(y_size_idx_1 * (int)sizeof
            (double)));
    memset(&X_data[0], 0, (unsigned int)(y_size_idx_1 * (int)sizeof(double)));
  }

  for (inner = 0; inner < y_size_idx_1; inner++) {
    X_data[inner] = ydata_data[(int)(smpl_data[inner] - 1.0) - 1];
  }

  if (0 <= y_size_idx_1 - 1) {
    memcpy(&b_X_data[0], &X_data[0], (unsigned int)(y_size_idx_1 * (int)sizeof
            (double)));
  }

  for (aoffset = 0; aoffset < y_size_idx_1; aoffset++) {
    b_X_data[aoffset + y_size_idx_1] = xdata_data[(int)smpl_data[aoffset] - 1];
  }

  var_u_size[0] = y_size_idx_1;
  for (aoffset = 0; aoffset < y_size_idx_1; aoffset++) {
    var_u_data[aoffset] = ydata_data[(int)smpl_data[aoffset] - 1];
  }

  X_size[0] = y_size_idx_1;
  X_size[1] = 2;
  for (aoffset = 0; aoffset < y_size_idx_1; aoffset++) {
    c_X_data[aoffset] = b_X_data[aoffset];
  }

  for (aoffset = 0; aoffset < y_size_idx_1; aoffset++) {
    inner = aoffset + y_size_idx_1;
    c_X_data[inner] = b_X_data[inner];
  }

  svd(c_X_data, X_size, vl_data, vl_size, d_data, d_size, var_xxi);
  diag(d_data, d_size, di_data, di_size);
  b_di_size[0] = di_size[0];
  coffset = di_size[0];
  for (aoffset = 0; aoffset < coffset; aoffset++) {
    b_di_data[aoffset] = (di_data[aoffset] > 2.2204460492503131E-14);
  }

  dfx = c_sum(b_di_data, b_di_size);
  *var_snglty = 2.0 - dfx;
  if (1.0 > dfx) {
    coffset = 0;
    loop_ub = 0;
    b_loop_ub = -1;
  } else {
    coffset = (int)dfx;
    loop_ub = (int)dfx;
    b_loop_ub = (int)dfx - 1;
  }

  inner = vl_size[0];
  for (aoffset = 0; aoffset < inner; aoffset++) {
    for (i = 0; i < loop_ub; i++) {
      a_data[i + loop_ub * aoffset] = vl_data[aoffset + vl_size[0] * i];
    }
  }

  if ((vl_size[0] == 1) || (y_size_idx_1 == 1)) {
    for (aoffset = 0; aoffset < loop_ub; aoffset++) {
      X_data[aoffset] = 0.0;
      for (i = 0; i < inner; i++) {
        X_data[aoffset] += a_data[aoffset + loop_ub * i] * var_u_data[i];
      }
    }
  } else {
    inner = vl_size[0];
    if (0 <= loop_ub - 1) {
      memset(&X_data[0], 0, (unsigned int)(loop_ub * (int)sizeof(double)));
    }

    for (k = 0; k < inner; k++) {
      aoffset = k * loop_ub;
      for (i = 0; i < loop_ub; i++) {
        X_data[i] += ydata_data[(int)smpl_data[k] - 1] * a_data[aoffset + i];
      }
    }
  }

  tmp_size[0] = coffset;
  for (aoffset = 0; aoffset < coffset; aoffset++) {
    tmp_data[aoffset] = 1.0 / di_data[aoffset];
  }

  b_diag(tmp_data, tmp_size, b_tmp_data, X_size);
  b_tmp_size_idx_0 = X_size[0];
  coffset = X_size[0] * X_size[1];
  if (0 <= coffset - 1) {
    memcpy(&b_b_tmp_data[0], &b_tmp_data[0], (unsigned int)(coffset * (int)
            sizeof(double)));
  }

  if ((b_loop_ub + 1 == 1) || (X_size[0] == 1)) {
    xxi_size_idx_1 = X_size[1];
    coffset = X_size[1];
    for (aoffset = 0; aoffset < coffset; aoffset++) {
      inner = aoffset << 1;
      xxi_data[inner] = 0.0;
      for (i = 0; i <= b_loop_ub; i++) {
        xxi_data[inner] += var_xxi[i << 1] * b_b_tmp_data[i + b_tmp_size_idx_0 *
          aoffset];
      }
    }

    for (aoffset = 0; aoffset < coffset; aoffset++) {
      inner = 1 + (aoffset << 1);
      xxi_data[inner] = 0.0;
      for (i = 0; i <= b_loop_ub; i++) {
        xxi_data[inner] += var_xxi[1 + (i << 1)] * b_b_tmp_data[i +
          b_tmp_size_idx_0 * aoffset];
      }
    }
  } else {
    inner = X_size[1];
    xxi_size_idx_1 = X_size[1];
    for (i = 0; i < inner; i++) {
      coffset = i << 1;
      boffset = i * (b_loop_ub + 1);
      xxi_data[coffset] = 0.0;
      xxi_data[coffset + 1] = 0.0;
      for (k = 0; k <= b_loop_ub; k++) {
        aoffset = k << 1;
        dfx = b_b_tmp_data[boffset + k];
        xxi_data[coffset] += dfx * var_xxi[aoffset % 2 + ((aoffset / 2) << 1)];
        aoffset++;
        xxi_data[coffset + 1] += dfx * var_xxi[aoffset % 2 + ((aoffset / 2) << 1)];
      }
    }
  }

  if ((xxi_size_idx_1 == 1) || (loop_ub == 1)) {
    *var_By = 0.0;
    for (aoffset = 0; aoffset < xxi_size_idx_1; aoffset++) {
      *var_By += xxi_data[aoffset << 1] * X_data[aoffset];
    }

    B_idx_1 = 0.0;
    for (aoffset = 0; aoffset < xxi_size_idx_1; aoffset++) {
      B_idx_1 += xxi_data[1 + (aoffset << 1)] * X_data[aoffset];
    }
  } else {
    *var_By = 0.0;
    B_idx_1 = 0.0;
    for (k = 0; k < xxi_size_idx_1; k++) {
      aoffset = k << 1;
      *var_By += X_data[k] * xxi_data[aoffset];
      B_idx_1 += X_data[k] * xxi_data[aoffset + 1];
    }
  }

  if (0 <= y_size_idx_1 - 1) {
    memset(&X_data[0], 0, (unsigned int)(y_size_idx_1 * (int)sizeof(double)));
  }

  for (i = 0; i < y_size_idx_1; i++) {
    X_data[i] += *var_By * b_X_data[i % y_size_idx_1 + y_size_idx_1 * (i /
      y_size_idx_1)];
  }

  for (i = 0; i < y_size_idx_1; i++) {
    aoffset = y_size_idx_1 + i;
    X_data[i] += B_idx_1 * b_X_data[aoffset % y_size_idx_1 + y_size_idx_1 *
      (aoffset / y_size_idx_1)];
  }

  for (aoffset = 0; aoffset < y_size_idx_1; aoffset++) {
    var_u_data[aoffset] -= X_data[aoffset];
  }

  if ((b_loop_ub + 1 == 1) || (X_size[0] == 1)) {
    xxi_size_idx_1 = X_size[1];
    coffset = X_size[1];
    for (aoffset = 0; aoffset < coffset; aoffset++) {
      inner = aoffset << 1;
      xxi_data[inner] = 0.0;
      for (i = 0; i <= b_loop_ub; i++) {
        xxi_data[inner] += var_xxi[i << 1] * b_b_tmp_data[i + b_tmp_size_idx_0 *
          aoffset];
      }
    }

    for (aoffset = 0; aoffset < coffset; aoffset++) {
      inner = 1 + (aoffset << 1);
      xxi_data[inner] = 0.0;
      for (i = 0; i <= b_loop_ub; i++) {
        xxi_data[inner] += var_xxi[1 + (i << 1)] * b_b_tmp_data[i +
          b_tmp_size_idx_0 * aoffset];
      }
    }
  } else {
    inner = X_size[1];
    xxi_size_idx_1 = X_size[1];
    for (i = 0; i < inner; i++) {
      coffset = i << 1;
      boffset = i * (b_loop_ub + 1);
      xxi_data[coffset] = 0.0;
      xxi_data[coffset + 1] = 0.0;
      for (k = 0; k <= b_loop_ub; k++) {
        aoffset = k << 1;
        dfx = b_b_tmp_data[boffset + k];
        xxi_data[coffset] += dfx * var_xxi[aoffset % 2 + ((aoffset / 2) << 1)];
        aoffset++;
        xxi_data[coffset + 1] += dfx * var_xxi[aoffset % 2 + ((aoffset / 2) << 1)];
      }
    }
  }

  for (aoffset = 0; aoffset < xxi_size_idx_1; aoffset++) {
    b_data[aoffset] = xxi_data[aoffset << 1];
  }

  for (aoffset = 0; aoffset < xxi_size_idx_1; aoffset++) {
    b_data[aoffset + xxi_size_idx_1] = xxi_data[1 + (aoffset << 1)];
  }

  if (xxi_size_idx_1 == 1) {
    var_xxi[0] = 0.0;
    for (aoffset = 0; aoffset < 1; aoffset++) {
      var_xxi[0] += xxi_data[0] * b_data[0];
    }

    var_xxi[2] = 0.0;
    for (aoffset = 0; aoffset < 1; aoffset++) {
      var_xxi[2] += xxi_data[0] * b_data[xxi_size_idx_1];
    }

    var_xxi[1] = 0.0;
    for (aoffset = 0; aoffset < 1; aoffset++) {
      var_xxi[1] += xxi_data[1] * b_data[0];
    }

    var_xxi[3] = 0.0;
    for (aoffset = 0; aoffset < 1; aoffset++) {
      var_xxi[3] += xxi_data[1] * b_data[xxi_size_idx_1];
    }
  } else {
    var_xxi[0] = 0.0;
    var_xxi[1] = 0.0;
    for (k = 0; k < xxi_size_idx_1; k++) {
      inner = k << 1;
      var_xxi[0] += b_data[k] * xxi_data[inner];
      var_xxi[1] += b_data[k] * xxi_data[inner + 1];
    }

    var_xxi[2] = 0.0;
    var_xxi[3] = 0.0;
    for (k = 0; k < xxi_size_idx_1; k++) {
      aoffset = k << 1;
      dfx = b_data[xxi_size_idx_1 + k];
      var_xxi[2] += dfx * xxi_data[aoffset];
      var_xxi[3] += dfx * xxi_data[aoffset + 1];
    }
  }

  if (xdata_size[0] == 0) {
    var_Bx_size[0] = 0;
    var_Bx_size[1] = 0;
  } else {
    var_Bx_size[0] = 1;
    var_Bx_size[1] = 1;
    var_Bx_data[0] = B_idx_1;
  }
}
