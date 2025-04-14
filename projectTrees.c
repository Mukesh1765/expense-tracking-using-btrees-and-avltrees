#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

#define MAX_USERS 1000
#define MAX_EXPENSES 1000
#define MAX_FAMILIES 100
#define MAX_USERS_IN_FAMILY 4
#define MAX_EXPENSE_CATEGORY_SIZE 15
#define TOTAL_DAYS 10
#define MAX_NAME_LENGTH 100
#define ORDER 5

const int min_children = (int)ceil(ORDER / 2.0);
const int min_keys = (int)ceil((ORDER - 1) / 2.0);

typedef enum {SUCCESS, FAILURE} statusCode;
typedef enum {false, true} bool;

typedef struct {
    unsigned int date;
    unsigned int month;
    unsigned int year;
} Date;

typedef struct user {
    int user_ID;//ranges from 1 to 1000
    char user_name[100];
    float income;
} user;

typedef struct expense {
    int expense_ID;
    int user_ID;
    char expense_category[MAX_EXPENSE_CATEGORY_SIZE]; // rent, utility, grocery, stationary, leisure;
    float expense_amount;
    Date date_of_expense;
} expense;

typedef struct family {
    int family_ID;
    char family_name[MAX_NAME_LENGTH];
    int total_members;
    int family_members_user_id[MAX_USERS_IN_FAMILY];
    float total_family_income;
    float total_family_expense;
} family;

typedef struct user_node {
    user data;
    int height;
    struct user_node *left;
    struct user_node *right;
} user_tree_node; //avl for users

typedef struct expense_tree_node {
    expense deck[ORDER];
    struct expense_tree_node *children[ORDER + 1];
    int keys;
} expense_tree_node; //b+ for expenses

typedef struct family_node {
    family deck[ORDER];
    struct family_node* children[ORDER + 1];
    int keys;
} family_tree_node; //b for family 

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void capitalize(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - ('a' - 'A');
        }
    }
}

static int user_count = 0;
static int expense_count = 0;
static int family_count = 0;

//user
user_tree_node* create_user_tree_node(user u) {
    user_tree_node* newNode = (user_tree_node*)malloc(sizeof(user_tree_node));
    if (newNode) {
        newNode->data = u;
        newNode->left = newNode->right = NULL;
        newNode->height = 1;
    } else {
        printf("user initialisation failed.\n");
    }
    return newNode;
}

user_tree_node* search_user(user_tree_node *root, int user_id) {
    user_tree_node *temp = root;
    while (temp != NULL && temp -> data.user_ID != user_id) {
        if (temp->data.user_ID > user_id) {
            temp = temp -> left;
        } else {
            temp = temp -> right;
        }
    }
    return temp;
}

int max(int a, int b) {
    return (a > b) ? a : b;
}

int height(user_tree_node *root) {
    return root ? root->height : 0;
}

int balance_factor(user_tree_node *root) {
    return root ? height(root->left) - height(root->right) : 0;
}

void updateHeight(user_tree_node* root) {
    if (root) {
        root->height = 1 + max(height(root->left), height(root->right));
    }
}

user_tree_node* rotate_right(user_tree_node *root) {
    user_tree_node *x = root->left;
    root->left = x->right;
    x->right = root;
    updateHeight(root);
    updateHeight(x);
    return x;
}

user_tree_node* rotate_left(user_tree_node *root) {
    user_tree_node *y = root->right;
    root->right = y->left;
    y->left = root;
    updateHeight(root);
    updateHeight(y);
    return y;
}

user_tree_node* rotate_left_right(user_tree_node *root) {
    root->left = rotate_left(root->left); 
    return rotate_right(root);
}

user_tree_node* rotate_right_left(user_tree_node *root) {
    root->right = rotate_right(root->right);
    return rotate_left(root);
}

user_tree_node* insert_user_helper(user_tree_node *root, user u) {
    user_tree_node *result = root;
    if (root == NULL) {
        result = create_user_tree_node(u);
    } else {
        if (u.user_ID < root->data.user_ID) {
            root->left = insert_user_helper(root->left, u);
        } else if (u.user_ID > root->data.user_ID) {
            root->right = insert_user_helper(root->right, u);
        } else {
            result = root;
        }
        updateHeight(root);
        int balance = balance_factor(root);
        if (balance > 1) { 
            if (u.user_ID < root->left->data.user_ID) {
                result = rotate_right(root);
            } else { 
                result = rotate_left_right(root);
            }
        } else if (balance < -1) { 
            if (u.user_ID > root->right->data.user_ID) { 
                result = rotate_left(root);
            } else { 
                result = rotate_right_left(root);
            }
        }
    }
    return result;
}

user_tree_node* insert_user(user_tree_node *root, user u) {
    user_count++;
    return insert_user_helper(root, u);
}

user_tree_node* FindMin(user_tree_node *root) {
    while (root && root->left) {
        root = root->left;
    }
    return root;
}

user_tree_node* delete_user_helper(user_tree_node *root, int user_id) {
    user_tree_node *result = root;
    if (root != NULL) {
        if (user_id < root->data.user_ID) {
            root->left = delete_user_helper(root->left, user_id);
        } else if (user_id > root->data.user_ID) {
            root->right = delete_user_helper(root->right, user_id);
        } else {
            if (root->left == NULL && root->right == NULL) {
                free(root);
                root = NULL;
                result = NULL;
            } else if (root->left != NULL && root->right == NULL) {
                user_tree_node *temp = root;
                root = root->left;
                result = root;
                free(temp);
            } else if (root->right != NULL && root->left == NULL) {
                user_tree_node *temp = root;
                root = root->right;
                result = root;
                free(temp);
            } else {
                user_tree_node *temp = FindMin(root->right); 
                root->data.user_ID = temp->data.user_ID;
                root->data.income = temp->data.income;
                strncpy(root->data.user_name, temp->data.user_name, sizeof(root->data.user_name) - 1);
                root->data.user_name[sizeof(root->data.user_name) - 1] = '\0'; 
                root->height = temp->height;
                root->right = delete_user_helper(root->right, temp->data.user_ID);
            }
        }
        updateHeight(root);
        
        int balance = balance_factor(root);
        if (balance > 1) { 
            if (balance_factor(root->left) >= 0) {
                result = rotate_right(root);
            } else { 
                result = rotate_left_right(root);
            }
        } else if (balance < -1) { 
            if (balance_factor(root->right) <= 0) { 
                result = rotate_left(root);
            } else { 
                result = rotate_right_left(root);
            }
        }
    }
    return result;
}

user_tree_node* delete_user(user_tree_node *root, int ind) {
    user_count--;
    return delete_user_helper(root, ind);
}

// expenses
void traverseExpenseTree(expense_tree_node* node) {
    if (node == NULL) return;

    for (int i = 0; i < node->keys; i++) {
        traverseExpenseTree(node->children[i]);

        expense exp = node->deck[i];
        printf("| %-10d | %-12d | %-15.2f | %-15s | %02u-%02u-%04u |\n",
               exp.user_ID,
               exp.expense_ID,
               exp.expense_amount,
               exp.expense_category,
               exp.date_of_expense.date,
               exp.date_of_expense.month,
               exp.date_of_expense.year);
    }
    traverseExpenseTree(node->children[node->keys]);
}

void traverseExpenses(expense_tree_node* root) {
    if (root == NULL) {
        printf("No expenses to display.\n");
        return;
    }

    const int user_id_width     = 10;
    const int expense_id_width  = 12;
    const int amount_width      = 15;
    const int category_width    = 15;
    const int date_width        = 12;

    printf("+------------+--------------+-----------------+-----------------+------------+\n");

    printf("| %-10s | %-12s | %-15s | %-15s | %-10s |\n",
           "User ID", "Expense ID", "Amount", "Category", "Date");

    printf("+------------+--------------+-----------------+-----------------+------------+\n");

    traverseExpenseTree(root);

    printf("+------------+--------------+-----------------+-----------------+------------+\n\n");
}

void initialize_BTreeNode_expense(expense_tree_node** node) {
    *node = (expense_tree_node*) malloc(sizeof(expense_tree_node));
    if (*node != NULL) {
        for (int i = 0; i <= ORDER; i++) {
            (*node)->children[i] = NULL;
        }
        (*node)->keys = 0;
        for (int i = 0; i < ORDER; i++) {
            (*node)->deck[i].expense_ID = 0;
            (*node)->deck[i].expense_amount = 0.0;
            (*node)->deck[i].expense_category[0] = '\0';
            (*node)->deck[i].user_ID = 0;
            (*node)->deck[i].date_of_expense.date = 0;
            (*node)->deck[i].date_of_expense.month = 0;
            (*node)->deck[i].date_of_expense.year = 0;
        }
    }
}

expense_tree_node* createBTreeNode_expense(expense r) {
    expense_tree_node *result = NULL;
    initialize_BTreeNode_expense(&result);
    if (result) {
        result->deck[0] = r;
        result->keys++;
    }
    return result;
}

expense_tree_node* createSpiltNode_expense(expense arr[], expense_tree_node *children[], int start, int end) {
    expense_tree_node *result;
    initialize_BTreeNode_expense(&result);
    if (result) {
        for (int i = start; i < end; i++) {
            result->deck[i - start] = arr[i];
        }
        for (int i = start; i <= end; i++) {
            result->children[i - start] = children[i];
        }
        result->keys = end - start;
    }
    return result;
}

expense_tree_node* insertInCorrectPlace_expense(expense_tree_node *node, expense r) {
    if (node == NULL) {
        node = createBTreeNode_expense(r);
        node->keys = 1;
    } else if (node->keys >= 0 && node->keys < ORDER) {
        int n = node->keys;
        node->keys += 1;
        int i = 0;
        while (i < n) {
            if (node->deck[i].user_ID < r.user_ID) {
                i++;
            } else if (node->deck[i].user_ID == r.user_ID && node->deck[i].expense_ID < r.expense_ID) {
                i++;
            } else {
                break;
            }
        }
        for (int j = node->keys - 2; j >= i; j--) node->deck[j + 1] = node->deck[j];
        node->deck[i] = r;
    }
    return node;
}

int find_index_expense(expense_tree_node *root, int n, expense r) {
    int i = 0;
    while (i < n) {
        if (root->deck[i].user_ID < r.user_ID) {
            i++;
        } else if (root->deck[i].user_ID == r.user_ID && root->deck[i].expense_ID < r.expense_ID) {
            i++;
        } else {
            break;
        }
    }
    return i;
}

expense_tree_node* insert_helper_expense(expense_tree_node *root, expense r1, expense_tree_node *parent) {
    expense_tree_node *result = root;
    int index = 0;
    if (root == NULL) {
        result = root = createBTreeNode_expense(r1); //only for root == null;
    } else if (root->children[0] == NULL) {
        result = root = insertInCorrectPlace_expense(root, r1);
    } else {
        index = find_index_expense(root, root->keys, r1);
        root->children[index] = insert_helper_expense(root->children[index], r1, root);
    }
    if (root && root->keys == ORDER) {
        int mid = ORDER / 2;
        expense median = root->deck[mid];
        expense_tree_node *second = createSpiltNode_expense(root->deck, root->children, mid + 1, ORDER);
        result = createSpiltNode_expense(root->deck, root->children, 0, mid);
        for (int i = mid; i < ORDER; i++) {
            result->deck[i].expense_ID = 0;
            result->deck[i].expense_amount = 0.0;
            result->deck[i].expense_category[0] = '\0';
            result->deck[i].user_ID = 0;
            result->deck[i].date_of_expense.date = 0;
            result->deck[i].date_of_expense.month = 0;
            result->deck[i].date_of_expense.year = 0;
        }
        for (int i = mid + 1; i <= ORDER; i++) {
            result->children[i] = NULL;
        }
        if (parent == NULL) {
            parent = createBTreeNode_expense(median);
            parent->children[0] = result;
            parent->children[1] = second;
            result = parent;
        } else {
            parent = insertInCorrectPlace_expense(parent, median);
            int pos = find_index_expense(parent, parent->keys, median);
            for (int i = parent->keys - 1; i > pos; i--) {
                parent->children[i + 1] = parent->children[i];
            }
            parent->children[pos] = result;
            parent->children[pos + 1] = second;
        }
    }
    return result;
}

