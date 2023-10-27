#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define true 1;
#define false 0;

struct vertex;

typedef struct edge {
    int weight;
    struct vertex *end;
    struct edge *next;
} Edge;

typedef struct {
    Edge *head;
    Edge *tail;
} Edges;

typedef struct vertex {
    char label;
    int grade;
    struct vertex *next;
    Edges *edges;
} Vertex;

typedef struct {
    Vertex *head;
    Vertex *tail;
    int n_vertices;
    int n_edges;
    int **adjacency_table;
} Graph;

Graph *init_graph() {
    Graph *g = (Graph *) malloc(sizeof(Graph));
    g -> head = g -> tail = NULL;
    g -> n_vertices = g -> n_edges = 0;
    g -> adjacency_table = NULL;

    return g;
}

void add_vertex(Graph *g, char label) {
    Vertex *v = (Vertex *) malloc(sizeof(Vertex));
    v -> label = label;
    v -> grade = 0;
    v -> edges = (Edges *) malloc(sizeof(Edges));
    v -> edges -> head = NULL;
    v -> edges -> tail = NULL;
    v -> next = NULL;

    if (g -> tail == NULL) {
        g -> head = v;
        g -> tail = v;
    }
    else {
        g -> tail -> next = v;
        g -> tail = v;
    }
    (g -> n_vertices)++;
}

void add_edge(Graph *g, int weight, char start, char end) {
    if (weight == 0) return;
    Vertex *v_start = g -> head;
    Vertex *v_end = g -> head;
    while (v_start != NULL | v_end != NULL) {
        if (v_start -> label != start) v_start = v_start -> next;
        if (v_end -> label != end) v_end = v_end -> next;
        if (v_start -> label == start & v_end -> label == end) break;
    }

    Edge *e = (Edge *) malloc(sizeof(Edge));
    e -> weight = weight;
    e -> next = NULL;
    e -> end = v_end;

    Edges *es = v_start -> edges;
    if (es -> head == NULL) es -> head = e;
    else es -> tail -> next = e;
    es -> tail = e;

    if (start == end) (v_start -> grade) += 2;
    else (v_start -> grade)++;
    (g -> n_edges)++;
}

void show_adjacency_table(Graph *g) {
    Vertex *v = g -> head;
    Edge *e;
    printf("=====Edges=====\n");
    while (v != NULL) {
        printf("Vertex %c, grade: %i, edges:", v -> label, v -> grade);
        e = v -> edges -> head;
        while (e != NULL) {
            printf(" %c", e -> end -> label);
            e = e -> next;
        }
        printf("\n");
        v = v -> next;
    }
}

Graph *graph_from_file(char *path) {
    FILE *file = fopen(path, "r");
    Graph *g = init_graph();
    char str[100], labels[100];

    fgets(str, 100, file);
    for (int i = 0, j = 0; i < strlen(str) - 1; i++) {
        if (str[i] == ' ') continue;
        labels[j] = str[i];
        add_vertex(g, str[i]);
        j++;
    }

    g -> adjacency_table = (int **) malloc(g -> n_vertices * sizeof(int *));

    for (int i = 0; i < strlen(labels); i++) {
        fgets(str, 100, file);
        g -> adjacency_table[i] = (int *) malloc(g -> n_vertices * sizeof(int));
        for (int j = 0, k = 0; j < strlen(str); j += 2) {
            int edge = str[j] - '0';
            g -> adjacency_table[i][k] = edge;
            add_edge(g, edge, labels[i], labels[k]);
            k++;   
        }
    }

    fclose(file);

    return g;
}

char *get_labels(Graph *g) {
    Vertex *v = g -> head;
    char *labels = (char *) malloc((g -> n_vertices + 1) * sizeof(char));
    labels[g -> n_vertices] = '\0';

    int i = 0;
    while (v != NULL) {
        labels[i] = v -> label;
        v = v -> next;
        i++;
    }

    return labels;
}

int is_adjacent(Vertex *v1, Vertex *v2, int *weight) {
    if (v1 == NULL || v2 == NULL) return 0;

    Edge *e = v1 -> edges -> head;

    while (e != NULL && e -> end != v2) {
        e = e -> next;
    }

    if (e != NULL) {
        *weight = e -> weight;
    }

    return e != NULL;
}

Vertex *get_vertex(Graph *g, char label) {
    Vertex *v = g -> head;

    while (v != NULL && v -> label != label) {
        v = v -> next;
    }

    return v;
}

