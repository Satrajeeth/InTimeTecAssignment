// Standard library headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Project headers
#define Player RawPlayer
#define players hdr_players
#define teams hdr_teams
#define teamCount hdr_teamCount
#define playerCount hdr_playerCount
#include "Players_data.h"
#undef Player
#undef players
#undef teams
#undef teamCount
#undef playerCount

#define MAX_TEAMS 10
#define NAME_LEN 64

typedef enum 
{ 
    ROLE_BAT = 1, 
    ROLE_BOWL = 2, 
    ROLE_ALL = 3 
} Role;

typedef struct 
{
    int id;
    char name[NAME_LEN];
    char team_name[NAME_LEN];
    Role role;
    int total_runs;
    double batting_avg;
    double strike_rate;
    int wickets;
    double econ;
    double perf_index;
} Player;

typedef struct 
{
    Player **arr;
    int size;
    int cap;
} PlayerList;

void plist_init(PlayerList *pl) 
{
    pl->arr = NULL;
    pl->size = 0;
    pl->cap = 0;
}

void plist_push(PlayerList *pl, Player *p) 
{
    if (pl->size == pl->cap) 
    {
        pl->cap = pl->cap ? pl->cap * 2 : 4;
        pl->arr = realloc(pl->arr, pl->cap * sizeof(Player*));
    }
    pl->arr[pl->size++] = p;
}

void plist_insert_at(PlayerList *pl, int idx, Player *p) 
{
    if (idx < 0) 
    {
        idx = 0;
    }
    if (idx > pl->size) 
    {
        idx = pl->size;
    }
    if (pl->size == pl->cap) 
    {
        pl->cap = pl->cap ? pl->cap * 2 : 4;
        pl->arr = realloc(pl->arr, pl->cap * sizeof(Player*));
    }
    memmove(&pl->arr[idx+1], &pl->arr[idx], (pl->size - idx) * sizeof(Player*));
    pl->arr[idx] = p;
    pl->size++;
}

void plist_free(PlayerList *pl) 
{
    free(pl->arr);
    pl->arr = NULL;
    pl->size = pl->cap = 0;
}

typedef struct 
{
    int team_id;
    char name[NAME_LEN];
    int total_players;
    double avg_batting_strike_rate;
    PlayerList batsmen;
    PlayerList bowlers;
    PlayerList allrounders;
} Team;

Team teams[MAX_TEAMS];
int teamCount = MAX_TEAMS;


int strange_unused(int x)
{
    int y = x * 42;
    y += 7;
    return y;
}

Player **all_players = NULL;
int all_players_count = 0;
int all_players_cap = 0;

Player* alloc_player() 
{
    if (all_players_count == all_players_cap) 
    {
        all_players_cap = all_players_cap ? all_players_cap * 2 : 16;
        all_players = realloc(all_players, all_players_cap * sizeof(Player*));
    }
    Player *p = malloc(sizeof(Player));
    all_players[all_players_count++] = p;
    return p;
}

double calcPerfIndex(const Player *p) 
{
    if (p == NULL)
    {
        return 0.0;
    }
    if (p->role == ROLE_BAT) 
    {
        return (p->batting_avg * p->strike_rate) / 100.0;
    } 
    else if (p->role == ROLE_BOWL) 
    {
        return (p->wickets * 2.0) + (100.0 - p->econ);
    } 
    else 
    {
        return ((p->batting_avg * p->strike_rate) / 100.0) + (p->wickets * 2.0);
    }
}

int cmp_desc_perf(const void *a, const void *b) 
{
    Player *pa = *(Player**)a;
    Player *pb = *(Player**)b;
    if (pa->perf_index > pb->perf_index) 
    {
        return -1;
    }
    if (pa->perf_index < pb->perf_index) 
    {
        return 1;
    }
    return pa->id - pb->id;
}

void updateTeamAvgSr(Team *t) 
{
    if (t == NULL)
    {
        return;
    }
    double sum = 0.0;
    int count = 0;
    for (int i = 0; i < t->batsmen.size; ++i) 
    {
        sum += t->batsmen.arr[i]->strike_rate;
        count++;
    }
    for (int i = 0; i < t->allrounders.size; ++i) 
    {
        sum += t->allrounders.arr[i]->strike_rate;
        count++;
    }
    t->avg_batting_strike_rate = count ? (sum / count) : 0.0;
    t->total_players = t->batsmen.size + t->bowlers.size + t->allrounders.size;
}

