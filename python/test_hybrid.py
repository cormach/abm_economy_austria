#!/usr/bin/env python3
"""
Quick test of hybrid model performance
"""

import time
import numpy as np
from model_hybrid import HybridEconomyModel


def test_single_run():
    """Test single model run"""
    print("Testing single run...")
    
    model = HybridEconomyModel(2013, 2, 42, 0)
    
    start = time.time()
    results = model.run(steps=13)
    elapsed = time.time() - start
    
    print(f"Single run: {elapsed:.3f}s")
    print(f"\nResults:\n{results.head()}")
    
    return elapsed


def test_multiple_runs(n_runs=10):
    """Test multiple runs"""
    print(f"\nTesting {n_runs} sequential runs...")
    
    start = time.time()
    for seed in range(n_runs):
        model = HybridEconomyModel(2013, 2, seed, 0)
        results = model.run(steps=13)
    elapsed = time.time() - start
    
    print(f"{n_runs} runs: {elapsed:.2f}s ({elapsed/n_runs:.3f}s per run)")
    
    return elapsed


def benchmark_vectorization():
    """Compare vectorized vs non-vectorized operations"""
    print("\nBenchmarking vectorization...")
    
    n = 620
    
    # Non-vectorized
    start = time.time()
    capital = [100.0] * n
    labor = [0.0] * n
    output = []
    for i in range(n):
        output.append(min(capital[i] * 0.5, labor[i] * 1.0))
    non_vec_time = time.time() - start
    
    # Vectorized
    start = time.time()
    capital = np.ones(n) * 100.0
    labor = np.zeros(n)
    output = np.minimum(capital * 0.5, labor * 1.0)
    vec_time = time.time() - start
    
    print(f"Non-vectorized: {non_vec_time*1000:.3f}ms")
    print(f"Vectorized: {vec_time*1000:.3f}ms")
    print(f"Speedup: {non_vec_time/vec_time:.1f}x")


if __name__ == "__main__":
    print("="*60)
    print("Hybrid Model Performance Test")
    print("="*60)
    
    # Test single run
    single_time = test_single_run()
    
    # Test multiple runs
    multi_time = test_multiple_runs(n_runs=10)
    
    # Benchmark vectorization
    benchmark_vectorization()
    
    print("\n" + "="*60)
    print("Summary")
    print("="*60)
    print(f"Single run: {single_time:.3f}s")
    print(f"Estimated time for 1000 runs: {single_time * 1000 / 60:.1f} minutes")
    print(f"With 8 cores parallel: {single_time * 1000 / 60 / 8:.1f} minutes")
