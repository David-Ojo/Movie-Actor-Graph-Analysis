#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <set>
#include <algorithm>
#include <queue>
#include <unordered_set>
using namespace std;
string decode_unicode(const string& input) {
string decoded;
size_t pos = 0;
string remaining = input;
while ((pos = remaining.find("\\u", pos)) != string::npos) {
decoded += remaining.substr(0, pos); // Append text before the Unicode sequence
if (pos + 6 <= remaining.size()) {
string hex_code = remaining.substr(pos + 2, 4);
char32_t code_point = static_cast<char32_t>(stoi(hex_code, nullptr, 16));
if (code_point <= 0x7F) {
decoded += static_cast<char>(code_point);
} else if (code_point <= 0x7FF) {
decoded += static_cast<char>((code_point >> 6) | 0xC0);
decoded += static_cast<char>((code_point & 0x3F) | 0x80);
} else if (code_point <= 0xFFFF) {
decoded += static_cast<char>((code_point >> 12) | 0xE0);
decoded += static_cast<char>(((code_point >> 6) & 0x3F) | 0x80);
decoded += static_cast<char>((code_point & 0x3F) | 0x80);
}
pos += 6;
} else {
cerr << "Warning: Malformed Unicode sequence in input: " << remaining << endl;
break;
}
remaining = remaining.substr(pos);
pos = 0;
}
decoded += remaining; // Append remaining text
return decoded;
}
vector<string> split_line(const string& line, char delimiter) {
vector<string> columns;
stringstream ss(line);
string column;
bool in_quotes = false;
for (char c : line) {
if (c == '"' && (column.empty() || column.back() != '\\')) {
in_quotes = !in_quotes;
} else if (c == delimiter && !in_quotes) {
columns.push_back(column);
column.clear();
} else {
column += c;
}
}
if (!column.empty()) {
columns.push_back(column);
}
return columns;
}
vector<string> extract_names_from_cast(const string& cast_column) {
vector<string> names;
stringstream ss(cast_column);
string part;
while (getline(ss, part, ',')) {
size_t name_pos = part.find("name:");
if (name_pos != string::npos) {
size_t name_start = part.find_first_not_of(" ", name_pos + 5);
size_t name_end = part.find_first_of(",}", name_start);
if (name_end == string::npos) name_end = part.size();
string raw_name = part.substr(name_start, name_end - name_start);
if (!raw_name.empty() && raw_name.front() == '\"') raw_name.erase(0, 1);
if (!raw_name.empty() && raw_name.back() == '\"') raw_name.pop_back();
if (raw_name.find("\\u") != string::npos) {
raw_name = decode_unicode(raw_name);
}
names.push_back(raw_name);
}
}
return names;
}
// adjacency list
void build_adjacency_list(const unordered_map<string, vector<string>>& movie_actors,
unordered_map<string, set<string>>& adjacency_list) {
for (const auto& [movie_id, actors] : movie_actors) {
for (size_t i = 0; i < actors.size(); ++i) {
for (size_t j = i + 1; j < actors.size(); ++j) {
adjacency_list[actors[i]].insert(actors[j]);
adjacency_list[actors[j]].insert(actors[i]);
}
}
}
}
void find_top_5_actors(const unordered_map<string, set<string>>& adjacency_list) {
vector<pair<string, int>> degree_centrality;
// Calculate degree centrality
for (const auto& [actor, co_actors] : adjacency_list) {
degree_centrality.emplace_back(actor, co_actors.size());
}
// Sort
sort(degree_centrality.begin(), degree_centrality.end(),
[](const pair<string, int>& a, const pair<string, int>& b) {
return b.second < a.second; // Sort by second element (degree) descending
});
// Display the top 5
cout << "Top 5 actors by degree centrality:" << endl;
for (size_t i = 0; i < 5 && i < degree_centrality.size(); ++i) {
cout << degree_centrality[i].first << " - Degree: " << degree_centrality[i].second << endl;
}
}
// BFS and mark all connected nodes
void bfs(const string& start_node, const unordered_map<string, set<string>>&
adjacency_list,
unordered_set<string>& visited) {
queue<string> to_visit;
to_visit.push(start_node);
visited.insert(start_node);
while (!to_visit.empty()) {
string current = to_visit.front();
to_visit.pop();
for (const auto& neighbor : adjacency_list.at(current)) {
if (visited.find(neighbor) == visited.end()) {
visited.insert(neighbor);
to_visit.push(neighbor);
}
}
}
}
//determine if the graph is connected and count connected components
void check_graph_connectivity(const unordered_map<string, set<string>>& adjacency_list) {
unordered_set<string> visited;
int connected_components = 0;
for (const auto& [actor, _] : adjacency_list) {
if (visited.find(actor) == visited.end()) {
// Start a new BFS
++connected_components;
bfs(actor, adjacency_list, visited);
}
}
if (connected_components == 1) {
cout << "The graph is connected." << endl;
} else {
cout << "The graph is not connected." << endl;
cout << "Number of connected components: " << connected_components << endl;
}
}
int shortest_degree_of_separation(const string& actor_a, const string& actor_b,
const unordered_map<string, set<string>>& adjacency_list) {
// Check if both actors exist
if (adjacency_list.find(actor_a) == adjacency_list.end()) {
cerr << "Error: Actor \"" << actor_a << "\" not found in the graph." << endl;
return -1;
}
if (adjacency_list.find(actor_b) == adjacency_list.end()) {
cerr << "Error: Actor \"" << actor_b << "\" not found in the graph." << endl;
return -1;
}
if (actor_a == actor_b) return 0;
unordered_set<string> visited;
queue<pair<string, int>> to_visit; // Pair of actor and current distance
to_visit.push({actor_a, 0});
visited.insert(actor_a);
while (!to_visit.empty()) {
auto [current_actor, distance] = to_visit.front();
to_visit.pop();
for (const auto& neighbor : adjacency_list.at(current_actor)) {
if (neighbor == actor_b) return distance + 1;
if (visited.find(neighbor) == visited.end()) {
visited.insert(neighbor);
to_visit.push({neighbor, distance + 1});
}
}
}
return -1; // Actors are in different connected components
}
// find the shortest path between two actors
vector<string> shortest_actor_chain(const string& actor_a, const string& actor_b,
const unordered_map<string, set<string>>& adjacency_list) {
// Check if both actors exist
if (adjacency_list.find(actor_a) == adjacency_list.end()) {
cerr << "Error: Actor \"" << actor_a << "\" not found in the graph." << endl;
return {};
}
if (adjacency_list.find(actor_b) == adjacency_list.end()) {
cerr << "Error: Actor \"" << actor_b << "\" not found in the graph." << endl;
return {};
}
if (actor_a == actor_b) return {actor_a};
unordered_map<string, string> predecessors;
unordered_set<string> visited;
queue<string> to_visit;
to_visit.push(actor_a);
visited.insert(actor_a);
while (!to_visit.empty()) {
string current_actor = to_visit.front();
to_visit.pop();
// Check if the current actor has neighbors
if (adjacency_list.find(current_actor) == adjacency_list.end()) {
cerr << "Warning: Actor \"" << current_actor << "\" has no connections." << endl;
continue;
}
for (const auto& neighbor : adjacency_list.at(current_actor)) {
if (visited.find(neighbor) == visited.end()) {
visited.insert(neighbor);
predecessors[neighbor] = current_actor;
if (neighbor == actor_b) {
vector<string> path;
string step = actor_b;
while (step != actor_a) {
path.push_back(step);
step = predecessors[step];
}
path.push_back(actor_a);
reverse(path.begin(), path.end());
return path;
}
to_visit.push(neighbor);
}
}
}
return {}; // No path
}
int main() {
string file_path = "tmdb_5000_credits.csv";
ifstream file(file_path);
if (!file.is_open()) {
cerr << "Error: Could not open file " << file_path << endl;
return 1;
}
unordered_map<string, vector<string>> movie_actors;
unordered_map<string, set<string>> adjacency_list;
string line;
bool is_header = true;
while (getline(file, line)) {
if (is_header) {
is_header = false;
continue;
}
vector<string> columns = split_line(line, ',');
if (columns.size() >= 3) {
string movie_id = columns[0];
string cast_column = columns[2];
vector<string> actors = extract_names_from_cast(cast_column);
movie_actors[movie_id] = actors;
}
}
file.close();
build_adjacency_list(movie_actors, adjacency_list);
cout << "Adjacency List:" << endl;
for (const auto& [actor, co_actors] : adjacency_list) {
cout << actor << ": ";
for (const auto& co_actor : co_actors) {
cout << co_actor << ", ";
}
cout << endl;
}
find_top_5_actors(adjacency_list);
check_graph_connectivity(adjacency_list);
cout << "\nShortest Degree of Separation Examples:" << endl;
vector<pair<string, string>> test_pairs = {
{"Samuel L. Jackson", "Morgan Freeman"},
{"Stan Lee", "Anne Fletcher"},
{"Nonexistent Actor", "Robert De Niro"} // Test case for missing actor
};
for (const auto& [actor_a, actor_b] : test_pairs) {
int distance = shortest_degree_of_separation(actor_a, actor_b, adjacency_list);
if (distance != -1) {
cout << "Degree of separation between " << actor_a << " and " << actor_b << ": " <<
distance << endl;
} else {
cout << "Could not determine degree of separation between " << actor_a << " and " <<
actor_b << "." << endl;
}
}
for (const auto& [actor_a, actor_b] : test_pairs) {
vector<string> path = shortest_actor_chain(actor_a, actor_b, adjacency_list);
if (!path.empty()) {
cout << "Shortest path between " << actor_a << " and " << actor_b << ": ";
for (size_t i = 0; i < path.size(); ++i) {
cout << path[i];
if (i < path.size() - 1) cout << " -> ";
}
cout << endl;
} else { cout << actor_a << " and " << actor_b <<"are in different connected components or not in the graph." << endl;
}
}
return 0;
}