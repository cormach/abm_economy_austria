"""
Reusable base classes for economic agent-based models
Can be extended for any economy (Austria, US, UK, etc.)
"""

from mesa import Agent, Model
from mesa.time import RandomActivation
from mesa.datacollection import DataCollector
import numpy as np
import pandas as pd
from abc import ABC, abstractmethod


# ============================================================================
# BASE AGENT CLASSES (Reusable)
# ============================================================================

class EconomicAgent(Agent, ABC):
    """Base class for all economic agents"""
    
    def __init__(self, unique_id, model):
        super().__init__(unique_id, model)
        self.wealth = 0.0
        self.income = 0.0
        
    @abstractmethod
    def step(self):
        """Each agent must implement their behavior"""
        pass


class Firm(EconomicAgent):
    """Generic firm agent - reusable across economies"""
    
    def __init__(self, unique_id, model, sector=0, **kwargs):
        super().__init__(unique_id, model)
        
        # Production
        self.sector = sector
        self.capital = kwargs.get('initial_capital', 100.0)
        self.labor = 0
        self.output = 0.0
        self.inventory = 0.0
        
        # Prices and costs
        self.price = kwargs.get('initial_price', 1.0)
        self.wage = kwargs.get('initial_wage', 1.0)
        self.unit_cost = 0.0
        
        # Financial
        self.deposits = kwargs.get('initial_deposits', 0.0)
        self.loans = 0.0
        self.equity = self.capital
        self.profits = 0.0
        
        # Expectations
        self.expected_demand = 0.0
        self.actual_sales = 0.0
        
        # Parameters (can be country-specific)
        self.markup = kwargs.get('markup', 1.15)
        self.capital_share = kwargs.get('capital_share', 0.3)
        self.labor_share = kwargs.get('labor_share', 0.7)
        
    def step(self):
        """Standard firm behavior"""
        self.plan_production()
        self.produce()
        self.set_price()
        self.sell()
        self.pay_wages()
        self.update_financials()
        
    def plan_production(self):
        """Adaptive expectations"""
        self.expected_demand = (0.9 * self.actual_sales + 
                               0.1 * self.expected_demand)
        
    def produce(self):
        """Cobb-Douglas production function"""
        self.output = (self.capital ** self.capital_share * 
                      self.labor ** self.labor_share)
        self.inventory += self.output
        
    def set_price(self):
        """Cost-plus pricing"""
        if self.output > 0:
            # Get input costs from I-O matrix if available
            io_costs = self.get_input_costs()
            labor_costs = self.wage * self.labor
            self.unit_cost = (labor_costs + io_costs) / self.output
            self.price = self.markup * self.unit_cost
        
    def get_input_costs(self):
        """Get intermediate input costs from I-O matrix"""
        if hasattr(self.model, 'io_matrix'):
            return self.model.io_matrix[self.sector, :].sum()
        return 0.0
        
    def sell(self):
        """Sell from inventory"""
        self.actual_sales = min(self.inventory, self.expected_demand)
        self.inventory -= self.actual_sales
        
    def pay_wages(self):
        """Pay workers"""
        wage_bill = self.wage * self.labor
        self.deposits -= wage_bill
        
    def update_financials(self):
        """Update balance sheet"""
        revenue = self.actual_sales * self.price
        costs = self.wage * self.labor + self.get_input_costs()
        self.profits = revenue - costs
        self.deposits += revenue
        self.equity += self.profits