expense_tree_node* insert_expense(expense_tree_node *root, expense e) {
    expense_count++;
    return insert_helper_expense(root, e, NULL);
}

int binary_search_expense(expense_tree_node *root, expense r) {
    int left = 0, right = root->keys - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (root->deck[mid].user_ID < r.user_ID) {
            left = mid + 1;
        } else if (root->deck[mid].user_ID > r.user_ID) {
            right = mid - 1;
        } else {
            if (root->deck[mid].expense_ID < r.expense_ID) {
                left = mid + 1;
            } else if (root->deck[mid].expense_ID > r.expense_ID) {
                right = mid - 1;
            } else {
                return mid;
            }
        }
    }
    return -1;
}

expense_tree_node* deleteFromCorrectPlace_expense(expense_tree_node *root, int ind) {
    for (int j = ind + 1; j < root->keys; j++) {
        root->deck[j - 1] = root->deck[j];
    }
    root->deck[root->keys - 1].expense_ID = 0;
    root->deck[root->keys - 1].expense_amount = 0.0;
    root->deck[root->keys - 1].expense_category[0] = '\0';
    root->deck[root->keys - 1].user_ID = 0;
    root->deck[root->keys - 1].date_of_expense.date = 0;
    root->deck[root->keys - 1].date_of_expense.month = 0;
    root->deck[root->keys - 1].date_of_expense.year = 0;
    root->keys -= 1;
    return root;
}

bool check_left_sibbling_expense(expense_tree_node *parent, int index) {
    bool ans = false;
    if (index > 0) {
        if (parent->children[index - 1]->keys > min_keys) {
            ans = true;
        }
    }
    return ans;
}

bool check_right_sibbling_expense(expense_tree_node *parent, int index) {
    bool ans = false;
    if (index < parent->keys) {
        if (parent->children[index + 1]->keys > min_keys) {
            ans = true;
        }
    }
    return ans;
}

expense getPredecessor_expense(expense_tree_node** node) {
    expense_tree_node *temp = *node;
    expense r;
    r.expense_ID = 0;
    while (temp->children[temp->keys] != NULL) {
        temp = temp->children[temp->keys];
    }
    if (temp->keys > min_keys) {
        r = temp->deck[temp->keys - 1];
        temp = deleteFromCorrectPlace_expense(temp, temp->keys - 1);
    }
    return r;
}

expense getSuccessor_expense(expense_tree_node** node) {
    expense_tree_node *temp = *node;
    expense r;
    r.expense_ID = 0;
    while (temp->children[0] != NULL) {
        temp = temp->children[0];
    }
    if (temp->keys > min_keys) {
        r = temp->deck[0];
        temp = deleteFromCorrectPlace_expense(temp, 0);
    }
    return r;
}

void mergeNodes_exepnse(expense_tree_node** parent, int idx) {
    if (idx == (*parent)->keys) {
        idx -= 1;
    }
    expense_tree_node* child = (*parent)->children[idx];
    expense_tree_node* sibling = (*parent)->children[idx + 1];
    child->deck[child->keys] = (*parent)->deck[idx];
    for (int i = 0; i < sibling->keys; i++) {
        child->deck[child->keys + i + 1] = sibling->deck[i];
    }
    for (int i = 0; i <= sibling->keys; i++) {
        child->children[child->keys + i + 1] = sibling->children[i];
    }
    child->keys += sibling->keys + 1;
    for (int i = idx; i < (*parent)->keys - 1; i++) {
        (*parent)->deck[i] = (*parent)->deck[i + 1];
        (*parent)->children[i + 1] = (*parent)->children[i + 2];
    }
    (*parent)->children[(*parent)->keys] = NULL;
    (*parent)->keys -= 1;
    free(sibling);
}

expense_tree_node* delete_helper_expense(expense_tree_node *root, expense r, expense_tree_node *parent, bool should_enter) {
    int ind;
    if (root == NULL) {
        printf("expense id %d not found.\n", r.expense_ID);
    } else {
        ind = binary_search_expense(root, r);
        if (ind == -1) {
            int index = find_index_expense(root, root->keys, r);
            root->children[index] = delete_helper_expense(root->children[index], r, root, false);
        } else {
            should_enter = true;
        }
    }
    if (root) {
        if (root->keys == 0 && parent == NULL) {
            expense_tree_node *temp = root->children[0];
            root->children[0] = NULL;
            root = temp;
            free(temp);
        }
        if (should_enter && parent == NULL) {
            if (root->children[0] == NULL) {
                root = deleteFromCorrectPlace_expense(root, ind);
            } else {
                expense left_max = getPredecessor_expense(&(root->children[ind]));
                if (left_max.expense_ID != 0) {
                    root->deck[ind] = left_max;
                } else {
                    expense right_min = getSuccessor_expense(&(root->children[ind + 1]));
                    if (right_min.expense_ID != 0) {
                        root->deck[ind] = right_min;
                    } else {
                        if (root->keys != 1) {
                            mergeNodes_exepnse(&root, ind);
                            root->children[ind] = deleteFromCorrectPlace_expense(root->children[ind], min_keys);
                        } else {
                            expense_tree_node *temp = root->children[ind];
                            while (temp->children[temp->keys] != NULL) {
                                temp = temp->children[temp->keys];
                            }
                            expense left_max = temp->deck[temp->keys - 1];
                            root->deck[ind] = left_max;
                            root = delete_helper_expense(root->children[ind], left_max, root, false);
                        }
                    }
                }
            }
        } else if (should_enter || (root->keys < min_keys && parent)) {
            if (root->children[0] == NULL) {
                if (root->keys > min_keys) {
                    root = deleteFromCorrectPlace_expense(root, ind);
                } else {
                    int pos = 0;
                    while (parent && parent->children[pos] != root) pos++;
    
                    bool check_left = check_left_sibbling_expense(parent, pos);
                    if (check_left) {
                        expense new_node = parent->deck[pos - 1];
                        parent->deck[pos - 1] = parent->children[pos - 1]->deck[parent->children[pos - 1]->keys - 1];
                        parent->children[pos - 1] = deleteFromCorrectPlace_expense(parent->children[pos - 1], parent->children[pos - 1]->keys - 1);
                        root = insertInCorrectPlace_expense(root, new_node);
                        root = deleteFromCorrectPlace_expense(root, ind);
                    } else {
                        bool check_right = check_right_sibbling_expense(parent, pos);
                        if (check_right) { 
                            if (pos == parent->keys) pos -= 1;
                            expense last = parent->deck[pos];
                            parent->deck[pos] = parent->children[pos + 1]->deck[0];
                            parent->children[pos + 1] = deleteFromCorrectPlace_expense(parent->children[pos + 1], 0);
                            root = insertInCorrectPlace_expense(root, last);
                            root = deleteFromCorrectPlace_expense(root, ind);
                        } else {
                            root = deleteFromCorrectPlace_expense(root, ind);
                            mergeNodes_exepnse(&parent, pos);
                        }
                    }
                }
            } else {
                if (root->keys >= min_keys) {
                    expense left_max = getPredecessor_expense(&(root->children[ind]));
                    if (left_max.expense_ID != 0) {
                        root->deck[ind] = left_max;
                    } else {
                        expense right_min = getSuccessor_expense(&(root->children[ind + 1]));
                        if (right_min.expense_ID != 0) {
                            root->deck[ind] = right_min;
                        } else {
                            mergeNodes_exepnse(&root, ind);
                            root->children[ind] = deleteFromCorrectPlace_expense(root->children[ind], min_keys);
                        }
                    }
                }
                if (parent && root->keys < min_keys) {
                    int pos = 0;
                    while (parent && parent->children[pos] != root) pos++;
                    if (check_left_sibbling_expense(parent, pos)) {
                        expense new_node = parent->deck[pos - 1];
                        parent->deck[pos - 1] = parent->children[pos - 1]->deck[parent->children[pos - 1]->keys - 1];
                        expense_tree_node *new_child = parent->children[pos - 1]->children[parent->children[pos - 1]->keys];
                        parent->children[pos - 1]->children[parent->children[pos - 1]->keys] = NULL;
                        parent->children[pos - 1] = deleteFromCorrectPlace_expense(parent->children[pos - 1], parent->children[pos - 1]->keys - 1);
                        root = insertInCorrectPlace_expense(root, new_node);
                        for (int i = root->keys; i > 0; i--) {
                            root->children[i] = root->children[i - 1];
                        }
                        root->children[0] = new_child;                      
                    } else {
                        if (check_right_sibbling_expense(parent, pos)) {
                            if (pos == parent->keys) pos -= 1;
                            expense last = parent->deck[pos];
                            parent->deck[pos] = parent->children[pos + 1]->deck[0];
                            expense_tree_node *newnode = parent->children[pos + 1]->children[0];
                            parent->children[pos + 1] = deleteFromCorrectPlace_expense(parent->children[pos + 1], 0);
                            root = insertInCorrectPlace_expense(root, last);
                            root->children[root->keys] = newnode;
                        } else {
                            mergeNodes_exepnse(&parent, pos);
                        }
                    }
                }
            }
        }
    }
    return root;
}

expense_tree_node* delete_expense(expense_tree_node *root, expense r) {
    expense_count--;
    return delete_helper_expense(root, r, NULL, false);
}

expense* search_expense(expense_tree_node *root, expense r) {
    expense *fam = NULL; 
    if (root == NULL) {
        printf("expense %d hasn't been found.\n", r.expense_ID);
    } else {
        int ind = binary_search_expense(root, r);
        if (ind != -1) {
            fam = &root->deck[ind];
        } else {
            int index = find_index_expense(root, root->keys, r);
            if (root->children[0] == NULL) {
                printf("expense %d hasn't been found.\n", r.expense_ID);
            } else {
                fam = search_expense(root->children[index], r);
            }
        }
    }
    return fam;
}

//family/
void printFamilyTreeHeader() {
    const int id_width = 10;
    const int name_width = 15;
    const int users_width = 20;
    const int expense_width = 15;
    const int income_width = 15;
    
    printf("+-%-*s-+-%-*s-+-%-*s-+-%-*s-+-%-*s-+\n",
            id_width, "----------",
            name_width, "---------------",
            users_width, "--------------------",
            expense_width, "---------------",
            income_width, "---------------");
    
    printf("| %-*s | %-*s | %-*s | %-*s | %-*s |\n",
            id_width, "Family ID",
            name_width, "Family Name",
            users_width, "Member User IDs",
            expense_width, "Total Expense",
            income_width, "Total Income");
    
    printf("+-%-*s-+-%-*s-+-%-*s-+-%-*s-+-%-*s-+\n",
            id_width, "----------",
            name_width, "---------------",
            users_width, "--------------------",
            expense_width, "---------------",
            income_width, "---------------");
}

void printFamilyTreeFooter() {
    const int id_width = 10;
    const int name_width = 15;
    const int users_width = 20;
    const int expense_width = 15;
    const int income_width = 15;
    
    printf("+-%-*s-+-%-*s-+-%-*s-+-%-*s-+-%-*s-+\n",
            id_width, "----------",
            name_width, "---------------",
            users_width, "--------------------",
            expense_width, "---------------",
            income_width, "---------------");
    printf("\n");
}

