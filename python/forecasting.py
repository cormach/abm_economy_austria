import numpy as np
from scipy import linalg


class VARForecaster:
    def __init__(self):
        self.history = []
        
    def forecast_euribor(self, model):
        """VAR forecast for EURIBOR rate"""
        if len(self.history) < 2:
            self.history.append(0.01)
            return 0.01
            
        rate_data = np.array(self.history[-20:])
        
        # Simple AR(1) model
        if len(rate_data) > 1:
            X = rate_data[:-1].reshape(-1, 1)
            X = np.hstack([np.ones((len(X), 1)), X])
            y = rate_data[1:]
            
            try:
                beta = linalg.lstsq(X, y)[0]
                forecast = beta[0] + beta[1] * rate_data[-1]
                forecast += np.random.normal(0, 0.001)
                self.history.append(forecast)
                return max(0, forecast)
            except:
                return rate_data[-1]
        
        return 0.01
    
    def forecast_var(self, data, exog=None):
        """General VAR forecasting"""
        if len(data) < 2:
            return data[-1] if len(data) > 0 else 0.0
            
        X = np.array(data[:-1]).reshape(-1, 1)
        
        if exog is not None:
            X = np.hstack([X, exog[:-1]])
            
        X = np.hstack([np.ones((len(X), 1)), X])
        y = np.array(data[1:])
        
        try:
            beta = linalg.lstsq(X, y)[0]
            last_x = np.array([1, data[-1]])
            
            if exog is not None:
                last_x = np.hstack([last_x, exog[-1]])
                
            forecast = last_x @ beta
            sigma = np.std(y - X @ beta)
            forecast += np.random.normal(0, sigma)
            
            return forecast
        except:
            return data[-1]
    
    def forecast_varx(self, endog_data, exog_data):
        """VARX with exogenous variables"""
        return self.forecast_var(endog_data, exog_data)
