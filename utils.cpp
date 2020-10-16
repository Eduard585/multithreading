#include "utils.h"
#define INF INT32_MAX
void utils::createFileAdjacencyMatrix(string filename, int nodesSize, int minBorder, int maxBorder) {
	ofstream file;
	random_device randomDevice;
	mt19937 generator(randomDevice());
	uniform_int_distribution<int> distr(minBorder, maxBorder);
	uniform_int_distribution<int> probability(0, 1);

	vector<vector<int>> matrix(nodesSize);
	for (auto& intra : matrix)
		intra.resize(nodesSize);

	for (int i = 0; i < nodesSize; ++i) {
		for (int j = i; j < nodesSize; ++j) {
			if (j == i) {
				matrix[i][j] = 0;
				continue;
			}
			auto prob = probability(generator);
			auto path_len = INF;
			if (prob) {
				path_len = distr(generator);
			}
			matrix[i][j] = matrix[j][i] = path_len;
		}
	}

	file.open(filename);
	if (file) {
		file << nodesSize << endl;
		for (const auto& inner : matrix) {
			for (auto val : inner) {
				if (val == INF) {
					file << "INF ";
					continue;
				}
				file << val << " ";
			}
			file << endl;
		}
		file.close();
	}
}

void utils::loadMatrix(const string filename, graph& graph) {
	long size;
	string temp;
	ifstream file(filename);
	vector <int> matrix;
	if (!file) {
		cerr << "Error opening file: " << filename << ".\n";
		throw runtime_error("Could not open the file");
	}
	file >> size;
	graph.resize(size);
	matrix.resize(size * size);
	for (long i = 0; i < size; i++) {
		for (long j = 0; j < size; j++) {
			int index = i * size + j;
			file >> temp;
			if ("INF" == temp) {
				matrix[index] = INF;
			}
			else {
				matrix[index] = stoi(temp);
			}
		}
	}
	for (long i = 0; i < size; i++) {
		for (long j = 0; j < size; j++) {
			long index = i * size + j;
			if (matrix[index] != INF) {
				graph[i].push_back(make_pair(j, matrix[index]));
			}
		}
	}
}

void utils::printAnswer(int root, const string& outFile, const vector<int>& distances) {
	ofstream fout(outFile, ios_base::trunc);
	fout << "From the node " << root << endl;
	for (long i = 0; i < distances.size(); i++) {
		auto dist = distances[i] == INF ? "INF" : to_string(distances[i]);
		fout << "[" << i << "] = " << dist << endl;
	}
}

void utils::printVector(const vector<int>& distances) {
	for (long i = 0; i < distances.size(); i++) {
		auto dist = distances[i] == INF ? "INF" : to_string(distances[i]);
		cout << "[" << i << "] - " << dist << endl;
	}
}

