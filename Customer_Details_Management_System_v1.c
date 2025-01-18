#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h> // Include this header for tolower function

struct contact {
    long ph;
    char name[20], add[20], email[30], due[10], dtd[15]; // dtd = Due Taking Date.
} list;

char query[20], name[20];
FILE* fp, * ft;
int ch, found;

void add_contact() {
    fp = fopen("contact.dat", "a");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }

    while (1) {
        printf("\n\n\t==> Enter Name (Leave blank to exit)\t: ");
        fgets(list.name, sizeof(list.name), stdin);
        list.name[strcspn(list.name, "\n")] = 0; // Remove newline

        if (strlen(list.name) == 0) break;

        printf("\t==> Phone\t\t\t\t: ");
        scanf("%ld", &list.ph);
        getchar(); // Clear newline from buffer

        printf("\t==> Address\t\t\t\t: ");
        fgets(list.add, sizeof(list.add), stdin);
        list.add[strcspn(list.add, "\n")] = 0;

        printf("\t==> Email\t\t\t\t: ");
        fgets(list.email, sizeof(list.email), stdin);
        list.email[strcspn(list.email, "\n")] = 0;

        printf("\t==> Amount Due\t\t\t\t: ");
        fgets(list.due, sizeof(list.due), stdin);
        list.due[strcspn(list.due, "\n")] = 0;

        printf("\t==> Due Date\t\t\t\t: ");
        fgets(list.dtd, sizeof(list.dtd), stdin);
        list.dtd[strcspn(list.dtd, "\n")] = 0;

        fwrite(&list, sizeof(list), 1, fp);
    }

    fclose(fp);
}

void list_contacts() {
    fp = fopen("contact.dat", "r");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }

    printf("\nList of Contacts:\n");
    printf("====================================\n\n");

    while (fread(&list, sizeof(list), 1, fp)) {
        printf("Name\t\t: %s\nPhone\t\t: %ld\nAddress\t\t: %s\nEmail\t\t: %s\nDue\t\t: %s\nDue Date\t: %s\n\n",
            list.name, list.ph, list.add, list.email, list.due, list.dtd);
    }

    fclose(fp);
}

void search_contact() {
    printf("\n..:: Enter name to search: ");
    fgets(query, sizeof(query), stdin);
    query[strcspn(query, "\n")] = 0;

    fp = fopen("contact.dat", "r");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }

    found = 0;
    while (fread(&list, sizeof(list), 1, fp)) {
        if (strcasecmp(list.name, query) == 0) {
            printf("\n\tName\t\t: %s\n\tPhone\t\t: %ld\n\tAddress\t\t: %s\n\tEmail\t\t: %s\n\tDue\t\t: %s\n\tDue Date\t: %s\n\n",
                list.name, list.ph, list.add, list.email, list.due, list.dtd);
            found = 1;
        }
    }

    if (!found) {
        printf("\n\t! No contact found with that name.\n");
    }

    fclose(fp);
}

