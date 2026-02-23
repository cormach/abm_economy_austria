import numpy as np
import pandas as pd
from mesa import Model
from mesa.time import RandomActivation
from mesa.datacollection import DataCollector
from agents import Firm, Household, Bank, Government
from forecasting import VARForecaster


class AustrianEconomyModel(Model):
    """
    Agent-based model of Austrian economy
    Based on Poledna et al. (2022) European Economic Review
    """
    
    def __init__(self, year, quarter, seed, predictors):
        super().__init__()
        self.random.seed(seed)
        np.random.seed(seed)
        
        self.year = year
        self.quarter = quarter
        self.predictors = predictors
        self.schedule = RandomActivation(self)
        
        # Time
        self.time_periods = 13
        self.current_period = 0
        self.n_sectors = 62
        
        # Load calibration data
        self.io_matrix = self._load_io_matrix()
        self.params = self._load_parameters()
        
        # Economic state variables
        self.euribor = 0.01
        self.prices = np.ones(self.n_sectors)
        self.wages = np.ones(self.n_sectors)
        self.unemployment_rate = 0.05
        self.inflation_rate = 0.02
        
        # Create agents (scaled down for testing)
        self._create_firms(n_per_sector=10)
        self._create_households(n=1000)
        self._create_banks(n=5)
        self._create_government()
        
        # Forecasting
        self.forecaster = VARForecaster()
        
        # Data collection
        self.datacollector = DataCollector(
            model_reporters={
                "nominal_gdp": self.compute_gdp,
                "real_gdp": self.compute_real_gdp,
                "nominal_gva": self.compute_gva,
                "real_gva": self.compute_real_gva,
                "unemployment": self.compute_unemployment,
                "inflation": self.compute_inflation,
                "euribor": lambda m: m.euribor,
                "government_debt": lambda m: m.government.debt,
                "total_consumption": self.compute_consumption,
                "total_investment": self.compute_investment,
                "exports": self.compute_exports,
                "imports": self.compute_imports,
            }
        )
        
    def _load_io_matrix(self):
        """Load 62x62 I-O matrix"""
        try:
            df = pd.read_csv('data/io_matrix.csv', index_col=0)
            return df.values
        except:
            return np.random.rand(self.n_sectors, self.n_sectors)
    
    def _load_parameters(self):
        """Load model parameters"""
        try:
            df = pd.read_csv('data/parameters.csv')
            return dict(zip(df['parameter'], df['value']))
        except:
            return {}
    
    def _create_firms(self, n_per_sector):
        """Create firms distributed across sectors"""
        for sector in range(self.n_sectors):
            for _ in range(n_per_sector):
                firm = Firm(self.next_id(), self, sector)
                self.schedule.add(firm)
    
    def _create_households(self, n):
        """Create households"""
        for _ in range(n):
            household = Household(self.next_id(), self)
            self.schedule.add(household)
    
    def _create_banks(self, n):
        """Create banks"""
        for _ in range(n):
            bank = Bank(self.next_id(), self)
            self.schedule.add(bank)
    
    def _create_government(self):
        """Create government agent"""
        self.government = Government(self.next_id(), self)
        self.schedule.add(self.government)
    
    def step(self):
        """Execute one quarterly time step"""
        # Update macro variables
        self.euribor = self.forecaster.forecast_euribor(self)
        
        # Agent actions
        self.schedule.step()
        
        # Update aggregates
        self.update_prices()
        self.update_wages()
        self.unemployment_rate = self.compute_unemployment()
        self.inflation_rate = self.compute_inflation()
        
        # Collect data
        self.datacollector.collect(self)
        self.current_period += 1
    
    def update_prices(self):
        """Update sector prices"""
        firms = [a for a in self.schedule.agents if isinstance(a, Firm)]
        for sector in range(self.n_sectors):
            sector_firms = [f for f in firms if f.sector == sector]
            if sector_firms:
                self.prices[sector] = np.mean([f.price for f in sector_firms])
    
    def update_wages(self):
        """Update sector wages"""
        firms = [a for a in self.schedule.agents if isinstance(a, Firm)]
        for sector in range(self.n_sectors):
            sector_firms = [f for f in firms if f.sector == sector]
            if sector_firms:
                self.wages[sector] = np.mean([f.wage for f in sector_firms])
    
    def compute_gdp(self):
        """Compute nominal GDP"""
        return self.compute_consumption() + self.compute_investment() + \
               self.government.consumption + self.compute_exports() - self.compute_imports()
    
    def compute_real_gdp(self):
        """Compute real GDP"""
        return self.compute_gdp() / np.mean(self.prices)
    
    def compute_gva(self):
        """Compute nominal GVA"""
        firms = [a for a in self.schedule.agents if isinstance(a, Firm)]
        return sum(f.actual_sales * f.price for f in firms)
    
    def compute_real_gva(self):
        """Compute real GVA"""
        firms = [a for a in self.schedule.agents if isinstance(a, Firm)]
        return sum(f.actual_sales for f in firms)
    
    def compute_consumption(self):
        """Compute household consumption"""
        households = [a for a in self.schedule.agents if isinstance(a, Household)]
        return sum(h.consumption for h in households)
    
    def compute_investment(self):
        """Compute gross capital formation"""
        firms = [a for a in self.schedule.agents if isinstance(a, Firm)]
        return sum(max(0, f.capital - 100) for f in firms)  # Net investment
    
    def compute_exports(self):
        """Compute exports (simplified)"""
        return self.compute_gva() * 0.5
    
    def compute_imports(self):
        """Compute imports (simplified)"""
        return self.compute_gva() * 0.4
    
    def compute_unemployment(self):
        """Compute unemployment rate"""
        households = [a for a in self.schedule.agents if isinstance(a, Household)]
        if not households:
            return 0.0
        unemployed = sum(1 for h in households if not h.employed)
        return unemployed / len(households)
    
    def compute_inflation(self):
        """Compute inflation rate"""
        if self.current_period == 0:
            return 0.02
        return (np.mean(self.prices) - 1.0) / 1.0
    
    def run(self, steps=13):
        """Run simulation for specified steps"""
        for _ in range(steps):
            self.step()
        return self.datacollector.get_model_vars_dataframe()