int find_team_index_by_id(int team_id) 
{
    int l = 0, r = teamCount - 1;
    while (l <= r) 
    {
        int m = (l + r) / 2;
        if (teams[m].team_id == team_id) 
        {
            return m;
        }
        if (teams[m].team_id < team_id) 
        {
            l = m + 1;
        } 
        else 
        {
            r = m - 1;
        }
    }
    return -1;
}

void insertPlayerSorted(PlayerList *pl, Player *p) 
{
    if (pl == NULL || p == NULL)
    {
        return;
    }
    int idx = 0;
    while (idx < pl->size && pl->arr[idx]->perf_index >= p->perf_index) 
    {
        idx++;
    }
    plist_insert_at(pl, idx, p);
}

int add_player_to_team_by_id(int team_id, Player *p) 
{
    if (p == NULL)
    {
        return 0;
    }
    int tidx = find_team_index_by_id(team_id);
    if (tidx < 0) 
    {
        return 0;
    }
    Team *t = &teams[tidx];
    strncpy(p->team_name, t->name, NAME_LEN-1);
    p->team_name[NAME_LEN-1] = '\0';
    p->perf_index = calcPerfIndex(p);
    if (p->role == ROLE_BAT) 
    {
        insertPlayerSorted(&t->batsmen, p);
    } 
    else if (p->role == ROLE_BOWL) 
    {
        insertPlayerSorted(&t->bowlers, p);
    } 
    else 
    {
        insertPlayerSorted(&t->allrounders, p);
    }
    updateTeamAvgSr(t);
    return 1;
}

const char* role_to_str(const Role r) 
{
    if (r == ROLE_BAT) 
    {
        return "Batsman";
    }
    if (r == ROLE_BOWL) 
    {
        return "Bowler";
    }
    return "All-Rounder";
}

void displayPlayersOfTeam(int team_id) 
{
    int tidx = find_team_index_by_id(team_id);
    if (tidx < 0) 
    {
        printf("Team ID %d not found.\n", team_id); 
        return; 
    }
    Team *t = &teams[tidx];
    printf("\nPlayers of Team %s:\n", t->name);
    printf("====================================================================================\n");
    printf("ID   Name                          Role         Runs    Avg     SR    Wkts   ER   Perf.Index\n");
    printf("====================================================================================\n");
    for (int i = 0; i < t->batsmen.size; ++i) 
    {
        Player *p = t->batsmen.arr[i];
        printf("%-4d %-28s %-11s %-7d %-6d %-6d %-6d %-5d %-8d\n",
               p->id, p->name, role_to_str(p->role), p->total_runs,
               (int)round(p->batting_avg), (int)round(p->strike_rate), 
               p->wickets, (int)round(p->econ), (int)round(p->perf_index));
    }
    for (int i = 0; i < t->allrounders.size; ++i) 
    {
        Player *p = t->allrounders.arr[i];
        printf("%-4d %-28s %-11s %-7d %-6d %-6d %-6d %-5d %-8d\n",
               p->id, p->name, role_to_str(p->role), p->total_runs, 
               (int)round(p->batting_avg), (int)round(p->strike_rate), 
               p->wickets, (int)round(p->econ), (int)round(p->perf_index));
    }
    for (int i = 0; i < t->bowlers.size; ++i) 
    {
        Player *p = t->bowlers.arr[i];
        printf("%-4d %-28s %-11s %-7d %-6d %-6d %-6d %-5d %-8d\n", 
               p->id, p->name, role_to_str(p->role), p->total_runs, 
               (int)round(p->batting_avg), (int)round(p->strike_rate), 
               p->wickets, (int)round(p->econ), (int)round(p->perf_index));
    }
    printf("====================================================================================\n");
    printf("Total Players: %d\n", t->total_players);
    printf("Average Batting Strike Rate: %d\n", (int)round(t->avg_batting_strike_rate));
}

void displayTeamsByAvgSr(void) 
{
    int idxs[MAX_TEAMS];
    for (int i = 0; i < teamCount; ++i) 
    {
        idxs[i] = i;
    }
    for (int i = 0; i < teamCount; ++i) 
    {
        for (int j = i + 1; j < teamCount; ++j) 
        {
            if (teams[idxs[j]].avg_batting_strike_rate > teams[idxs[i]].avg_batting_strike_rate) 
            {
                int tmp = idxs[i]; 
                idxs[i] = idxs[j]; 
                idxs[j] = tmp;
            }
        }
    }
    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("=========================================================\n");
    printf("ID   Team Name                  Avg Bat SR    Total Players\n");
    printf("=========================================================\n");
    for (int i = 0; i < teamCount; ++i) 
    {
        Team *t = &teams[idxs[i]];
        printf("%-4d %-26s %-12d %d\n", 
               t->team_id, t->name, 
               (int)round(t->avg_batting_strike_rate), 
               t->total_players);
    }
    printf("=========================================================\n");
}

