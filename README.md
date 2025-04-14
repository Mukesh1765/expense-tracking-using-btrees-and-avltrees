# 💸 Family Expense Tracking System (AVL Tree + B-Tree)

This C-based project implements a powerful **Family Expense Tracking System** that enables efficient management and analysis of personal and household expenses. It supports real-time user and family tracking with a focus on performance, structure, and data integrity.

The system uses:
- ✅ **AVL Tree** for managing users
- 🌳 **B-Tree** for managing families
- 🌳 **B-Tree** for managing expenses

> All data is initialized via file handling from unsorted input, and loaded into sorted structures dynamically at runtime.

---

## ✨ Features

### 📌 Core Functionalities
- Add and manage individual users using **AVL Tree** (sorted by User ID)
- Add and manage expense records using **B-Tree** (sorted by User ID and Expense ID)
- Create and manage families using **B-Tree** (sorted by Family ID)
- File handling support for loading initial **unsorted data**

---

### 📊 Expense Analytics
- 🔹 **Get Total Family Expense** – with income vs expense comparison
- 🔹 **Categorical Expense Analysis** – total and individual-wise breakdown per category
- 🔹 **Highest Expense Day** – find the date with maximum family expense
- 🔹 **Individual Expense Report** – category-wise breakdown in descending order
- 🔹 **Period-Based Expense Query** – all expenses between two dates
- 🔹 **Range-Based Expense Query** – all expenses between two expense IDs for a user

---

### ⚙️ System Intelligence
- 🧠 Automatically deletes family if last member is removed
- 🔁 Auto-updates family income and expense when individual/expense is updated
- 🔍 Tree-based searching and updates in **O(log n)** time
- If user is removed, then all his expenses are removed and his contribution to family and th user are also removed from family
- Input validations in all functions

---

### 📂 File Handling
Supports file-based initialization with **unsorted data**, which is parsed and inserted into:
- `users.txt` – at least 25 user records
- `families.txt` – at least 10 families (1–4 members each)
- `expenses.txt` – at least 40 expenses across multiple categories

---

## 🌲 Data Structures

### 👤 Individual (AVL Tree)
```c
typedef struct user {
    int user_ID;             // Unique ID (1-1000)
    char user_name[100];
    float income;
} user;

typedef struct user_node {
    user data;
    int height;              // For AVL balancing
    struct user_node *left, *right;
} user_tree_node;
```

---

### 🧾 Expense (B-Tree)
```c
typedef struct expense {
    int expense_ID;
    int user_ID;             // Linked to User ID
    char expense_category[MAX_EXPENSE_CATEGORY_SIZE]; // Rent, Utility, Grocery, etc.
    float expense_amount;
    Date date_of_expense;
} expense;

typedef struct expense_tree_node {
    expense deck[ORDER];                    // Array of expenses in B-Tree node
    struct expense_tree_node *children[ORDER + 1];
    int keys;
} expense_tree_node;
```

---

### 🏠 Family (B-Tree)
```c
typedef struct family {
    int family_ID;
    char family_name[MAX_NAME_LENGTH];
    int total_members;
    int family_members_user_id[MAX_USERS_IN_FAMILY]; // Max 4 members
    float total_family_income;
    float total_family_expense;
} family;

typedef struct family_node {
    family deck[ORDER];
    struct family_node* children[ORDER + 1];
    int keys;
} family_tree_node;
```

---

## 🔧 Functional Overview

| Function | Description |
|---------|-------------|
| `AddUser()` | Adds a new user to AVL Tree (sorted by user_ID). |
| `AddExpense()` | Adds a new expense into B-Tree (grouped by user, sorted by expense_ID). |
| `CreateFamily()` | Creates a family of 1–4 members, calculates income and expenses. |
| `Update_or_delete_individual_Family_details()` | Updates/deletes user or family. Auto-syncs related data. |
| `Update_delete_expense()` | Updates/deletes an expense. Recalculates totals. |
| `Get_total_expense()` | Prints family expense and whether it exceeds income. |
| `Get_categorical_expense(category)` | Shows total & individual expense per category. |
| `Get_highest_expense_day()` | Finds the day with highest family spending. |
| `Get_individual_expense(userID)` | Shows individual monthly & category-wise expense. |
| `Get_expense_in_period(Date1, Date2)` | Lists expenses within a given date range. |
| `Get_expense_in_range(ID1, ID2, userID)` | Lists expenses in ID range for a user. |

---

## ⚙️ Assumptions

- Max number of users: `1000`
- Max number of expenses: `1000`
- Max number of families: `100`
- Unique user and expense IDs in range `1–1000`
- Each family has 1 to 4 users
- Income/Expense: Float values (2 decimal precision)
- Assume 10 days in each month
- All input is initially **unsorted** and gets sorted upon insertion

---

## 🧪 Compilation & Execution

```bash
gcc projectTrees.c -o expense_tracker
./expense_tracker
```

Ensure `users.txt`, `families.txt`, and `expenses.txt` exist in the same directory with valid unsorted data.

---

## 📈 Future Enhancements

- 🔲 Graphical User Interface (GUI)
- 🔔 Alert system for overspending
- 📊 Budget goal tracking per family

---

## 📜 License

This project is licensed under the **MIT License** – feel free to use and contribute!

---

## 🙌 Authors

Developed with 💻 and logic by **[Mukesh Sai]**

---

> 📌 *Feel free to fork, clone, and extend this project for your college assignments, personal tools, or larger system integrations!*