class Household(EconomicAgent):
    """Generic household agent - reusable across economies"""
    
    def __init__(self, unique_id, model, **kwargs):
        super().__init__(unique_id, model)
        
        # Employment
        self.employed = False
        self.employer = None
        self.wage_income = 0.0
        
        # Wealth
        self.deposits = kwargs.get('initial_wealth', 100.0)
        self.consumption = 0.0
        
        # Expectations
        self.expected_income = kwargs.get('initial_income', 1.0)
        
        # Parameters (can be country-specific)
        self.mpc = kwargs.get('mpc', 0.75)
        self.wealth_effect = kwargs.get('wealth_effect', 0.05)
        
    def step(self):
        """Standard household behavior"""
        self.search_job()
        self.consume()
        self.save()
        
    def search_job(self):
        """Random job matching"""
        if not self.employed:
            firms = [a for a in self.model.schedule.agents 
                    if isinstance(a, Firm) and a.labor < a.capital]
            if firms and self.random.random() < 0.3:
                firm = self.random.choice(firms)
                firm.labor += 1
                self.employer = firm
                self.employed = True
                
    def consume(self):
        """Permanent income hypothesis"""
        permanent_income = (0.9 * self.expected_income + 
                          0.1 * self.wage_income)
        self.consumption = self.mpc * (permanent_income + 
                                      self.wealth_effect * self.deposits)
        self.deposits -= self.consumption
        
    def save(self):
        """Update wealth"""
        self.deposits += self.wage_income
        self.expected_income = (0.9 * self.expected_income + 
                               0.1 * self.wage_income)
        self.wage_income = 0.0


class Bank(EconomicAgent):
    """Generic bank agent - reusable across economies"""
    
    def __init__(self, unique_id, model, **kwargs):
        super().__init__(unique_id, model)
        
        self.equity = kwargs.get('initial_equity', 100.0)
        self.reserves = kwargs.get('initial_reserves', 100.0)
        self.loans = []
        self.deposits = 0.0
        self.interest_income = 0.0
        
        # Parameters (can be country-specific)
        self.max_leverage = kwargs.get('max_leverage', 3.0)
        self.interest_spread = kwargs.get('interest_spread', 0.03)
        
    def step(self):
        """Standard bank behavior"""
        self.process_loan_requests()
        self.collect_interest()
        self.handle_defaults()
        
    def process_loan_requests(self):
        """Basel-style lending"""
        firms = [a for a in self.model.schedule.agents if isinstance(a, Firm)]
        
        for firm in firms:
            if firm.equity > 0:
                leverage = firm.loans / firm.equity
                if leverage < self.max_leverage and self.random.random() < 0.1:
                    loan_amount = min(firm.equity * 0.5, self.equity * 0.1)
                    firm.loans += loan_amount
                    firm.deposits += loan_amount
                    self.loans.append((firm, loan_amount))
                    
    def collect_interest(self):
        """Collect interest on loans"""
        rate = self.model.interest_rate + self.interest_spread
        for firm, amount in self.loans:
            interest = amount * rate / 4  # Quarterly
            firm.deposits -= interest
            self.interest_income += interest
            
    def handle_defaults(self):
        """Write off bad loans"""
        self.loans = [(f, a) for f, a in self.loans if f.equity > 0]


class Government(EconomicAgent):
    """Generic government agent - reusable across economies"""
    
    def __init__(self, unique_id, model, **kwargs):
        super().__init__(unique_id, model)
        
        self.debt = kwargs.get('initial_debt', 0.0)
        self.deficit = 0.0
        
        # Revenue
        self.tax_revenue = 0.0
        
        # Expenditure
        self.consumption = 0.0
        self.unemployment_benefits = 0.0
        self.subsidies = 0.0
        
        # Parameters (country-specific)
        self.vat_rate = kwargs.get('vat_rate', 0.20)
        self.income_tax_rate = kwargs.get('income_tax_rate', 0.25)
        self.corporate_tax_rate = kwargs.get('corporate_tax_rate', 0.25)
        self.ub_replacement_rate = kwargs.get('ub_replacement_rate', 0.5)
        
    def step(self):
        """Standard government behavior"""
        self.collect_taxes()
        self.spend()
        self.update_debt()
        
    def collect_taxes(self):
        """Collect VAT, income tax, corporate tax"""
        firms = [a for a in self.model.schedule.agents if isinstance(a, Firm)]
        households = [a for a in self.model.schedule.agents if isinstance(a, Household)]
        
        corporate_tax = sum(max(0, f.profits) * self.corporate_tax_rate 
                          for f in firms)
        income_tax = sum(h.wage_income * self.income_tax_rate 
                        for h in households)
        vat = sum(h.consumption * self.vat_rate for h in households)
        
        self.tax_revenue = corporate_tax + income_tax + vat
        
    def spend(self):
        """Government consumption and transfers"""
        households = [a for a in self.model.schedule.agents if isinstance(a, Household)]
        unemployed = sum(1 for h in households if not h.employed)
        
        self.unemployment_benefits = unemployed * self.ub_replacement_rate
        self.consumption = self.tax_revenue * 0.8
        
    def update_debt(self):
        """Update government debt"""
        expenditure = self.consumption + self.unemployment_benefits + self.subsidies
        self.deficit = expenditure - self.tax_revenue
        self.debt += self.deficit


