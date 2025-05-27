/*

Before the main function I imported 4 libraries
standard input output, library functions, time related functions, string manipulation(string.h) and
ctype.h allows me to use character handling functions which let me format the aliases. Following that theres 
a macro definition that prevents the program from accepting aliases that are greater than 10 characters in length

Then theres the data structure address_t, it holds the address data for the aliases and networks
octet[4] is an array that holds the digits for the network address alias[MAX_ALIAS_LEN + 1] is where
the alias is stored and its limited to 10 characters.


Following this theres several function and variable prototypes that are used throughout the program
that do various tasks like converting aliases to uppercase to have a clean consistent format and theres
several helper functions that help the program run smoother and more efficiently.


Now continuing with the functions, the main function reads in the aliases and network addresses and 
prompts the user to choose 1-7 options which are various tasks like adding an adress, looking up an address,
displaying a list or its size, displaying a list for location, saving a file and quitting. Once the user
inputs one of the numbers it calls the function based on the number typed, and for each number they type
the menu is automatically prompted again until the user quits the program.

Read data file opens the raw list of data containing ip addresses and its respective alias, skips alias
with None and does not include any that are more than 10 characters in length. Finally it converts the alias 
names to uppercase and stores them in in the addresses array.

Display list displays the stored addresses with their corresponding alises and depending on the list 
it may print the contents of it or tell the user that its empty(No values in the list).

Look up alias, prompts the user to enter an alias they want to find, depending on what they type,
they may get a DNE or they alias and its address is provided, it also does not matter how they type
the alias as long as it spelled correctly.


Add adress prompts the user to type a new address(as long as its formatted correctly) and to 
type an alias to the corresponding address(as long as its 10 characters or less).

Display list size will tell the user how many nodes are in the current list

Display Alias for location, allows the user to type in the first two octets of the address and
it displays all matching addresses and aliases that correspond to what they typed in and save to file
just saves the data the user was working with.

Finally theres several helper functions that help the program run smoother and correctly,
address_exists makes sure that theres no duplicate addresses in the list, alias_exists checks 
if within the list theres a duplicate address and str_to_upper converts all aliases to uppercase


AUTHOR: Federico Giudice
DATE: 4/28/25

*/




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ALIAS_LEN 10

typedef struct address_t {
    int octet[4];
    char alias[MAX_ALIAS_LEN + 1];
    struct address_t *next;
} address_t;

address_t *head = NULL;

void read_file(const char *filename);
void display_list();
void lookup_alias();
void add_address();
void display_list_size();
void display_aliases_for_location();
void save_to_file();
int address_exists(int o1, int o2, int o3, int o4);
int alias_exists(const char *alias);
char *str_to_upper(char *str);

int main() {
    int choice;
    read_file("IpList.txt");

    while (1) {
        printf("\nMenu Options:\n");
        printf("1) Add address\n");
        printf("2) Look up address\n");
        printf("3) Display list\n");
        printf("4) Display list size\n");
        printf("5) Display aliases for location\n");
        printf("6) Save to file\n");
        printf("7) Quit\n");
        printf("Enter menu option (1-7): ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1: add_address(); break;
            case 2: lookup_alias(); break;
            case 3: display_list(); break;
            case 4: display_list_size(); break;
            case 5: display_aliases_for_location(); break;
            case 6: save_to_file(); break;
            case 7: 
            	printf("Goodbye!\n");
            	exit(0);
            	break;
            default: printf("Invalid option. Try again.\n");
        }
    }
    return 0;
}

void read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Could not open file");
        return;
    }

    int a, b, c, d;
    char alias[50];

    while (fscanf(file, "%d.%d.%d.%d %s", &a, &b, &c, &d, alias) == 5) {
        if (a == 0 && b == 0 && c == 0 && d == 0 && strcmp(alias, "NONE") == 0)
            break;

        address_t *new_node = malloc(sizeof(address_t));
        

        (*new_node).octet[0] = a;
        (*new_node).octet[1] = b;
        (*new_node).octet[2] = c;
        (*new_node).octet[3] = d;
        strncpy((*new_node).alias, str_to_upper(alias), MAX_ALIAS_LEN);
        (*new_node).alias[MAX_ALIAS_LEN] = '\0';
        (*new_node).next = head;
        head = new_node;
    }

    fclose(file);
}


