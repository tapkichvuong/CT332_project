#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <random>

#define ROW 8
#define COLUMN 8
#define STOP_CTR 28
#define MAX_ITER 16777216
#define MUTATE 0.00001

using namespace std;

typedef struct
{
    vector<int> sequence;
    int fit;
} Chromosome;

typedef struct
{
    Chromosome father, mother;
} Parents;

vector<int> generateChromosome()
{
    vector<int> s(COLUMN);
    int i;
    for (i = 0; i < s.size(); i++)
    {
        s.at(i) = (rand() % ROW) + 1;
    }
    int seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(s.begin(), s.end(), default_random_engine(seed));
    return s;
}

int fitness(Chromosome S)
{
    int clashes = 0;
    vector<int>::iterator ip;
    int i, j;
    // calculate diagonal clashes
    for (i = 0; i < COLUMN; i++)
    {
        for (j = i; j < COLUMN; j++)
        {
            if (i != j)
            {
                int dx = abs(i - j);
                int dy = abs(S.sequence.at(i) - S.sequence.at(j));
                if (dx == dy)
                    clashes++;
            }
        }
    }
    // fine unique position which means it have no conflict with another queen
    sort(S.sequence.begin(), S.sequence.end());
    ip = unique(S.sequence.begin(), S.sequence.begin() + COLUMN);
    S.sequence.resize(distance(S.sequence.begin(), ip));
    int uniqueROW = S.sequence.size();
    // 1 column have 1 queens so no conflict in column
    int row_clashes = ROW - uniqueROW; // remove unique position out of array -> we have row clashes
    clashes += row_clashes;
    return 28 - clashes;
}

Chromosome *generatePopulation(int populationSize)
{
    Chromosome *population;
    population = new Chromosome[populationSize];
    int i;
    for (i = 0; i < populationSize; i++)
    {
        population[i].sequence = generateChromosome();
        population[i].fit = fitness(population[i]);
    }
    return population;
}

Parents selection(Chromosome *Population, int size)
{
    Parents a;
    int fa = rand() % size;
    a.father = *(Population + fa);
    while (1)
    {
        int mo = rand() % size;
        if (mo != fa)
        {
            a.mother = *(Population + mo);
            break;
        }
    }
    return a;
}

Chromosome crossOver(Parents a)
{
    int n = a.father.sequence.size();
    int c = rand() % n;
    Chromosome child;
    vector<int> genFather = a.father.sequence;
    vector<int> genMother = a.mother.sequence;
    genFather.erase(genFather.begin() + c, genFather.end());
    genMother.erase(genMother.begin(), genMother.begin() + c);
    genFather.insert(genFather.end(), genMother.begin(), genMother.end());
    child.sequence = genFather;
    child.fit = fitness(child);
    return child;
}

Chromosome mutate(Chromosome child)
{
    int c = (rand() % COLUMN);
    child.sequence.at(c) = (rand() % ROW) + 1;
    return child;
}

Chromosome *GA(Chromosome *Population, int iteration, int size)
{
    // remove unqualified individuals and re-size the population
    Chromosome *nextGeneration = new Chromosome[size];
    int i, j = 0;
    for (i = 0; i < size; i++)
    {
        Parents a = selection(Population, size);
        Chromosome child = crossOver(a);
        int magicRandom = rand() % 100;
        if (magicRandom % 2 == 0)
            child = mutate(child);
        nextGeneration[i] = child;
    }
    return nextGeneration;
}

typedef struct
{
    int row, column;
} Input;

int stop(Chromosome *Population, int size, int iteration, Input ip[], int n)
{
    int i, j;
    for (i = 0; i < size; i++)
    {
        if (Population[i].fit == STOP_CTR)
        {
            for (j = 0; j < n; j++)
                if (Population[i].sequence.at(ip[j].column) != ip[j].row)
                {
                    return 0;
                }
            return 1;
        }
    }
    if (MAX_ITER == iteration)
        return 1;
    return 0;
}

int main()
{
    srand(time(NULL));
    int size = 1000; // the size of the original population
    Chromosome *Population = generatePopulation(size);
    int n;
    cin >> n;
    Input ip[8];
    int i, j;
    for (i = 0; i < n; i++)
    {
        cin >> ip[i].column;
        cin >> ip[i].row;
    }
    int iteration = 0;
    while (!stop(Population, size, iteration, ip, n))
    {
        Population = GA(Population, iteration, size);
        iteration++;
    }
    cout << "Interation number: " << iteration << endl;

    for (i = 0; i < size; i++)
    {
        if (Population[i].fit == 28)
        {
            for (j = 0; j < COLUMN; j++)
            {
                cout << Population[i].sequence.at(j) << " ";
            }
            cout << Population[i].fit << endl;
        }
    }
    return 0;
}