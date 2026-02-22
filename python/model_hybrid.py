import numpy as np
import pandas as pd
from agents_vectorized import VectorizedFirms, VectorizedHouseholds


class HybridEconomyModel:
    """Hybrid model with vectorized operations"""
    
    def __init__(self, year, quarter, seed, predictors, 
                 markup=1.15, mpc=0.75, spread=0.02):
        np.random.seed(seed)
        
        self.year = year
        self.quarter = quarter
        self.n_sectors = 62
        
        # Parameters
        self.markup = markup
        self.mpc = mpc
        self.spread = spread
        
        # Vectorized agents
        self.firms = VectorizedFirms(n_firms=620, n_sectors=62)
        self.households = VectorizedHouseholds(n_households=1000)
        
        # State
        self.euribor = 0.01
        self.prices = np.ones(self.n_sectors)
        
    def step(self):
        """Single time step - vectorized"""
        
        # Firms produce (vectorized)
        self.firms.produce()
        
        # Firms set prices (vectorized)
        self.firms.set_prices(markup=self.markup)
        
        # Households consume (vectorized)
        self.households.consume(mpc=self.mpc)
        
        # Update wealth (vectorized)
        self.households.update_wealth()
        
        # Aggregate
        sector_output, sector_prices = self.firms.aggregate_by_sector(self.n_sectors)
        self.prices = sector_prices
        
        # Compute aggregates
        gdp = np.sum(self.households.consumption)
        gva = np.sum(self.firms.output * self.firms.price)
        unemployment = 1.0 - np.mean(self.households.employed)
        
        return {
            'nominal_gdp': gdp,
            'nominal_gva': gva,
            'unemployment': unemployment,
            'mean_price': np.mean(self.prices)
        }
    
    def run(self, steps=13):
        """Run simulation"""
        results = []
        for t in range(steps):
            result = self.step()
            result['period'] = t
            results.append(result)
        
        return pd.DataFrame(results)
