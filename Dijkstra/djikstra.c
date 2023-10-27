#include <stdio.h>
#include <stdlib.h>
#include "../graph.hpp"

int main() {

    Graph *g = graph_from_file("./graph.txt");

    djikstra(g, 'A', 'H');

    destroy_graph(g);

    return 0;

}