import numpy as np


class VectorizedFirms:
    """Vectorized firm operations using NumPy"""
    
    def __init__(self, n_firms, n_sectors):
        self.n = n_firms
        self.capital = np.ones(n_firms) * 100.0
        self.labor = np.zeros(n_firms)
        self.output = np.zeros(n_firms)
        self.price = np.ones(n_firms)
        self.sectors = np.arange(n_firms) % n_sectors
        
    def produce(self, capital_prod=0.5, labor_prod=1.0):
        """Vectorized production for all firms at once"""
        self.output = np.minimum(
            self.capital * capital_prod,
            self.labor * labor_prod
        )
        return self.output
    
    def set_prices(self, markup=1.15):
        """Vectorized pricing for all firms"""
        unit_costs = self.labor / np.maximum(self.output, 1.0)
        self.price = markup * unit_costs
        return self.price
    
    def aggregate_by_sector(self, n_sectors):
        """Aggregate firm-level to sector-level"""
        sector_output = np.zeros(n_sectors)
        sector_prices = np.zeros(n_sectors)
        
        for s in range(n_sectors):
            mask = self.sectors == s
            sector_output[s] = np.sum(self.output[mask])
            sector_prices[s] = np.mean(self.price[mask]) if np.any(mask) else 1.0
        
        return sector_output, sector_prices


class VectorizedHouseholds:
    """Vectorized household operations"""
    
    def __init__(self, n_households):
        self.n = n_households
        self.wealth = np.ones(n_households) * 100.0
        self.income = np.zeros(n_households)
        self.consumption = np.zeros(n_households)
        self.employed = np.zeros(n_households, dtype=bool)
    
    def consume(self, mpc=0.75, wealth_effect=0.05):
        """Vectorized consumption for all households"""
        self.consumption = mpc * (self.income + wealth_effect * self.wealth)
        return self.consumption
    
    def update_wealth(self):
        """Vectorized wealth update"""
        self.wealth += self.income - self.consumption
        self.income[:] = 0
