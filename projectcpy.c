
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "listops.c"
#include <assert.h>
#include <math.h>


/* stage heading */
#define STAGE_NUM_ONE 1						  /* stage numbers */ 
#define STAGE_NUM_TWO 2
#define STAGE_NUM_THREE 3
#define STAGE_NUM_FOUR 4
#define STAGE_NUM_FIVE 5
#define HEADING "==========================Stage %d==========================\n"
#define MAXVAL 99
#define MINVAL 1
#define MAXRESNAME 101
#define MAXCUISINE 21
#define MAXCUSID 7

/* typedefs */
/*resinfo_t is used to store all information about 1 particular restaurant as inputted.*/
typedef struct {
    int ID;
    double x;
    double y;
    int avg_price;
    char cuisine[MAXCUISINE];
    char res_name[MAXRESNAME];
} resinfo_t;

/*allresinfo_t stores an array of resinfo_t type variable (Info of all restaurants inputted) 
   and the quantity of restaurants.*/
typedef struct{
    resinfo_t res[MAXVAL];
    int resno;
} allresinfo_t;

/*Saves the logs of each customer visit*/
typedef struct{
    char date[20];
    char cus_ID[MAXCUSID];
    int visited_res;
    int amnt_spent;
}cus_info_t;



/****************************************************************/
/* function prototypes */
void print_stage_header(int stage);
void stage_one(allresinfo_t *all_res);
void stage_two(allresinfo_t *all_res, list_t *list);
void stage_three(allresinfo_t *all_res, list_t* list);
void stage_four(allresinfo_t *all_res, list_t *list);
int read_res(resinfo_t *res);
void read_all_res(allresinfo_t *all_res);
char* cheapest(allresinfo_t *res);
void traverse_add(list_t *list, cus_info_t *temp, allresinfo_t *all_res);
int find_index(allresinfo_t *all_res, cus_info_t *temp);
void list_cmp(node_t *curr_node, allresinfo_t *all_res);
int similarity(node_t *node1, node_t *node2, int res_amnt);
void sim_search(node_t *comp_node, allresinfo_t *all_res, list_t *list);
/****************************************************************/

int
main(int argc, char *argv[]) {

    /*Allocates memory for all restaurant information to be stored*/
    allresinfo_t *all_res = (allresinfo_t*)malloc(sizeof(allresinfo_t));
    /*Stage 1*/
    stage_one(all_res);
    /*Stage 2*/
    /*Creates an empty list that is used in the following functions.*/
    list_t *list = make_empty_list();
    stage_two(all_res,list);

    stage_three(all_res, list);

    stage_four(all_res,list);

    return 0;
}

/* print stage header given stage number */
void 
print_stage_header(int stage_num) {
	printf(HEADING, stage_num);
}

/* TODO: Define more functions! */
/*Fuction that scans in restaurant info ensuring that a specific amount of inputs are given.*/
int read_res(resinfo_t *res){
    return scanf("%d %lf %lf %d %s %s\n", &res->ID, &res->x, &res->y, 
                    &res->avg_price, res->cuisine, res->res_name) == 6;
}

/*Function takes in all the reads and retrieves the number of restaurant inputs.*/
void read_all_res(allresinfo_t *all_res){
    all_res->resno = 0;
    while (all_res->resno <= MAXVAL && read_res(&(all_res->res[all_res->resno]))){
        ++all_res->resno;
    }
}

/*Function is used to find the restaurant with the cheapest average price*/
char* cheapest(allresinfo_t *all_res){
    /*The function first searches for the index of the cheapest restaurant*/
    int count = 1, cheap_ind;
    cheap_ind = 0;
    while (count < all_res->resno){
        if (all_res->res[count].avg_price < all_res->res[cheap_ind].avg_price){
            cheap_ind = count;
        }
        count += 1;
    }
    /*returning the string name of the cheapest restaurant.*/
    return all_res->res[cheap_ind].res_name;
}

/*Function traverses the linked list and adds to foot when a new customer is 
introduced or adjusts the data of a node when a repeated customer appears*/
void traverse_add(list_t *list, cus_info_t *temp, allresinfo_t *all_res){
	node_t* curr_node = list->head;
	while(curr_node != NULL){
        /*Case when a customer has a repeated visit*/
		if((strcmp(curr_node->data.cus_ID_rep, temp->cus_ID) == 0)){
            int ind_res = 0;
            ind_res = find_index(all_res, temp); //Index of restaurant visited is obtained.
            curr_node->data.visits[ind_res] += 1; //value at the index obtained is added by 1 in the integer array.
            break;
    }
		curr_node = curr_node->next; //Moves on to the next node
	}
    /*Case when a new customer is introduced*/
    if (curr_node == NULL){
        data_t* temp_rec = (data_t*)malloc(sizeof(data_t)); //A temporary data_t type variable is introduced.
        temp_rec->visits = (int*)malloc(sizeof(int)*all_res->resno); //Memory is allocated to visits according to the number of restaurants inputted.
        strcpy(temp_rec->cus_ID_rep, temp->cus_ID);
        /*Integer array is appended with zeros*/
        for(int count = 0; count < all_res->resno; count++){
            temp_rec->visits[count] = 0;
        }
        int ind_res = 0;
        ind_res = find_index(all_res, temp); //Index of the sutomers first visit is obtained.
        temp_rec->visits[ind_res] += 1; //The index found is added by one in the integer array.
        list = insert_at_foot(list, *temp_rec); //Node is added to the foot of the linked list.
    }
}