void display_list() {
    if (!head) {
        printf("The list is empty.\n");
        return;
    }
    address_t *curr = head;
    while (curr) {
        printf("%d.%d.%d.%d %-10s\n", 
            (*curr).octet[0], (*curr).octet[1], (*curr).octet[2], (*curr).octet[3], (*curr).alias);
        curr = (*curr).next;
    }
}


void lookup_alias() {
    char input[50];
    printf("Enter alias: ");
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;
    str_to_upper(input);

    address_t *curr = head;
    while (curr) {
        if (strcmp((*curr).alias, input) == 0) {
            printf("%s: %d.%d.%d.%d\n", 
                (*curr).alias, (*curr).octet[0], (*curr).octet[1], (*curr).octet[2], (*curr).octet[3]);
            return;
        }
        curr = (*curr).next;
    }
    printf("Alias '%s' not found.\n", input);
}

void add_address() {
    int a, b, c, d;
    char input[100];
    while (1) {
        printf("Enter address: ");
        fgets(input, sizeof(input), stdin);
        if (sscanf(input, "%d.%d.%d.%d", &a, &b, &c, &d) != 4 || a < 0 || a > 255 || b < 0 || b > 255 || c < 0 || c > 255 || d < 0 || d > 255) {
            printf("Error: Illegal address. Please reenter: ");
            continue;
        }
        if (address_exists(a, b, c, d)) {
            printf("Error: %d.%d.%d.%d already exists. Please reenter: ", a, b, c, d);
            continue;
        }
        break;
    }

    char alias[100];
    while (1) {
        printf("Enter alias (10 characters or less): ");
        fgets(alias, sizeof(alias), stdin);
        alias[strcspn(alias, "\n")] = 0;
        str_to_upper(alias);
        if (strlen(alias) > MAX_ALIAS_LEN) {
            printf("Error: Illegal alias. Please reenter: ");
            continue;
        }
        if (alias_exists(alias)) {
            printf("Error: Alias already exists. Please reenter: ");
            continue;
        }
        break;
    }

    address_t *new_node = malloc(sizeof(address_t));
    new_node->octet[0] = a;
    new_node->octet[1] = b;
    new_node->octet[2] = c;
    new_node->octet[3] = d;
    strncpy(new_node->alias, alias, MAX_ALIAS_LEN);
    new_node->alias[MAX_ALIAS_LEN] = '\0';
    new_node->next = head;
    head = new_node;
    printf("Address added successfully.\n");
}

void display_list_size() {
    int count = 0;
    address_t *curr = head;
    while (curr) {
        count++;
        curr = curr->next;
    }
    printf("List size: %d node%s\n", count, count == 1 ? "" : "s");
}

void display_aliases_for_location() {
    int a, b;
    printf("Enter Locality: ");
    while (scanf("%d.%d", &a, &b) != 2 || a < 0 || a > 255 || b < 0 || b > 255) {
        printf("Invalid input. Please enter a valid locality: ");
        while (getchar() != '\n');  
    }
    getchar();  

    int found = 0;
    address_t *curr = head;
    while (curr) {
        if ((*curr).octet[0] == a && (*curr).octet[1] == b) {
            printf("%s\n", (*curr).alias);
            found = 1;
        }
        curr = (*curr).next;
    }
    if (!found) {
        printf("No aliases found for that location.\n");
    }
}



void save_to_file() {
    char filename[100];
    printf("Enter file name: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;  

    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error opening file");
        return;
    }

    address_t *curr = head;
    while (curr) {
        fprintf(file, "%d.%d.%d.%d %s\n", 
            (*curr).octet[0], (*curr).octet[1], (*curr).octet[2], (*curr).octet[3], (*curr).alias);
        curr = (*curr).next;
    }

    fclose(file);
    printf("%s has been saved.\n", filename);
}



int address_exists(int o1, int o2, int o3, int o4) {
    address_t *curr = head;
    while (curr) {
        if ((*curr).octet[0] == o1 && (*curr).octet[1] == o2 && (*curr).octet[2] == o3 && (*curr).octet[3] == o4)
            return 1;
        curr = (*curr).next;
    }
    return 0;
}


int alias_exists(const char *alias) {
    address_t *curr = head;
    while (curr) {
        if (strcmp((*curr).alias, alias) == 0)
            return 1;
        curr = (*curr).next;
    }
    return 0;
}


char *str_to_upper(char *str) {
    for (int i = 0; str[i]; i++)
        str[i] = toupper((unsigned char)str[i]);
    return str;
}