vector<int> utils::dijkstraStatic(graph graph, long startNode, long chunkSize) {

	long j, path_len, len, dest, graph_size, shortest;
	long nodes = graph.size();

	vector<int> distances(nodes, INF);
	vector<char> visited(nodes);
	vector<int> thread_min(omp_get_max_threads(), INF);
	vector<long> thread_id(omp_get_max_threads(), nodes + 1);
	distances[startNode] = 0;
	if (chunkSize == -1) {
		chunkSize = nodes / omp_get_max_threads();
	}

	for (long i = 0; i < nodes; ++i) {
		fill(thread_min.begin(), thread_min.end(), INF);
		fill(thread_id.begin(), thread_id.end(), nodes + 1);
#pragma omp parallel for schedule(static, chunkSize) firstprivate(nodes) private(shortest, j)
		for (j = 0; j < nodes; ++j) {
			shortest = thread_id[omp_get_thread_num()];
			if (!visited[j] && (shortest == nodes + 1
				|| distances[j] < distances[shortest])) {
				thread_id[omp_get_thread_num()] = j;
				thread_min[omp_get_thread_num()] = distances[j];
			}
		}

		shortest = thread_id[std::min_element(thread_min.begin(), thread_min.end()) - thread_min.begin()];

		if (distances[shortest] == INF)
			break;
		visited[shortest] = true;

		graph_size = graph[shortest].size();
		if (chunkSize == -1) {
			chunkSize = graph_size / omp_get_max_threads();
		}

#pragma omp parallel for schedule(static, chunkSize) firstprivate(shortest, graph_size) private(dest, len, path_len, j) 
		for (j = 0; j < graph_size; ++j) {
			dest = graph[shortest][j].first;
			len = graph[shortest][j].second;
			path_len = distances[shortest] + len;
			if (path_len < distances[dest]) {
				distances[dest] = path_len;
			}
		}
	}
	return distances;
}
vector<int> utils::dijkstraDynamic(graph graph, long startNode, long chunkSize) {

	long j, path_len, len, dest, graph_size, shortest;
	long nodes = graph.size();

	vector<int> distances(nodes, INF);
	vector<char> visited(nodes);
	vector<int> thread_min(omp_get_max_threads(), INF);
	vector<long> thread_id(omp_get_max_threads(), nodes + 1);
	distances[startNode] = 0;
	if (chunkSize == -1) {
		chunkSize = nodes / omp_get_max_threads();
	}

	for (int i = 0; i < nodes; ++i) {
		fill(thread_min.begin(), thread_min.end(), INF);
		fill(thread_id.begin(), thread_id.end(), nodes + 1);
#pragma omp parallel for schedule(dynamic, chunkSize) firstprivate(nodes) private(shortest, j)
		for (j = 0; j < nodes; ++j) {
			shortest = thread_id[omp_get_thread_num()];
			if (!visited[j] && (shortest == nodes + 1
				|| distances[j] < distances[shortest])) {
				thread_id[omp_get_thread_num()] = j;
				thread_min[omp_get_thread_num()] = distances[j];
			}
		}

		shortest = thread_id[min_element(thread_min.begin(), thread_min.end()) - thread_min.begin()];

		if (distances[shortest] == INF)
			break;
		visited[shortest] = true;

		graph_size = graph[shortest].size();
		if (chunkSize == -1) {
			chunkSize = graph_size / omp_get_max_threads();
		}

#pragma omp parallel for schedule(dynamic, chunkSize) firstprivate(shortest, graph_size) private(dest, len, path_len, j) 
		for (j = 0; j < graph_size; ++j) {
			dest = graph[shortest][j].first;
			len = graph[shortest][j].second;
			path_len = distances[shortest] + len;
			if (path_len < distances[dest]) {
				distances[dest] = path_len;
			}
		}
	}
	return distances;
}

struct node {
	int distance;
	int vertex;
};

vector<int> utils::dijkstraReduction(graph graph, int startNode, int nodes) {

	int j, path_len, len;
	int dest, graph_size;
	vector<int> distances(nodes, INF);
	vector<int> visited(nodes, 0);
	distances[startNode] = 0;

#pragma omp declare reduction(min : node: \
	omp_out.distance = omp_in.distance > omp_out.distance ? omp_out.distance:omp_in.distance,\
	omp_out.vertex   = omp_in.distance > omp_out.distance ? omp_out.vertex:omp_in.vertex) \
	initializer(omp_priv={INF,0})

	for (int i = 0; i < nodes; i++) {

		node shortest = { INF, startNode };
#pragma omp parallel for firstprivate(nodes) private(j) reduction(min : shortest)
		for (j = 0; j < nodes; ++j) {
			if (!visited[j] && (shortest.vertex == nodes + 1
				|| distances[j] < distances[shortest.vertex])) {
				shortest.vertex = j;
				shortest.distance = distances[j];
			}
		}

		if (distances[shortest.vertex] == INF)
			break;
		visited[shortest.vertex] = true;

		graph_size = graph[shortest.vertex].size();

#pragma omp parallel for firstprivate(shortest, graph_size) private(dest, len, path_len, j) 
		for (j = 0; j < graph_size; ++j) {
			dest = graph[shortest.vertex][j].first;
			len = graph[shortest.vertex][j].second;
			path_len = distances[shortest.vertex] + len;
			if (path_len < distances[dest]) {
				distances[dest] = path_len;
			}
		}
	}
	return distances;
}

void utils::writeLogTime(string& str) {
	ofstream filelog("log.txt", ios::out | ios::app);
	filelog << str;
	filelog.close();
}