void displayTopKOfTeam(int team_id, Role r, int K) 
{
    int tidx = find_team_index_by_id(team_id);
    if (tidx < 0) 
    { 
        printf("Team ID %d not found.\n", team_id); 
        return; 
    }
    Team *t = &teams[tidx];
    PlayerList *pl = (r == ROLE_BAT) ? &t->batsmen : 
                     (r == ROLE_BOWL ? &t->bowlers : &t->allrounders);
    printf("\nTop %d %s of Team %s:\n", K, role_to_str(r), t->name);
    printf("====================================================================================\n");
    printf("ID   Name                          Role         Runs    Avg     SR    Wkts   ER   Perf.Index\n");
    printf("====================================================================================\n");
    int to = (K < pl->size) ? K : pl->size;
    for (int i = 0; i < to; ++i) 
    {
        Player *p = pl->arr[i];
        printf("%-4d %-28s %-11s %-7d %-6d %-6d %-6d %-5d %-8d\n",
               p->id, p->name, role_to_str(p->role), p->total_runs, 
               (int)round(p->batting_avg), (int)round(p->strike_rate), 
               p->wickets, (int)round(p->econ), (int)round(p->perf_index));
    }
    if (to == 0) 
    {
        printf("No players of that role in this team.\n");
    }
}

typedef struct {
    Player *p;
    int team_idx;
    int pos_in_list;
} HeapNode;

typedef struct {
    HeapNode *arr;
    int size;
    int cap;
} MaxHeap;

void heap_init(MaxHeap *h) { h->arr = NULL; h->size = 0; h->cap = 0; }
void heap_push(MaxHeap *h, HeapNode node) {
    if (h->size == h->cap) {
        h->cap = h->cap ? h->cap * 2 : 8;
        h->arr = realloc(h->arr, h->cap * sizeof(HeapNode));
    }
    int i = h->size++;
    h->arr[i] = node;
    while (i > 0) {
        int p = (i - 1) / 2;
        if (h->arr[p].p->perf_index >= h->arr[i].p->perf_index) {
            break;
        }
        HeapNode tmp = h->arr[p]; h->arr[p] = h->arr[i]; h->arr[i] = tmp;
        i = p;
    }
}
HeapNode heap_pop(MaxHeap *h) {
    HeapNode ret = h->arr[0];
    h->size--;
    h->arr[0] = h->arr[h->size];
    int i = 0;
    while (1) {
        int l = 2*i + 1, r = 2*i + 2, largest = i;
        if (l < h->size && h->arr[l].p->perf_index > h->arr[largest].p->perf_index) {
            largest = l;
        }
        if (r < h->size && h->arr[r].p->perf_index > h->arr[largest].p->perf_index) {
            largest = r;
        }
        if (largest == i) {
            break;
        }
        HeapNode tmp = h->arr[i]; h->arr[i] = h->arr[largest]; h->arr[largest] = tmp;
        i = largest;
    }
    return ret;
}
int heap_empty(MaxHeap *h) 
{ 
    return h->size == 0; 
}

void heap_free(MaxHeap *h) 
{ 
    free(h->arr); 
    h->arr = NULL; 
    h->size = h->cap = 0; 
}

void displayAllPlayersOfRole(Role r) 
{
    PlayerList *lists[MAX_TEAMS];
    for (int i = 0; i < teamCount; ++i) 
    {
        lists[i] = (r == ROLE_BAT) ? &teams[i].batsmen : 
                   (r == ROLE_BOWL ? &teams[i].bowlers : &teams[i].allrounders);
    }
    MaxHeap heap; 
    heap_init(&heap);
    for (int i = 0; i < teamCount; ++i) 
    {
        if (lists[i]->size > 0) 
        {
            HeapNode node = { lists[i]->arr[0], i, 0 };
            heap_push(&heap, node);
        }
    }
    printf("\nAll %s players across all teams (sorted by performance index desc):\n", 
           role_to_str(r));
    printf("======================================================================================\n");
    printf("ID   Name                          Team               Role         Runs  Avg   SR   Wkts  ER   Perf.Index\n");
    printf("======================================================================================\n");
    while (!heap_empty(&heap)) 
    {
        HeapNode top = heap_pop(&heap);
        Player *p = top.p;
        printf("%-4d %-28s %-18s %-11s %-6d %-6d %-6d %-6d %-5d %-8d\n", 
               p->id, p->name, p->team_name, role_to_str(p->role), p->total_runs, 
               (int)round(p->batting_avg), (int)round(p->strike_rate), 
               p->wickets, (int)round(p->econ), (int)round(p->perf_index));
        int team_i = top.team_idx;
        int nextpos = top.pos_in_list + 1;
        PlayerList *pl = lists[team_i];
        if (nextpos < pl->size) 
        {
            HeapNode next = { pl->arr[nextpos], team_i, nextpos };
            heap_push(&heap, next);
        }
    }
    printf("======================================================================================\n");
    heap_free(&heap);
}