void edit_contact() {
    printf("\n..:: Enter name to edit: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    fp = fopen("contact.dat", "r");
    ft = fopen("temp.dat", "w");

    if (fp == NULL || ft == NULL) {
        perror("Unable to open file");
        return;
    }

    while (fread(&list, sizeof(list), 1, fp)) {
        if (strcasecmp(list.name, name) != 0) {
            fwrite(&list, sizeof(list), 1, ft);
        }
        else {
            printf("Editing contact: %s\n", name);
            printf("New Name: ");
            fgets(list.name, sizeof(list.name), stdin);
            list.name[strcspn(list.name, "\n")] = 0;

            printf("New Phone: ");
            scanf("%ld", &list.ph);
            getchar(); // Clear newline

            printf("New Address: ");
            fgets(list.add, sizeof(list.add), stdin);
            list.add[strcspn(list.add, "\n")] = 0;

            printf("New Email: ");
            fgets(list.email, sizeof(list.email), stdin);
            list.email[strcspn(list.email, "\n")] = 0;

            printf("New Amount Due: ");
            fgets(list.due, sizeof(list.due), stdin);
            list.due[strcspn(list.due, "\n")] = 0;

            printf("New Due Date: ");
            fgets(list.dtd, sizeof(list.dtd), stdin);
            list.dtd[strcspn(list.dtd, "\n")] = 0;

            fwrite(&list, sizeof(list), 1, ft);
        }
    }

    fclose(fp);
    fclose(ft);

    remove("contact.dat");
    rename("temp.dat", "contact.dat");
}

void delete_contact() {
    printf("\n\t==> Enter name to delete: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = 0;

    fp = fopen("contact.dat", "r");
    ft = fopen("temp.dat", "w");

    if (fp == NULL || ft == NULL) {
        perror("Unable to open file");
        return;
    }

    while (fread(&list, sizeof(list), 1, fp)) {
        if (strcasecmp(list.name, name) != 0) {
            fwrite(&list, sizeof(list), 1, ft);
        }
    }

    fclose(fp);
    fclose(ft);

    remove("contact.dat");
    rename("temp.dat", "contact.dat");

    printf("\n\tContact '%s' deleted successfully.\n", name);
}

void export_contacts() {
    fp = fopen("contact.dat", "r");
    FILE* csv_fp = fopen("contacts.csv", "w");

    if (fp == NULL || csv_fp == NULL) {
        perror("Unable to open file");
        return;
    }

    fprintf(csv_fp, "Name,Phone,Address,Email,Due,Due Date\n");

    while (fread(&list, sizeof(list), 1, fp)) {
        fprintf(csv_fp, "%s,%ld,%s,%s,%s,%s\n", list.name, list.ph, list.add, list.email, list.due, list.dtd);
    }

    fclose(fp);
    fclose(csv_fp);

    printf("\n\t==> Contacts have been successfully exported to contacts.csv\n");
}

void import_contacts() {
    FILE* csv_fp = fopen("contacts.csv", "r");

    if (csv_fp == NULL) {
        perror("Unable to open file");
        return;
    }

    // Skip the header line
    char buffer[256];
    fgets(buffer, sizeof(buffer), csv_fp);

    while (fgets(buffer, sizeof(buffer), csv_fp)) {
        sscanf(buffer, "%19[^,],%ld,%19[^,],%29[^,],%9[^,],%14[^\n]", list.name, &list.ph, list.add, list.email, list.due, list.dtd);

        fp = fopen("contact.dat", "a");
        if (fp == NULL) {
            perror("Unable to open file");
            return;
        }

        fwrite(&list, sizeof(list), 1, fp);
        fclose(fp);
    }

    fclose(csv_fp);

    printf("Contacts have been successfully imported from contacts.csv\n");
}

void set_password() {
    char password[20], confirm_password[20];

    printf("\n\t==> Enter a new password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0;

    printf("\t\tConfirm password: ");
    fgets(confirm_password, sizeof(confirm_password), stdin);
    confirm_password[strcspn(confirm_password, "\n")] = 0;

    if (strcmp(password, confirm_password) == 0) {
        FILE* password_file = fopen("password.txt", "w");
        if (password_file == NULL) {
            perror("Unable to open file");
            return;
        }

        fwrite(password, sizeof(password), 1, password_file);
        fclose(password_file);

        printf("\nPassword has been successfully set.\n\n");
    }
    else {
        printf("\nPasswords do not match. Try again.\n\n");
    }
}

int validate_password() {
    char entered_password[20], stored_password[20];

    printf("\n\t\t\t==> Enter password: ");
    fgets(entered_password, sizeof(entered_password), stdin);
    entered_password[strcspn(entered_password, "\n")] = 0;

    FILE* password_file = fopen("password.txt", "r");
    if (password_file == NULL) {
        return 0;
    }

    fread(stored_password, sizeof(stored_password), 1, password_file);
    fclose(password_file);

    if (strcmp(entered_password, stored_password) == 0) {
        return 1; // Valid password
    }
    else {
        printf("Incorrect password.\n");
        return 0; // Invalid password
    }
}

void display_statistics() {
    fp = fopen("contact.dat", "r");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }

    int total_contacts = 0, due_contacts = 0, alphabetical_count[26] = { 0 };

    while (fread(&list, sizeof(list), 1, fp)) {
        total_contacts++;
        if (atoi(list.due) > 0) {
            due_contacts++;
        }
        alphabetical_count[tolower(list.name[0]) - 'a']++;
    }

    fclose(fp);

    printf("\n\tTotal contacts: %d\n", total_contacts);
    printf("\tContacts with due amounts: %d\n", due_contacts);

    printf("\t Contacts by letter distribution => ");
    for (int i = 0; i < 26; i++) {
        if (alphabetical_count[i] > 0) {
            printf("%c: %d\n", 'A' + i, alphabetical_count[i]);
        }
    }
}


int compare_by_name(const void* a, const void* b) {
    return strcmp(((struct contact*)a)->name, ((struct contact*)b)->name);
}

int compare_by_phone(const void* a, const void* b) {
    return ((struct contact*)a)->ph - ((struct contact*)b)->ph;
}

void sort_contacts() {
    fp = fopen("contact.dat", "r");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }

    // Get the total number of contacts
    fseek(fp, 0, SEEK_END);
    int total_contacts = ftell(fp) / sizeof(list);
    fseek(fp, 0, SEEK_SET);

    struct contact contacts[total_contacts];
    fread(contacts, sizeof(struct contact), total_contacts, fp);
    fclose(fp);

    int choice;
    printf("Sort by:\n1. Name\n2. Phone\n3. Due Date\nEnter choice: ");
    scanf("%d", &choice);
    getchar(); // Clear newline

    switch (choice) {
    case 1:
        qsort(contacts, total_contacts, sizeof(struct contact), compare_by_name);
        break;
    case 2:
        qsort(contacts, total_contacts, sizeof(struct contact), compare_by_phone);
        break;
    case 3:
        qsort(contacts, total_contacts, sizeof(struct contact), compare_by_name);  // Sort by name for now (implement sorting by due date)
        break;
    default:
        printf("Invalid choice.\n");
        return;
    }

    // Write the sorted contacts back to the file
    fp = fopen("contact.dat", "w");
    if (fp == NULL) {
        perror("Unable to open file");
        return;
    }

    fwrite(contacts, sizeof(struct contact), total_contacts, fp);
    fclose(fp);

    printf("Contacts have been successfully sorted.\n");
}

int validate_password(); // Forward declaration

int main() {

    printf("\n\n\n\t..:: Customer Details Management System ::..\n");
    printf("============================================================\n\n");

    FILE* password_file = fopen("password.txt", "r");
    if (password_file == NULL) {
        set_password();
        fclose(password_file);
    }
    else {
        if (validate_password()) { // Correct function name

            while (1) {
                printf("\n==> MAIN MENU\t:\n\n");
                printf("\t\t1. Add a new Contact\n");
                printf("\t\t2. List all Contacts\n");
                printf("\t\t3. Search for Contact\n");
                printf("\t\t4. Edit a Contact\n");
                printf("\t\t5. Delete a Contact\n");
                printf("\t\t6. Export Contacts to CSV\n");
                printf("\t\t7. Import Contacts from CSV\n");
                printf("\t\t8. Set Password\n");
                printf("\t\t9. Display Statistics\n");
                printf("\t\t10. Sort Contacts\n");
                printf("\t\t0. Exit\n");
                printf("\n\n..:: Enter your choice: ");
                scanf("%d", &ch);
                getchar(); // To consume the newline character after scanf

                if (ch == 8) {
                    set_password();
                    continue;;
                }
                switch (ch) {
                case 1:
                    add_contact();
                    break;
                case 2:
                    list_contacts();
                    break;
                case 3:
                    search_contact();
                    break;
                case 4:
                    edit_contact();
                    break;
                case 5:
                    delete_contact();
                    break;
                case 6:
                    export_contacts();
                    break;
                case 7:
                    import_contacts();
                    break;
                case 9:
                    display_statistics();
                    break;
                case 10:
                    sort_contacts();
                    break;
                case 0:
                    printf("Exiting the system.\n");
                    return 0;
                default:
                    printf("\nInvalid choice! Please try again.\n");
                }
            }
        }
        else {
            printf("\n\nIncorrect password. Please try again.\n\n");
        }

        return 0; // Ensure main function ends correctly
    }

}
