    #include <stdio.h>
    #include <stdlib.h>

    typedef struct node{
        int info;
        struct node *next;
        
    }node;

    void add_node(node *current, node *newNode);
    void delete_list(node **head);
    void print_list(node *current_node);
    void remove_node(node **head, int value);
    void delete_node(node *current_node, node *prev, int value);
    void insert_node(node **head, int value);


    int main(){
        node *head = NULL;
        insert_node(&head, 1);
        insert_node(&head, 2);
        insert_node(&head, 3);
        insert_node(&head, 4);
        insert_node(&head, 1);
        insert_node(&head, 2);
        remove_node(&head, 4);
        insert_node(&head, 3);
        insert_node(&head, 4);
        
        print_list(head);
        delete_list(&head);

        



        return 0;
    }
    void insert_node(node **head, int value){
        node *newNode = (node *)malloc(sizeof(node));
        newNode->info = value;
        if (*head == NULL || newNode->info > (*head)->info){
            newNode->next = *head;
            *head = newNode;
            return;
        }
        add_node(*head, newNode);
        
    }

    void add_node(node *current, node *newNode){
        
        if (current -> next == NULL){
            current->next = newNode;
            newNode->next = NULL;
            return;
        }
        else if(newNode->info > current->next->info){
            newNode->next = current->next;
            current->next = newNode;
            return;
        }
        add_node(current->next, newNode);
    }

    void delete_list(node **head){
        if (*head == NULL){
            return;
        }
        delete_list(&((*head)->next));
        free(*head);
        *head = NULL;

    }

    void print_list(node *current_node){
        if (current_node == NULL){
            return;
        }
        printf("%d\n",current_node->info);
        print_list(current_node->next);
        return;
    }


    void remove_node(node **head, int value){
        if (*head == NULL){
            return;
        }
        else if ((*head)->info == value){
            if((*head)->next == NULL){
                free(*head);
                *head = NULL;

            }
        
            else{
                node *temp = (*head)->next;
                free(*head);
                *head = temp;
            }}
        else{
            delete_node((*head)->next, *head, value);
        }
        }

    void delete_node(node *current, node *prev, int value){
        if (current == NULL){
            return;
        }
        if (value == current->info){
            prev->next = current->next;
            free(current);
            return;
        }
        delete_node(current->next, current, value);



    }



