import os
import sys
import linecache


def parse_model(file_name):
    status = linecache.getline(file_name, 1)
    status = status[:-1]
    model = linecache.getline(file_name, 2)
    model = list(map(int, model.split()[:-1]))
    return (status, model)

def parse_problem(problem_file, model):
    for line in open(problem_file, "r"):
        line = line.rstrip()
        if (len(line) > 0 and line[0] == 'p'):
            continue
        values = list(map(int, line.split()))
        values = values[:-1]

        satisfied = False
        for lit in values:

            var = abs(lit) - 1
            if (lit > 0):
                if (model[var] > 0):
                    satisfied = True
                    break
            else:
                if (model[var] < 0):
                    satisfied = True
                    break
        if (not satisfied):
            return "UNSAT"
        
    return "SAT"
if __name__ == "__main__":
    problem_file = sys.argv[1]
    model_file = sys.argv[2]

    status, model = parse_model(model_file)
    calc_status = parse_problem(problem_file,  model)
    problem_name = os.path.basename(problem_file)
    print (problem_name, status, calc_status, status == calc_status)