/*Function is used to find the index of the restaurant visited*/
int find_index(allresinfo_t *all_res, cus_info_t *temp){
    int count = 0;
    while(all_res->res[count].ID != temp->visited_res){
        count++;
   }
    return count;
}

/*Function compares the restaurant info visited by the customer and the restaurant list. A recommendation is given
  in accordance to certain cases*/
void list_cmp(node_t *curr_node, allresinfo_t* all_res){
    for(int count = 0; count < all_res->resno; count++){
        if(curr_node->data.visits[count] > 0){
            for(int loop_count = 0; loop_count<all_res->resno; loop_count++){
                /*Case 1: If the restaurant has already been visited or 
                            value in the visits integer array for that index has already been adjusted.*/
                if (all_res->res[loop_count].ID == all_res->res[count].ID 
                        || curr_node->data.visits[loop_count] != 0){
                    continue;
                }
                /*Case 2: If restaurants have the same cuisine*/
                else if(strcmp(all_res->res[loop_count].cuisine, all_res->res[count].cuisine) == 0){
                    curr_node->data.visits[loop_count] = -1;
                }
                /*Case 3: If restaurants are between 30units of each other.*/
                else if(sqrt(pow(all_res->res[count].x - all_res->res[loop_count].x, 2) + 
                            (pow(all_res->res[count].y - all_res->res[loop_count].y, 2))) < 30){
                    curr_node->data.visits[loop_count] = -1;
                }
                /*Case 4: If restaurants are between +- 20$ of each other*/
                else if(all_res->res[loop_count].avg_price >= (all_res->res[count].avg_price - 20) 
                        && all_res->res[loop_count].avg_price <= (all_res->res[count].avg_price + 20)){
                        curr_node->data.visits[loop_count] = -1;
                }
            }
        }
    }
}

int similarity(node_t *node1, node_t *node2, int res_amnt){
    int sim_score = 0;
    for(int count = 0; count < res_amnt; count++){
        if(node1->data.visits[count]>= 0 && node2->data.visits[count] >=0){
            sim_score += node1->data.visits[count] * node2->data.visits[count];
        }
    }
    return sim_score;
}
/*Function takes a singular node, comparing it with each 'other' node, finding the top 
2 similar integer arrays, adjusting the values within the original node accordingly.*/
void sim_search(node_t *comp_node, allresinfo_t *all_res, list_t *list){
    node_t *curr_node = list->head;
    int sim1 = 0;
    int sim2 = 0;
    /*2 integer arrays are initialized to save the top 2 most similar arrays for comparison later in the function.*/
    int* sim_array1 = (int*)malloc(sizeof(int) * all_res->resno);
    assert(sim_array1 != NULL);
    int* sim_array2 = (int*)malloc(sizeof(int) * all_res->resno);
    assert(sim_array2 != NULL);
    /*Loop finds the top 2 most similar nodes comparing it to comp_node, traversing through each node*/
    while(curr_node != NULL){
        int sim_score = 0;
        if (curr_node->data.cus_ID_rep != comp_node->data.cus_ID_rep){
            sim_score = similarity(comp_node, curr_node, all_res->resno);
            /*Case 1: When the comparable node is more similar than the previous most similar node, the previous 
            most similar node is made to be the second most similar while the comparable node is made most similar.*/
            if (sim_score > sim1){
                sim2 = sim1;
                memcpy(sim_array2, sim_array1, sizeof(int) * all_res->resno);
                sim1 = sim_score;
                memcpy(sim_array1, curr_node->data.visits, sizeof(int) * all_res->resno);
            }
            /*Case 2: When the comparable node is less similar than the most similar node but more similar than the 
            previous second similar node, the second similar node is replaced with this comparable node.*/
            else if (sim_score > sim2){
                sim2 = sim_score;
                memcpy(sim_array2, curr_node->data.visits, sizeof(int) * all_res->resno);
            }
        }
        curr_node = curr_node->next;
    }
    /*The loop adjusts the visits integer array for comp_node according to the top 2 most similar nodes.*/
    for(int count = 0; count < all_res->resno; count ++){
        if(comp_node->data.visits[count] <= 0){
            if (sim_array1[count] >= 1){
                comp_node->data.visits[count] -= 1;
            }
            if (sim_array2[count] >= 1){
                comp_node->data.visits[count] -= 1;
            }
        }
    }
}

