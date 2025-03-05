"""
MIT License

Copyright (c) 2025 Andrey Gumirov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
"""

import sys
from collections import deque
import itertools
from typing import List, Tuple
from random import randint, choices
from math import sqrt
from tqdm import tqdm
import traceback
import argparse

class TwoSAT:
    def __init__(self, n):
        self.n = n
        self.graph = [[] for _ in range(2 * n)]
        self.rev_graph = [[] for _ in range(2 * n)]
    
    def add_clause(self, a, b):
        self.graph[a^1].append(b)
        self.rev_graph[b].append(a^1)
        self.graph[b^1].append(a)
        self.rev_graph[a].append(b^1)
    
    def add_implication(self, a, b):
        self.add_clause(a^1, b)
    
    def solve(self):
        visited = [False] * (2 * self.n)
        order = []
        def dfs(u):
            if not visited[u]:
                visited[u] = True
                for v in self.graph[u]:
                    dfs(v)
                order.append(u)
        for u in range(2 * self.n):
            dfs(u)
        visited = [False] * (2 * self.n)
        component = [0] * (2 * self.n)
        current = 0
        for u in reversed(order):
            if not visited[u]:
                stack = [u]
                visited[u] = True
                component[u] = current
                while stack:
                    node = stack.pop()
                    for v in self.rev_graph[node]:
                        if not visited[v]:
                            visited[v] = True
                            component[v] = current
                            stack.append(v)
                current += 1
        assignment = []
        for i in range(self.n):
            if component[2*i] == component[2*i + 1]:
                return None
            assignment.append(component[2*i] > component[2*i + 1])
        return assignment

def dumb(n_sources: int, n_sinks: int,
         transfer_cost: int,
         sources: List[Tuple[int, int]],
         sinks: List[Tuple[int, int, List[int]]]) -> Tuple[int, List[int], List[int]]:
    # # Read input
    # n_sources, n_sinks = map(int, input().split())
    # transfer_cost = int(input())

    # sources = []
    # for _ in range(n_sources):
    #     t1, t2 = map(int, input().split())
    #     sources.append((t1, t2))

    # sinks = []
    # for _ in range(n_sinks):
    #     parts = list(map(int, input().split()))
    #     t1_sink = parts[0]
    #     t2_sink = parts[1]
    #     k = parts[2]
    #     predecessors = [x - 1 for x in parts[3:]]  # Convert to 0-based indices
    #     sinks.append((t1_sink, t2_sink, predecessors))

    # sinks = [(t1, t2, [e - 1 for e in pred]) for t1, t2, pred in _sinks]

    best_makespan = float('inf')
    best_source = []
    best_sink = []

    # Iterate all possible source platform combinations
    for source_platforms in itertools.product([0, 1], repeat=n_sources):
        # Calculate finish times for sources
        sources_finish = [sources[i][p] for i, p in enumerate(source_platforms)]
        current_max = max(sources_finish) if sources_finish else 0
        # Determine best platforms for sinks and their finish times
        sink_platforms = []
        sinks_finish = []
        for sink in sinks:
            t1, t2, pred_indices = sink
            best_finish = float('inf')
            best_p = 0
            for sink_p in [0, 1]:
                start_time = 0
                for pi in pred_indices:
                    source_p = source_platforms[pi]
                    t = sources_finish[pi]
                    if source_p != sink_p:
                        t += transfer_cost
                    if t > start_time:
                        start_time = t
                finish = start_time + (t1 if sink_p == 0 else t2)
                if finish < best_finish:
                    best_finish = finish
                    best_p = sink_p
            sink_platforms.append(best_p)
            sinks_finish.append(best_finish)
        # Compute overall makespan
        total_max = max(sources_finish + sinks_finish)
        if total_max < best_makespan:
            best_makespan = total_max
            best_source = source_platforms
            best_sink = sink_platforms
        elif total_max == best_makespan:
            # Optional: Check if this solution is lexicographically smaller if needed
            pass

    # # Output the results
    # for p in best_source:
    #     print(p + 1)
    # for p in best_sink:
    #     print(p + 1)
    
    return best_makespan, best_source, best_sink

