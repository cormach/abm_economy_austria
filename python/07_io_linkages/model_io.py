import numpy as np
import pandas as pd


class IOLinkagesModel:
    """Economy model with I-O matrix linkages between sectors"""
    
    def __init__(self, year, quarter, seed, n_firms=620, n_households=1000):
        np.random.seed(seed)
        
        # Load I-O matrix
        io_df = pd.read_csv('../data/io_matrix.csv', index_col=0)
        self.io_matrix = io_df.values
        self.n_sectors = len(io_df)
        
        # Load initial sectoral output
        sector_gva = pd.read_csv('../data/calibration_nominal_sector_gva.csv')
        initial_output = sector_gva.iloc[0, 1:].values  # period 0, all sectors
        
        # Firms
        self.n_firms = n_firms
        self.firm_sector = np.random.randint(0, self.n_sectors, n_firms)
        self.firm_labor = np.zeros(n_firms)
        self.firm_output = np.zeros(n_firms)
        self.firm_inventory = np.zeros(n_firms)
        self.firm_sales = np.zeros(n_firms)
        self.firm_expected_demand = np.zeros(n_firms)
        
        # Initialize firm output from sectoral data
        firms_per_sector = n_firms // self.n_sectors
        for s in range(self.n_sectors):
            sector_firms = np.where(self.firm_sector == s)[0]
            if len(sector_firms) > 0:
                output_per_firm = initial_output[s] / len(sector_firms)
                self.firm_output[sector_firms] = output_per_firm
                self.firm_inventory[sector_firms] = output_per_firm
                self.firm_expected_demand[sector_firms] = output_per_firm
        
        self.firm_wage = np.ones(n_firms) * 75
        
        # Households
        self.n_households = n_households
        self.hh_employed = np.zeros(n_households, dtype=bool)
        self.hh_employer = np.full(n_households, -1, dtype=int)  # firm id
        self.hh_income = np.zeros(n_households)
        self.hh_wealth = np.random.uniform(5000, 20000, n_households)
        
        # Parameters
        self.productivity = 0.5  # output per worker
        self.mpc = 0.75
        self.search_prob = 0.3  # probability unemployed worker searches
        
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
        """Firms produce using labor and intermediate inputs"""
        # Labor input
        labor_output = self.firm_labor * self.productivity
        
        # Intermediate inputs from I-O matrix
        for firm in range(self.n_firms):
            sector = self.firm_sector[firm]
            # Input requirements from I-O matrix (row = inputs needed)
            input_needs = self.io_matrix[sector, :] * labor_output[firm]
            
            # For now, assume inputs are available (simplified)
            # Full implementation would match firms to suppliers
            
        self.firm_output = labor_output
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
        """Households consume based on income"""
        consumption = self.hh_income * self.mpc
        self.hh_wealth += self.hh_income - consumption
        return consumption
    
    def step(self):
        """Single time step"""
        self.labor_market()
        self.production()
        self.sales()
        self.wages()
        consumption = self.consumption()
        
        # Aggregates
        total_wages = np.sum(self.hh_income)
        # GDP = value added = wages + profits (for now, just wages)
        total_output = total_wages  # GDP = compensation of employees
        unemployment = 1.0 - np.mean(self.hh_employed)
        
        return {
            'output': total_output,
            'wages': total_wages,
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
    model = IOLinkagesModel(2013, 2, 42)
    results = model.run(steps=13)
    print(results)
