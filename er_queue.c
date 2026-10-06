#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_PATIENTS     256
#define NAME_LEN          60
#define COMPLAINT_LEN     90
#define APPT_NOTE_LEN     40
#define PID_LEN            8
#define METHOD_LEN        16

typedef enum { BOOKED, WAITING, IN_TREATMENT } Status;

typedef struct {
    int    id;                      
    char   patient_id[PID_LEN];     
    char   name[NAME_LEN];
    int    age;                     
    char   complaint[COMPLAINT_LEN];
    int    esi_level;               
    Status status;
    time_t arrived;                 
    time_t treat_start;             
    char   appt_note[APPT_NOTE_LEN];
    int    paid;                    
    double amount_due;
    char   payment_method[METHOD_LEN];
} Patient;

static Patient patients[MAX_PATIENTS];
static int     patient_count = 0;
static int     next_id       = 1;

/* Target wait time (minutes) before a waiting patient is flagged overdue,
 * indexed by ESI level. */
static const int esi_target_minutes[6] = { 0, 0, 10, 30, 60, 120 };

/* Consultation fee by ESI level. Level 1 (resuscitation) is billed
 * separately through the trauma team, so it's shown as "billed later". */
static const double esi_fee[6] = { 0, 0, 75.00, 50.00, 30.00, 20.00 };

static const char *esi_label(int level) {
    switch (level) {
        case 1: return "ESI 1 - Resuscitation";
        case 2: return "ESI 2 - Emergent";
        case 3: return "ESI 3 - Urgent";
        case 4: return "ESI 4 - Less urgent";
        case 5: return "ESI 5 - Non-urgent";
        default: return "Not yet triaged";
    }
}

