#include <iostream>
#include <vector>
#include <omp.h>
#include <windows.h>
#include <string>
#include <fstream>
#include <random>
#include <climits>

#define INF INT32_MAX

using namespace std;
using graph = vector<vector<pair<int, int>>>;

void loadMatrix(const string filename, graph& graph);
void createFileAdjacencyMatrix(string filename, int nodesSize, int minBorder, int maxBorder);
void printAnswer(int root, const string& outFile, const vector<int>& distances);
void writeLogTime(string& str);

int main(int argc, char* argv[])
{
    if (argc == 5) {
        createFileAdjacencyMatrix(argv[1], atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));
    }
    else if (argc < 6) {
        cout << "Not all arguments specified!" << endl;
        return (10);
    }
    else {
        graph graph;
        vector<int> distances;
        char buf[1000];
        int res = -1;

        string matrixFile = argv[1];
        int startNode = atol(argv[2]);
        string resultFile = argv[3];
        int threadNum = atoi(argv[4]);
        string mode = argv[5];
        if (mode != "static") {
            mode = "reduction";
        }
        int chunkSize = argv[6] != nullptr ? atoi(argv[6]) : -1;
        omp_set_dynamic(0);
        omp_set_num_threads(threadNum);

        loadMatrix(matrixFile, graph);
        ULONGLONG startTime = GetTickCount(), timeMultiply;
        
        distances = dijkstraStatic(graph, startNode, chunkSize);
        
        timeMultiply = GetTickCount() - startTime;
        res = snprintf(buf, sizeof(buf), "%d threads: %s MODE: %Id size, spent %Id milliseconds\n", threadNum, mode.c_str(), distances.size(), timeMultiply);
        string str = "error!";
        if (res >= 0 && res < sizeof(buf))
            str = buf;
        cout << str << std::endl;
        writeLogTime(str);
        printAnswer(startNode, resultFile, distances);
    }
    return 0;
}

void createFileAdjacencyMatrix(string filename, int nodesSize, int minBorder, int maxBorder) {
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
        file << nodesSize << std::endl;
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

void loadMatrix(const string filename, graph& graph) {
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

vector<int> dijkstraStatic(graph graph, long startNode, long chunkSize) {

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

void writeLogTime(string& str) {
    ofstream filelog("Log.txt", ios::out | ios::app);
    filelog << str;
    filelog.close();
}

void printAnswer(int root, const string& outFile, const vector<int>& distances) {
    ofstream fout(outFile, ios_base::trunc);
    fout << "From the node " << root << std::endl;
    for (long i = 0; i < distances.size(); i++) {
        auto dist = distances[i] == INF ? "INF" : std::to_string(distances[i]);
        fout << "[" << i << "] = " << dist << std::endl;
    }
}