# ============================================================================
# BASE MODEL CLASS (Reusable)
# ============================================================================

class EconomicModel(Model):
    """
    Generic economic model - can be configured for any country
    
    Usage:
        model = EconomicModel(
            country='Austria',
            io_matrix=austria_io_matrix,
            n_firms=620,
            n_households=1000,
            parameters=austria_params
        )
    """
    
    def __init__(self, country='Generic', io_matrix=None, 
                 n_firms=100, n_households=1000, n_banks=5,
                 parameters=None, **kwargs):
        super().__init__()
        
        self.country = country
        self.schedule = RandomActivation(self)
        
        # Load country-specific data
        self.io_matrix = io_matrix
        self.n_sectors = io_matrix.shape[0] if io_matrix is not None else 10
        
        # Parameters (with defaults)
        self.params = parameters or {}
        
        # Economic state
        self.interest_rate = self.params.get('initial_interest_rate', 0.01)
        self.prices = np.ones(self.n_sectors)
        self.wages = np.ones(self.n_sectors)
        
        # Create agents
        self._create_agents(n_firms, n_households, n_banks)
        
        # Data collection
        self.datacollector = DataCollector(
            model_reporters=self.get_model_reporters(),
            agent_reporters=self.get_agent_reporters()
        )
        
    def _create_agents(self, n_firms, n_households, n_banks):
        """Create agents with country-specific parameters"""
        
        # Firms
        for i in range(n_firms):
            firm = Firm(
                self.next_id(), self, 
                sector=i % self.n_sectors,
                **self.params.get('firm_params', {})
            )
            self.schedule.add(firm)
        
        # Households
        for i in range(n_households):
            household = Household(
                self.next_id(), self,
                **self.params.get('household_params', {})
            )
            self.schedule.add(household)
        
        # Banks
        for i in range(n_banks):
            bank = Bank(
                self.next_id(), self,
                **self.params.get('bank_params', {})
            )
            self.schedule.add(bank)
        
        # Government
        self.government = Government(
            self.next_id(), self,
            **self.params.get('government_params', {})
        )
        self.schedule.add(self.government)
    
    def step(self):
        """Execute one time step"""
        self.schedule.step()
        self.update_aggregates()
        self.datacollector.collect(self)
    
    def update_aggregates(self):
        """Update aggregate variables"""
        firms = [a for a in self.schedule.agents if isinstance(a, Firm)]
        
        for sector in range(self.n_sectors):
            sector_firms = [f for f in firms if f.sector == sector]
            if sector_firms:
                self.prices[sector] = np.mean([f.price for f in sector_firms])
                self.wages[sector] = np.mean([f.wage for f in sector_firms])
    
    def get_model_reporters(self):
        """Define what to collect - can be overridden"""
        return {
            "GDP": self.compute_gdp,
            "GVA": self.compute_gva,
            "Unemployment": self.compute_unemployment,
            "Inflation": self.compute_inflation,
            "Government_Debt": lambda m: m.government.debt,
        }
    
    def get_agent_reporters(self):
        """Define agent-level data collection"""
        return {
            "Wealth": "wealth",
            "Income": "income",
        }
    
    def compute_gdp(self):
        """Compute GDP"""
        households = [a for a in self.schedule.agents if isinstance(a, Household)]
        consumption = sum(h.consumption for h in households)
        investment = 0  # Simplified
        gov_spending = self.government.consumption
        return consumption + investment + gov_spending
    
    def compute_gva(self):
        """Compute GVA"""
        firms = [a for a in self.schedule.agents if isinstance(a, Firm)]
        return sum(f.actual_sales * f.price for f in firms)
    
    def compute_unemployment(self):
        """Compute unemployment rate"""
        households = [a for a in self.schedule.agents if isinstance(a, Household)]
        if not households:
            return 0.0
        unemployed = sum(1 for h in households if not h.employed)
        return unemployed / len(households)
    
    def compute_inflation(self):
        """Compute inflation rate"""
        return (np.mean(self.prices) - 1.0) / 1.0
    
    def run(self, steps=13):
        """Run simulation"""
        for _ in range(steps):
            self.step()
        return self.datacollector.get_model_vars_dataframe()