static void flush_stdin_line(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void read_line(char *buf, size_t len) {
    if (fgets(buf, (int)len, stdin) == NULL) { buf[0] = '\0'; return; }
    size_t n = strlen(buf);
    if (n > 0 && buf[n - 1] == '\n') {
        buf[n - 1] = '\0';
    } else {
        flush_stdin_line(); /* input was longer than the buffer */
    }
}

static int read_int(const char *prompt, int min, int max) {
    char line[32];
    long val;
    char *end;
    for (;;) {
        printf("%s", prompt);
        read_line(line, sizeof(line));
        val = strtol(line, &end, 10);
        if (end != line && val >= min && val <= max) {
            return (int)val;
        }
        printf("  Enter a whole number between %d and %d.\n", min, max);
    }
}

static const char *format_wait(time_t seconds_elapsed, char *out, size_t out_len) {
    long total_min = (long)(seconds_elapsed / 60);
    if (total_min < 60) {
        snprintf(out, out_len, "%ldm", total_min);
    } else {
        snprintf(out, out_len, "%ldh %ldm", total_min / 60, total_min % 60);
    }
    return out;
}

static int compare_waiting(const void *a, const void *b) {
    const Patient *pa = *(const Patient * const *)a;
    const Patient *pb = *(const Patient * const *)b;
    if (pa->esi_level != pb->esi_level) return pa->esi_level - pb->esi_level;
    if (pa->arrived != pb->arrived) return (pa->arrived < pb->arrived) ? -1 : 1;
    return 0;
}

static Patient *find_by_patient_id(const char *pid) {
    for (int i = 0; i < patient_count; i++) {
        if (strcmp(patients[i].patient_id, pid) == 0) return &patients[i];
    }
    return NULL;
}

static Patient *prompt_find_patient(const char *label) {
    char pid[PID_LEN];
    printf("%s (e.g. PT0001): ", label);
    read_line(pid, sizeof(pid));
    Patient *p = find_by_patient_id(pid);
    if (!p) printf("  No patient with ID %s.\n", pid);
    return p;
}

/* ---------------- Registration / booking ---------------- */

static Patient *new_patient_slot(void) {
    if (patient_count >= MAX_PATIENTS) {
        printf("Queue is full (%d patients). Discharge someone before adding more.\n", MAX_PATIENTS);
        return NULL;
    }
    Patient *p = &patients[patient_count++];
    memset(p, 0, sizeof(*p));
    p->id = next_id++;
    snprintf(p->patient_id, sizeof(p->patient_id), "PT%04d", p->id);
    return p;
}

/* Walk-in patient: triaged and placed in the live waiting queue now. */
static void register_walkin(void) {
    Patient *p = new_patient_slot();
    if (!p) return;

    printf("Patient name: ");
    read_line(p->name, sizeof(p->name));
    if (p->name[0] == '\0') {
        printf("  Name is required. Registration cancelled.\n");
        patient_count--; next_id--;
        return;
    }

    p->age = read_int("Age (or -1 to skip): ", -1, 130);

    printf("Chief complaint: ");
    read_line(p->complaint, sizeof(p->complaint));

    printf("Triage level:\n");
    for (int lvl = 1; lvl <= 5; lvl++) printf("  %d) %s\n", lvl, esi_label(lvl));
    p->esi_level = read_int("Select 1-5: ", 1, 5);

    p->status  = WAITING;
    p->arrived = time(NULL);
    p->amount_due = esi_fee[p->esi_level];

    printf("Registered %s (%s) as %s.\n", p->patient_id, p->name, esi_label(p->esi_level));
}

/* Online booking: reserves a patient ID and a preferred slot ahead of
 * arrival. No wait clock runs and no triage is assigned until check-in. */
static void book_appointment(void) {
    Patient *p = new_patient_slot();
    if (!p) return;

    printf("Patient name: ");
    read_line(p->name, sizeof(p->name));
    if (p->name[0] == '\0') {
        printf("  Name is required. Booking cancelled.\n");
        patient_count--; next_id--;
        return;
    }

    p->age = read_int("Age (or -1 to skip): ", -1, 130);

    printf("Reason for visit: ");
    read_line(p->complaint, sizeof(p->complaint));

    printf("Preferred date/time (free text, e.g. 'Fri 2pm'): ");
    read_line(p->appt_note, sizeof(p->appt_note));

    p->status    = BOOKED;
    p->esi_level = 0; /* triaged at check-in */

    printf("Booked %s (%s) for %s.\n", p->patient_id, p->name,
           p->appt_note[0] ? p->appt_note : "an upcoming slot");
    printf("Give the patient their ID (%s) to check in with on arrival.\n", p->patient_id);
}

/* Converts a BOOKED patient into the live WAITING queue on arrival. */
static void check_in(void) {
    Patient *p = prompt_find_patient("Patient ID to check in");
    if (!p) return;
    if (p->status != BOOKED) {
        printf("  %s is not a pending booking.\n", p->patient_id);
        return;
    }

    printf("Confirm/assign triage level for %s:\n", p->name);
    for (int lvl = 1; lvl <= 5; lvl++) printf("  %d) %s\n", lvl, esi_label(lvl));
    p->esi_level = read_int("Select 1-5: ", 1, 5);

    p->status  = WAITING;
    p->arrived = time(NULL);
    p->amount_due = esi_fee[p->esi_level];

    printf("  %s checked in and added to the waiting queue as %s.\n",
           p->patient_id, esi_label(p->esi_level));
}

/* ---------------- Display ---------------- */

static void print_patient_row(const Patient *p) {
    char wait_buf[16];
    time_t now = time(NULL);

    printf("  %-7s %-20s %-22s", p->patient_id, p->name, esi_label(p->esi_level));

    if (p->status == WAITING) {
        time_t elapsed = now - p->arrived;
        int overdue = (elapsed / 60) > esi_target_minutes[p->esi_level];
        printf(" %-8s%s", format_wait(elapsed, wait_buf, sizeof(wait_buf)),
               overdue ? "  [OVERDUE]" : "");
    } else if (p->status == IN_TREATMENT) {
        time_t elapsed = now - p->treat_start;
        printf(" %-8s in treatment", format_wait(elapsed, wait_buf, sizeof(wait_buf)));
    } else { /* BOOKED */
        printf(" slot: %s", p->appt_note[0] ? p->appt_note : "(unspecified)");
    }

    printf("  [%s]", p->paid ? "PAID" : "UNPAID");
    printf("\n");

    if (p->complaint[0] != '\0') printf("          %s", p->complaint);
    if (p->age >= 0) printf("   age: %d", p->age);
    if (p->complaint[0] != '\0' || p->age >= 0) printf("\n");
}

/* Dedicated "display waiting patients" view: only checked-in patients
 * currently waiting, sorted by triage priority then arrival order. */
static void display_waiting(void) {
    Patient *waiting[MAX_PATIENTS];
    int wcount = 0;

    for (int i = 0; i < patient_count; i++) {
        if (patients[i].status == WAITING) waiting[wcount++] = &patients[i];
    }
    qsort(waiting, (size_t)wcount, sizeof(Patient *), compare_waiting);

    printf("\n=== Waiting patients (%d) ===\n", wcount);
    if (wcount == 0) {
        printf("  No patients currently waiting.\n\n");
        return;
    }
    for (int i = 0; i < wcount; i++) print_patient_row(waiting[i]);

    double total_sec = 0;
    time_t now = time(NULL);
    for (int i = 0; i < wcount; i++) total_sec += difftime(now, waiting[i]->arrived);
    printf("\nAverage wait: %.0f min\n\n", total_sec / wcount / 60.0);
}

static void list_all(void) {
    Patient *booked[MAX_PATIENTS], *waiting[MAX_PATIENTS], *treating[MAX_PATIENTS];
    int bcount = 0, wcount = 0, tcount = 0;

    for (int i = 0; i < patient_count; i++) {
        Patient *p = &patients[i];
        if      (p->status == BOOKED)       booked[bcount++]   = p;
        else if (p->status == WAITING)      waiting[wcount++]  = p;
        else                                treating[tcount++] = p;
    }
    qsort(waiting, (size_t)wcount, sizeof(Patient *), compare_waiting);
    qsort(treating, (size_t)tcount, sizeof(Patient *), compare_waiting);

    printf("\n--- Booked / upcoming (%d) ---\n", bcount);
    if (bcount == 0) printf("  No pending bookings.\n");
    for (int i = 0; i < bcount; i++) print_patient_row(booked[i]);

    printf("\n--- Waiting (%d) ---\n", wcount);
    if (wcount == 0) printf("  No patients waiting.\n");
    for (int i = 0; i < wcount; i++) print_patient_row(waiting[i]);

    printf("\n--- In treatment (%d) ---\n", tcount);
    if (tcount == 0) printf("  No patients currently in treatment.\n");
    for (int i = 0; i < tcount; i++) print_patient_row(treating[i]);
    printf("\n");
}

/* ---------------- Treatment / discharge ---------------- */

static void start_treatment(void) {
    Patient *p = prompt_find_patient("Patient ID to start treatment");
    if (!p) return;
    if (p->status != WAITING) { printf("  %s is not in the waiting queue.\n", p->patient_id); return; }
    p->status = IN_TREATMENT;
    p->treat_start = time(NULL);
    printf("  %s (%s) moved to treatment.\n", p->patient_id, p->name);
}

static void discharge_patient(void) {
    char pid[PID_LEN];
    printf("Patient ID to discharge (e.g. PT0001): ");
    read_line(pid, sizeof(pid));
    for (int i = 0; i < patient_count; i++) {
        if (strcmp(patients[i].patient_id, pid) == 0) {
            if (!patients[i].paid && patients[i].amount_due > 0) {
                printf("  Note: %s has an outstanding balance of $%.2f.\n",
                       patients[i].patient_id, patients[i].amount_due);
            }
            printf("  Discharged %s (%s).\n", patients[i].patient_id, patients[i].name);
            for (int j = i; j < patient_count - 1; j++) patients[j] = patients[j + 1];
            patient_count--;
            return;
        }
    }
    printf("  No patient with ID %s.\n", pid);
}

/* ---------------- Payment ---------------- */

static void process_payment(void) {
    Patient *p = prompt_find_patient("Patient ID to process payment");
    if (!p) return;

    if (p->esi_level == 0) {
        printf("  %s has not been triaged yet; check in first to establish the fee.\n", p->patient_id);
        return;
    }
    if (p->paid) {
        printf("  %s is already marked as paid ($%.2f via %s).\n",
               p->patient_id, p->amount_due, p->payment_method);
        return;
    }
    if (p->esi_level == 1) {
        printf("  %s is ESI 1 (Resuscitation) - billed later through case management, no charge collected now.\n",
               p->patient_id);
        return;
    }

    printf("  Consultation fee for %s: $%.2f\n", p->patient_id, p->amount_due);
    printf("Payment method:\n  1) Card\n  2) UPI/Mobile wallet\n  3) Cash\n");
    int m = read_int("Select 1-3: ", 1, 3);
    const char *methods[] = { "", "Card", "UPI/Mobile wallet", "Cash" };
    snprintf(p->payment_method, sizeof(p->payment_method), "%s", methods[m]);
    p->paid = 1;

    printf("\n--- Receipt ---\n");
    printf("  Patient:  %s (%s)\n", p->patient_id, p->name);
    printf("  Service:  %s\n", esi_label(p->esi_level));
    printf("  Amount:   $%.2f\n", p->amount_due);
    printf("  Method:   %s\n", p->payment_method);
    printf("  Status:   PAID\n---------------\n\n");
}

/* ---------------- Menu ---------------- */

static void print_menu(void) {
    printf("=================================\n");
    printf(" ER QUEUE MANAGEMENT\n");
    printf("=================================\n");
    printf(" 1) Register walk-in patient\n");
    printf(" 2) Book appointment online\n");
    printf(" 3) Check in a booked patient\n");
    printf(" 4) Display waiting patients\n");
    printf(" 5) View full queue (booked/waiting/treatment)\n");
    printf(" 6) Start treatment\n");
    printf(" 7) Process payment\n");
    printf(" 8) Discharge patient\n");
    printf(" 0) Exit\n");
}

int main(void) {
    int choice;
    do {
        print_menu();
        choice = read_int("Select an option: ", 0, 8);
        switch (choice) {
            case 1: register_walkin();    break;
            case 2: book_appointment();   break;
            case 3: check_in();           break;
            case 4: display_waiting();    break;
            case 5: list_all();           break;
            case 6: start_treatment();    break;
            case 7: process_payment();    break;
            case 8: discharge_patient();  break;
            case 0: printf("Goodbye.\n"); break;
        }
    } while (choice != 0);
    return 0;
}
