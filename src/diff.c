#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "diff.h"

void b_diff(const double x[13], double y[12])
{
  int ixLead;
  int iyLead;
  double work;
  int m;
  double tmp2;
  ixLead = 1;
  iyLead = 0;
  work = x[0];
  for (m = 0; m < 12; m++) {
    tmp2 = work;
    work = x[ixLead];
    y[iyLead] = x[ixLead] - tmp2;
    ixLead++;
    iyLead++;
  }
}

void diff(const double x_data[], const int x_size[1], double y_data[], int
          y_size[1])
{
  int dimSize;
  int ixLead;
  int iyLead;
  double work_data_idx_0;
  int m;
  double tmp1;
  dimSize = x_size[0];
  y_size[0] = (signed char)(x_size[0] - 1);
  ixLead = 1;
  iyLead = 0;
  work_data_idx_0 = x_data[0];
  for (m = 2; m <= dimSize; m++) {
    tmp1 = work_data_idx_0;
    work_data_idx_0 = x_data[ixLead];
    tmp1 = x_data[ixLead] - tmp1;
    ixLead++;
    y_data[iyLead] = tmp1;
    iyLead++;
  }
}
