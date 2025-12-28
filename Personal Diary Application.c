#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DATE_LEN 20
#define MAX_CONTENT_LEN 500
#define PASSWORD "diary123"

typedef struct DiaryEntry {
    char date[MAX_DATE_LEN];
    char content[MAX_CONTENT_LEN];
    struct DiaryEntry* next;
} DiaryEntry;

typedef struct Action {
    char actionType[10];
    DiaryEntry entry;
    struct Action* next;
} Action;

DiaryEntry* head = NULL;
Action* undoStack = NULL;

void pushUndo(char* actionType, DiaryEntry entry) {
    Action* newAction = (Action*)malloc(sizeof(Action));
    strcpy(newAction->actionType, actionType);
    newAction->entry = entry;
    newAction->next = undoStack;
    undoStack = newAction;
}

Action* popUndo() {
    if (undoStack == NULL) return NULL;
    Action* temp = undoStack;
    undoStack = undoStack->next;
    return temp;
}

DiaryEntry* createEntry(char date[], char content[]) {
    DiaryEntry* newEntry = (DiaryEntry*)malloc(sizeof(DiaryEntry));
    strcpy(newEntry->date, date);
    strcpy(newEntry->content, content);
    newEntry->next = NULL;
    return newEntry;
}

void addEntry(char date[], char content[], int silent) {
    DiaryEntry* newEntry = createEntry(date, content);
    if (head == NULL) {
        head = newEntry;
    } else {
        DiaryEntry* temp = head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newEntry;
    }
    pushUndo("add", *newEntry);
    if (!silent) printf("Entry added successfully.\n");
}

void updateEntry(char date[], char newContent[]) {
    DiaryEntry* temp = head;
    while (temp != NULL) {
        if (strcmp(temp->date, date) == 0) {
            pushUndo("update", *temp);
            strcpy(temp->content, newContent);
            printf("Entry updated successfully.\n");
            return;
        }
        temp = temp->next;
    }
    printf("Entry not found.\n");
}

void deleteEntry(char date[]) {
    DiaryEntry* temp = head;
    DiaryEntry* prev = NULL;

    if (temp != NULL && strcmp(temp->date, date) == 0) {
        head = temp->next;
        pushUndo("delete", *temp);
        free(temp);
        printf("Entry deleted.\n");
        return;
    }

    while (temp != NULL && strcmp(temp->date, date) != 0) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        printf("Entry not found.\n");
        return;
    }

    prev->next = temp->next;
    pushUndo("delete", *temp);
    free(temp);
    printf("Entry deleted.\n");
}

void displayEntries() {
    if (head == NULL) {
        printf("Diary is empty.\n");
        return;
    }
    DiaryEntry* temp = head;
    printf("\n--- Diary Entries ---\n");
    while (temp != NULL) {
        printf("Date: %s\nContent: %s\n---------------------\n", temp->date, temp->content);
        temp = temp->next;
    }
}

void searchByKeyword(char keyword[]) {
    DiaryEntry* temp = head;
    int found = 0;
    while (temp != NULL) {
        if (strstr(temp->content, keyword)) {
            printf("Date: %s\nContent: %s\n------------------\n", temp->date, temp->content);
            found = 1;
        }
        temp = temp->next;
    }
    if (!found) printf("No entries with keyword found.\n");
}

int compareDates(char* d1, char* d2) {
    return strcmp(d1, d2);
}

void searchByDateRange(char* startDate, char* endDate) {
    DiaryEntry* temp = head;
    while (temp != NULL) {
        if (compareDates(temp->date, startDate) >= 0 && compareDates(temp->date, endDate) <= 0) {
            printf("Date: %s\nContent: %s\n------------------\n", temp->date, temp->content);
        }
        temp = temp->next;
    }
}

DiaryEntry* mergeSorted(DiaryEntry* a, DiaryEntry* b) {
    if (!a) return b;
    if (!b) return a;
    DiaryEntry* result;
    if (compareDates(a->date, b->date) <= 0) {
        result = a;
        result->next = mergeSorted(a->next, b);
    } else {
        result = b;
        result->next = mergeSorted(a, b->next);
    }
    return result;
}

