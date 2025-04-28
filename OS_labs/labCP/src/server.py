dag = {
    "A": [],
    "B": ["A"],
    "C": ["A"],
    "D": ["B"],
    "E": ["D"],
}

def has_cucle(dag):
    visited = set()
    recursion_stack = set()

    def dfs(node):
        if node in recursion_stack:
            return True
        if node in visited:
            return False
        
        visited.add(node)
        recursion_stack.add(node)

        for next in dag.get(node, []):
            if dfs(next):
                return True
            
        recursion_stack.remove(node)
        return False
    
    for node in dag:
        if dfs(node):
            return True
    return False

if has_cucle(dag):
    print("Error: DAG have a cycle")
else:
    print("DAG is correct")