def two_sat_solution(n_sources: int, n_sinks: int,
            c: int,
            source_times: List[Tuple[int, int]],
            sinks: List[Tuple[int, int, List[int]]]) -> Tuple[int, List[int], List[int]]:
    # Read sink tasks and their predecessors
    
    isolated = [True] * n_sources
    for _, _, preds in sinks:
        for p in preds:
            isolated[p] = False
    
    sink_times = [(t1, t2) for t1, t2, _ in sinks]
    predecessors = [preds for _, _, preds in sinks]
    
    isolated_sources = [i for i in range(n_sources) if isolated[i]]
    
    # print("n_sources, n_sinks, c", n_sources, n_sinks, c)
    # print("source_times", source_times)
    # print("sink_times, predecessors", sink_times, predecessors)
    # print("isolated_sources, isolated", isolated_sources, isolated)

    low = 0
    high = 0
    # Compute initial upper bound
    for t1, t2 in source_times:
        high = max(high, t1, t2)
    for t1, t2 in sink_times:
        high += max(t1, t2) + c
    original_high = high
    
    # print("upper bound", high)

    answer = None
    while low <= high:
        mid = (low + high) // 2
        feasible = True
        
        # Check isolated sources first
        for p in isolated_sources:
            t1, t2 = source_times[p]
            if t1 > mid and t2 > mid:
                feasible = False
                break
        if not feasible:
            low = mid + 1
            continue
        
        # Build 2-SAT instance
        total_vars = n_sources + n_sinks
        two_sat = TwoSAT(total_vars)
        
        # Enforce constraints for isolated sources
        for p in range(n_sources):
            if isolated[p]:
                t1, t2 = source_times[p]
                if t1 > mid:
                    two_sat.add_clause(2*p + 1, 2*p + 1)  # Must choose platform 2
                if t2 > mid:
                    two_sat.add_clause(2*p, 2*p)  # Must choose platform 1
        
        # Process each sink task
        for s in range(n_sinks):
            sink_t1, sink_t2 = sink_times[s]
            preds = predecessors[s]
            z_s = n_sources + s  # Sink variable
            
            # Check platform 1 for the sink
            valid_platform1 = True
            for p in preds:
                t_p1, t_p2 = source_times[p]
                max_p1 = mid - sink_t1
                max_p2 = mid - sink_t1 - c
                if t_p1 > max_p1 and t_p2 > max_p2:
                    valid_platform1 = False
                    break
                if t_p1 > max_p1:
                    two_sat.add_implication(2*z_s, 2*p + 1)  # If sink is 1, p must be 2
                elif t_p2 > max_p2:
                    two_sat.add_implication(2*z_s, 2*p)  # If sink is 1, p must be 1
            
            if not valid_platform1:
                two_sat.add_clause(2*z_s + 1, 2*z_s + 1)  # Exclude platform 1
            
            # Check platform 2 for the sink
            valid_platform2 = True
            for p in preds:
                t_p1, t_p2 = source_times[p]
                max_p1 = mid - sink_t2 - c
                max_p2 = mid - sink_t2
                if t_p1 > max_p1 and t_p2 > max_p2:
                    valid_platform2 = False
                    break
                if t_p1 > max_p1:
                    two_sat.add_implication(2*z_s + 1, 2*p + 1)  # If sink is 2, p must be 2
                elif t_p2 > max_p2:
                    two_sat.add_implication(2*z_s + 1, 2*p)  # If sink is 2, p must be 1
            
            if not valid_platform2:
                two_sat.add_clause(2*z_s, 2*z_s)  # Exclude platform 2
            
            if not valid_platform1 and not valid_platform2:
                feasible = False
                break
        
        if not feasible:
            low = mid + 1
            continue
        
        # Solve 2-SAT and check assignment
        assignment = two_sat.solve()
        # print("assignment", assignment)
        if assignment is not None:
            answer = mid
            high = mid - 1
        else:
            low = mid + 1
    
    # print(answer if answer is not None else -1)
    return answer, [], []