/*Stages*/
/*Stage 1*/
/*Function takes in inputs of the restaurants and displays the number of restaurants inputted and the cheapest restaurant.*/
void stage_one(allresinfo_t *all_res){
    print_stage_header(STAGE_NUM_ONE);
    char* cheapest_res;
    read_all_res(all_res);
    cheapest_res = cheapest(all_res);
    printf("Number of restaurants: %d\n", all_res->resno);
    printf("Restaurant with the smallest average price: %s\n", cheapest_res);
    printf("\n");
}
/*Stage 2*/
/*Function takes in the reads of customer visits, putting them into a linked list 
    in accordance to the amount of visits they have made to each restaurant*/
void stage_two(allresinfo_t *all_res, list_t *list){
    print_stage_header(STAGE_NUM_TWO);
    char redundant[6]; //Used to scan in the divider #####
    data_t *head = (data_t*)malloc(sizeof(data_t)); //Head of the linked list is allocated memory.
    assert(head!=NULL);
    cus_info_t *temp= (cus_info_t*)malloc(sizeof(cus_info_t)); //A temporary variable with cus_info_t type is used to save the records of the restaurant visits
    assert(temp != NULL);
    scanf("%s\n", redundant);
    scanf("%s %s %d %d\n", temp->date, temp->cus_ID, &temp->visited_res, &temp->amnt_spent); //Takes in the scan of the first visit record
    /*Assigns the first scan to the head of the linked list.*/
    strcpy(head->cus_ID_rep, temp->cus_ID);
    head->visits = (int*)malloc(sizeof(int)*all_res->resno);
    assert(head->visits != NULL);
    for(int count = 0; count<all_res->resno; count++){
        head->visits[count] = 0;
        if (temp->visited_res == all_res->res[count].ID){
            head->visits[count] += 1;
        }
    }
    insert_at_head(list, *head); //head vairable is made the head of the linked list.
    /*Takes in inputs until EOF, running the traverse_add function with the inputs given*/
    while(scanf("%s %s %d %d", temp->date, temp->cus_ID, &temp->visited_res, &temp->amnt_spent)!= EOF){
        traverse_add(list, temp, all_res);
    }
    /*Prints the ID of each customer and the values saved in the integer array*/
    node_t* curr_node = list->head;
    while(curr_node != NULL){
        printf("%s:",curr_node->data.cus_ID_rep);
        for(int count = 0; count < all_res->resno; count++){
                printf("  %d", curr_node->data.visits[count]);
            }
        printf("\n");
        curr_node = curr_node->next;
    }
    printf("\n");
}
/*Function adjusts the values in the visit integer array in accordance 
    to certain cases where restaurants are recommended accordingly.*/
void stage_three(allresinfo_t *all_res, list_t *list){
    print_stage_header(STAGE_NUM_THREE);
    node_t* curr_node = list->head;
    /*Loop runs list_cmp function for each node in the linked list, adjusting the visits integer array.*/
    while(curr_node != NULL){
        list_cmp(curr_node, all_res);
        curr_node = curr_node->next;
    }
    /*Prints the customers IDs and the adjusted integer array.*/
    curr_node = list->head;
    while(curr_node != NULL){
        printf("%s:",curr_node->data.cus_ID_rep);
        for(int count = 0; count < all_res->resno; count++){
            if(curr_node->data.visits[count] == -1){
                printf("  -");
            }
            else{
                printf("  %d", curr_node->data.visits[count]);
            }
        }
        printf("\n");
        curr_node = curr_node->next;
    }
    printf("\n");
}

/*Stage 4*/
/*The function takes a node from the linked list and compares it to all other nodes, finding similarities 
eventually recommending customers other restaurants they may want to visit*/
void stage_four(allresinfo_t *all_res, list_t *list){
    print_stage_header(STAGE_NUM_FOUR);
    node_t * curr_node = list->head;
    /*The loop runs the sim_search function for each node*/
    while(curr_node != NULL){
        sim_search(curr_node, all_res, list);
        curr_node = curr_node->next;
    }
    curr_node = list->head;
    /*Loop traverses the adjusted linked list, printing the values according to certain conditions met.*/
    while(curr_node != NULL){
        printf("%s:",curr_node->data.cus_ID_rep);
        for(int count = 0; count < all_res->resno; count++){
            if(curr_node->data.visits[count] == -1){// When value is -1 in the int array.
                printf("  -");
            }
            else if(curr_node->data.visits[count] == -2){// When value is -2 in the int array.
                printf("  +");
            }
            else if(curr_node->data.visits[count] == -3){// When value is -3 in the int array.
                printf("  *");
            }
            else{
                printf("  %d", curr_node->data.visits[count]);// Prints un adjusted value.
            }
        }
        printf("\n");
        curr_node = curr_node->next;
    }
    printf("\n");
}