......Hospital emergency queue management......
              *  project  *

1. Main Objectives

* Register walk-in patients
* Book appointments
* Check in booked patients
* Manage emergency priority
* Display waiting patients
* Start treatment
* Process payments
* Discharge patients

2. Header Files

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
Explanation
stdio.h → Input and output functions such as printf() and fgets()
stdlib.h → Functions such as strtol() and qsort()
string.h → String functions such as strlen() and strcmp()
time.h → Used for arrival time, treatment time, and waiting time
Presentation line:
“These header files provide input-output, string operations, sorting, and time-related functionality.”

3. Constants

#define MAX_PATIENTS 256
#define NAME_LEN 60
#define COMPLAINT_LEN 90
These constants define the maximum sizes used by the program.
For example:
MAX_PATIENTS = 256
means the system can store up to 256 patients.

4. Patient Status

typedef enum { BOOKED, WAITING, IN_TREATMENT } Status;
There are three patient statuses:
Status
Meaning
BOOKED
Appointment has been booked
WAITING
Patient is waiting for treatment
IN_TREATMENT
Patient is currently being treated
Say:
“I used an enumeration to represent the current status of each patient.”

5. Patient Structure

The most important part of the program is the Patient structure.
typedef struct {
    int id;
    char patient_id[PID_LEN];
    char name[NAME_LEN];
    int age;
    char complaint[COMPLAINT_LEN];
    int esi_level;
    Status status;
    time_t arrived;
    time_t treat_start;
    char appt_note[APPT_NOTE_LEN];
    int paid;
    double amount_due;
    char payment_method[METHOD_LEN];
} Patient;
This structure stores all information about a patient.
Important fields
id → Internal patient number
patient_id → Unique ID such as PT0001
name → Patient name
age → Patient age
complaint → Patient's complaint
esi_level → Emergency priority
status → Current patient status
arrived → Arrival time
treat_start → Treatment start time
paid → Payment status
amount_due → Consultation fee
payment_method → Card, UPI, or Cash

6. ESI Priority System

The project uses ESI (Emergency Severity Index).
ESI 1 → Resuscitation
ESI 2 → Emergent
ESI 3 → Urgent
ESI 4 → Less urgent
ESI 5 → Non-urgent
Important: ESI 1 has the highest priority.
Therefore, the waiting queue is organized like:
ESI 1
  ↓
ESI 2
  ↓
ESI 3
  ↓
ESI 4
  ↓
ESI 5
Say:
“The ESI level determines the priority of a patient. A more critical patient is treated before a less critical patient.”

7. Waiting Time

