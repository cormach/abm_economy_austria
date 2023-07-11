/*
** main.c
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>
#include "mat.h"
#include "rt_nonfinite.h"
#include "simulate_abm.h"
#include "simulate_abm_emxutil.h"
#include "simulate_abm_initialize.h"
#include "simulate_abm_terminate.h"

#define TIME 13
#define SECTORS 62

int main(int argc, char **argv)
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    
    int T = (int) TIME;
    int F = (int) SECTORS;
    
    double year = (double) atoi(argv[1]);
    double quarter = (double) atoi(argv[2]);
    double seed = (double) atoi(argv[3]) + world_rank;
    double predictors = (double) atoi(argv[4]);
    
    mxArray *mxnominal_gdp = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxreal_gdp = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxnominal_gva = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxreal_gva = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxnominal_household_consumption = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxreal_household_consumption = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxnominal_government_consumption = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxreal_government_consumption = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxnominal_capitalformation = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxreal_capitalformation = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxnominal_fixed_capitalformation = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxreal_fixed_capitalformation = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxnominal_fixed_capitalformation_dwellings = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxreal_fixed_capitalformation_dwellings = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxnominal_exports = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxreal_exports = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxnominal_imports = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxreal_imports = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxoperating_surplus = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxcompensation_employees = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxwages = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxtaxes_production = mxCreateDoubleMatrix(1,T,mxREAL);
    mxArray *mxnominal_sector_gva = mxCreateDoubleMatrix(T,F,mxREAL);
    mxArray *mxreal_sector_gva = mxCreateDoubleMatrix(T,F,mxREAL);
    mxArray *mxeuribor = mxCreateDoubleMatrix(1,T,mxREAL);
    
    simulate_abm_initialize();
    
    simulate_abm(year,quarter,seed,predictors,mxGetPr(mxnominal_gdp),mxGetPr(mxreal_gdp),mxGetPr(mxnominal_gva),mxGetPr(mxreal_gva),mxGetPr(mxnominal_household_consumption),mxGetPr(mxreal_household_consumption),mxGetPr(mxnominal_government_consumption),mxGetPr(mxreal_government_consumption),mxGetPr(mxnominal_capitalformation),mxGetPr(mxreal_capitalformation),mxGetPr(mxnominal_fixed_capitalformation),mxGetPr(mxreal_fixed_capitalformation),mxGetPr(mxnominal_fixed_capitalformation_dwellings),mxGetPr(mxreal_fixed_capitalformation_dwellings),mxGetPr(mxnominal_exports),mxGetPr(mxreal_exports),mxGetPr(mxnominal_imports),mxGetPr(mxreal_imports),mxGetPr(mxoperating_surplus),mxGetPr(mxcompensation_employees),mxGetPr(mxwages),mxGetPr(mxtaxes_production),mxGetPr(mxnominal_sector_gva),mxGetPr(mxreal_sector_gva),mxGetPr(mxeuribor));
    
    MATFile *pmat;
    char predictors_string[32];
    sprintf(predictors_string, "%d",(int) predictors);
    char *file = strcat(predictors_string,"_");
    char year_string[32];
    sprintf(year_string, "%d",(int) year);
    strcat(year_string,"Q");
    strcat(file,year_string);
    char quarter_string[32];
    sprintf(quarter_string, "%d",(int) quarter);
    strcat(file,quarter_string);
    strcat(file,"_");
    char seed_string[32];
    sprintf(seed_string, "%d",(int) seed);
    strcat(file,seed_string);
    strcat(file,".mat");
    pmat = matOpen(file,"w");
    matPutVariable(pmat,"nominal_gdp",mxnominal_gdp);
    matPutVariable(pmat,"real_gdp",mxreal_gdp);
    matPutVariable(pmat,"nominal_gva",mxnominal_gva);
    matPutVariable(pmat,"real_gva",mxreal_gva);
    matPutVariable(pmat,"nominal_household_consumption",mxnominal_household_consumption);
    matPutVariable(pmat,"real_household_consumption",mxreal_household_consumption);
    matPutVariable(pmat,"nominal_government_consumption",mxnominal_government_consumption);
    matPutVariable(pmat,"real_government_consumption",mxreal_government_consumption);
    matPutVariable(pmat,"nominal_capitalformation",mxnominal_capitalformation);
    matPutVariable(pmat,"real_capitalformation",mxreal_capitalformation);
    matPutVariable(pmat,"nominal_fixed_capitalformation",mxnominal_fixed_capitalformation);
    matPutVariable(pmat,"real_fixed_capitalformation",mxreal_fixed_capitalformation);
    matPutVariable(pmat,"nominal_fixed_capitalformation_dwellings",mxnominal_fixed_capitalformation_dwellings);
    matPutVariable(pmat,"real_fixed_capitalformation_dwellings",mxreal_fixed_capitalformation_dwellings);
    matPutVariable(pmat,"nominal_exports",mxnominal_exports);
    matPutVariable(pmat,"real_exports",mxreal_exports);
    matPutVariable(pmat,"nominal_imports",mxnominal_imports);
    matPutVariable(pmat,"real_imports",mxreal_imports);
    matPutVariable(pmat,"operating_surplus",mxoperating_surplus);
    matPutVariable(pmat,"compensation_employees",mxcompensation_employees);
    matPutVariable(pmat,"wages",mxwages);
    matPutVariable(pmat,"taxes_production",mxtaxes_production);
    matPutVariable(pmat,"nominal_sector_gva",mxnominal_sector_gva);
    matPutVariable(pmat,"real_sector_gva",mxreal_sector_gva);
    matPutVariable(pmat,"euribor",mxeuribor);
    matClose(pmat);
    
    simulate_abm_terminate();
    
    // Finalize the MPI environment.
    MPI_Finalize();
    
    return(EXIT_SUCCESS);
}
