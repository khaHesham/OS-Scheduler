#include "memory.h"

int rec_allocate(int bucket)
{
    //base case--> no place in memory
    if(bucket > MAXLEVEL)
    {
        printf("no place in memory");
        return -1;
    }

    //recursive case
    if(free_lists[bucket] == NULL)
    {
        int start_index = rec_allocate(bucket + 1);

        MNode* node = malloc(sizeof(MNode));
        node->index = start_index + pow(2, bucket);
        node->next = NULL;
        free_lists[bucket] = node;

        return start_index;
    }


    else
    {
        MNode* temp = free_lists[bucket];
        free_lists[bucket] = free_lists[bucket]->next;
        int index = temp->index;
        free(temp);

        return index;
    }
}

int allocate(int size)
{
    int bucket = ceil(log2(size));
    return rec_allocate(bucket);
}




/*case 1: no MNode-> prev=cur=prevprev = NULL
case 2: 1 MNode-> 
                insert before-> cur=prev=cur=first MNode
                insert after-> cur=NULL, prev=prevprev=first MNode
case 3: 2 MNodes->
                insert at beginning-> prev=cur=prevprev = 1st MNode
                insert between-> cur=2nd MNode, prev=prevprev=1st MNode
                insert end-> cur=NULL prev=2nd MNode, prevprev=1st MNode
*/

void rec_deallocate(int index, int bucket)
{
    //printf("rec_allocate hereeeee\n");
    int buddyNumber = index / pow(2, bucket);
    int buddyRight = index + pow(2, bucket);
    int buddyLeft = index - pow(2, bucket);

    MNode* prevprev, *prev, *cur;
    prevprev = prev = cur = free_lists[bucket];

    while(cur!=NULL && index > cur->index)
    {
        prevprev = prev;
        prev = cur;
        cur = cur->next;
    }
    //printf("after while\n");

    //merge right
    if(cur!=NULL && buddyNumber % 2 == 0 && cur->index == buddyRight)
    {
        //printf("merge right\n");
        //if first MNode
        if(cur ==  free_lists[bucket])
            free_lists[bucket] = cur->next;
        else
            prev->next = cur->next;
        free(cur);

        rec_deallocate(index, bucket + 1);
    }

    //merge left
    else if(cur!=prev && buddyNumber % 2 != 0 && prev->index == buddyLeft)
    {

        if(prev == prevprev)
            free_lists[bucket] = cur;
            
        else
            prevprev->next = cur;
        free(prev);

        rec_deallocate(buddyLeft, bucket + 1);
    }

    else//no merge
    {
        MNode* MNode = malloc(sizeof(MNode));
        
        MNode->index = index;
        MNode->next = NULL;
        
        if(cur ==  free_lists[bucket])
            free_lists[bucket] = MNode;

        else
            prev->next = MNode;

        MNode->next = cur;

        return;
    }
}

void deallocate(int index, int size)
{
    int bucket = ceil(log2(size));
    rec_deallocate(index, bucket);
}

void initMemory()
{
    for (int i = 0; i <= MAXLEVEL; i++)
    {
        free_lists[i] = NULL;
    }

    free_lists[MAXLEVEL] = malloc(sizeof(MNode));
    free_lists[MAXLEVEL]->index = 0;
    free_lists[MAXLEVEL]->next = NULL;
}

void print()
{
    printf("\n============================================FREE LISTS=========================================================\n");
    for (int i = 0; i <= 7; i++)
    {
        int size = pow(2, i);
        printf("%d: ", size);
        for (MNode* ptr = free_lists[i]; ptr; ptr = ptr->next)
        {
            printf("(%d : %d), ", ptr->index, ptr->index + size);
        }
        printf("\n");
    }
    printf("============================================FREE LISTS=========================================================\n");
}

/*int main()
{

    
    int x20 = allocate(20);
    int x15 = allocate(15);
    int x10 = allocate(10);
    int x25 = allocate(25);

    deallocate(x20, 20);
    deallocate(x10, 10);
    int x8 = allocate(8);
    int x30 = allocate(30);
    deallocate(x15, 15);
    x15 = allocate(15);
    deallocate(x8, 8);
    deallocate(x30, 30);
    deallocate(x15, 15);
    deallocate(x25, 25);

    print();
}*/

// int buddyNumber = index / 2^bucket
// if(buddyNumber is even)//to the right
//     buddyIndex = buddyNumber+2^bucket
// else//to the left 
//     buddyNumber-2^bucket



// search_left()
// if(!found in left)
//     search_right()
// else 
//     terminate


// targeted_MNode = 2^ceil(lg(size));


// 1-no children
//     if(size == MNode_size) 
//         return index
//     split-->rec_allocate(left)
// 2-children
//     int index = rec_allocate(left)
//     if(index != -1)
//         return index
//     rec_allocate(right)



// left_index = parent_index
// right_index = parent_index + size


// if you have enough time check for flag to make sure all children are occupied

    // bool mergable = cur->index == buddyRight || prev->index == buddyLeft;
    // if(!mergable) //no merge-> normal insert 
    // {
    //     MNode* MNode = malloc(sizeof(MNode));
    //     MNode->index = index;
    //     prev->next = MNode;
    //     MNode->next = cur;
    //     return;
    // }

    // //there is a merge
    // if(buddyNumber % 2 == 0)//to the right
    // {
    //     prev->next = cur->next;
    //     free(cur);

    //     rec_deallocate(index, bucket + 1);
    // }
    // else//to the left 
    // {
    //     prevprev->next = cur->next;
    //     free(prev);

    //     rec_deallocate(buddyLeft, bucket + 1);
    // }

//===========================================================TREE=============================================================================
/*typedef struct MNode
{
    int start_index;

    bool free;
    MNode* left;
    MNode* right;

}MNode;


MNode* createMNode()
{
    MNode* MNode = malloc(sizeof(MNode));
    MNode->free = true;
    MNode->left = NULL;
    MNode->right = NULL;

    return MNode;
}

MNode* root;

int allocate(int size)
{
    int targeted_size = pow(2, ceil(log2(size)));
    rec_allocate(targeted_size, root, 0, 0);
}

int rec_allocate(int size, MNode* root, int level, int index)
{
    root->start_index = index;
    int MNode_size = pow(2, (MAXLEVEL - level));

    if(!(root->free)) 
        return -1;

    if(!root->left && !root->right) //no children
    {
        if(size == MNode_size) 
        {
             root->free = false;
            return root->start_index;
        }
            
        //split
        root->left = createMNode();
        root->right = createMNode();
        return rec_allocate(size, root->left, level+1, root->start_index);
    }

    else //children
    {
        int index = rec_allocate(size, root->left, level+1, root->start_index);
        if(index != -1)
            return index;
        return rec_allocate(size, root->right, level+1, root->start_index + MNode_size / 2);
    }
}

deallocate(int index, int size, MNode* root, int level)
{
    int MNode_size = pow(2, (MAXLEVEL - level));

    if(MNode_size == size)
    {
        root->free = true;
        return;
    }
    if(index < root->right->start_index)
        deallocate(index, size, root->left, level+1);
    else
        deallocate(index, size, root->right, level+1);

    //The parent will merge only if its two children are free and none has an children
    bool mergable = root->left->free && root->right->free && !root->left->left && !root->right->right;
    if(mergable)
    {
        free(root->left);
        free(root->right);
        root->left = NULL;
        root->right = NULL;
        root->free = true;
    }
}*/