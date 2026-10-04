Step 1: Header Files

Sabse pehle program mein required libraries include ki gayi hain:

stdio.h → input/output ke liye
stdlib.h → strtol(), qsort() etc.
string.h → string operations
time.h → patient arrival aur treatment ka time

Step 2: Constants

Program ki maximum capacity aur string sizes define ki gayi hain.

#define MAX_PATIENTS 256
#define NAME_LEN 60
#define COMPLAINT_LEN 90

Matlab system mein maximum 256 patients store ho sakte hain.

Step 3: Patient Status

Patient ki current condition ko 3 status mein divide kiya gaya hai:

BOOKED → appointment booked
WAITING → queue mein wait kar raha hai
IN_TREATMENT → doctor ke paas treatment chal raha hai

Step 4: Patient Structure

struct ke andar patient ki complete information store hoti hai:

Patient ID
Name
Age
Complaint
ESI level
Status
Arrival time
Treatment start time
Appointment note
Payment information

Step 5: Patient Array
static Patient patients[MAX_PATIENTS];

Ye array saare patients ka record store karta hai.

patient_count

batata hai ki abhi kitne patients registered hain.

next_id

next patient ko ID dene ke liye use hota hai.

Step 6: ESI Priority

Emergency patients ko ESI level 1–5 diya jata hai.

ESI 1 → Resuscitation
ESI 2 → Emergent
ESI 3 → Urgent
ESI 4 → Less urgent
ESI 5 → Non-urgent

Lower ESI number ka matlab higher priority.

Step 7: Patient Registration

Option 1 – Register walk-in patient choose karne par:

Patient ka naam liya jata hai.
Age li jati hai.
Complaint li jati hai.
ESI level select kiya jata hai.
Patient ka status WAITING hota hai.
Arrival time save hota hai.
Fee calculate hoti hai.

Step 8: Appointment Booking

Option 2 se patient future appointment book kar sakta hai.

Patient:

Name → Age → Reason → Preferred Date/Time

enter karta hai.

Is stage par patient ka status BOOKED hota hai aur ESI baad mein check-in ke time assign hota hai.

Step 9: Check-In

Option 3 mein booked patient apni Patient ID deta hai.

System:

BOOKED → WAITING

mein patient ko convert karta hai aur ESI level assign karta hai.

Step 10: Waiting Queue

Yahi project ka main DSA concept – Priority Queue hai.

Waiting patients ko pehle:

ESI priority

ke according arrange kiya jata hai.

Agar 2 patients ka ESI same hai, to jo pehle aaya hai usko pehle rakha jata hai.

qsort() aur compare_waiting() is sorting mein help karte hain.

Step 11: Display Waiting Patients

Option 4 se sirf currently waiting patients display hote hain.

System:

Priority → Arrival Time → Waiting Time

ke basis par list show karta hai aur average waiting time bhi calculate karta hai.

Step 12: Start Treatment

Option 6 mein Patient ID enter karne par:

WAITING
   ↓
IN_TREATMENT

Patient ka treatment start time bhi record hota hai.

Step 13: Payment

Option 7 mein payment process hoti hai.

Payment methods:

Card
UPI/Mobile Wallet
Cash

Payment complete hone ke baad patient ka status:

UNPAID → PAID

ho jata hai aur receipt display hoti hai.

Step 14: Discharge

Option 8 se Patient ID enter karke patient discharge hota hai.

Patient ka record array se remove ho jata hai aur remaining records shift ho jate hain.

Step 15: Main Menu

Program ka main menu ye hai:

1) Register walk-in patient
2) Book appointment online
3) Check in a booked patient
4) Display waiting patients
5) View full queue
6) Start treatment
7) Process payment
8) Discharge patient
0) Exit

main() mein switch-case ke through selected operation call hota hai.

🔄 Complete Project Flow
START
  ↓
Main Menu
  ↓
Register / Book Appointment
  ↓
Check-In
  ↓
ESI Triage
  ↓
Priority Queue
  ↓
Waiting Patients
  ↓
Start Treatment
  ↓
Payment
  ↓
Discharge
  ↓
EXIT