int readInt(void) 
{
    int x;
    if (scanf("%d", &x) != 1) 
    {
        while (getchar() != '\n');
        return -1;
    }
    return x;
}

double readDouble(void) 
{
    double d;
    if (scanf("%lf", &d) != 1) 
    {
        while (getchar() != '\n');
        return 0.0;
    }
    return d;
}

void load_initial_data_sample() 
{
    const char *names[MAX_TEAMS] = 
    {
        "India", "Australia", "England", "Pakistan", "New Zealand", 
        "South Africa", "Bangladesh", "Sri Lanka", "Afghanistan", "West Indies"
    };
    for (int i = 0; i < MAX_TEAMS; ++i) 
    {
        teams[i].team_id = i + 1;
        strncpy(teams[i].name, names[i], NAME_LEN-1);
        teams[i].name[NAME_LEN-1] = '\0';
        plist_init(&teams[i].batsmen);
        plist_init(&teams[i].bowlers);
        plist_init(&teams[i].allrounders);
        teams[i].total_players = 0;
        teams[i].avg_batting_strike_rate = 0.0;
    }

    Player *p;

    p = alloc_player(); 
    p->id = 101; 
    strcpy(p->name, "Rohit Sharma"); 
    p->role = ROLE_BAT; 
    p->total_runs = 9800; 
    p->batting_avg = 48.3; 
    p->strike_rate = 94.5; 
    p->wickets = 0; 
    p->econ = 0.0;
    add_player_to_team_by_id(1, p);

    p = alloc_player(); p->id = 102; strcpy(p->name, "Virat Kohli"); p->role = ROLE_BAT; p->total_runs=13000; p->batting_avg=57.3; p->strike_rate=93.2; p->wickets=0; p->econ=0.0;
    add_player_to_team_by_id(1, p);

    p = alloc_player(); p->id = 103; strcpy(p->name, "Jasprit Bumrah"); p->role = ROLE_BOWL; p->total_runs=120; p->batting_avg=7.8; p->strike_rate=70.0; p->wickets=145; p->econ=4.6;
    add_player_to_team_by_id(1, p);

    p = alloc_player(); p->id = 104; strcpy(p->name, "Hardik Pandya"); p->role = ROLE_ALL; p->total_runs=2300; p->batting_avg=36.5; p->strike_rate=121.0; p->wickets=80; p->econ=5.2;
    add_player_to_team_by_id(1, p);

    p = alloc_player(); p->id = 201; strcpy(p->name, "David Warner"); p->role = ROLE_BAT; p->total_runs=5600; p->batting_avg=45.0; p->strike_rate=99.0; p->wickets=0; p->econ=0.0;
    add_player_to_team_by_id(2, p);

    p = alloc_player(); p->id = 202; strcpy(p->name, "Mitchell Starc"); p->role = ROLE_BOWL; p->total_runs=210; p->batting_avg=10.5; p->strike_rate=84.2; p->wickets=240; p->econ=4.7;
    add_player_to_team_by_id(2, p);
}

