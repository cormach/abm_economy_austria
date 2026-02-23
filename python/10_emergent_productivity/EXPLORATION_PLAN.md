# Exploration Plan: Replicating Emergent Productivity

## Goal
Understand how the C code generates productivity = 1.93 as an **emergent property** rather than a parameter, and replicate this in Python.

---

## Hypothesis

The C code doesn't have a "productivity parameter". Instead, productivity emerges from:
1. **Initial sectoral output** (from I-O tables)
2. **Initial sectoral employment** (from data)
3. **Production function** (Cobb-Douglas or Leontief)
4. **I-O linkages** (intermediate demand)

The ratio `Output / Wages = 1.93` is a **result**, not an input.

---

## Exploration Steps

### Step 1: Find Initial Conditions in C Code

**Search for:**
```bash
grep -n "sqlimport_employees\|initial\|sector_output" mbu.c
```

**Questions:**
- Does C code load initial sectoral employment?
- Does C code load initial sectoral output?
- Are these from external data files or hard-coded?

**Expected finding:** C code imports sectoral data that already embeds the 1.93 ratio.

---

### Step 2: Analyze Production Function in C Code

**Search for:**
```bash
grep -n "Y_prev\|output\|production" mbu.c | grep -v "sqlimport"
```

**Questions:**
- Is there a Cobb-Douglas function: `Y = A × L^α × K^β`?
- Is there a Leontief function: `Y = min(L/a, K/b)`?
- Or is output simply: `Y = initial_output × growth_factor`?

**Expected finding:** Output is calculated from labor and capital, not a fixed multiplier.

---

### Step 3: Test Sectoral Productivity in Python

**Experiment:**
```python
# Load sectoral data
sector_output = load_sector_gva()  # 62 sectors
sector_employment = estimate_sector_employment()  # From compensation

# Calculate sectoral productivity
sector_prod = sector_output / sector_employment

# Assign firms to sectors
for firm in firms:
    sector = firm.sector
    firm.productivity = sector_prod[sector]

# Production
firm.output = firm.labor × firm.wage × firm.productivity
```

**Test:**
- Does aggregate output match target?
- Does productivity distribution match empirical?
- Is aggregate productivity = 1.93?

---

### Step 4: Compare Approaches

| Approach | Productivity | Pros | Cons |
|----------|-------------|------|------|
| **Single parameter** | 1.7 (calibrated) | Simple, fast | Misses sectoral variation |
| **Aggregate empirical** | 1.93 (fixed) | Matches aggregate | Still misses sectoral variation |
| **Sectoral empirical** | 0.01-12.67 (62 values) | Realistic, emergent | Complex, needs employment data |
| **C code (unknown)** | Emergent from structure | Most realistic | Need to reverse-engineer |

---

### Step 5: Reverse-Engineer C Code Logic

**Approach:**
1. Run C code with known inputs
2. Extract intermediate variables (Y_prev, labor, capital)
3. Calculate implied productivity: `prod = Y_prev / (wage × labor)`
4. Check if it varies by sector or time

**Tools:**
```bash
# Add debug prints to C code
printf("Sector %d: output=%.2f, labor=%.2f, prod=%.2f\n", 
       sector, Y_prev[i], labor[i], Y_prev[i]/labor[i]);
```

---

### Step 6: Implement Emergent Productivity in Python

**Option A: Use sectoral productivity directly**
```python
# Load empirical sectoral productivity
self.sector_productivity = load_from_data()  # 62 values

def production(self):
    for firm in firms:
        sector = firm.sector
        firm.output = firm.labor × firm.wage × self.sector_productivity[sector]
```

**Option B: Use production function with sectoral parameters**
```python
# Cobb-Douglas with sectoral TFP
self.sector_tfp = load_from_data()  # Total factor productivity
self.alpha = 0.65  # Labor share

def production(self):
    for firm in firms:
        sector = firm.sector
        firm.output = self.sector_tfp[sector] × (firm.labor ** self.alpha) × (firm.capital ** (1-self.alpha))
```

**Option C: Bootstrap from initial conditions**
```python
# Use initial sectoral output, let it evolve
self.initial_sector_output = load_from_data()

def initialize_firms(self):
    for sector in sectors:
        sector_firms = firms_in_sector(sector)
        output_per_firm = self.initial_sector_output[sector] / len(sector_firms)
        for firm in sector_firms:
            firm.output = output_per_firm
            # Productivity emerges from: output / (labor × wage)
```

---

### Step 7: Validate Emergent Productivity

**Tests:**
1. **Aggregate test:** Does `sum(output) / sum(wages) = 1.93`?
2. **Sectoral test:** Does each sector match empirical productivity?
3. **Time series test:** Does productivity stay stable over time?
4. **Calibration test:** Can we match GDP without calibrating productivity?

**Success criteria:**
- GDP error < 3% without calibrating productivity parameter
- Sectoral output distribution matches data
- Productivity emerges naturally from structure

---

## Expected Findings

### Finding 1: C Code Uses Initial Conditions
The C code likely loads:
- `sqlimport_sector_output[62]` - Initial output by sector
- `sqlimport_employees[62]` - Initial employment by sector

These already embed the 1.93 ratio. The C code doesn't "create" productivity, it **preserves** it from data.

### Finding 2: Production Function is Simple
Likely:
```c
Y[t] = Y[t-1] × (1 + growth_rate) × adjustment_factors
```

Not a complex Cobb-Douglas. Just evolving initial conditions with shocks.

### Finding 3: Sectoral Variation Matters
Using sectoral productivity (0.01-12.67) will:
- Generate more realistic output distribution
- Create sectoral dynamics (high-prod sectors grow faster)
- Match empirical patterns better

### Finding 4: My Model Can Replicate This
By using:
```python
self.sector_productivity = sector_output / (total_comp / 62)
firm.output = firm.labor × firm.wage × self.sector_productivity[firm.sector]
```

We get emergent aggregate productivity = 1.93 without calibration!

---

## Implementation Priority

### Phase 1: Quick Test (30 min)
- Load sectoral productivity from data
- Assign to firms by sector
- Run model, check if GDP matches

### Phase 2: C Code Analysis (2 hours)
- Search C code for production function
- Find initial condition loading
- Document how productivity emerges

### Phase 3: Full Implementation (1 hour)
- Implement sectoral productivity in model
- Remove productivity parameter from calibration
- Validate against targets

### Phase 4: Documentation (30 min)
- Document findings
- Compare single vs sectoral productivity
- Show emergent property proof

---

## Key Questions to Answer

1. **Does C code have a productivity parameter?**
   - Hypothesis: No, it uses initial sectoral data

2. **How does C code calculate output?**
   - Hypothesis: `Y = f(initial_Y, labor, capital, I-O linkages)`

3. **Why is aggregate productivity = 1.93?**
   - Hypothesis: Weighted average of sectoral productivities

4. **Can we replicate without calibration?**
   - Hypothesis: Yes, by using sectoral data directly

5. **Does sectoral productivity improve model accuracy?**
   - Hypothesis: Yes, reduces GDP error and matches distribution

---

## Success Metrics

- [ ] Identify production function in C code
- [ ] Extract sectoral productivity from data
- [ ] Implement sectoral productivity in Python
- [ ] Achieve GDP error < 3% without calibrating productivity
- [ ] Match sectoral output distribution
- [ ] Document emergent property mechanism

---

## Next Steps

1. **Run explore_productivity.py** ✓ (Done - found productivity = 1.93)
2. **Search C code for production function** (Next)
3. **Implement sectoral productivity** (After C code analysis)
4. **Test and validate** (Final step)
