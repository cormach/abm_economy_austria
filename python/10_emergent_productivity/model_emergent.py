import numpy as np
import pandas as pd


class EmergentProductivityModel:
    """Economy model with sectoral productivity derived from empirical data"""
    
    def __init__(self, year, quarter, seed, n_firms=620, n_households=1000):
        np.random.seed(seed)
        
        # Load I-O matrix
        io_df = pd.read_csv('../data/io_matrix.csv', index_col=0)
        self.io_matrix = io_df.values
        self.n_sectors = len(io_df)
        
        # Load initial sectoral data
        sector_gva = pd.read_csv('../data/calibration_nominal_sector_gva.csv')
        initial_output = sector_gva.iloc[0, 1:].values  # period 0, all sectors
        comp_data = pd.read_csv('../data/calibration_compensation_employees.csv')
        total_comp = comp_data.iloc[0, 1]
        capital_data = pd.read_csv('../data/calibration_nominal_fixed_capitalformation.csv')
        total_capital = capital_data.iloc[0, 1]
        
        # 2. Sectoral employment distribution (proportional to output)
        total_output = initial_output.sum()
        sector_employment_share = initial_output / total_output
        
        # 4. Capital stock by sector (proportional to output)
        sector_capital_share = initial_output / total_output
        sector_capital = sector_capital_share * total_capital
        
        # Firms
        self.n_firms = n_firms
        # Assign firms to sectors based on employment shares
        sector_firm_counts = np.round(sector_employment_share * n_firms).astype(int)
        sector_firm_counts[-1] += n_firms - sector_firm_counts.sum()  # Adjust for rounding
        self.firm_sector = np.repeat(np.arange(self.n_sectors), sector_firm_counts)
        
        self.firm_labor = np.zeros(n_firms)
        self.firm_output = np.zeros(n_firms)
        self.firm_inventory = np.zeros(n_firms)
        self.firm_sales = np.zeros(n_firms)
        self.firm_expected_demand = np.zeros(n_firms)
        self.firm_capital = np.zeros(n_firms)
        self.firm_profits = np.zeros(n_firms)
        
        # Initialize firm output and capital from sectoral data
        for s in range(self.n_sectors):
            sector_firms = np.where(self.firm_sector == s)[0]
            if len(sector_firms) > 0:
                self.firm_output[sector_firms] = initial_output[s] / len(sector_firms)
                self.firm_inventory[sector_firms] = initial_output[s] / len(sector_firms)
                self.firm_expected_demand[sector_firms] = initial_output[s] / len(sector_firms)
                self.firm_capital[sector_firms] = sector_capital[s] / len(sector_firms)
        
        self.firm_wage = np.ones(n_firms) * 75
        
        # Households
        self.n_households = n_households
        self.hh_employed = np.zeros(n_households, dtype=bool)
        self.hh_employer = np.full(n_households, -1, dtype=int)
        self.hh_income = np.zeros(n_households)
        self.hh_income_net = np.zeros(n_households)  # After-tax income
        self.hh_wealth = np.random.uniform(5000, 20000, n_households)
        
        # Government
        self.gov_revenue = 0
        self.gov_spending = 0
        self.gov_deficit = 0
        self.taxes_on_production = 0
        self.taxes_on_products = 0
        
        # 1. Sectoral productivity from data (output per unit compensation)
        self.sector_productivity = initial_output / (total_comp / self.n_sectors)
        
        # 3. I-O matrix linkages affect productivity through intermediate inputs
        # Calculate sectoral total requirements (Leontief inverse)
        I = np.eye(self.n_sectors)
        A = self.io_matrix / initial_output[:, np.newaxis]  # Technical coefficients
        A = np.nan_to_num(A, 0)  # Handle division by zero
        try:
            self.leontief_inverse = np.linalg.inv(I - A)
        except:
            self.leontief_inverse = I  # Fallback if singular
        
        # Parameters
        self.productivity = 1.93  # Aggregate productivity (for reference)
        self.capital_productivity = 0.3
        self.mpc = 0.75
        self.search_prob = 0.3
        self.markup = 1.15
        
        # Government parameters (from C code - Austrian empirical rates)
        self.tax_rate_income = 0.213  # Income tax rate
        self.tax_rate_corporate = 0.25  # Corporate tax (approximate)
        self.vat_rate = 0.153  # VAT rate
        self.social_insurance_firm = 0.212  # Employer social contributions
        self.social_insurance_worker = 0.171  # Employee social contributions
        self.gov_consumption_rate = 0.20  # Gov spending as % of GDP
        
    def labor_market(self):
        """Match unemployed workers to firms with vacancies"""
        
        # Firms post vacancies based on expected demand
        target_labor = self.firm_expected_demand / self.productivity
        vacancies = np.maximum(0, target_labor - self.firm_labor)
        
        # Unemployed workers search (parameterized probability)
        unemployed = np.where(~self.hh_employed)[0]
        searching = unemployed[np.random.random(len(unemployed)) < self.search_prob]
        
        # Match
        for worker in searching:
            firms_with_vacancies = np.where(vacancies > 0)[0]
            if len(firms_with_vacancies) == 0:
                break
            
            # Random matching
            firm = np.random.choice(firms_with_vacancies)
            self.hh_employed[worker] = True
            self.hh_employer[worker] = firm
            self.firm_labor[firm] += 1
            vacancies[firm] -= 1
    
    def production(self):
        """Firms produce using labor with sectoral productivity"""
        labor_value = np.zeros(self.n_firms)
        for firm in range(self.n_firms):
            labor_value[firm] = self.firm_labor[firm] * self.firm_wage[firm]
        
        # Output = labor_value × sectoral_productivity
        # Each firm uses its sector's productivity
        for firm in range(self.n_firms):
            sector = self.firm_sector[firm]
            self.firm_output[firm] = labor_value[firm] * self.sector_productivity[sector]
        
        self.firm_inventory += self.firm_output
    
    def sales(self):
        """Firms sell to households and other firms"""
        # Household consumption
        household_consumption = np.sum(self.hh_income * self.mpc)
        
        # Aggregate output by sector
        sector_output = np.zeros(self.n_sectors)
        for s in range(self.n_sectors):
            sector_firms = np.where(self.firm_sector == s)[0]
            sector_output[s] = np.sum(self.firm_output[sector_firms])
        
        # Intermediate demand from I-O matrix (column = outputs sold to others)
        sector_intermediate_demand = self.io_matrix.T @ sector_output  # 62x62 @ 62 = 62
        
        # Distribute sector demand to firms
        intermediate_demand = np.zeros(self.n_firms)
        for firm in range(self.n_firms):
            sector = self.firm_sector[firm]
            sector_firms = np.where(self.firm_sector == sector)[0]
            if len(sector_firms) > 0:
                intermediate_demand[firm] = sector_intermediate_demand[sector] / len(sector_firms)
        
        # Total demand = household + intermediate
        total_inventory = np.sum(self.firm_inventory)
        if total_inventory > 0:
            # Distribute household consumption proportional to inventory
            hh_share = self.firm_inventory / total_inventory
            hh_demand = hh_share * household_consumption
            
            # Total sales
            self.firm_sales = hh_demand + intermediate_demand
            self.firm_sales = np.minimum(self.firm_sales, self.firm_inventory)
            self.firm_inventory -= self.firm_sales
        else:
            self.firm_sales[:] = 0
        
        # Update expected demand (adaptive expectations)
        self.firm_expected_demand = 0.9 * self.firm_sales + 0.1 * self.firm_expected_demand
    
    def wages(self):
        """Pay wages to employed workers"""
        self.hh_income[:] = 0
        for worker in range(self.n_households):
            if self.hh_employed[worker]:
                firm = self.hh_employer[worker]
                self.hh_income[worker] = self.firm_wage[firm]
    
    def consumption(self):
        """Households consume based on after-tax income"""
        consumption = self.hh_income_net * self.mpc
        self.hh_wealth += self.hh_income_net - consumption
        return consumption
    
    def government(self):
        """Government collects taxes and spends"""
        # Income tax (on gross wages)
        income_tax = np.sum(self.hh_income * self.tax_rate_income)
        
        # Social insurance contributions
        social_insurance_worker = np.sum(self.hh_income * self.social_insurance_worker)
        social_insurance_firm = np.sum(self.hh_income * self.social_insurance_firm)
        
        # Corporate tax (on profits)
        corporate_tax = np.sum(np.maximum(0, self.firm_profits) * self.tax_rate_corporate)
        
        # VAT (on household consumption)
        gross_consumption = np.sum(self.hh_income_net * self.mpc)
        vat = gross_consumption * self.vat_rate
        
        # Taxes on products (VAT + other product taxes)
        # From data: taxes_on_products = GDP - GVA ≈ 8,533
        # Approximate as VAT + additional product taxes
        taxes_on_products = vat * 1.5  # VAT is ~2/3 of total product taxes
        
        # Total revenue
        self.gov_revenue = (income_tax + social_insurance_worker + social_insurance_firm + 
                           corporate_tax + vat + self.taxes_on_production)
        
        # Government spending (20% of target GDP)
        self.gov_spending = self.gov_consumption_rate * 80000
        
        # After-tax income for households (income - income_tax - social_insurance)
        self.hh_income_net = self.hh_income * (1 - self.tax_rate_income - self.social_insurance_worker)
        
        # Store taxes on products for GDP calculation
        self.taxes_on_products = taxes_on_products
        
        # Deficit
        self.gov_deficit = self.gov_spending - self.gov_revenue
    
    def calculate_profits(self):
        """Calculate operating surplus (profits) for firms"""
        # Gross surplus = output - wages
        wage_bill = np.zeros(self.n_firms)
        for firm in range(self.n_firms):
            wage_bill[firm] = self.firm_wage[firm] * self.firm_labor[firm]
        
        gross_surplus = self.firm_output - wage_bill
        
        # Taxes on production (3.9% of gross surplus)
        self.taxes_on_production = 0.039 * np.sum(np.maximum(0, gross_surplus))
        
        # Operating surplus = gross surplus - taxes on production
        # Distribute tax burden proportionally
        tax_per_firm = np.zeros(self.n_firms)
        total_gross = np.sum(np.maximum(0, gross_surplus))
        if total_gross > 0:
            tax_per_firm = (np.maximum(0, gross_surplus) / total_gross) * self.taxes_on_production
        
        self.firm_profits = np.maximum(0, gross_surplus - tax_per_firm)
    
    def step(self):
        """Single time step"""
        self.labor_market()
        self.production()
        self.sales()
        self.wages()
        self.calculate_profits()
        self.government()
        consumption = self.consumption()
        
        # Aggregates
        total_wages = np.sum(self.hh_income)
        total_profits = np.sum(self.firm_profits)
        
        # GDP (income approach) = GVA + taxes on products
        # GVA = compensation + operating surplus + taxes on production
        # GDP = wages + profits + taxes_on_production + taxes_on_products
        total_output = total_wages + total_profits + self.taxes_on_production + self.taxes_on_products
        unemployment = 1.0 - np.mean(self.hh_employed)
        
        return {
            'output': total_output,
            'wages': total_wages,
            'profits': total_profits,
            'gov_spending': self.gov_spending,
            'gov_revenue': self.gov_revenue,
            'consumption': np.sum(consumption),
            'unemployment': unemployment,
            'employed': np.sum(self.hh_employed)
        }
    
    def run(self, steps=13):
        """Run simulation"""
        results = []
        for t in range(steps):
            result = self.step()
            result['period'] = t
            results.append(result)
        return pd.DataFrame(results)


if __name__ == "__main__":
    model = EmergentProductivityModel(2013, 2, 42)
    results = model.run(steps=13)
    print(results[['output', 'wages', 'profits', 'gov_spending', 'gov_revenue']])
