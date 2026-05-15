# Robot Optimisation Analysis

## KPI Comparison: Baseline vs Optimised (52 weeks)

| Run | Units | Weight | Distance | Energy | Damage |
|-----|-------|--------|----------|--------|--------|
| Baseline | 941 | 14009 | 69674 | 86278 | 21 |
| Optimised | 3032 | 45616 | 139385 | 141928 | 0 |

## Key Improvements
- Units delivered improved by 3.2x
- Weight delivered improved by 3.3x
- Distance travelled increased by 1.2x
- Damage reduced from 21 to 0

## Why the improvements occurred

### Charging
- Charging was optimized as different classes of robots had different thresholds depending on their charge and weight capacity as well as the volitant variable, allowing different classes to charge whenever they were feeling low and minimizing damage. a
- Opportunistic charging was implemented where before collecting pizzas, bots could check if they were close to their charging threshold (10% of their charging threshold) and charge on the way to collecting pizza.

### Pizza Allocation
- Despite increased pizza weights, pizza weight thresholds were established for each robot and calculated assigning robots to only carry within specific tolerances, so different robots were more suited for different weights of pizza thanks to the select_optimized_pizza function.