void load_initial_data_from_header() 
{
    for (int i = 0; i < MAX_TEAMS; ++i) 
    {
        teams[i].team_id = i + 1;
        strncpy(teams[i].name, hdr_teams[i], NAME_LEN-1);
        teams[i].name[NAME_LEN-1] = '\0';
        plist_init(&teams[i].batsmen);
        plist_init(&teams[i].bowlers);
        plist_init(&teams[i].allrounders);
        teams[i].total_players = 0;
        teams[i].avg_batting_strike_rate = 0.0;
    }

    for (int i = 0; i < hdr_playerCount; ++i) 
    {
        const RawPlayer *rp = &hdr_players[i];
        Player *p = alloc_player();
        p->id = rp->id;
        strncpy(p->name, rp->name, NAME_LEN-1); 
        p->name[NAME_LEN-1] = '\0';
        p->total_runs = rp->totalRuns;
        p->batting_avg = rp->battingAverage;
        p->strike_rate = rp->strikeRate;
        p->wickets = rp->wickets;
        p->econ = rp->economyRate;
        if (rp->role && strcmp(rp->role, "Bowler") == 0) 
        {
            p->role = ROLE_BOWL;
        } 
        else if (rp->role && strcmp(rp->role, "Batsman") == 0) 
        {
            p->role = ROLE_BAT;
        } 
        else 
        {
            p->role = ROLE_ALL;
        }

        int tidx = -1;
        for (int j = 0; j < MAX_TEAMS; ++j) 
        {
            if (strcmp(rp->team, hdr_teams[j]) == 0) 
            { 
                tidx = j; 
                break; 
            }
        }
        if (tidx < 0) 
        {
            for (int j = 0; j < MAX_TEAMS; ++j) 
            {
                if (strstr(hdr_teams[j], rp->team) || strstr(rp->team, hdr_teams[j])) 
                { 
                    tidx = j; 
                    break; 
                }
            }
        }
        if (tidx < 0) 
        {
            tidx = 0;
        }

        add_player_to_team_by_id(teams[tidx].team_id, p);
    }
}

void initialize(void) 
{
    load_initial_data_from_header();
}

void menu_loop(void) 
{
    while (1) 
    {
        printf("\n===================================================\n\n");
        printf("ICC ODI Player Performance Analyzer\n");
        printf("===================================================\n");
        printf("1. Add Player to Team\n");
        printf("2. Display Players of a Specific Team\n");
        printf("3. Display Teams by Average Batting Strike Rate\n");
        printf("4. Display Top K Players of a Specific Team by Role\n");
        printf("5. Display all Players of specific role Across All Teams by performance index\n");
        printf("6. Exit\n\n");
        printf("===================================================\n");
        printf("Enter your choice : ");
        int choice = readInt();
        if (choice == -1) 
        {
            continue;
        }
        if (choice == 6) 
        {
            break;
        }
        if (choice == 1) 
        {
            printf("Enter Team ID to add player: ");
            int teamId = readInt();
            if (find_team_index_by_id(teamId) < 0) 
            { 
                printf("Team ID not found.\n"); 
                continue; 
            }
            Player *p = alloc_player();
            printf("Enter Player ID: "); 
            p->id = readInt(); 
            getchar();
            printf("Enter Name: "); 
            fgets(p->name, NAME_LEN, stdin); 
            p->name[strcspn(p->name, "\n")] = 0;
            printf("Role (1-Batsman, 2-Bowler, 3-All-rounder): "); 
            p->role = (Role) readInt();
            printf("Total Runs: "); 
            p->total_runs = readInt();
            printf("Batting Average: "); 
            p->batting_avg = readDouble();
            printf("Strike Rate: "); 
            p->strike_rate = readDouble();
            printf("Wickets: "); 
            p->wickets = readInt();
            printf("Economy Rate: "); 
            p->econ = readDouble();
            p->perf_index = calcPerfIndex(p);
            if (add_player_to_team_by_id(teamId, p)) 
            {
                printf("Player added successfully to Team %s!\n", 
                       teams[find_team_index_by_id(teamId)].name);
            } 
            else 
            {
                printf("Failed to add player — Team ID not found.\n");
            }
        } 
        else if (choice == 2) 
        {
            printf("Enter Team ID: ");
            int teamId = readInt();
            displayPlayersOfTeam(teamId);
        } 
        else if (choice == 3) 
        {
            displayTeamsByAvgSr();
        } 
        else if (choice == 4) 
        {
            printf("Enter Team ID: "); 
            int teamId = readInt();
            printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): "); 
            int roleChoice = readInt();
            printf("Enter number of players: "); 
            int topCount = readInt();
            displayTopKOfTeam(teamId, (Role)roleChoice, topCount);
        } 
        else if (choice == 5) 
        {
            printf("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): "); 
            int roleChoice = readInt();
            displayAllPlayersOfRole((Role)roleChoice);
        } 
        else 
        {
            printf("Invalid choice, Try again.\n");
        }
    }
}

void cleanup(void) 
{
    for (int i = 0; i < teamCount; ++i) 
    {
        plist_free(&teams[i].batsmen);
        plist_free(&teams[i].bowlers);
        plist_free(&teams[i].allrounders);
    }
    for (int i = 0; i < all_players_count; ++i) 
    {
        free(all_players[i]);
    }
    free(all_players);
}

int main(void) 
{
    initialize();
    menu_loop();
    cleanup();
    return 0;
}