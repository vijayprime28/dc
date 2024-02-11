#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_RESOURCES 100
#define MAX_PROCESSES 100

// Forward declaration of Resource structure
typedef struct Resource Resource;

// Structure to represent a process
typedef struct {
    int id;
    Resource* holding;
    Resource* waiting;
} Process;

// Structure to represent a resource
struct Resource {
    int id;
    int site;
    int heldBy; // Process ID of the process holding this resource, -1 if not held
};

// Function to check for cycles in the resource allocation graph
bool detectCycle(Process* processes, Resource* resources, int curId, int start) {
    if (curId == -1) {
        return false;
    }

    if (curId == start) {
        return true;  // Detected a cycle
    }

    return detectCycle(processes, resources, processes[curId].waiting->heldBy, start);
}

// Function to check for deadlock in a site
bool checkDeadlockSite(Process* processes, Resource* resources, int site) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].id != -1 && processes[i].holding != NULL && processes[i].waiting != NULL &&
            processes[i].holding->site == site && processes[i].waiting->site == site) {
            if (detectCycle(processes, resources, i, i)) {
                return true;  // Detected a deadlock in the site
            }
        }
    }
    return false;  // No deadlock in the site
}

// Function to check for deadlock in the coordinator
bool checkDeadlock(Process* processes, Resource* resources, int processCount) {
    for (int i = 0; i < processCount; i++) {
        if (processes[i].waiting != NULL && detectCycle(processes, resources, i, i)) {
            // Check if the waiting resource is from a different site
            bool waitingFromDifferentSite = false;
            for (int j = 0; j < processCount; j++) {
                if (processes[j].id != -1 && processes[j].holding != NULL && processes[j].waiting != NULL &&
                    processes[j].holding->site != processes[j].waiting->site) {
                    waitingFromDifferentSite = true;
                    break;
                }
            }
            if (waitingFromDifferentSite) {
                return true;  // Global deadlock detected
            } else {
                return false;  // Deadlock within a site, not global
            }
        }
    }
    return false;  // No deadlock in the coordinator
}

int main() {
    Resource resources[MAX_RESOURCES];
    Process processes[MAX_PROCESSES];

    int processCount = 0;  // Counter to keep track of the number of processes

    // Initialize processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].id = -1;  // Indicates empty slot
        processes[i].holding = NULL;
        processes[i].waiting = NULL;
    }

    // Input resources for site 1
    int s1No, s2No;
    printf("No. of resources in site 1: ");
    scanf("%d", &s1No);
    for (int i = 0; i < s1No; i++) {
        resources[i].id = i;
        resources[i].site = 1;
        resources[i].heldBy = -1;  // Initially not held by any process
    }

    // Input resources for site 2
    printf("No. of resources in site 2: ");
    scanf("%d", &s2No);
    for (int i = s1No; i < s1No + s2No; i++) {
        resources[i].id = i;
        resources[i].site = 2;
        resources[i].heldBy = -1;  // Initially not held by any process
    }

    printf("\nResources in site 1:\n");
    for (int i = 0; i < s1No; i++) {
        printf("%d ", resources[i].id);
    }
    printf("\nResources in site 2:\n");
    for (int i = s1No; i < s1No + s2No; i++) {
        printf("%d ", resources[i].id);
    }
    printf("\n\n");

    // Input processes
    int NoOfProcesses;
    printf("Enter number of processes: ");
    scanf("%d", &NoOfProcesses);

    for (int i = 0; i < NoOfProcesses && processCount < MAX_PROCESSES; i++) {
        int holdingResourceId, waitingResourceId;
        printf("Enter resource ID process-%d is holding (-1 for none): ", processCount);
        scanf("%d", &holdingResourceId);
        printf("Enter resource ID process-%d is waiting for (-1 for none): ", processCount);
        scanf("%d", &waitingResourceId);

        processes[processCount].id = processCount;
        if (holdingResourceId != -1) {
            processes[processCount].holding = &resources[holdingResourceId];
            resources[holdingResourceId].heldBy = processCount;
        } else {
            processes[processCount].holding = NULL;
        }

        if (waitingResourceId != -1) {
            processes[processCount].waiting = &resources[waitingResourceId];
        } else {
            processes[processCount].waiting = NULL;
        }

        processCount++;
    }

    bool globalDeadlock = checkDeadlock(processes, resources, processCount);
    bool site1Deadlock = checkDeadlockSite(processes, resources, 1);
    bool site2Deadlock = checkDeadlockSite(processes, resources, 2);

    if (globalDeadlock) {
        printf("Deadlock detected in central coordinator\n");
    }
    if (site1Deadlock) {
        printf("Deadlock detected in site 1\n");
    }
    if (site2Deadlock) {
        printf("Deadlock detected in site 2\n");
    }
    if (!globalDeadlock && !site1Deadlock && !site2Deadlock) {
        printf("No deadlock detected\n");
    }

    return 0;
}
