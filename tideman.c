#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool has_cycle(int w, int l);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // Stores the 'i'th-preferred candidate for each voter in a unique ranks array
    // Remember: arrays are passed by reference and a copy is not created when passed into a function
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }

    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // Update the direction of preference for each pair of candidates based on one voter's rankings
    for (int i = 0; i < candidate_count - 1; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    pair_count = 0;
    // Note: repeats work, could be more efficient
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pair temp_pair;
                temp_pair.winner = i;
                temp_pair.loser = j;
                pairs[pair_count] = temp_pair;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // Implement selection sort to compare and sort the strengths of each pair in paris array
    for (int i = 0; i < pair_count - 1; i++)
    {
        pair temp = pairs[i];
        int max = i;
        for (int j = i + 1; j < pair_count; j++)
        {
            if (preferences[pairs[j].winner][pairs[j].loser] - preferences[pairs[j].loser][pairs[j].winner] >
                preferences[pairs[max].winner][pairs[max].loser] - preferences[pairs[max].loser][pairs[max].winner])
            {
                max = j;
            }
        }
        pairs[i] = pairs[max];
        pairs[max] = temp;

    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // Update locked status for each pair depending on whether they create a cycle or not
    for (int i = 0; i < pair_count; i++)
    {
        // We presumtively add the pair...
        locked[pairs[i].winner][pairs[i].loser] = true;
        if (has_cycle(pairs[i].winner, pairs[i].loser))
        {
            // ...then remove it if it indeed causes a cycle
            locked[pairs[i].winner][pairs[i].loser] = false;
        }
    }
    return;
}
// Recursive helper function to check if an edge creates a cycle
bool has_cycle(int w, int l)
{
    // Base case is when the reverse edge exists
    if (locked[l][w])
    {
        return true;
    }

    // For all existing incoming edges to the winner, check if there is a parent edge incoming from the loser
    for (int i = 0; i < candidate_count; i++)
    {
        if (locked[i][w])
        {
            return has_cycle(i, l);
        }
    }

    return false;
}

// Print the winner of the election
void print_winner(void)
{
    // Print the candidate with no incoming edges (whose column in 'locked' array is all falses)
    for (int j = 0; j < candidate_count; j++)
    {
        bool is_winner = true;
        for (int i = 0; i < candidate_count; i++)
        {
            if (locked[i][j])
            {
                is_winner = false;
            }
        }
        if (is_winner)
        {
            printf("%s\n", candidates[j]);
        }
    }
    return;
}