def solve(n_sources: int, n_sinks: int,
            c: int,
            source_times: List[Tuple[int, int]],
            sinks: List[Tuple[int, int, List[int]]]) -> int:
    
    da, _, _ = dumb(n_sources, n_sinks, c, source_times, sinks)
    nd, _, _ = two_sat_solution(n_sources, n_sinks, c, source_times, sinks)

    # print(da, nd)
    assert da == nd or (da == float('inf') and nd == None)

    return da

def gen_sample() -> Tuple[int, int, int, List[Tuple[int, int]], List[Tuple[int, int, List[int]]]]:
    
    N, S = randint(1, 10), randint(1, 20)

    sources = [(randint(1, 1_000_000), randint(1, 1_000_000)) for _ in range(N)]
    sinks = [(randint(1, 1_000_000), randint(1, 1_000_000), set(choices(list(range(N)), k=randint(1, N)))) for _ in range(S)]

    l = list(itertools.chain.from_iterable(sources + [e[:2] for e in sinks]))

    c = randint(1, int(std_dev(l)))

    return N, S, c, sources, sinks

def mean(a: List[int]) -> float:
    return sum(a) / len(a)

def std_dev(a: List[int]) -> float:
    m = mean(a)
    return sqrt(sum((e - m) ** 2 for e in a) / (len(a)))

def bruteforce():
    for _ in tqdm(range(1_000_000)):
        N, S, c, sources, sinks = gen_sample()

        try:
            solve(N, S, c, sources, sinks)
        except:
            print(f"Error: {traceback.format_exc()}: {N, S, c, sources, sinks}")
            print(f"{N} {S}\n{c}\n{'\n'.join([str(t1) + ' ' + str(t2) for t1, t2 in sources])}")
            print('\n'.join([f"{t1} {t2} {' '.join([str(e) for e in preds])}" for t1, t2, preds in sinks]))
            print('\n\n\nSOLUTION:', dumb(N, S, c, sources, sinks))
            break

def main():
    # Read number of sources and sinks
    n_sources, n_sinks = map(int, sys.stdin.readline().split())
    # Read transfer cost
    c = int(sys.stdin.readline())
    
    # Read source tasks' execution times
    source_times = []
    for _ in range(n_sources):
        t1, t2 = map(int, sys.stdin.readline().split())
        source_times.append((t1, t2))
    
    sink_times = []
    # predecessors = []
    for _ in range(n_sinks):
        parts = list(map(int, sys.stdin.readline().split()))
        t1, t2, k = parts[0], parts[1], parts[2]
        preds = [x-1 for x in parts[3:3+k]]  # Convert to 0-based indices
        
        sink_times.append((t1, t2, preds))
        # predecessors.append(preds)
    
    print(solve(n_sources, n_sinks, c, source_times, sink_times))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(usage="""
deepseek_solution_check.py [-h] (-s | -b)

Input format when using 'solve' mode:
<number of tasks 'N'> <number of dependencies 'S'>

After that S lines of:
<predecessor where predecessor is from 1 to N> <successor where successor is from 1 to N> <integer transfer cost from platform 1 to platform 2> <integer transfer cost from platform 2 to platform 1>

After that N lines of:
<integer time on platform 1> <integer time on platform 2>
""")
    gr = parser.add_mutually_exclusive_group(required=True)
    gr.add_argument("-s", "--solve", help="Solve task for the single input.", action="store_true")
    gr.add_argument("-b", "--bruteforce", help="Generate random inputs and check that both naive and polynomial solution outputs match.", action="store_true")

    args = parser.parse_args()
    if args.solve:
        main()
    elif args.bruteforce:
        bruteforce()
    else:
        raise RuntimeError("Unknown command!")