void splitList(DiaryEntry* source, DiaryEntry** front, DiaryEntry** back) {
    DiaryEntry* fast;
    DiaryEntry* slow = source;
    if (!source || !source->next) {
        *front = source;
        *back = NULL;
        return;
    }
    fast = source->next;
    while (fast) {
        fast = fast->next;
        if (fast) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *front = source;
    *back = slow->next;
    slow->next = NULL;
}

void mergeSort(DiaryEntry** headRef) {
    DiaryEntry* headNode = *headRef;
    DiaryEntry* a;
    DiaryEntry* b;
    if (!headNode || !headNode->next) return;
    splitList(headNode, &a, &b);
    mergeSort(&a);
    mergeSort(&b);
    *headRef = mergeSorted(a, b);
}

void sortEntries() {
    mergeSort(&head);
    printf("Entries sorted by date.\n");
}

void undoLastAction() {
    Action* action = popUndo();
    if (!action) {
        printf("Nothing to undo.\n");
        return;
    }
    if (strcmp(action->actionType, "add") == 0) {
        deleteEntry(action->entry.date);
    } else if (strcmp(action->actionType, "delete") == 0) {
        addEntry(action->entry.date, action->entry.content, 1); // silent = 1
    } else if (strcmp(action->actionType, "update") == 0) {
        updateEntry(action->entry.date, action->entry.content);
    }
    free(action);
    printf("Last action undone.\n");
}

int main() {
    int choice;
    char date[MAX_DATE_LEN];
    char content[MAX_CONTENT_LEN];
    char keyword[50], startDate[MAX_DATE_LEN], endDate[MAX_DATE_LEN];
    char inputPassword[50];

    do {
        printf("Enter password to access the diary: ");
        fgets(inputPassword, sizeof(inputPassword), stdin);
        inputPassword[strcspn(inputPassword, "\n")] = '\0';

        if (strcmp(inputPassword, PASSWORD) != 0) {
            printf("Incorrect password. Please try again.\n");
        }
    } while (strcmp(inputPassword, PASSWORD) != 0);

    printf("Access granted.\n");

    addEntry("2025-04-01", "Started working on my C diary project. Excited!", 1);
    addEntry("2025-04-02", "Implemented add and view functionality.", 1);
    addEntry("2025-04-03", "Added update and delete features today.", 1);
    addEntry("2025-04-04", "Debugged some memory issues. All good now.", 1);
    addEntry("2025-04-05", "Started working on search by keyword.", 1);
    addEntry("2025-04-06", "Completed search by date range feature.", 1);
    addEntry("2025-04-07", "Sorting entries using merge sort implemented.", 1);
    addEntry("2025-04-08", "Undo functionality using stack added!", 1);
    addEntry("2025-04-09", "Final touches and testing.", 1);
    addEntry("2025-04-10", "Diary project completed successfully!", 1);

    do {
        printf("\n--- Personal Diary Menu ---\n");
        printf("1. Add Entry\n2. View Entries\n3. Update Entry\n4. Delete Entry\n");
        printf("5. Search by Keyword\n6. Search by Date Range\n7. Sort Entries by Date\n");
        printf("8. Undo Last Action\n9. Exit\nEnter choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter date (YYYY-MM-DD): ");
                fgets(date, MAX_DATE_LEN, stdin);
                date[strcspn(date, "\n")] = '\0';
                printf("Enter content: ");
                fgets(content, MAX_CONTENT_LEN, stdin);
                content[strcspn(content, "\n")] = '\0';
                addEntry(date, content, 0); // show message
                break;
            case 2:
                displayEntries();
                break;
            case 3:
                printf("Enter date to update: ");
                fgets(date, MAX_DATE_LEN, stdin);
                date[strcspn(date, "\n")] = '\0';
                printf("Enter new content: ");
                fgets(content, MAX_CONTENT_LEN, stdin);
                content[strcspn(content, "\n")] = '\0';
                updateEntry(date, content);
                break;
            case 4:
                printf("Enter date to delete: ");
                fgets(date, MAX_DATE_LEN, stdin);
                date[strcspn(date, "\n")] = '\0';
                deleteEntry(date);
                break;
            case 5:
                printf("Enter keyword to search: ");
                fgets(keyword, 50, stdin);
                keyword[strcspn(keyword, "\n")] = '\0';
                searchByKeyword(keyword);
                break;
            case 6:
                printf("Enter start date: ");
                fgets(startDate, MAX_DATE_LEN, stdin);
                startDate[strcspn(startDate, "\n")] = '\0';
                printf("Enter end date: ");
                fgets(endDate, MAX_DATE_LEN, stdin);
                endDate[strcspn(endDate, "\n")] = '\0';
                searchByDateRange(startDate, endDate);
                break;
            case 7:
                sortEntries();
                break;
            case 8:
                undoLastAction();
                break;
            case 9:
                printf("Exiting diary...\n");
                break;
            default:
                printf("Invalid choice. Try again.\n");
        }
    } while (choice != 9);

    while (head != NULL) {
        DiaryEntry* temp = head;
        head = head->next;
        free(temp);
    }

    return 0;
}