void traverseFamilyTreeRecursive(family_tree_node* node) {
    if (node == NULL) {
        return;
    }
    
    const int id_width = 10;
    const int name_width = 15;
    const int users_width = 20;
    const int expense_width = 15;
    const int income_width = 15;

    traverseFamilyTreeRecursive(node->children[0]);

    for (int i = 0; i < node->keys; i++) {
        family* f = &node->deck[i];
        
        printf("| %-*d | %-*s | ",
                id_width, f->family_ID,
                name_width, f->family_name);
        
        char user_ids[users_width * 2];
        int pos = 0;
        for (int j = 0; j < f->total_members; j++) {
            if (j > 0) {
                pos += snprintf(user_ids + pos, sizeof(user_ids) - pos, ",%d", 
                              f->family_members_user_id[j]);
            } else {
                pos += snprintf(user_ids + pos, sizeof(user_ids) - pos, "%d", 
                              f->family_members_user_id[j]);
            }
            
            if (pos >= sizeof(user_ids) - 10) {
                strcat(user_ids, "...");
                break;
            }
        }
        
        printf("%-*s | %-*.2f | %-*.2f |\n",
                users_width, user_ids,
                expense_width, f->total_family_expense,
                income_width, f->total_family_income);

        traverseFamilyTreeRecursive(node->children[i + 1]);
    }
}

void traverseFamilyTree(family_tree_node* root) {
    printFamilyTreeHeader();
    traverseFamilyTreeRecursive(root);
    printFamilyTreeFooter();
}

void initialize_BTreeNode(family_tree_node** node) {
    *node = (family_tree_node*) malloc(sizeof(family_tree_node));
    if (*node != NULL) {
        for (int i = 0; i <= ORDER; i++) {
            (*node)->children[i] = NULL;
        }
        (*node)->keys = 0;
        for (int i = 0; i < ORDER; i++) {
            (*node)->deck[i].family_ID = 0;
            (*node)->deck[i].total_family_expense = 0.0;
            (*node)->deck[i].total_family_income = 0.0;
            (*node)->deck[i].total_members = 0;
            for (int j = 0; j < MAX_USERS_IN_FAMILY; j++) {
                (*node)->deck[i].family_members_user_id[j] = 0;
            }
        }
    }
}

family_tree_node* createBTreeNode(family fam_node) {
    family_tree_node *result = NULL;
    initialize_BTreeNode(&result);
    if (result) {
        result->deck[0] = fam_node;
        result->keys++;
    }
    return result;
}

family_tree_node* createSpiltNode(family arr[], family_tree_node *children[], int start, int end) {
    family_tree_node *result;
    initialize_BTreeNode(&result);
    if (result) {
        for (int i = start; i < end; i++) {
            result->deck[i - start] = arr[i];
        }
        for (int i = start; i <= end; i++) {
            result->children[i - start] = children[i];
        }
        result->keys = end - start;
    }
    return result;
}

family_tree_node* insertInCorrectPlace(family_tree_node *node, family r) {
    if (node == NULL) {
        node = createBTreeNode(r);
        node->keys = 1;
    } else if (node->keys >= 0 && node->keys < ORDER) {
        int n = node->keys;
        node->keys += 1;
        int i = 0;
        while (i < n && node->deck[i].family_ID < r.family_ID) i++;
        for (int j = ORDER - 2; j >= i; j--) node->deck[j + 1] = node->deck[j];
        node->deck[i] = r;
    }
    return node;
}

int find_index(family_tree_node *root, int n, int fam_id) {
    int i = 0;
    while (i < n && root->deck[i].family_ID < fam_id) i++;
    return i;
}

family_tree_node* insert_helper(family_tree_node *root, family r1, family_tree_node *parent) {
    family_tree_node *result = root;
    int index = 0;
    if (root == NULL) {
        result = root = createBTreeNode(r1); //only for root == null;
    } else if (root->children[0] == NULL) {
        result = root = insertInCorrectPlace(root, r1);
    } else {
        index = find_index(root, root->keys, r1.family_ID);
        root->children[index] = insert_helper(root->children[index], r1, root);
    }
    if (root && root->keys == ORDER) {
        int mid = ORDER / 2;
        family median = root->deck[mid];
        family_tree_node *second = createSpiltNode(root->deck, root->children, mid + 1, ORDER);
        result = createSpiltNode(root->deck, root->children, 0, mid);
        for (int i = mid; i < ORDER; i++) {
            result->deck[i].family_ID = 0;
            result->deck[i].total_family_expense = 0.0;
            result->deck[i].total_family_income = 0.0;
            result->deck[i].total_members = 0;
            for (int j = 0; j < MAX_USERS_IN_FAMILY; j++) {
                result->deck[i].family_members_user_id[j] = 0;
            }
        }
        for (int i = mid + 1; i <= ORDER; i++) {
            result->children[i] = NULL;
        }
        if (parent == NULL) {
            parent = createBTreeNode(median);
            parent->children[0] = result;
            parent->children[1] = second;
            result = parent;
        } else {
            parent = insertInCorrectPlace(parent, median);
            int pos = find_index(parent, parent->keys, median.family_ID);
            for (int i = parent->keys - 1; i > pos; i--) {
                parent->children[i + 1] = parent->children[i];
            }
            parent->children[pos] = result;
            parent->children[pos + 1] = second;
        }
    }
    return result;
}

family_tree_node* insert_family(family_tree_node *root, family e) {
    family_count++;
    return insert_helper(root, e, NULL);
}

