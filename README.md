#   Movie-Actor-Graph-Analysis

Movie-Actor-Graph-Analysis takes in tmdb_5000_credits.csv and hits each of these milestones: 

Milestone 1: Construct an adjacency list where the vertices are actors. Two vertices are connected by an edge, if the actors co-acted in the same movie.

Milestone 2: Determine the top 5 actors that have acted with the most number of other actors (degree centrality). Include your list in the report. 

Milestone 3: Determine if the graph is connected. If not, determine how many connected components are present in the graph. Include this information in the report.

Milestone 4: Given any two actors, determine the shortest degree of separation between them. The distance is 0 if the two actors A, and B are co-actors. The distance is 1 if A and B are not co-actors, but A, C, and B, C are co-actors. Distances 2, 3 etc. are similarly defined.
Include at least 3 examples of different distances in the report. 

Milestone 5: Determine the actor chain (path) that constitutes the shortest between the two input actors. For example, input actors A, B, the shortest path could be A - C - D - E - B. If there are multiple such shortest paths, choose any one.
Include at least 3 examples of different paths in the report. 
