import sys


def output_cnf(clauses, W, H):
    num_var = 0
    num_cla = len(clauses)
    for clause in clauses:
        for lit in clause:
            var = abs(lit)
            num_var = max(num_var, var)
    file_name = "%d_%d_n_queens_problem.cnf" % (W, H)
    print (num_var, num_cla)
    with open(file_name, "w") as f:
        f.write("p cnf %d %d\n" % (num_var, num_cla))
        for clause in clauses:
            clause.append(0)
            line = " ".join(map(str, clause))
            f.write(line + "\n")

        
def get_constraint(W, H):
    clauses = []
    for x in range(W):
        clause = []
        for y in range(H):
            p_lit = (y * H + x) + 1
            clause.append(p_lit)
        clauses.append(clause)
        
    for y in range(H):
        for x in range(W):
            p_lit = (y * H + x) + 1
            neg_p_lit = -p_lit
            #vertical
            for k in range(y + 1, H):
                q_lit = k * H + x + 1
                neg_q_lit = -q_lit
                clause = []
                clause.append(neg_p_lit)
                clause.append(neg_q_lit)
                clauses.append(clause)
            #horizon
            for k in range(x + 1, W):
                q_lit = (y * H + k) + 1
                neg_q_lit = -q_lit
                clause = []
                clause.append(neg_p_lit)
                clause.append(neg_q_lit)
                clauses.append(clause)

            #lower right diagonal
            d = 1
            while (True):
                clause = []
                if (y + d >= H or x + d >= W):
                    break
                ny = y + d
                nx = x + d
                q_lit = (ny * H + nx) + 1
                neg_q_lit = -q_lit
                clause.append(neg_p_lit)
                clause.append(neg_q_lit)
                clauses.append(clause)
                d += 1


            #lower left diagonal
            d = 1        
            while (True):
                clause = []
                if (y + d >= H or x - d < 0):
                    break
                ny = y + d
                nx = x - d
                q_lit = (ny * H + nx) + 1
                neg_q_lit = -q_lit
                clause.append(neg_p_lit)
                clause.append(neg_q_lit)
                clauses.append(clause)
                d += 1
    return clauses
        
if __name__ == "__main__":
    params = sys.argv
    if (len(params) != 3):
        print ("Usage: you should add \"(width) (height)\".")
        exit(2)
    W = int(params[1])
    H = int(params[2])
    clauses = get_constraint(W, H)
    output_cnf(clauses, W, H)