int binary_search(family_tree_node *root, int fam_id) {
    int left = 0, right = root->keys - 1;
    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (root->deck[mid].family_ID == fam_id) {
            return mid;
        } else if (root->deck[mid].family_ID < fam_id) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

family_tree_node* deleteFromCorrectPlace(family_tree_node *root, int ind) {
    for (int j = ind + 1; j < root->keys; j++) {
        root->deck[j - 1] = root->deck[j];
    }
    root->deck[root->keys - 1].family_ID = 0;
    root->deck[root->keys - 1].total_family_expense = 0.0;
    root->deck[root->keys - 1].total_family_income = 0.0;
    root->deck[root->keys - 1].total_members = 0;
    for (int j = 0; j < MAX_USERS_IN_FAMILY; j++) {
        root->deck[root->keys - 1].family_members_user_id[j] = 0;
    }
    root->keys -= 1;
    return root;
}

bool check_left_sibbling(family_tree_node *parent, int index) {
    bool ans = false;
    if (index > 0) {
        if (parent->children[index - 1]->keys > min_keys) {
            ans = true;
        }
    }
    return ans;
}

bool check_right_sibbling(family_tree_node *parent, int index) {
    bool ans = false;
    if (index < parent->keys) {
        if (parent->children[index + 1]->keys > min_keys) {
            ans = true;
        }
    }
    return ans;
}

family getPredecessor(family_tree_node** node) {
    family_tree_node *temp = *node;
    family r;
    r.family_ID = 0;
    while (temp->children[temp->keys] != NULL) {
        temp = temp->children[temp->keys];
    }
    if (temp->keys > min_keys) {
        r = temp->deck[temp->keys - 1];
        temp = deleteFromCorrectPlace(temp, temp->keys - 1);
    }
    return r;
}

family getSuccessor(family_tree_node** node) {
    family_tree_node *temp = *node;
    family r;
    r.family_ID = 0;
    while (temp->children[0] != NULL) {
        temp = temp->children[0];
    }
    if (temp->keys > min_keys) {
        r = temp->deck[0];
        temp = deleteFromCorrectPlace(temp, 0);
    }
    return r;
}

void mergeNodes(family_tree_node** parent, int idx) {
    if (idx == (*parent)->keys) {
        idx -= 1;
    }
    family_tree_node* child = (*parent)->children[idx];
    family_tree_node* sibling = (*parent)->children[idx + 1];
    child->deck[child->keys] = (*parent)->deck[idx];
    for (int i = 0; i < sibling->keys; i++) {
        child->deck[child->keys + i + 1] = sibling->deck[i];
    }
    for (int i = 0; i <= sibling->keys; i++) {
        child->children[child->keys + i + 1] = sibling->children[i];
    }
    child->keys += sibling->keys + 1;
    for (int i = idx; i < (*parent)->keys - 1; i++) {
        (*parent)->deck[i] = (*parent)->deck[i + 1];
        (*parent)->children[i + 1] = (*parent)->children[i + 2];
    }
    (*parent)->children[(*parent)->keys] = NULL;
    (*parent)->keys -= 1;
    free(sibling);
}

family_tree_node* delete_helper(family_tree_node *root, int fam_id, family_tree_node *parent, bool should_enter) {
    int ind;
    if (root == NULL) {
        printf("value not found.\n");
    } else {
        ind = binary_search(root, fam_id);
        if (ind == -1) {
            int index = find_index(root, root->keys, fam_id);
            root->children[index] = delete_helper(root->children[index], fam_id, root, false);
        } else {
            should_enter = true;
        }
    }
    if (root) {
        if (root->keys == 0 && parent == NULL) {
            family_tree_node *temp = root->children[0];
            root->children[0] = NULL;
            root = temp;
            free(temp);
        }
        if (should_enter && parent == NULL) {
            if (root->children[0] == NULL) {
                root = deleteFromCorrectPlace(root, ind);
            } else {
                family left_max = getPredecessor(&(root->children[ind]));
                if (left_max.family_ID != 0) {
                    root->deck[ind] = left_max;
                } else {
                    family right_min = getSuccessor(&(root->children[ind + 1]));
                    if (right_min.family_ID != 0) {
                        root->deck[ind] = right_min;
                    } else {
                        if (root->keys != 1) {
                            mergeNodes(&root, ind);
                            root->children[ind] = deleteFromCorrectPlace(root->children[ind], min_keys);
                        } else {
                            family_tree_node *temp = root->children[ind];
                            while (temp->children[temp->keys] != NULL) {
                                temp = temp->children[temp->keys];
                            }
                            family left_max = temp->deck[temp->keys - 1];
                            root->deck[ind] = left_max;
                            root = delete_helper(root->children[ind], left_max.family_ID, root, false);
                        }
                    }
                }
            }
        } else if (should_enter || (root->keys < min_keys && parent)) {
            if (root->children[0] == NULL) {
                if (root->keys > min_keys) {
                    root = deleteFromCorrectPlace(root, ind);
                } else {
                    int pos = 0;
                    while (parent && parent->children[pos] != root) pos++;
    
                    bool check_left = check_left_sibbling(parent, pos);
                    if (check_left) {
                        family new_node = parent->deck[pos - 1];
                        parent->deck[pos - 1] = parent->children[pos - 1]->deck[parent->children[pos - 1]->keys - 1];
                        parent->children[pos - 1] = deleteFromCorrectPlace(parent->children[pos - 1], parent->children[pos - 1]->keys - 1);
                        root = insertInCorrectPlace(root, new_node);
                        root = deleteFromCorrectPlace(root, ind);
                    } else {
                        bool check_right = check_right_sibbling(parent, pos);
                        if (check_right) { 
                            if (pos == parent->keys) pos -= 1;
                            family last = parent->deck[pos];
                            parent->deck[pos] = parent->children[pos + 1]->deck[0];
                            parent->children[pos + 1] = deleteFromCorrectPlace(parent->children[pos + 1], 0);
                            root = insertInCorrectPlace(root, last);
                            root = deleteFromCorrectPlace(root, ind);
                        } else {
                            root = deleteFromCorrectPlace(root, ind);
                            mergeNodes(&parent, pos);
                        }
                    }
                }
            } else {
                if (root->keys >= min_keys) {
                    family left_max = getPredecessor(&(root->children[ind]));
                    if (left_max.family_ID != 0) {
                        root->deck[ind] = left_max;
                    } else {
                        family right_min = getSuccessor(&(root->children[ind + 1]));
                        if (right_min.family_ID != 0) {
                            root->deck[ind] = right_min;
                        } else {
                            mergeNodes(&root, ind);
                            root->children[ind] = deleteFromCorrectPlace(root->children[ind], min_keys);
                        }
                    }
                }
                if (parent && root->keys < min_keys) {
                    int pos = 0;
                    while (parent && parent->children[pos] != root) pos++;
                    if (check_left_sibbling(parent, pos)) {
                        family new_node = parent->deck[pos - 1];
                        parent->deck[pos - 1] = parent->children[pos - 1]->deck[parent->children[pos - 1]->keys - 1];
                        family_tree_node *new_child = parent->children[pos - 1]->children[parent->children[pos - 1]->keys];
                        parent->children[pos - 1]->children[parent->children[pos - 1]->keys] = NULL;
                        parent->children[pos - 1] = deleteFromCorrectPlace(parent->children[pos - 1], parent->children[pos - 1]->keys - 1);
                        root = insertInCorrectPlace(root, new_node);
                        for (int i = root->keys; i > 0; i--) {
                            root->children[i] = root->children[i - 1];
                        }
                        root->children[0] = new_child;                      
                    } else {
                        if (check_right_sibbling(parent, pos)) {
                            if (pos == parent->keys) pos -= 1;
                            family last = parent->deck[pos];
                            parent->deck[pos] = parent->children[pos + 1]->deck[0];
                            family_tree_node *newnode = parent->children[pos + 1]->children[0];
                            parent->children[pos + 1] = deleteFromCorrectPlace(parent->children[pos + 1], 0);
                            root = insertInCorrectPlace(root, last);
                            root->children[root->keys] = newnode;
                        } else {
                            mergeNodes(&parent, pos);
                        }
                    }
                }
            }
        }
    }
    return root;
}

family_tree_node* delete_family(family_tree_node *root, int fam_id) {
    family_count--;
    return delete_helper(root, fam_id, NULL, false);
}

family* search_family(family_tree_node *root, int fam_id) {
    family* fam = NULL; 
    if (root == NULL) {
        printf("Record does not exist.\n");
    } else {
        int ind = binary_search(root, fam_id);
        if (ind != -1) {
            fam = &root->deck[ind];
        } else {
            int index = find_index(root, root->keys, fam_id);
            if (root->children[0] == NULL) {
                printf("Record does not exist.\n");
            } else {
                fam = search_family(root->children[index], fam_id);
            }
        }
    }
    return fam;
}

family_tree_node* addUserToFamily(family_tree_node *family_root, int user_id, float income) {
    int fam_id;
    printf("enter family id to which you want to add this user : ");
    scanf("%d", &fam_id);
    family* fam = search_family(family_root, fam_id);
    if (fam) {
        if (fam->total_members == 4) {
            printf("Max number of users are in family\n");
        } else {
            fam->total_members  += 1;
            int i = 0;
            while (fam->family_members_user_id[i]  != 0) {
                i++;
            }
            fam->family_members_user_id[i] = user_id;
            fam->total_family_income += income;
        }
    } else {
        family f;
        f.family_ID = fam_id;
        printf("enter family name :");
        scanf("%s", f.family_name);
        f.total_members = 1;
        f.family_members_user_id[0] = user_id;
        f.total_family_income = income;
        f.total_family_expense = 0;
        family_root = insert_family(family_root, f);
    }
    return family_root;
}

statusCode AddUser(user_tree_node **user_root, family_tree_node **family_root) {
    statusCode sc = SUCCESS;
    if (user_count >= MAX_USERS || user_count < 0) {
        printf("User list is out of bounds.\n");
        sc = FAILURE;
    } else {
        user newNode;
        printf("enter the details\n");
        printf("enter user_id : ");
        scanf("%d",&newNode.user_ID);
        while (newNode.user_ID > MAX_USERS || newNode.user_ID < 1) {
            printf("wrong input. user id cannot exceed 1000 and cannot be less than 1.\n");
            printf("enter user_id : ");
            scanf("%d",&newNode.user_ID);
        }
        user_tree_node *check_node = search_user(*user_root, newNode.user_ID);
        if (check_node != NULL) {
            printf("user id %d already exists.\n", newNode.user_ID);
            sc = FAILURE;
        } else {
            printf("enter income of user : ");
            scanf("%f", &newNode.income);
            while (newNode.income < 0) {
                printf("wrong input. Income is never negative.\n");
                printf("enter income of user : ");
                scanf("%f", &newNode.income);
            }
            printf("enter name of individual user : ");
            scanf("%s", newNode.user_name);
            while (newNode.user_name[0] == '\0') {
                printf("wrong input. please enter your name.\n");
                printf("enter name of individual user : ");
                clear_input_buffer();
                fgets(newNode.user_name, 100, stdin);
            }
            *user_root = insert_user(*user_root, newNode);
            printf("user added successfully.\n");
            sc = SUCCESS;
            char c;
            printf("do you want to add this user to a family? enter y to add, n to continue : ");
            do {
                scanf("%c", &c);
            } while (c != 'y' && c != 'n' && c != 'Y' && c != 'N');
            if (c == 'y' || c == 'Y') {
                *family_root = addUserToFamily(*family_root, newNode.user_ID, newNode.income);
            }
        }
    }
    return sc;
}

void print_user_table_header_boxed() {
    printf("+------------+---------------------------+------------+\n");
    printf("| %-10s | %-25s | %-10s |\n", "User ID", "User Name", "Income");
    printf("+------------+---------------------------+------------+\n");
}

void print_user_row_boxed(user u) {
    printf("| %-10d | %-25s | %-10.2f |\n", u.user_ID, u.user_name, u.income);
}

void print_user_table_footer() {
    printf("+------------+---------------------------+------------+\n");
}

void traverse_users_helper(user_tree_node *root, int *header_printed) {
    if (root == NULL) return;
    
    traverse_users_helper(root->left, header_printed);
    
    if (!*header_printed) {
        print_user_table_header_boxed();
        *header_printed = 1;
    }
    
    print_user_row_boxed(root->data);
    
    traverse_users_helper(root->right, header_printed);
}

void traverse_users(user_tree_node *root) {
    int header_printed = 0;
    traverse_users_helper(root, &header_printed);
    
    if (header_printed) {
        print_user_table_footer();
    }
}

statusCode AddExpense(expense_tree_node **expense_root, user_tree_node **user_root) {
    statusCode sc = SUCCESS;
    if (expense_count >= MAX_EXPENSES) {
        printf("expenses limit reached.\n");
        sc = FAILURE;
    } else {
        expense e;
        printf("Enter expense ID for expense: ");
        scanf("%d", &e.expense_ID);
        while(e.expense_ID > MAX_EXPENSES || e.expense_ID < 1){
            printf("wrong input. expense id cannot exceed 1000 and cannot be less than 1.\n");
            printf("enter expense_id : ");
            scanf("%d",&e.expense_ID);
        }
        expense *exp = search_expense(*expense_root, e);
        if (exp) {
            printf("expense id already exists.\n");
            sc = FAILURE;
        } else {
            printf("Enter user ID for expense: ");
            scanf("%d", &e.user_ID);
            user_tree_node *check_node = search_user(*user_root, e.user_ID);
            if (!check_node) {
                sc = FAILURE;
                printf("given user doesnot exist.\n");
            } else {
                printf("Enter expense amount: ");
                scanf("%f", &e.expense_amount);
                while(e.expense_amount < 0){
                    printf("wrong input. expense can never be negative.\n");
                    printf("enter income again : ");
                    scanf("%f", &e.expense_amount);
                }
                printf("Enter expense category: ");
                scanf("%14s", e.expense_category);
                while(strcmp(e.expense_category, "rent") != 0 && strcmp(e.expense_category, "utility") != 0 && strcmp(e.expense_category, "stationary") != 0 && strcmp(e.expense_category, "grocery") != 0 && strcmp(e.expense_category, "leisure") != 0){
                    printf("enter only rent or utility or stationary or grocery or leisure\n");
                    printf("Enter expense category again : ");
                    scanf("%14s", e.expense_category);         
                }
                capitalize(e.expense_category);
                printf("Enter date of expense (DD MM YYYY): ");
                scanf("%u", &e.date_of_expense.date);
                scanf("%u", &e.date_of_expense.month);
                scanf("%u", &e.date_of_expense.year);
                while (e.date_of_expense.date > 10 || e.date_of_expense.date <= 0) {
                    printf("enter only 1 - 10\n");
                    printf("enter date again: ");
                    scanf("%u", &e.date_of_expense.date);
                }
                while (e.date_of_expense.month > 12 || e.date_of_expense.month <= 0) {
                    printf("enter only 1 - 12\n");
                    printf("enter month again: ");
                    scanf("%u", &e.date_of_expense.month);
                }
                while (e.date_of_expense.year > 2025 || e.date_of_expense.year <= 0) {
                    printf("enter only 1 - 12\n");
                    printf("enter year again: ");
                    scanf("%u", &e.date_of_expense.year);
                }
                
                *expense_root = insert_expense(*expense_root, e);
                printf("Added new expense (ID: %d).\n", e.expense_ID);
            }
        }
    }
    return sc;
}

bool doesUserExistsInfamily(family_tree_node *root, int user_id) {
    bool ans = false;
    int fam_keys = root->keys;
    for (int i = 0; i < fam_keys && !ans; i++) {
        family fam = root->deck[i];
        int n = fam.total_members;
        for (int j = 0; j < n && !ans; j++) {
            if (fam.family_members_user_id[j] == user_id) {
                ans = true;
            }
        }
    }
    return ans;
}

bool checkifUserInAnotherFamily(family_tree_node* root, int user_id) {   
    bool found = false;
    if (root) {
        found = doesUserExistsInfamily(root, user_id);
        if (found) return found;
        int n = root->keys;
        for (int i = 0; i <= n; i++) {
            found = checkifUserInAnotherFamily(root->children[i], user_id);
        }
    }
    return found;
}

void calculate_monthly_expense_of_family_helper(expense_tree_node *expenseRoot, family fam, float *total_expense) {
    if(expenseRoot) {
        for (int j = 0; j < expenseRoot->keys; j++) {
            for (int i = 0; i < fam.total_members; i++) {
                if (expenseRoot->deck[j].user_ID == fam.family_members_user_id[i]) (*total_expense) += expenseRoot->deck[j].expense_amount;
            }
        }
        for (int j = 0; j <= expenseRoot->keys; j++) {
            calculate_monthly_expense_of_family_helper(expenseRoot->children[j], fam, total_expense);
        }
    }
}

float calculate_monthly_expense_of_family(expense_tree_node *expenseRoot, family fam) {
    float total_expense = 0.0;
    calculate_monthly_expense_of_family_helper(expenseRoot, fam, &total_expense);
    return total_expense;
}

float calculate_monthly_income_of_family(family fam, user_tree_node *root){
    float total_income = 0;
    int n = fam.total_members;
    for (int i = 0; i < n; i++) {
        user_tree_node *node = search_user(root, fam.family_members_user_id[i]);
        if (node) total_income += node->data.income;
    }
    return total_income;
}

statusCode CreateFamily(family_tree_node **fam_root, user_tree_node **user_root, expense_tree_node **expense_root) {
    statusCode sc = SUCCESS;
    if (family_count >= MAX_FAMILIES) {
        printf("families count is out of bounds.\n");
        sc = FAILURE;
    } else {
        int family_id;
        int total_members; 
        printf("Enter family id: ");
        scanf("%d", &family_id);
        while (family_id > 100 || family_id < 1) {
            printf("wrong input. family id cannot exceed 100 and cannot be less than 1.\n");
            printf("enter family id : ");
            scanf("%d",&family_id);
        }
        family *fam = search_family(*fam_root, family_id);
        if (fam) {
            printf("family with id %d exists already.\n", family_id);
        } else {
            printf("Enter total members in the family: ");
            scanf("%d", &total_members);
            while (total_members > 4 || total_members < 1) {
                printf("wrong input. total members should be between 1 and 4.\n");
                printf("enter total members again :");
                scanf("%d", &total_members);
            }
            int family_user_id[total_members];
            bool can_continue = true;
            for (int i = 0; i < total_members && can_continue; i++) {
                printf("Enter user ID of family member %d: ", i + 1);
                scanf("%d", &family_user_id[i]);
                user_tree_node *check_node = search_user(*user_root, family_user_id[i]);
                if(check_node) {
                    bool user_in_other_family = checkifUserInAnotherFamily(*fam_root, family_user_id[i]);
                    if(user_in_other_family) {
                        printf("User %d exists in another family, Cant add user to this family.\n",family_user_id[i]);
                        can_continue = false;
                    } else {
                        can_continue = true;
                    }
                } else {
                    printf("User %d doesnt exist, cant add user to the family.\n", family_user_id[i]);
                    can_continue = false;
                }
            }
            if(can_continue) {
                char family_name[25]; 
                do {
                    printf("Enter family name: ");
                    if (fgets(family_name, sizeof(family_name), stdin) == NULL) {
                        printf("Input error occurred\n");
                        break;
                    }
                    family_name[strcspn(family_name, "\n")] = '\0';
                    if (strlen(family_name) == sizeof(family_name) - 1 && !strchr(family_name, '\n')) {
                        printf("Input too long. Maximum 24 characters allowed.\n");
                        int c;
                        while ((c = getchar()) != '\n' && c != EOF);
                        family_name[0] = '\0';
                        continue;
                    }
                    
                    if (family_name[0] == '\0') {
                        printf("Family name cannot be empty. Please try again.\n");
                    }
                } while (family_name[0] == '\0');
                family e;
                e.family_ID = family_id;
                e.total_family_expense = 0.0;
                e.total_family_income = 0.0;
                e.total_members = total_members;
                strcpy(e.family_name, family_name);
                for (int j = 0; j < total_members; j++) {
                    e.family_members_user_id[j] = family_user_id[j];
                }
                e.total_family_expense = calculate_monthly_expense_of_family(*expense_root, e);
                e.total_family_income = calculate_monthly_income_of_family(e, *user_root);
                *fam_root = insert_family(*fam_root, e);
            } else {
                sc = FAILURE;
                printf("operation failed.\n");
            }
        }
    }
    return sc;
}

family* find_family_by_user_id(family_tree_node *root, int user_id) {
    if (root == NULL) {
        return NULL;
    }
    for (int i = 0; i < root->keys; i++) {
        for (int j = 0; j < root->deck[i].total_members; j++) {
            if (root->deck[i].family_members_user_id[j] == user_id) {
                return &(root->deck[i]);
            }
        }
    }
    for (int i = 0; i <= root->keys; i++) {
        family *result = find_family_by_user_id(root->children[i], user_id);
        if (result != NULL) {
            return result;
        }
    }
    
    return NULL; 
}

statusCode updateUserDetails (user_tree_node **user_root, family_tree_node** fam_root) {
    statusCode sc = SUCCESS;
    if (user_count <= 0) {
        printf("user list is empty\n");
        sc = FAILURE;
    } else {
        int user_id;
        printf("enter which user id you want to update :");
        scanf("%d", &user_id);
        user_tree_node *check_node = search_user(*user_root, user_id);
        float past_income = check_node->data.income;
        if (!check_node) {
            printf("user not found\n");
            sc = FAILURE;
        } else {
            printf("1. enter 1 to change user name.\n");
            printf("2. enter 2 to update user income.\n");
            printf("3. enter 3 to change both user name and user income.\n");
            int option;
            printf("enter your option :");
            scanf("%d", &option);
            if (option == 1) {
                char userName[100];
                printf("enter new user name :");
                scanf("%99s", userName);
                strcpy(check_node->data.user_name, userName);
            } else if (option == 2) {
                float income;
                printf("enter your new income :");
                scanf("%f", &income);
                check_node->data.income = income;
            } else if (option == 3) {
                char userName[100];
                float income;
                printf("enter new user name :");
                scanf("%99s", userName);
                printf("enter your new income :");
                scanf("%f", &income);
                check_node->data.income = income;
                strcpy(check_node->data.user_name, userName);
            } else {
                printf("you have entered a invalid option\n");
                sc = FAILURE;
            }
            if (option == 2 || option == 3) {
                if (family_count <= 0) {
                    printf("family list is empty");
                    sc = FAILURE;
                } else {
                    family* fam_node = find_family_by_user_id(*fam_root, user_id);
                    if (fam_node != NULL) {
                        fam_node->total_family_income += (check_node->data.income - past_income);
                    }
                }
            }
            printf("user %d details updated successfully.\n", user_id);
        }
    }
    return sc;
}

void findExpensesByUserID_helper(expense_tree_node* root, int user_id, float *sum) {
    if (root != NULL) {
        for (int i = 0; i < root->keys; i++) {
            if (root->deck[i].user_ID == user_id) {
                *sum += root->deck[i].expense_amount; 
            } 
        }
        for (int i = 0; i <= root->keys; i++) {
            findExpensesByUserID_helper(root->children[i], user_id, sum);
        }
    }
}

float findExpensesByUserID(expense_tree_node* root, int user_id) {
    float sum = 0.0;
    findExpensesByUserID_helper(root, user_id, &sum);
    return sum;
} 

statusCode updateFamilyDetails(family_tree_node **fam_root, user_tree_node **user_root, expense_tree_node** expense_root, int *f_id) {
    statusCode sc = SUCCESS;
    if (family_count <= 0) {
        printf("family list is empty\n");
        sc = FAILURE;
    } else {
        int famId;
        printf("enter family id to update :");
        scanf("%d", &famId);
        family *fam = search_family(*fam_root, famId);
        if (fam == NULL) {
            printf("family id %d doesnot exist.\n", famId);
            sc = FAILURE;
        } else {
            *f_id = famId;
            char famName[100];
            printf("1. enter 1 to change family name :\n");
            printf("2. enter 2 to update user list.\n");
            printf("3. enter 3 to update family expense.\n");
            printf("4. enter 4 to update family income.\n");

            int option;
            printf("enter your option :");
            scanf("%d", &option);
            switch (option) {
                case 1: {
                    printf("enter new family name :");
                    clear_input_buffer();
                    fgets(famName, sizeof(famName), stdin);
                    famName[strcspn(famName, "\n")] = 0;
                    strcpy(fam->family_name, famName);
                    break;
                }
                case 2: {
                    printf("enter 1 : to add a user to family.\n");
                    printf("enter 2 : update existing user ids.\n");
                    int op;
                    printf("enter your option : ");
                    scanf("%d", &op);
                    clear_input_buffer();
                    switch (op) {
                        case 1 : {
                            if (fam->total_members == 4) {
                                printf("Max number of users are in family\n");
                                sc = FAILURE;
                            } else {
                                printf("enter the details\n");
                                int user_id;
                                printf("enter user_id : ");
                                scanf("%d",&user_id);
                                while (user_id > MAX_USERS || user_id < 1) {
                                    printf("wrong input. user id cannot exceed 1000 and cannot be less than 1.\n");
                                    printf("enter user_id : ");
                                    scanf("%d",&user_id);
                                }
                                user_tree_node *check_node = search_user(*user_root, user_id);
                                if (check_node == NULL) {
                                    printf("user id %d does not exist.\n", user_id);
                                    sc = FAILURE;
                                } else {
                                    bool exists_in_family = checkifUserInAnotherFamily(*fam_root, user_id);
                                    if (exists_in_family) {
                                        printf("User already exists in another family.\n");
                                        sc = FAILURE;
                                    } else {
                                        fam->total_members += 1;
                                        int j = 0;
                                        while (fam->family_members_user_id[j] != 0) {
                                            j++;
                                        }
                                        fam->family_members_user_id[j] = user_id;
                                        float income = check_node->data.income;
                                        fam->total_family_income += income;
                                        float expenses = findExpensesByUserID(*expense_root, user_id);
                                        fam->total_family_expense += expenses;
                                    }
                                }
                            }
                            break;
                        }
                        case 2 : {
                            printf("enter which user id of family you want to change : ");
                            int user_id;
                            scanf("%d", &user_id);
                            int ind = -1;
                            for (int j = 0; j < fam->total_members; j++) {
                                if (user_id == fam->family_members_user_id[j]) ind = j;
                            }
                            if (ind != -1) {
                                printf("enter new user id : ");
                                int new_user_id;
                                scanf("%d", &new_user_id);
                                user_tree_node *node = search_user(*user_root, new_user_id);
                                if (node != NULL) { 
                                    bool check = checkifUserInAnotherFamily(*fam_root, new_user_id);
                                    if (!check) {
                                        user_tree_node *old_node = search_user(*user_root, user_id);
                                        if (old_node != NULL) {
                                            fam->total_family_income -= old_node->data.income;
                                            float old_expenses = findExpensesByUserID(*expense_root, user_id);
                                            fam->total_family_expense -= old_expenses;
                                        }
                                        
                                        fam->family_members_user_id[ind] = new_user_id;
                                        fam->total_family_income += node->data.income;
                                        float new_expenses = findExpensesByUserID(*expense_root, new_user_id);
                                        fam->total_family_expense += new_expenses;
                                    } else {
                                        printf("user exists in other family.\n");
                                        sc = FAILURE;
                                    }
                                } else {
                                    printf("New user ID %d not found.\n", new_user_id);
                                    sc = FAILURE;
                                }
                            } else {
                                printf("User ID %d not found in this family.\n", user_id);
                                sc = FAILURE;
                            }
                            break;
                        }
                        default : {
                            printf("wrong input.\n");
                            sc = FAILURE;
                            break;
                        }
                    }
                    break;
                }
                case 3: {
                    fam->total_family_expense = calculate_monthly_expense_of_family(*expense_root, *fam);
                    printf("Family expense updated to: %.2f\n", fam->total_family_expense);
                    break;
                }
                case 4: {
                    fam->total_family_income = calculate_monthly_income_of_family(*fam, *user_root);
                    printf("Family income updated to: %.2f\n", fam->total_family_income);
                    break;
                }
                default: {
                    printf("you entered a wrong number\n");
                    sc = FAILURE;
                    break;
                }
            }
        }
    }
    return sc;
}

void getExpenseIdsWithUserId(expense_tree_node *root, int user_id, expense *id_array, int *k) {
    if (root != NULL) {
        for (int i = 0; i < root->keys; i++) {
            if (root->deck[i].user_ID == user_id) {
                id_array[*k] = root->deck[i];
                (*k)++;
            }
        }
        for (int i = 0; i <= root->keys; i++) {
            getExpenseIdsWithUserId(root->children[i], user_id, id_array, k);
        }
    }
}

void deleteAllUserExpenses(expense_tree_node** root, int user_id) {
    expense *id_array = (expense*) malloc(MAX_EXPENSES * sizeof(expense));
    int k = 0;

    getExpenseIdsWithUserId(*root, user_id, id_array, &k);

    for (int i = 0; i < k; i++) {
        *root = delete_expense(*root, id_array[i]);
    }

    free(id_array);
}

statusCode deleteUser(user_tree_node **user_root, family_tree_node **fam_root, expense_tree_node** expense_root) {
    statusCode sc = SUCCESS;
    if (user_count <= 0) {
        printf("User list is empty\n");
        return FAILURE;
    }
    int user_id;
    printf("Enter user id to delete: ");
    scanf("%d", &user_id);
    user_tree_node *user_node = search_user(*user_root, user_id);
    if (!user_node) {
        printf("User %d not found\n", user_id);
        return FAILURE;
    }
    deleteAllUserExpenses(expense_root, user_id);
    if (family_count > 0) {
        family* fam = find_family_by_user_id(*fam_root, user_id);
        if (fam) {
            if (fam->total_members == 1) {
                *fam_root = delete_family(*fam_root, fam->family_ID);
            } else {
                int pos = -1;
                for (int i = 0; i < fam->total_members; i++) {
                    if (fam->family_members_user_id[i] == user_id) {
                        pos = i;
                        break;
                    }
                }          
                if (pos != -1) {
                    fam->total_family_income -= user_node->data.income;
                    for (int i = pos; i < fam->total_members - 1; i++) {
                        fam->family_members_user_id[i] = fam->family_members_user_id[i+1];
                    }
                    fam->family_members_user_id[fam->total_members-1] = 0;
                    fam->total_members--;
                }
            }
        }
    }
    *user_root = delete_user(*user_root, user_id);
    printf("User %d deleted successfully\n", user_id);
    return SUCCESS;
}

statusCode deleteFamily (family_tree_node **fam_root, user_tree_node **user_root, expense_tree_node** expense_root) {
    statusCode sc = SUCCESS;
    if (family_count <= 0) {
        printf("family list is empty\n");
        sc = FAILURE;
    } else {
        int fam_id;
        printf("enter family id to delete :");
        scanf("%d", &fam_id);
        family *fam = search_family(*fam_root, fam_id);
        if (fam == NULL) {
            printf("family id doesnot exist\n");
            sc = FAILURE;
        } else {
            if (user_count <= 0) {
                sc = FAILURE;
            } else {
                for (int i = 0; i < fam->total_members; i++) {
                    *user_root = delete_user(*user_root, fam->family_members_user_id[i]);
                }
            }
            if (expense_count <= 0) {
                printf("expense list is empty.\n");
                sc = FAILURE; 
            } else {
                for (int i = 0; i < fam->total_members; i++) {
                    deleteAllUserExpenses(expense_root, fam->family_members_user_id[i]);
                }
            }
            *fam_root = delete_family(*fam_root, fam_id);
        }
    }
    return sc;
}

int binary_search_expense_by_id(expense_tree_node *root, int expense_id) {
    int low = 0;
    int high = root->keys - 1;
    
    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (root->deck[mid].expense_ID == expense_id) {
            return mid; 
        } else if (root->deck[mid].expense_ID < expense_id) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

int find_index_expense_id(expense_tree_node *root, int n, int expense_id) {
    int i = 0;
    while (i < n && root->deck[i].expense_ID < expense_id) {
        i++;
    }
    return i;
}

expense* search_expense_by_id(expense_tree_node *root, int expense_id) {
    if (root == NULL) {
        return NULL;
    }
    for (int i = 0; i < root->keys; i++) {
        if (root->deck[i].expense_ID == expense_id) {
            return &root->deck[i];
        }
    }
    for (int i = 0; i <= root->keys; i++) {
        expense *result = search_expense_by_id(root->children[i], expense_id);
        if (result != NULL) {
            return result;
        }
    }
    return NULL;
}

statusCode updateExpense(expense_tree_node **expense_root, family_tree_node **fam_root) {
    statusCode sc = SUCCESS;
    if (expense_count <= 0) {
        printf("expense list is empty\n");
        return FAILURE;
    }

    int expense_id;
    printf("enter expense id to update: ");
    scanf("%d", &expense_id);
    
    expense *exp_node = search_expense_by_id(*expense_root, expense_id);
    if (!exp_node) {
        printf("expense id does not exist\n");
        return FAILURE;
    }

    printf("1. enter 1 to update expense category\n");
    printf("2. enter 2 to update expense amount\n");
    printf("3. enter 3 to update date of expense\n");
    
    int option;
    printf("enter your option: ");
    scanf("%d", &option);
    
    float old_expense_amount = exp_node->expense_amount;
    
    switch (option) {
        case 1: {
            char expense_category[15];
            printf("enter expense category: ");
            scanf("%14s", expense_category);
            while(strcmp(expense_category, "rent") != 0 && 
                  strcmp(expense_category, "utility") != 0 && 
                  strcmp(expense_category, "stationary") != 0 && 
                  strcmp(expense_category, "grocery") != 0 && 
                  strcmp(expense_category, "leisure") != 0) {
                printf("enter only rent, utility, stationary, grocery or leisure\n");
                printf("Enter expense category again: ");
                scanf("%14s", expense_category);         
            }
            capitalize(expense_category);
            strcpy(exp_node->expense_category, expense_category);
            printf("expense %d updated successfully\n", expense_id);
            break;
        }
        case 2: {
            float expense_amount;
            printf("enter expense amount: ");
            scanf("%f", &expense_amount);
            while (expense_amount <= 0) {
                printf("Invalid input. Enter only positive values: ");
                scanf("%f", &expense_amount);
            }
            exp_node->expense_amount = expense_amount;
            printf("expense %d updated successfully\n", expense_id);
            
            if (family_count > 0) {
                family *fam = find_family_by_user_id(*fam_root, exp_node->user_ID);
                if (fam) {
                    fam->total_family_expense += (expense_amount - old_expense_amount);
                }
            }
            break;
        }
        case 3: {
            Date d;
            printf("enter date: ");
            scanf("%u", &d.date);
            while (d.date > 10 || d.date <= 0) {
                printf("enter only 1 - 10: ");
                scanf("%u", &d.date);
            }
            printf("enter month: ");
            scanf("%u", &d.month);
            while (d.month > 12 || d.month <= 0) {
                printf("enter only 1 - 12: ");
                scanf("%u", &d.month);
            }
            printf("enter year: ");
            scanf("%u", &d.year);
            while (d.year <= 0 || d.year > 2025) {
                printf("enter only 1 - 2025: ");
                scanf("%u", &d.year);
            }
            exp_node->date_of_expense = d;
            printf("expense %d updated successfully\n", expense_id);
            break;
        }
        default: {
            printf("invalid option\n");
            return FAILURE;
        }
    }
    
    return SUCCESS;
}

statusCode deleteExpense(expense_tree_node **expense_root, family_tree_node** fam_root) {
    if (expense_count <= 0) {
        printf("expense list is empty\n");
        return FAILURE;
    }

    int expense_id;
    printf("enter expense id to delete: ");
    scanf("%d", &expense_id);
    
    expense *exp_node = search_expense_by_id(*expense_root, expense_id);
    if (!exp_node) {
        printf("expense id does not exist\n");
        return FAILURE;
    }

    int user_id = exp_node->user_ID;
    float expense_amount = exp_node->expense_amount;

    *expense_root = delete_expense(*expense_root, *exp_node);
    
    if (family_count > 0) {
        family *fam = find_family_by_user_id(*fam_root, user_id);
        if (fam) {
            fam->total_family_expense -= expense_amount;
        }
    }

    if (*expense_root == NULL) {
        printf("expense tree is now empty\n");
    }

    printf("expense %d deleted successfully\n", expense_id);
    
    return SUCCESS;
}

// Case 6: Get Total Expense for a family
statusCode Get_Total_Expense(family_tree_node* family_root) {
    if (family_count <= 0) {
        printf("No families exist in the database.\n");
        return FAILURE;
    }

    int family_id;
    printf("Enter Family ID: ");
    scanf("%d", &family_id);

    family* fam = search_family(family_root, family_id);
    if (!fam) {
        printf("Family with ID %d not found.\n", family_id);
        return FAILURE;
    }

    printf("\n=== Family %d (%s) Total Expenses ===\n", fam->family_ID, fam->family_name);
    printf("Total Income: %.2f\n", fam->total_family_income);
    printf("Total Expenses: %.2f\n", fam->total_family_expense);
    
    float difference = fam->total_family_income - fam->total_family_expense;
    if (difference >= 0) {
        printf("Status: UNDER BUDGET (Savings: %.2f)\n", difference);
    } else {
        printf("Status: OVER BUDGET (Deficit: %.2f)\n", -difference);
    }
    
    printf("\nFamily Members:\n");
    for (int i = 0; i < fam->total_members; i++) {
        printf("- User ID: %d\n", fam->family_members_user_id[i]);
    }
    
    return SUCCESS;
}

void category_expense_helper(expense_tree_node *expense_root, family fam, float *amount_array) {
    if (expense_root) {
        int i;
        for (i = 0; i < expense_root->keys; i++) {
            category_expense_helper(expense_root->children[i], fam, amount_array);

            for (int j = 0; j < fam.total_members; j++) {
                if (expense_root->deck[i].user_ID == fam.family_members_user_id[j]) {
                    if (strcmp(expense_root->deck[i].expense_category, "RENT") == 0) {
                        amount_array[0] += expense_root->deck[i].expense_amount;
                    } else if (strcmp(expense_root->deck[i].expense_category, "UTILITY") == 0) {
                        amount_array[1] += expense_root->deck[i].expense_amount;
                    } else if (strcmp(expense_root->deck[i].expense_category, "GROCERY") == 0) {
                        amount_array[2] += expense_root->deck[i].expense_amount;
                    } else if (strcmp(expense_root->deck[i].expense_category, "STATIONARY") == 0) {
                        amount_array[3] += expense_root->deck[i].expense_amount;
                    } else if (strcmp(expense_root->deck[i].expense_category, "LEISURE") == 0) {
                        amount_array[4] += expense_root->deck[i].expense_amount;
                    }
                    break;
                }
            }
        }
        category_expense_helper(expense_root->children[i], fam, amount_array);
    }
}

// Case 7: Get Categorical Expense for a family
statusCode get_categorical_expense(family_tree_node* family_root, expense_tree_node* expense_root) {
    if (family_count <= 0) {
        printf("No families exist in the database.\n");
        return FAILURE;
    }

    int family_id;
    printf("Enter Family ID: ");
    scanf("%d", &family_id);

    family* fam = search_family(family_root, family_id);
    if (!fam) {
        printf("Family with ID %d not found.\n", family_id);
        return FAILURE;
    }

    float amount_array[5];
    for (int i = 0; i < 5; i++) {
        amount_array[i] = 0.0;
    }

    category_expense_helper(expense_root, *fam, amount_array);

    float total = 0.0;
    for (int i = 0; i < 5; i++) {
        total += amount_array[i];
    }

    printf("\n=== Family %d (%s) Categorical Expenses ===\n", fam->family_ID, fam->family_name);
    printf("RENT: %.2f (%.1f%%)\n", amount_array[0], (amount_array[0]/total)*100);
    printf("UTILITY: %.2f (%.1f%%)\n", amount_array[1], (amount_array[1]/total)*100);
    printf("GROCERY: %.2f (%.1f%%)\n", amount_array[2], (amount_array[2]/total)*100);
    printf("STATIONARY: %.2f (%.1f%%)\n", amount_array[3], (amount_array[3]/total)*100);
    printf("LEISURE: %.2f (%.1f%%)\n", amount_array[4], (amount_array[4]/total)*100);
    printf("TOTAL: %.2f\n", total);
    
    printf("\nBudget Recommendations:\n");
    if (amount_array[0]/total > 0.3) printf("- Consider reducing rent expenses\n");
    if (amount_array[4]/total > 0.2) printf("- Consider reducing leisure expenses\n");
    if (amount_array[2]/total < 0.15) printf("- You're spending relatively little on groceries\n");
    
    return SUCCESS;
}

typedef struct {
    Date date;
    float amount;
} DateExpense;

void highest_expense_day_helper(expense_tree_node *expense_root, family fam, unsigned int month, unsigned int year, DateExpense **arr, int *dateCount) {
    if (expense_root == NULL) return;

    int i;
    for (i = 0; i < expense_root->keys; i++) {
        highest_expense_day_helper(expense_root->children[i], fam, month, year, arr, dateCount);

        for (int j = 0; j < fam.total_members; j++) {
            if (expense_root->deck[i].user_ID == fam.family_members_user_id[j]) {
                Date d = expense_root->deck[i].date_of_expense;

                if (d.month == month && d.year == year) {
                    int day = d.date;

                    if ((arr[day])->date.date == 0) {
                        (arr[day])->date = d;
                        (*dateCount)++;
                    }

                    (arr[day])->amount += expense_root->deck[i].expense_amount;
                }
            }
        }
    }

    highest_expense_day_helper(expense_root->children[i], fam, month, year, arr, dateCount);
}

// Case 8: Get Highest Expense Day for a family
statusCode get_highest_expense_day(family_tree_node* family_root, expense_tree_node* expense_root) {
    if (family_count <= 0) {
        printf("No families exist in the database.\n");
        return FAILURE;
    }

    int family_id;
    printf("Enter Family ID: ");
    scanf("%d", &family_id);

    family* fam = search_family(family_root, family_id);
    if (!fam) {
        printf("Family with ID %d not found.\n", family_id);
        return FAILURE;
    }

    unsigned int month;
    unsigned int year;
    printf("Enter month: ");
    scanf("%u", &month);
    printf("Enter year: ");
    scanf("%u", &year);

    int dateCount = 0;

    DateExpense **dateExpenses = malloc(sizeof(DateExpense*) * (TOTAL_DAYS + 1)); 
    for (int i = 0; i <= TOTAL_DAYS; i++) {
        dateExpenses[i] = malloc(sizeof(DateExpense));
        dateExpenses[i]->amount = 0.0;
        dateExpenses[i]->date.date = 0;
        dateExpenses[i]->date.month = 0;
        dateExpenses[i]->date.year = 0;
    }

    highest_expense_day_helper(expense_root, *fam, month, year, dateExpenses, &dateCount);

    if (dateCount == 0) {
        printf("No expenses found for this family in the specified month and year.\n");

        for (int i = 0; i <= TOTAL_DAYS; i++) {
            free(dateExpenses[i]);
        }
        free(dateExpenses);
        return SUCCESS;
    }

    int maxIndex = 1;
    for (int i = 2; i <= TOTAL_DAYS; i++) {
        if (dateExpenses[i]->amount > dateExpenses[maxIndex]->amount) {
            maxIndex = i;
        }
    }

    printf("\n=== Family %d (%s) Highest Expense Day ===\n", fam->family_ID, fam->family_name);
    printf("Date: %02u-%02u-%04u\n", 
           dateExpenses[maxIndex]->date.date,
           dateExpenses[maxIndex]->date.month,
           dateExpenses[maxIndex]->date.year);
    printf("Total Amount: %.2f\n", dateExpenses[maxIndex]->amount);

    printf("\nAll Expense Days:\n");
    for (int i = 1; i <= TOTAL_DAYS; i++) {
        if (dateExpenses[i]->date.date != 0) {
            printf("%02u-%02u-%04u: %.2f\n", 
                   dateExpenses[i]->date.date,
                   dateExpenses[i]->date.month,
                   dateExpenses[i]->date.year,
                   dateExpenses[i]->amount);
        }
    }
    for (int i = 0; i <= TOTAL_DAYS; i++) {
        free(dateExpenses[i]);
    }
    free(dateExpenses);

    return SUCCESS;
}

typedef struct {
    char name[20];
    float value;
    float percentage;
} CategoryValue;

void individual_expense_helper(expense_tree_node *expense_root, int user_id, float *amount_array) {
    if (expense_root) {
        int i;
        for (i = 0; i < expense_root->keys; i++) {
            individual_expense_helper(expense_root->children[i], user_id, amount_array);
            if (expense_root->deck[i].user_ID == user_id) {
                if (strcmp(expense_root->deck[i].expense_category, "RENT") == 0) {
                    amount_array[0] += expense_root->deck[i].expense_amount;
                } else if (strcmp(expense_root->deck[i].expense_category, "UTILITY") == 0) {
                    amount_array[1] += expense_root->deck[i].expense_amount;
                } else if (strcmp(expense_root->deck[i].expense_category, "GROCERY") == 0) {
                    amount_array[2] += expense_root->deck[i].expense_amount;
                } else if (strcmp(expense_root->deck[i].expense_category, "STATIONARY") == 0) {
                    amount_array[3] += expense_root->deck[i].expense_amount;
                } else if (strcmp(expense_root->deck[i].expense_category, "LEISURE") == 0) {
                    amount_array[4] += expense_root->deck[i].expense_amount;
                } else {
                    printf("Unknown category: %s\n", expense_root->deck[i].expense_category);
                }
            }
        }
        individual_expense_helper(expense_root->children[i], user_id, amount_array);
    }
}

// Case 9: Get Individual Expense for a user
statusCode Get_Individual_Expense(expense_tree_node* expense_root, user_tree_node* user_root) {
    if (user_count <= 0) {
        printf("No users exist in the database.\n");
        return FAILURE;
    }

    int user_id;
    printf("Enter User ID: ");
    scanf("%d", &user_id);

    user_tree_node* user_node = search_user(user_root, user_id);
    if (!user_node) {
        printf("User with ID %d not found.\n", user_id);
        return FAILURE;
    }

    float amount_array[5] = {0.0, 0.0, 0.0, 0.0, 0.0};

    individual_expense_helper(expense_root, user_id, amount_array);

    float total = 0.0;
    for (int i = 0; i < 5; i++) {
        total += amount_array[i];
    }

    printf("\n=== User %d (%s) Expense Report ===\n", user_node->data.user_ID, user_node->data.user_name);
    printf("Income: %.2f\n", user_node->data.income);
    printf("Total Expenses: %.2f\n", total);

    if (total == 0.0) {
        printf("No expenses recorded for this user.\n");
        return SUCCESS;
    }

    float difference = user_node->data.income - total;
    if (difference >= 0) {
        printf("Status: UNDER BUDGET (Savings: %.2f)\n", difference);
    } else {
        printf("Status: OVER BUDGET (Deficit: %.2f)\n", -difference);
    }

    CategoryValue categories[5] = {
        {"RENT", amount_array[0], (amount_array[0] / total) * 100},
        {"UTILITY", amount_array[1], (amount_array[1] / total) * 100},
        {"GROCERY", amount_array[2], (amount_array[2] / total) * 100},
        {"STATIONARY", amount_array[3], (amount_array[3] / total) * 100},
        {"LEISURE", amount_array[4], (amount_array[4] / total) * 100}
    };

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 - i; j++) {
            if (categories[j].value < categories[j + 1].value) {
                CategoryValue temp = categories[j];
                categories[j] = categories[j + 1];
                categories[j + 1] = temp;
            }
        }
    }

    printf("\nCategory Breakdown (Descending Order):\n");
    for (int i = 0; i < 5; i++) {
        printf("%s: %.2f (%.1f%%)\n", 
               categories[i].name, 
               categories[i].value, 
               categories[i].percentage);
    }

    printf("\nBudget Recommendations:\n");
    if (amount_array[0] > 0 && amount_array[0]/total > 0.35)
        printf("- Consider reducing housing costs\n");
    if (amount_array[4] > 0 && amount_array[4]/total > 0.25)
        printf("- Consider reducing leisure expenses\n");
    if (amount_array[2] > 0 && amount_array[2]/total < 0.15)
        printf("- You're spending relatively little on groceries\n");

    return SUCCESS;
}

