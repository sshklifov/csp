#include <cstring>
#include <cstdio>
#include <algorithm>
#include <random>

struct Node
{
    Node* leftChld;
    Node* rightSibl;
    int val;
};

int* AssignPrefix(Node* root, int* values)
{
    if (root == NULL) return values;

    root->val = *values++;

    Node* it = root->leftChld;
    while (it != NULL)
    {
        values = AssignPrefix(it, values);
        it = it->rightSibl;
    }
    return values;
}

bool CheckAssignment(Node* root, int* used)
{
    Node* it = root->leftChld;
    while (it != NULL)
    {
        int dif = abs(root->val - it->val);
        if (used[dif]) return false;
        used[dif] = true;

        if (!CheckAssignment(it, used)) return false;

        it = it->rightSibl;
    }

    return true;
}

Node* RandomTree(std::default_random_engine& gen, int maxDepth = 3)
{
    if (maxDepth == 0) return NULL;

    Node* root = new Node{NULL, NULL, 0};
    if (maxDepth == 1) return root;

    int nchld = 1 + std::geometric_distribution<int>(0.1f)(gen) % 3;
    Node** tail = &root->leftChld;
    while (nchld --> 0)
    {
        Node* chld = RandomTree(gen, maxDepth-1);
        *tail = chld;
        tail = &chld->rightSibl;
    }
    *tail = NULL;

    return root;
}

void PrintTree(Node* root)
{
    if (root == NULL) return;

    Node* it = root->leftChld;
    while (it != NULL)
    {
        PrintTree(it);
        printf("%d %d\n", root->val, it->val);
        it = it->rightSibl;
    }
}

int CountNodes(Node* root)
{
    if (root == NULL) return 0;

    int sum = 1;
    root = root->leftChld;
    while (root != NULL)
    {
        sum += CountNodes(root);
        root = root->rightSibl;
    }

    return sum;
}

int main()
{
    int seed = time(NULL);
    /* seed = 1577222795; */
    /* seed = 1577222908; */
    /* seed = 1577223197; */
    /* seed = 1577223294; */
    seed = 1577223384;
    /* seed = 1577223463; */
    /* seed = 1577223560; */

    printf("seed: %d\n", seed);
    std::default_random_engine eng(seed);

    Node* root = RandomTree(eng, 4);
    int n = CountNodes(root);

    if (n > 11)
    {
        printf("rip!\n");
        return 0;
    }
    /* Node node9{NULL, NULL, 9}; */
    /* Node node8{NULL, NULL, 8}; */
    /* Node node7{NULL, &node8, 7}; */
    /* Node node6{NULL, NULL, 6}; */
    /* Node node5{NULL, &node6, 5}; */
    /* Node node4{&node9, NULL, 4}; */
    /* Node node3{&node7, &node4, 3}; */
    /* Node node2{&node5, &node3, 2}; */
    /* Node node1{&node2, NULL, 1}; */

    std::vector<int> perms (n);
    for (int i = 0; i < (int)perms.size(); ++i)
    {
        perms[i] = 2*i + 1;
    }
    std::vector<int> used(perms[n-1]);

    bool toggle = 0;
    do
    {
        /* AssignPrefix(root, perms.data()); */
        /* PrintTree(root); */
        /* exit(1); */

        if (toggle)
        {
            if (perms[0] != 2*n-1) continue;
        }

        AssignPrefix(root, perms.data());
        used.assign(used.size(), 0);
        if(CheckAssignment(root, used.data()))
        {
            printf("using assignment: {");
            for (int i = 0; i < n-1; ++i)
            {
                printf("%d, ", perms[i]);
            }
            printf("%d}\n", perms[n-1]);

            if (toggle) exit(0);
            PrintTree(root);
            /* if (!toggle) PrintTree(root); */
            /* exit(0); */
        }
    }
    while (std::next_permutation(perms.begin(), perms.end()));

    /* printf("no feasable solution\n"); */
    exit(1);
}
