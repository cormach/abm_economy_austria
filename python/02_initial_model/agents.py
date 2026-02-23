import numpy as np
from mesa import Agent


class Firm(Agent):
    """Firm agent based on Appendix A-C"""
    def __init__(self, unique_id, model, sector):
        super().__init__(unique_id, model)
        self.sector = sector
        
        # Production
        self.capital = 100.0
        self.labor = 0
        self.output = 0.0
        self.inventory = 0.0
        
        # Prices and costs
        self.price = 1.0
        self.unit_cost = 1.0
        self.wage = 75.0
        
        # Financial
        self.deposits = 0.0
        self.loans = 0.0
        self.equity = 100.0
        self.profits = 0.0
        
        # Demand
        self.expected_demand = 0.0
        self.actual_sales = 0.0
        
    def step(self):
        self.plan_production()
        self.produce()
        self.set_price()
        self.sell()
        self.pay_wages()
        self.update_financials()
        
    def plan_production(self):
        """Adaptive expectations for demand"""
        self.expected_demand = 0.9 * self.actual_sales + 0.1 * self.expected_demand
        
    def produce(self):
        """Leontief production with capital and labor"""
        labor_productivity = 1.0
        capital_productivity = 0.5
        
        potential_output = min(
            self.capital * capital_productivity,
            self.labor * labor_productivity
        )
        self.output = potential_output
        self.inventory += self.output
        
    def set_price(self):
        """Cost-plus pricing with markup"""
        markup = 1.15
        io_costs = self.model.io_matrix[self.sector, :].sum()
        self.unit_cost = (self.wage * self.labor + io_costs) / max(self.output, 1)
        self.price = markup * self.unit_cost
        
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
        costs = self.wage * self.labor
        self.profits = revenue - costs
        self.deposits += revenue
        self.equity += self.profits


class Household(Agent):
    """Household agent based on Appendix D"""
    def __init__(self, unique_id, model):
        super().__init__(unique_id, model)
        
        # Employment
        self.employed = False
        self.employer = None
        self.wage_income = 0.0
        
        # Wealth
        self.deposits = 100.0
        self.consumption = 0.0
        
        # Expectations
        self.expected_income = 1.0
        
    def step(self):
        self.search_job()
        self.consume()
        self.save()
        
    def search_job(self):
        """Random matching in labor market"""
        if not self.employed and self.random.random() < 0.3:
            firms = [a for a in self.model.schedule.agents if isinstance(a, Firm)]
            if firms:
                firm = self.random.choice(firms)
                firm.labor += 1
                self.employer = firm
                self.employed = True
                
    def consume(self):
        """Consumption based on permanent income"""
        propensity = 0.75
        permanent_income = 0.9 * self.expected_income + 0.1 * self.wage_income
        self.consumption = propensity * (permanent_income + 0.05 * self.deposits)
        self.deposits -= self.consumption
        
    def save(self):
        """Update wealth"""
        self.deposits += self.wage_income
        self.expected_income = 0.9 * self.expected_income + 0.1 * self.wage_income
        self.wage_income = 0.0


class Bank(Agent):
    """Bank agent based on Appendix E"""
    def __init__(self, unique_id, model):
        super().__init__(unique_id, model)
        
        self.equity = 100.0
        self.reserves = 100.0
        self.loans = []
        self.deposits = 0.0
        self.interest_income = 0.0
        
    def step(self):
        self.process_loan_requests()
        self.collect_interest()
        self.handle_defaults()
        
    def process_loan_requests(self):
        """Credit supply based on Basel rules"""
        firms = [a for a in self.model.schedule.agents if isinstance(a, Firm)]
        
        for firm in firms:
            if firm.equity > 0:
                leverage = firm.loans / firm.equity
                if leverage < 3.0 and self.random.random() < 0.1:
                    loan_amount = min(firm.equity * 0.5, self.equity * 0.1)
                    firm.loans += loan_amount
                    firm.deposits += loan_amount
                    self.loans.append((firm, loan_amount))
                    
    def collect_interest(self):
        """Collect interest on loans"""
        rate = self.model.euribor + 0.03
        for firm, amount in self.loans:
            interest = amount * rate / 4  # Quarterly
            firm.deposits -= interest
            self.interest_income += interest
            
    def handle_defaults(self):
        """Write off bad loans"""
        self.loans = [(f, a) for f, a in self.loans if f.equity > 0]


class Government(Agent):
    """Government agent based on Appendix F"""
    def __init__(self, unique_id, model):
        super().__init__(unique_id, model)
        
        self.debt = 0.0
        self.deficit = 0.0
        
        # Revenue
        self.tax_revenue = 0.0
        self.vat_rate = 0.20
        self.income_tax_rate = 0.25
        self.corporate_tax_rate = 0.25
        
        # Expenditure
        self.consumption = 0.0
        self.unemployment_benefits = 0.0
        self.subsidies = 0.0
        
    def step(self):
        self.collect_taxes()
        self.spend()
        self.update_debt()
        
    def collect_taxes(self):
        """Collect VAT, income tax, corporate tax"""
        firms = [a for a in self.model.schedule.agents if isinstance(a, Firm)]
        households = [a for a in self.model.schedule.agents if isinstance(a, Household)]
        
        # Corporate tax
        corporate_tax = sum(max(0, f.profits) * self.corporate_tax_rate for f in firms)
        
        # Income tax
        income_tax = sum(h.wage_income * self.income_tax_rate for h in households)
        
        # VAT
        vat = sum(h.consumption * self.vat_rate for h in households)
        
        self.tax_revenue = corporate_tax + income_tax + vat
        
    def spend(self):
        """Government consumption and transfers"""
        households = [a for a in self.model.schedule.agents if isinstance(a, Household)]
        unemployed = sum(1 for h in households if not h.employed)
        
        self.unemployment_benefits = unemployed * 0.5
        self.consumption = self.tax_revenue * 0.8
        
    def update_debt(self):
        """Update government debt"""
        expenditure = self.consumption + self.unemployment_benefits + self.subsidies
        self.deficit = expenditure - self.tax_revenue
        self.debt += self.deficit