int compare_dates(Date date1, Date date2) {
    if (date1.year < date2.year) return -1;
    if (date1.year > date2.year) return 1;
    
    if (date1.month < date2.month) return -1;
    if (date1.month > date2.month) return 1;
    
    if (date1.date < date2.date) return -1;
    if (date1.date > date2.date) return 1;
    
    return 0; 
}

int is_valid_date(Date date) {
    if (date.year < 1900 || date.year > 2100) return 0;
    if (date.month < 1 || date.month > 12) return 0;
    
    int days_in_month = 10;
    if (date.date < 1 || date.date > days_in_month) return 0;
    
    return 1; 
}

void expense_in_period_helper(expense_tree_node *expense_root, Date d1, Date d2, int *count, float *total_amount) {
    if (expense_root == NULL) return;

    int i;
    for (i = 0; i < expense_root->keys; i++) {
        expense_in_period_helper(expense_root->children[i], d1, d2, count, total_amount);
        Date d = expense_root->deck[i].date_of_expense;
        if (compare_dates(d, d1) >= 0 && compare_dates(d, d2) <= 0) {
            printf("%-10d %-20s $%-14.2f %02d/%02d/%04d   %-10d\n", 
                expense_root->deck[i].expense_ID,
                expense_root->deck[i].expense_category,
                expense_root->deck[i].expense_amount,
                expense_root->deck[i].date_of_expense.date, expense_root->deck[i].date_of_expense.month, expense_root->deck[i].date_of_expense.year,
                expense_root->deck[i].user_ID);

            (*count) += 1;
            (*total_amount) += expense_root->deck[i].expense_amount;
        }
    }
    expense_in_period_helper(expense_root->children[i], d1, d2, count, total_amount);
}