# ============================================================================
# COUNTRY-SPECIFIC CONFIGURATION
# ============================================================================

def load_country_config(country='Austria'):
    """Load country-specific parameters and data"""
    
    configs = {
        'Austria': {
            'io_matrix': 'data/io_matrix.csv',
            'n_sectors': 62,
            'firm_params': {
                'markup': 1.15,
                'capital_share': 0.3,
                'labor_share': 0.7,
            },
            'household_params': {
                'mpc': 0.75,
                'wealth_effect': 0.05,
            },
            'bank_params': {
                'max_leverage': 3.0,
                'interest_spread': 0.03,
            },
            'government_params': {
                'vat_rate': 0.20,
                'income_tax_rate': 0.25,
                'corporate_tax_rate': 0.25,
            }
        },
        'US': {
            'io_matrix': 'data/us_io_matrix.csv',
            'n_sectors': 71,
            'firm_params': {
                'markup': 1.20,
                'capital_share': 0.35,
                'labor_share': 0.65,
            },
            'household_params': {
                'mpc': 0.70,
                'wealth_effect': 0.08,
            },
            'government_params': {
                'vat_rate': 0.0,  # No federal VAT
                'income_tax_rate': 0.22,
                'corporate_tax_rate': 0.21,
            }
        },
        'UK': {
            'io_matrix': 'data/uk_io_matrix.csv',
            'n_sectors': 105,
            'firm_params': {
                'markup': 1.18,
            },
            'household_params': {
                'mpc': 0.72,
            },
            'government_params': {
                'vat_rate': 0.20,
                'income_tax_rate': 0.20,
                'corporate_tax_rate': 0.19,
            }
        }
    }
    
    return configs.get(country, configs['Austria'])


# ============================================================================
# USAGE EXAMPLES
# ============================================================================

if __name__ == "__main__":
    
    # Example 1: Austrian economy
    austria_config = load_country_config('Austria')
    io_matrix = pd.read_csv(austria_config['io_matrix'], index_col=0).values
    
    austria_model = EconomicModel(
        country='Austria',
        io_matrix=io_matrix,
        n_firms=620,
        n_households=1000,
        parameters=austria_config
    )
    
    results = austria_model.run(steps=13)
    print("Austria Results:")
    print(results.head())
    
    # Example 2: US economy (if data available)
    # us_config = load_country_config('US')
    # us_model = EconomicModel(
    #     country='US',
    #     io_matrix=load_us_io_matrix(),
    #     n_firms=1000,
    #     n_households=5000,
    #     parameters=us_config
    # )
    
    # Example 3: Custom economy
    # custom_model = EconomicModel(
    #     country='CustomCountry',
    #     io_matrix=my_io_matrix,
    #     parameters={
    #         'firm_params': {'markup': 1.25},
    #         'household_params': {'mpc': 0.80},
    #     }
    # )
