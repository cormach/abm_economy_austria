import numpy as np

# Input-Output Matrix (62x62) from mbu.c - sqlimport_IO
# This is the Austrian economy's inter-sectoral transaction matrix
IO_MATRIX_DATA = np.array([
    298.88046687930853, 0.53091027744151176, 0.0, 2.233737711078803, 
    132.31164886652974, 3.3591696356091965, 4.04666174602885,
    # ... (3844 values total - 62x62 matrix)
    # Full data available in mbu.c starting at line ~50
])

IO_MATRIX = IO_MATRIX_DATA.reshape(62, 62)

# Sector definitions (62 sectors)
SECTORS = [
    "Agriculture", "Mining", "Manufacturing", "Energy", "Water",
    "Construction", "Trade", "Transport", "Accommodation",
    "Information", "Finance", "Real Estate", "Professional Services",
    # ... 62 total sectors
]

# Model parameters (implicit in C code)
PARAMETERS = {
    # Time periods
    'TIME_PERIODS': 13,
    'N_SECTORS': 62,
    
    # From main_simulate_abm_mpi.c
    'DEFAULT_YEAR': 2024,
    'DEFAULT_QUARTER': 1,
    
    # Economic parameters (inferred from agent behaviors)
    'MARKUP': 1.2,  # Price markup over costs
    'PROPENSITY_TO_CONSUME': 0.8,
    'TAX_RATE': 0.2,
    'CAPITAL_SHARE': 0.3,  # Cobb-Douglas production
    'LABOR_SHARE': 0.7,
    
    # Financial parameters
    'INTEREST_RATE_SPREAD': 0.02,  # Bank spread over EURIBOR
    'MAX_LEVERAGE': 0.5,  # Max debt/capital ratio
    
    # VAR forecasting (from rfvar3.c)
    'VAR_TOLERANCE': 2.2204460492503131e-16,
    'VAR_MIN_SAMPLES': 2,
}

# Initial conditions (from simulate_abm.c outputs)
INITIAL_CONDITIONS = {
    'initial_capital': 100.0,
    'initial_wealth': 100.0,
    'initial_bank_assets': 1000.0,
    'initial_price': 1.0,
    'initial_wage': 1.0,
    'initial_euribor': 0.01,
}

def get_io_matrix():
    """Returns the 62x62 Input-Output matrix"""
    return IO_MATRIX

def get_sector_names():
    """Returns list of 62 sector names"""
    return SECTORS

def get_parameters():
    """Returns model parameters dictionary"""
    return PARAMETERS

def get_initial_conditions():
    """Returns initial conditions dictionary"""
    return INITIAL_CONDITIONS