statusCode Get_expense_in_period(expense_tree_node* expense_root) {
    if (!expense_root) {
        printf("No expenses exist in the database.\n");
        return FAILURE;
    }
    
    Date date1, date2;
    printf("Enter start date (DD MM YYYY): ");
    scanf("%u %u %u", &date1.date, &date1.month, &date1.year);
    
    printf("Enter end date (DD MM YYYY): ");
    scanf("%d %d %d", &date2.date, &date2.month, &date2.year);
    
    if (!is_valid_date(date1) || !is_valid_date(date2)) {
        printf("Invalid date format.\n");
        return FAILURE;
    }
    
    if (compare_dates(date1, date2) > 0) {
        Date temp = date1;
        date1 = date2;
        date2 = temp;
        printf("Note: Dates were swapped to ensure correct order.\n");
    }
    
    printf("\n=== Expenses from %02d/%02d/%04d to %02d/%02d/%04d ===\n", 
        date1.date, date1.month, date1.year, 
        date2.date, date2.month, date2.year);
    printf("%-10s %-20s %-15s %-12s %-10s\n", 
                "Exp ID", "Category", "Amount", "Date", "User ID");
    printf("--------------------------------------------------------------\n");
    
    int count = 0;
    float total_amount = 0.0;

    expense_in_period_helper(expense_root, date1, date2, &count, &total_amount);
    
    if (count == 0) {
        printf("No expenses found within the specified date range.\n");
        return FAILURE;
    } else {
        printf("--------------------------------------------------------------\n");
        printf("Total expenses in period: %d\n", count);
        printf("Total amount spent: $%.2f\n", total_amount);
        return SUCCESS;
    }
}