void djikstra(Graph *g, char start, char end) {
    int n_vtx = g -> n_vertices;

    int **adj_table = g -> adjacency_table;
    int adj_y_idx;
    //label vector, that serves as axis to adjacency table and the path table
    char *labels = get_labels(g);
    //flag of each vector
    int *flags = (int *) calloc(n_vtx, sizeof(int));

    //the path vector with all max value, except the position of the start vertex
    int *paths = (int *) malloc(n_vtx * sizeof(int));
    for (int i = 0; i < n_vtx; i++) paths[i] = __INT_MAX__;
    paths[adj_y_idx] = 0;
    
    //index for limit how far the algorithm goes in the table
    int end_idx;
    //find the index above and the initial row of the adjacency table  
    for (int i = 0; i < n_vtx; i++) {
        if (labels[i] == start) adj_y_idx = i;
        if (labels[i] == end) end_idx = i;
    }

    //the vertex which results in the shortest path to each vertex
    char *prev_vertices = (char *) malloc((n_vtx + 1) * sizeof(char));
    for (int i = 0; i < n_vtx; i++) prev_vertices[i] = ' ';
    prev_vertices[n_vtx] = '\0';

    //initial conditions of the algorithm
    flags[adj_y_idx] = 1;
    prev_vertices[adj_y_idx] = ' ';
    char previous_label = start;
    int previous_weight = 0;

    while (1) {
        //loop to travel for each row of the path matrix
        for (int x_idx = 0; x_idx < n_vtx; x_idx++) {
            //if the djikstra flag of the vertex is active, it means that the previous vertex of the shortest path to it is already set
            if (flags[x_idx]) continue;

            //get the weight of the edge that connects the vertex and the vertex of the previous line
            int weight = adj_table[adj_y_idx][x_idx];
            //if the weight is 0, it means that the vertices doesn't have a connection, else it has a connection
            if (weight != 0) {
                int total_weight = previous_weight + weight;
                //if the weight of the edge actual edge added with the total weight until previous vertex is lower than the value right above, than this resultant value is set in the current line
                if (total_weight < paths[x_idx]) {
                    paths[x_idx] = total_weight;
                    prev_vertices[x_idx] = previous_label;
                }
            }
        }
        
        //find the minor value in the current line of the path matrix
        int minor_idx = 0;
        int minor_weight = __INT_MAX__;
        for (int i = 0; i < n_vtx; i++) {
            if (flags[i]) continue;
            if (paths[i] < minor_weight) {
                minor_idx = i;
                minor_weight = paths[i];
            }
        }

        //active the flag of the vertex in the position of the minor index
        flags[minor_idx] = 1;
        //the previous vertex for the next line will be the vertex in the position of the minor index
        previous_label = labels[minor_idx];
        previous_weight = paths[minor_idx];
        //in the adjacency table, goes to the vertex in the position of the minor index
        adj_y_idx = minor_idx;

        //if the flag of end vertex is active, the algorithm have reached it, ending the loop
        if (flags[end_idx]) break;
    }

    //all the code below reads the vector of previous vertex and, starting by the end vertex, get the vertex before, then set this vertex as the actual and get the vertex before it and so on
    char *minor_path = (char *) malloc((n_vtx + 1) * sizeof(char));
    minor_path[0] = end;
    minor_path[1] = '\0';
    char label = end;
    while (label != start) {
        for (int i = 0; i < n_vtx; i++) {
            if (labels[i] == label) {
                int tam = strlen(minor_path);
                minor_path[tam] = label = prev_vertices[i];
                minor_path[tam + 1] = '\0';
                break;
            }
        }
    }

    for (int i = strlen(minor_path) - 1; i > 0; i--) {
        printf("%c -> ", minor_path[i]);
    }
    printf("%c", minor_path[0]);
}

void destroy_edges(Edges *es) {
    Edge *del = es -> head;
    Edge *next = del -> next;

    while (es -> head != NULL) {
        es -> head = del -> next;
        free(del);
        del = next;
        if (next != NULL) next = next -> next;
    }
    es -> tail = NULL;
}

void destroy_graph(Graph *g) {
    Vertex *del = g -> head;
    Vertex *next = del -> next;

    while (g -> head != NULL) {
        g -> head = del -> next;
        destroy_edges(del -> edges);
        free(del);
        del = next;
        if (next != NULL) next = next -> next;
    }
    g -> tail = NULL;
    g = NULL;
}