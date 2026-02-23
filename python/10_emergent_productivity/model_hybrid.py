import numpy as np
import pandas as pd


class HybridProductivityModel:
    """Hybrid: firm agents with sectoral productivity initialization"""
    
    def __init__(self, year, quarter, seed, n_firms=620, n_households=1000):
        np.random.seed(seed)
        
        # Load I-O matrix
        io_df = pd.read_csv('../data/io_matrix.csv', index_col=0)
        self.io_matrix = io_df.values
        self.n_sectors = len(io_df)
        
        # Load sectoral data
        sector_gva = pd.read_csv('../data/calibration_nominal_sector_gva.csv')
        comp_data = pd.read_csv('../data/calibration_compensation_employees.csv')
        capital_data = pd.read_csv('../data/calibration_nominal_fixed_capitalformation.csv')
        
        initial_output = sector_gva.iloc[0, 1:].values
        total_comp = comp_data.iloc[0, 1]
        total_capital = capital_data.iloc[0, 1]
        
        # Calculate sectoral characteristics
        total_output = initial_output.sum()
        sector_output_share = initial_output / total_output
        sector_capital = sector_output_share * total_capital
        
        # Sectoral productivity (output per unit compensation)
        sector_comp = sector_output_share * total_comp
        self.sector_productivity = initial_output / sector_comp
        
        # Firms: assign to sectors proportionally
        self.n_firms = n_firms
        sector_firm_counts = np.round(sector_output_share * n_firms).astype(int)
        sector_firm_counts[-1] += n_firms - sector_firm_counts.sum()
        
        # Hard-code: ensure small sectors get at least 1 firm
        sector_firm_counts[2] = max(1, sector_firm_counts[2])  # Sector 3 (index 2)
        # Adjust total by taking from largest sector
        if sector_firm_counts.sum() > n_firms:
            largest_sector = np.argmax(sector_firm_counts)
            sector_firm_counts[largest_sector] -= (sector_firm_counts.sum() - n_firms)
        
        self.firm_sector = np.repeat(np.arange(self.n_sectors), sector_firm_counts)
        
        self.firm_labor = np.zeros(n_firms)
        self.firm_output = np.zeros(n_firms)
        self.firm_inventory = np.zeros(n_firms)
        self.firm_sales = np.zeros(n_firms)
        self.firm_expected_demand = np.zeros(n_firms)
        self.firm_capital = np.zeros(n_firms)
        self.firm_profits = np.zeros(n_firms)
        self.firm_productivity = np.zeros(n_firms)  # Firm-specific productivity
        
        # Initialize firms with sectoral data + small variation
        for s in range(self.n_sectors):
            sector_firms = np.where(self.firm_sector == s)[0]
            n_sector_firms = len(sector_firms)
            if n_sector_firms > 0:
                # Output: sectoral total with small random variation (sum = sectoral total)
                base_output = initial_output[s] / n_sector_firms
                variation = np.random.normal(1, 0.05, n_sector_firms)
                variation = variation / variation.mean()  # Normalize to preserve sum
                self.firm_output[sector_firms] = base_output * variation
                self.firm_inventory[sector_firms] = self.firm_output[sector_firms]
                self.firm_expected_demand[sector_firms] = self.firm_output[sector_firms]
                
                # Capital: sectoral total with variation
                base_capital = sector_capital[s] / n_sector_firms
                capital_var = np.random.normal(1, 0.05, n_sector_firms)
                capital_var = capital_var / capital_var.mean()
                self.firm_capital[sector_firms] = base_capital * capital_var
                
                # Productivity: sectoral value with small variation
                prod_var = np.random.normal(1, 0.05, n_sector_firms)
                self.firm_productivity[sector_firms] = self.sector_productivity[s] * prod_var
        
        self.firm_wage = np.ones(n_firms) * 75
        
        # Households
        self.n_households = n_households
        self.hh_employed = np.zeros(n_households, dtype=bool)
        self.hh_employer = np.full(n_households, -1, dtype=int)
        self.hh_income = np.zeros(n_households)
        self.hh_income_net = np.zeros(n_households)
        self.hh_wealth = np.random.uniform(5000, 20000, n_households)
        
        # Government
        self.gov_revenue = 0
        self.gov_spending = 0
        self.taxes_on_production = 0
        self.taxes_on_products = 0
        
        # Parameters
        self.mpc = 0.75
        self.search_prob = 0.3
        self.tax_rate_income = 0.213
        self.tax_rate_corporate = 0.25
        self.vat_rate = 0.153
        self.social_insurance_firm = 0.212
        self.social_insurance_worker = 0.171
        self.gov_consumption_rate = 0.20
        
    def labor_market(self):
        """Match unemployed workers to firms"""
        target_labor = self.firm_expected_demand / (self.firm_wage * self.firm_productivity)
        vacancies = np.maximum(0, target_labor - self.firm_labor)
        
        unemployed = np.where(~self.hh_employed)[0]
        searching = unemployed[np.random.random(len(unemployed)) < self.search_prob]
        
        for worker in searching:
            firms_with_vacancies = np.where(vacancies > 0)[0]
            if len(firms_with_vacancies) == 0:
                break
            firm = np.random.choice(firms_with_vacancies)
            self.hh_employed[worker] = True
            self.hh_employer[worker] = firm
            self.firm_labor[firm] += 1
            vacancies[firm] -= 1
    
    def production(self):
        """Firms produce using labor with firm-specific productivity"""
        labor_value = self.firm_labor * self.firm_wage
        self.firm_output = labor_value * self.firm_productivity
        self.firm_inventory += self.firm_output
    
    def sales(self):
        """Firms sell to households and other firms"""
        household_consumption = np.sum(self.hh_income * self.mpc)
        
        # Aggregate to sectors for I-O
        sector_output = np.zeros(self.n_sectors)
        for s in range(self.n_sectors):
            sector_firms = np.where(self.firm_sector == s)[0]
            sector_output[s] = np.sum(self.firm_output[sector_firms])
        
        # Intermediate demand from I-O matrix
        sector_intermediate_demand = self.io_matrix.T @ sector_output
        
        # Distribute to firms
        intermediate_demand = np.zeros(self.n_firms)
        for firm in range(self.n_firms):
            sector = self.firm_sector[firm]
            sector_firms = np.where(self.firm_sector == sector)[0]
            if len(sector_firms) > 0:
                # Distribute proportional to firm output
                sector_total = sector_output[sector]
                if sector_total > 0:
                    firm_share = self.firm_output[firm] / sector_total
                    intermediate_demand[firm] = sector_intermediate_demand[sector] * firm_share
        
        # Total demand
        total_inventory = np.sum(self.firm_inventory)
        if total_inventory > 0:
            hh_share = self.firm_inventory / total_inventory
            hh_demand = hh_share * household_consumption
            self.firm_sales = hh_demand + intermediate_demand
            self.firm_sales = np.minimum(self.firm_sales, self.firm_inventory)
            self.firm_inventory -= self.firm_sales
        else:
            self.firm_sales[:] = 0
        
        self.firm_expected_demand = 0.9 * self.firm_sales + 0.1 * self.firm_expected_demand
    
    def wages(self):
        """Pay wages"""
        self.hh_income[:] = 0
        for worker in range(self.n_households):
            if self.hh_employed[worker]:
                firm = self.hh_employer[worker]
                self.hh_income[worker] = self.firm_wage[firm]
    
    def consumption(self):
        """Households consume"""
        consumption = self.hh_income_net * self.mpc
        self.hh_wealth += self.hh_income_net - consumption
        return consumption
    
    def government(self):
        """Government taxes and spends"""
        income_tax = np.sum(self.hh_income * self.tax_rate_income)
        social_insurance_worker = np.sum(self.hh_income * self.social_insurance_worker)
        social_insurance_firm = np.sum(self.hh_income * self.social_insurance_firm)
        corporate_tax = np.sum(np.maximum(0, self.firm_profits) * self.tax_rate_corporate)
        
        gross_consumption = np.sum(self.hh_income_net * self.mpc)
        vat = gross_consumption * self.vat_rate
        taxes_on_products = vat * 1.5
        
        self.gov_revenue = (income_tax + social_insurance_worker + social_insurance_firm + 
                           corporate_tax + vat + self.taxes_on_production)
        self.gov_spending = self.gov_consumption_rate * 80000
        self.hh_income_net = self.hh_income * (1 - self.tax_rate_income - self.social_insurance_worker)
        self.taxes_on_products = taxes_on_products
    
    def calculate_profits(self):
        """Calculate operating surplus"""
        wage_bill = self.firm_wage * self.firm_labor
        gross_surplus = self.firm_output - wage_bill
        self.taxes_on_production = 0.039 * np.sum(np.maximum(0, gross_surplus))
        
        total_gross = np.sum(np.maximum(0, gross_surplus))
        if total_gross > 0:
            tax_per_firm = (np.maximum(0, gross_surplus) / total_gross) * self.taxes_on_production
        else:
            tax_per_firm = np.zeros(self.n_firms)
        
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
        
        total_wages = np.sum(self.hh_income)
        total_profits = np.sum(self.firm_profits)
        total_output = total_wages + total_profits + self.taxes_on_production + self.taxes_on_products
        unemployment = 1.0 - np.mean(self.hh_employed)
        
        # Calculate emergent aggregate productivity
        total_labor_value = np.sum(self.firm_labor * self.firm_wage)
        if total_labor_value > 0:
            emergent_productivity = np.sum(self.firm_output) / total_labor_value
        else:
            emergent_productivity = 0
        
        return {
            'output': total_output,
            'wages': total_wages,
            'profits': total_profits,
            'gov_spending': self.gov_spending,
            'gov_revenue': self.gov_revenue,
            'consumption': np.sum(consumption),
            'unemployment': unemployment,
            'employed': np.sum(self.hh_employed),
            'emergent_productivity': emergent_productivity
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
    model = HybridProductivityModel(2013, 2, 42)
    
    # Validate INITIAL sectoral aggregates
    sector_gva = pd.read_csv('../data/calibration_nominal_sector_gva.csv')
    initial_output = sector_gva.iloc[0, 1:].values
    
    print("\n=== Initial Sectoral Validation (t=0) ===")
    initial_firm_output = model.firm_output.copy()
    for s in range(min(5, model.n_sectors)):
        sector_firms = np.where(model.firm_sector == s)[0]
        firm_total = initial_firm_output[sector_firms].sum()
        empirical = initial_output[s]
        error = abs(firm_total - empirical) / empirical * 100
        print(f"Sector {s+1}: Firms={firm_total:.1f}, Empirical={empirical:.1f}, Error={error:.2f}%")
    
    results = model.run(steps=13)
    
    print("\n=== Hybrid Model Results ===")
    print(results[['output', 'wages', 'profits', 'emergent_productivity']])
    print(f"\nFinal emergent productivity: {results['emergent_productivity'].iloc[-1]:.3f}")
    print(f"Target productivity: 1.931")