void expense_in_range_helper(expense_tree_node *expense_root, int user_id, int exp_id1, int exp_id2, int *count, float *total_amount, float *user_total) {
    if (expense_root == NULL) return;

    int i;
    for (i = 0; i < expense_root->keys; i++) {
        expense_in_range_helper(expense_root->children[i], user_id, exp_id1, exp_id2, count, total_amount, user_total);
        if (expense_root->deck[i].expense_ID >= exp_id1 && expense_root->deck[i].expense_ID <= exp_id2 && expense_root->deck[i].user_ID == user_id) {       
            printf("%-10d %-20s $%-14.2f %02d/%02d/%04d\n", 
                expense_root->deck[i].expense_ID,
                expense_root->deck[i].expense_category,
                expense_root->deck[i].expense_amount,
                expense_root->deck[i].date_of_expense.date, expense_root->deck[i].date_of_expense.month, expense_root->deck[i].date_of_expense.year);
            
            *count += 1;
            *total_amount += expense_root->deck[i].expense_amount;
        }
        if (expense_root->deck[i].user_ID == user_id) {
            *user_total += expense_root->deck[i].expense_amount;
        }
    }
    expense_in_range_helper(expense_root->children[i], user_id, exp_id1, exp_id2, count, total_amount, user_total);
}