static const int esi_target_minutes[6] =
{0, 0, 10, 30, 60, 120};
This defines the target waiting time for each ESI level.
For example:
ESI 2 → 10 minutes
ESI 3 → 30 minutes
ESI 4 → 60 minutes
ESI 5 → 120 minutes
If a patient waits longer than the target time, the program displays:
[OVERDUE

8. Consultation Fee

static const double esi_fee[6] =
{0, 0, 75.00, 50.00, 30.00, 20.00};
The consultation fee depends on the ESI level.
ESI Level
Fee
ESI 1
Billed later
ESI 2
$75
ESI 3
$50
ESI 4
$30
ESI 5
$20

9. Register Walk-In Patient

Function:
static void register_walkin(void)
This function registers a patient who comes directly to the emergency department.
Steps:
Create a new patient record.
Enter patient name.
Enter age.
Enter chief complaint.
Select ESI level.
Set status to WAITING.
Record arrival time.
Calculate the consultation fee.
Example:
Registered PT0001 as ESI 3 - Urgent
Say:
“After registration, the patient is immediately added to the waiting queue with an assigned emergency priority.”

10. Book Appointment

Function:
static void book_appointment(void)
This function is used to book an appointment in advance.
The program asks for:
Patient Name
Age
Reason for Visit
Preferred Date/Time
The status is set to:
BOOKED
The patient does not enter the waiting queue until they actually arrive and check in.

11. Check-In

Function:
static void check_in(void)
When a booked patient arrives, the system changes:
BOOKED
   ↓
WAITING
At check-in, the ESI level is assigned.
The arrival time is also recorded.
Say:
“Check-in converts a booked appointment into an active waiting patient.”

12. Priority Queue and Sorting

This is the main DSA concept in the project.
The function:
static int compare_waiting(...)
compares patients based on:
ESI level
Arrival time
The program uses:
qsort()
to sort the waiting patients.
Example
Suppose patients are:
PT0001 → ESI 4
PT0002 → ESI 2
PT0003 → ESI 3
PT0004 → ESI 2
The queue becomes:
PT0002 → ESI 2
PT0004 → ESI 2
PT0003 → ESI 3
PT0001 → ESI 4
For patients with the same ESI level, the patient who arrived earlier comes first.
Say:
“This implements a priority queue concept where emergency severity is considered before normal arrival order.”

13. Display Waiting Patients

Function:
static void display_waiting(void)
This function displays only the patients who are currently waiting.
It:
Collects waiting patients.
Sorts them.
Displays their information.
Calculates their waiting time.
Calculates the average waiting time.
Example:
=== Waiting patients (3) ===

PT0002  John   ESI 2 - Emergent
PT0004  David  ESI 3 - Urgent
PT0001  Alex   ESI 4 - Less urgent

Average wait: 25 min

14. Start Treatment

Function:
static void start_treatment(void)
When treatment starts:
WAITING
   ↓
IN_TREATMENT
The treatment start time is recorded using:
p->treat_start = time(NULL);
Say:
“This function moves a waiting patient into treatment and records the treatment start time.”

15. Payment System

Function:
static void process_payment(void)
The system supports three payment methods:
1. Card
2. UPI/Mobile Wallet
3. Cash
After successful payment:
p->paid = 1;
The program prints a receipt containing:
Patient
Service
Amount
Payment Method
Payment Status
Example:
--- Receipt ---
Patient: PT0002 (John)
Service: ESI 3 - Urgent
Amount: $50.00
Method: Cash
Status: PAID

16. Discharge Patient

Function:
static void discharge_patient(void)
This function removes a patient from the system.
The program searches for the patient using their ID.
When the patient is found, the remaining array elements are shifted left:
for (int j = i; j < patient_count - 1; j++)
    patients[j] = patients[j + 1];
Then:
patient_count--;
Say:
“This demonstrates deletion from an array because after removing a patient, the remaining records are shifted to maintain continuous storage.”

17. Main Function

The program starts from:
int main(void)
The main function continuously displays the menu.
1) Register walk-in patient
2) Book appointment online
3) Check in a booked patient
4) Display waiting patients
5) View full queue
6) Start treatment
7) Process payment
8) Discharge patient
0) Exit
The switch statement calls the appropriate function.
For example:
case 1: register_walkin(); break;
case 2: book_appointment(); break;
case 6: start_treatment(); break;

18. Complete Project Flow

Remember this flow for your presentation:
                 START
                   ↓
              Display Menu
                   ↓
        ┌──────────┴──────────┐
        ↓                     ↓
 Register Patient       Book Appointment
        ↓                     ↓
  Assign ESI              BOOKED
        ↓                     ↓
     WAITING          Patient Arrives
        ↓                     ↓
        └───────→ CHECK-IN ←──┘
                   ↓
                 WAITING
                   ↓
            Priority Sorting
                   ↓
            Start Treatment
                   ↓
             IN_TREATMENT
                   ↓
             Process Payment
                   ↓
               Discharge
                   ↓
                  END

19. DSA Concepts Used

If your teacher asks “Where is DSA used in your project?”, answer:
“The main DSA concepts used in my project are arrays, structures, searching, sorting, and priority queue.”
1. Array
static Patient patients[MAX_PATIENTS];
Used to store patient records.
2. Structure
typedef struct { ... } Patient;
Used to store different types of patient information together.
3. Priority Queue
Used to give priority to emergency patients according to ESI level.
4. Sorting
qsort()
Used to arrange waiting patients according to priority and arrival time.
5. Searching
find_by_patient_id()
Used to find a patient using their patient ID.
6. Array Deletion
During discharge, the remaining records are shifted left.

20. Final Presentation Ending

You can end your presentation with:
“In conclusion, the ER Queue Management System provides an organized way to manage emergency patients. It uses C programming concepts such as structures, arrays, functions, enumeration, searching, sorting, and priority queue logic. The system manages the complete patient flow from registration and check-in to treatment, payment, and discharge. Thank you.”