statusCode Get_expense_in_range(expense_tree_node* expense_root, user_tree_node* user_root) {
    if (!expense_root) {
        printf("No expenses exist in the database.\n");
        return FAILURE;
    }
    
    int expense_id_1, expense_id_2, user_id;
    printf("Enter first expense ID: ");
    scanf("%d", &expense_id_1);
    
    printf("Enter second expense ID: ");
    scanf("%d", &expense_id_2);
    
    printf("Enter user ID: ");
    scanf("%d", &user_id);
    
    user_tree_node* user_node = search_user(user_root, user_id);
    if (!user_node) {
        printf("User with ID %d not found.\n", user_id);
        return FAILURE;
    }
    
    if (expense_id_1 > expense_id_2) {
        int temp = expense_id_1;
        expense_id_1 = expense_id_2;
        expense_id_2 = temp;
        printf("Note: Expense IDs were swapped to ensure correct order.\n");
    }
    
    printf("\n=== Expenses for User %d (%s) with IDs from %d to %d ===\n", 
           user_id, user_node->data.user_name, expense_id_1, expense_id_2);
    printf("%-10s %-20s %-15s %-12s\n", 
           "Exp ID", "Category", "Amount", "Date");
    printf("----------------------------------------------------------\n");
    
    int count = 0;
    float total_amount = 0.0;
    float user_total = 0.0;
    expense_in_range_helper(expense_root, user_id, expense_id_1, expense_id_2, &count, &total_amount, &user_total);
    
    // expense_tree_node* current = expense_root;
    // while (current && !current->is_leaf) {
    //     current = current->children[0];
    // }
    
    // while (current) {
    //     for (int i = 0; i < current->num_keys; i++) {
    //         expense exp = current->expenses[i];
            
    //         if (exp.expense_ID >= expense_id_1 && 
    //             exp.expense_ID <= expense_id_2 &&
    //             exp.user_ID == user_id) {
                
    //             printf("%-10d %-20s $%-14.2f %02d/%02d/%04d\n", 
    //                    exp.expense_ID,
    //                    exp.expense_category,
    //                    exp.expense_amount,
    //                    exp.date_of_expense.date, exp.date_of_expense.month, exp.date_of_expense.year);
                
    //             count++;
    //             total_amount += exp.expense_amount;
    //         }
    //     }
    //     current = current->next; 
    // }
    
    if (count == 0) {
        printf("No expenses found within the specified ID range for user %d.\n", user_id);
        return FAILURE;
    } else {
        printf("----------------------------------------------------------\n");
        printf("Total expenses found: %d\n", count);
        printf("Total amount: $%.2f\n", total_amount);
        
        // current = expense_root;
        // while (current && !current->is_leaf) {
        //     current = current->children[0];
        // }
        
        // while (current) {
        //     for (int i = 0; i < current->num_keys; i++) {
        //         expense exp = current->expenses[i];
        //         if (exp.user_ID == user_id) {
        //             user_total += exp.expense_amount;
        //         }
        //     }
        //     current = current->next;
        // }
        
        if (user_total > 0) {
            float percentage = (total_amount / user_total) * 100;
            printf("This represents %.2f%% of user's total expenses ($%.2f)\n", 
                   percentage, user_total);
        }
        
        return SUCCESS;
    }
}

statusCode readUsersFile(user_tree_node** user_root){
    statusCode sc;
    FILE *file = fopen("users.txt", "r");
    if (!file) { 
        sc = FAILURE;
    } else {
        user u;
        while (user_count < 25 && 
            fscanf(file, "%d,%99[^,],%f", &u.user_ID, u.user_name, &u.income) == 3) {
            *user_root = insert_user(*user_root, u);
            
        }
        fclose(file);
        sc = SUCCESS;
    }
    return sc;
}

statusCode readExpensesFile(expense_tree_node** expense_root) {
    FILE *file = fopen("expenses.txt", "r");
    if (!file) {
        perror("Error opening expenses file");
        return FAILURE;
    }

    expense e;
    int line_num = 0;
    char line[256];

    while (fgets(line, sizeof(line), file) && expense_count < 40) {
        line_num++;
        line[strcspn(line, "\n")] = '\0';
        memset(&e, 0, sizeof(expense));
        int parsed = sscanf(line, "%d,%d,%63[^,],%f,%u,%u,%u",
                          &e.expense_ID,
                          &e.user_ID,
                          e.expense_category,
                          &e.expense_amount,
                          &e.date_of_expense.date,
                          &e.date_of_expense.month,
                          &e.date_of_expense.year);

        if (parsed != 7) {
            printf("Warning: Line %d has incorrect format (parsed %d/7 fields): %s\n", 
                  line_num, parsed, line);
            continue; 
        }
        *expense_root = insert_expense(*expense_root, e);
    }

    fclose(file);
    
    if (expense_count == 0) {
        printf("Warning: No expenses were read from the file.\n");
        return FAILURE;
    }

    return SUCCESS;
}

statusCode readFamiliesFile(family_tree_node **family_root, user_tree_node *user_root, expense_tree_node *expense_root) {
    FILE *file = fopen("families.txt", "r");
    if (!file) {
        perror("Error opening families file");
        return FAILURE;
    }

    char line[256];
    int line_num = 0;
    int families_read = 0;

    while (fgets(line, sizeof(line), file) && families_read < MAX_FAMILIES) {
        line_num++;
        line[strcspn(line, "\n")] = '\0';
        if (strlen(line) == 0) continue;
        family f;
        memset(&f, 0, sizeof(family));
        
        char *token = strtok(line, ",");
        if (!token) {
            printf("Warning: Line %d missing family ID\n", line_num);
            continue;
        }
        f.family_ID = atoi(token);
        
        token = strtok(NULL, ",");
        if (!token) {
            printf("Warning: Line %d missing family name\n", line_num);
            continue;
        }
        strncpy(f.family_name, token, sizeof(f.family_name)-1);
        
        token = strtok(NULL, ",");
        if (!token) {
            printf("Warning: Line %d missing total members\n", line_num);
            continue;
        }
        f.total_members = atoi(token);
        int members_read = 0;
        while ((token = strtok(NULL, ",")) != NULL && members_read < f.total_members) {
            if (members_read >= 100) {
                printf("Warning: Line %d has too many members (max %d)\n", 
                      line_num, 100);
                break;
            }
            f.family_members_user_id[members_read++] = atoi(token);
        }
        
        if (members_read != f.total_members) {
            printf("Warning: Line %d expected %d members but found %d\n",
                  line_num, f.total_members, members_read);
            continue;
        }
        f.total_family_income = calculate_monthly_income_of_family(f, user_root);
        f.total_family_expense = calculate_monthly_expense_of_family(expense_root, f);
        *family_root = insert_family(*family_root, f);
        families_read++;
    }

    fclose(file);
    
    if (families_read == 0) {
        printf("Warning: No families were read from the file.\n");
        return FAILURE;
    }

    return SUCCESS;
}


int main() {
    user_tree_node *user_root = NULL;
    expense_tree_node *expense_root = NULL;
    family_tree_node *family_root = NULL;
    statusCode sc;
    sc = readUsersFile(&user_root);
    sc = readExpensesFile(&expense_root);
    sc = readFamiliesFile(&family_root, user_root, expense_root);

    traverse_users(user_root);
    printf("\n");
    traverseExpenses(expense_root);
    printf("\n");
    traverseFamilyTree(family_root);
    printf("\n");

    int n = 1;
    while (n < 16 && n != 15 && n > 0) {
        printf("Enter 1  : Add User\n");
        printf("Enter 2  : Add Expense\n");
        printf("Enter 3  : Create Family\n");
        printf("Enter 4  : Update or Delete individual family\n");
        printf("Enter 5  : Update or Delete Expense\n");
        printf("Enter 6  : Get Total Expense\n");
        printf("Enter 7  : Get Categorical Expense\n");
        printf("Enter 8  : Get Highest Expense Day\n");
        printf("Enter 9  : Get Individual Expense\n");
        printf("Enter 10 : Get_expense_in_period\n");
        printf("Enter 11 : Get_expense_in_range\n");
        printf("Enter 12 : print users details\n");
        printf("Enter 13 : print expenses details\n");
        printf("Enter 14 : print families details\n");
        printf("Enter 15 : Exit\n");
        printf("enter n : ");
        scanf("%d", &n);

        switch (n) {
            case 1 :{
                statusCode status = AddUser(&user_root, &family_root);
                if (status == SUCCESS) {
                    printf("user added successfully.\n");
                    traverse_users(user_root);
                    printf("\n");
                    traverseFamilyTree(family_root);
                    printf("\n");
                } else {
                    printf("user adding operation failed.\n");
                }
                break;
            }
            case 2 :{
                statusCode status = AddExpense(&expense_root, &user_root);
                if (status == SUCCESS) {
                    printf("expense added successfully.\n");
                    traverseExpenses(expense_root);
                } else {
                    printf("expense operation failed.\n");
                }
                break;
            }
            case 3 : {
                statusCode status = CreateFamily(&family_root, &user_root, &expense_root);
                if(status == SUCCESS) {
                    printf("Family Created Successfully.\n");
                    traverseFamilyTree(family_root);
                } else {
                    printf("Error creating family.\n");
                }
                break;
            }
            case 4:{
                int j;
                printf("enter 1 : update_user\n");
                printf("enter 2 : update_family\n");
                printf("enter 3 : delete user\n");
                printf("enter 4 : delete family\n");
                printf("enter j : ");
                scanf("%d", &j);
                switch(j) {
                    case 1: {
                        statusCode status = updateUserDetails(&user_root, &family_root);
                        if (status == FAILURE) {
                            printf("update user unsuccessful.\n");
                        }
                        break;
                    }
                    case 2:{
                        int fam_id;
                        statusCode status = updateFamilyDetails(&family_root, &user_root, &expense_root, &fam_id);
                        if (status == SUCCESS) {
                            printf("family %d details updated successfully.\n", fam_id);
                        } else {
                            printf("family %d details updated was unsuccessful.\n", fam_id);
                        }
                        break;
                    }
                    case 3:{
                        statusCode status = deleteUser(&user_root, &family_root, &expense_root);
                        break;
                    }
                    case 4:{
                        statusCode status = deleteFamily(&family_root, &user_root, &expense_root);
                        break;
                    }
                    default:{
                        printf("you entered wrong number.\n");
                        break;
                    }
                }
                break;
            }
            case 5:{
                int p;
                printf("enter 1 : update expenses\n");
                printf("enter 2 : delete expenses\n");
                printf("enter your number : ");
                scanf("%d", &p);
                switch(p){
                    case 1: {
                        statusCode status = updateExpense(&expense_root, &family_root);
                        break;
                    }
                    case 2: {
                        statusCode status = deleteExpense(&expense_root, &family_root);
                        break;
                    }
                    default: {
                        printf("you entered a wrong number.\n");
                        break;
                    }
                }
                break;
            }
            case 6: {
                if(family_count > 0) {
                    statusCode sc = Get_Total_Expense(family_root);
                }
                break;
            }
            case 7: {
                if(family_count > 0 && expense_count > 0) {
                    statusCode sc = get_categorical_expense(family_root, expense_root);
                }
                break;
            }
            case 8: {
                if (family_count > 0 && expense_count > 0) {
                    statusCode sc = get_highest_expense_day(family_root, expense_root);
                }
                break;
            }
            case 9: {
                if(expense_count > 0 && user_count > 0) {
                    statusCode sc = Get_Individual_Expense(expense_root, user_root);
                }
                break;
            }
            case 10: {
                statusCode sc = Get_expense_in_period(expense_root);
                break;
            }
            case 11: {
                statusCode sc = Get_expense_in_range(expense_root, user_root);
                break;
            }
            case 12: {
                traverse_users(user_root);
                break;
            }
            case 13: {
                traverseExpenses(expense_root);
                break;
            }
            case 14: {
                traverseFamilyTree(family_root);
                break;
            }
            case 15: {
                printf("Exiting.....\n");
                break;
            }
            default:{
                printf("you entered a wrong number.\n");
                break;
            }
        }
    }
    return 0;
}