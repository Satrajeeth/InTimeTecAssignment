#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

#define NAME_BUFFER_SIZE 100

typedef enum {
    RESULT_SUCCESS = 0,
    RESULT_INVALID_INPUT,
    RESULT_NOT_FOUND
} ResultCode;

typedef struct Product Product;
void addProduct(Product **inventory , int *count);
void viewProducts(const Product *inventory , int count);
ResultCode updateQuantity(Product *inventory , int count);
void searchByID(const Product *inventory , int count);
void searchByName(const Product *inventory , int count);
void searchByPriceRange(const Product *inventory , int count);
ResultCode deleteById(Product **inventory , int *count);
void freeInventory(Product *inventory , int *count);

struct Product{
    int id;
    char *name;
    float price;
    int quantity;
};

void addProduct(Product **inventory , int *count){
    *count+=1;
    *inventory = realloc(*inventory,(*count)*sizeof(Product));

    (*inventory)[*count-1].name=malloc(NAME_BUFFER_SIZE * sizeof(char));

    printf("Enter Product ID: ");
    scanf("%d",&((*inventory)[*count-1].id));

    printf("Enter Product Name: ");
    scanf("%[^\n]",(*inventory)[*count-1].name);

    printf("Enter Product Price: ");
    scanf("%f",&((*inventory)[*count-1].price));

    printf("Enter Product Quantity: ");
    scanf("%d",&((*inventory)[*count-1].quantity));

    printf("Product Added Successfully\n");
}

void viewProducts(const Product *inventory , int count){
    if(count==0){
        printf("No Products in Inventory.\n\n\n");
        return;
    }
    for(int i=0;i<count;i++){
        printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n\n",
            inventory[i].id,
            inventory[i].name,
            inventory[i].price,
            inventory[i].quantity);
    }
    printf("\n\n\n");
}

ResultCode updateQuantity(Product *inventory , int count){
    int id = 0, newQty = 0;
    bool found=0;
    printf("Enter Product ID to update quantity: ");
    scanf("%d",&id);

    for(int i=0;i<count;i++){
        if(inventory[i].id==id){
            printf("Enter new quantity: ");
            scanf("%d",&newQty);
            inventory[i].quantity=newQty;
            found=1;
            printf("Quantity Updated Successfully\n\n");
            return RESULT_SUCCESS;
        }
    }
    if(!found){
        printf("Product Not Found\n\n");
        return RESULT_NOT_FOUND;
    }
    return RESULT_NOT_FOUND;
}

void searchByID(const Product *inventory , int count){
    int id = 0;
    bool found=0;
    printf("Enter Prodcut ID to search : ");
    scanf("%d",&id);

    for (int i=0;i<count;i++){
        if(inventory[i].id==id){
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n\n",
                inventory[i].id,
                inventory[i].name,
                inventory[i].price,
                inventory[i].quantity);
            found=1;
            break;
    }
}
    if(!found){
        printf("Product Not Found\n\n");
    }
}

void searchByName(const Product *inventory , int count){
    char keyword[100];
    bool found=0;
    printf("Enter Product Name to search : ");
    scanf("%[^\n]",keyword);

    for(int i=0;i<count;i++){
        if(strstr(inventory[i].name,keyword)){
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n\n",
                inventory[i].id,
                inventory[i].name,
                inventory[i].price,
                inventory[i].quantity);
            found=1;
        }
    }
    if(!found){
        printf("Product Not Found\n\n");
    }
    else{
        printf("\n\n\n");
    }
}

void searchByPriceRange(const Product *inventory , int count){
    float min = 0.0f, max = 0.0f;
    bool found=0;
    printf("Enter Minimum Price: ");
    scanf("%f",&min);
    printf("Enter Maximum Price:");
    scanf("%f",&max);

        for(int i=0;i<count;i++){
        if(inventory[i].price>=min && inventory[i].price<=max){
            printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n\n",
                inventory[i].id,
                inventory[i].name,
                inventory[i].price,
                inventory[i].quantity);
            found=1;
        }
    }
    if(!found){
        printf("No products found in that price range.\n\n");
    }
    else{
        printf("\n\n\n");
    }

}

ResultCode deleteById(Product **inventory , int *count){
    int id;
    int found=-1;
    printf("Enter Product ID to delete: ");
    scanf("%d",&id);

    for(int i=0;i<*count;i++){
        if((*inventory)[i].id==id){
            found=i;
            break;
        }
    }
    if(found==-1){
        printf("Product Not Found\n\n");
        return RESULT_NOT_FOUND;
    }
    
    free((*inventory)[found].name);
    for(int i=found;i<*count-1;i++){
        (*inventory)[i]=(*inventory)[i+1];
    }

    *count -=1;
    *inventory = realloc(*inventory,(*count)*sizeof(Product));
    printf("Product Deleted Successfully\n\n");
    return RESULT_SUCCESS;
}

void freeInventory(Product *inventory , int *count){
    for(int i=0;i<*count;i++){
        free(inventory[i].name);
    }
    free(inventory);
}

int main(){
    int productCount = 0, choice = 0;
    Product *inventory = NULL;

    printf("Enter inital number of priducts : ");
    scanf("%d",&productCount);

    if(productCount<=0){
        printf("Invalid prodcut count\n\n");
        return 1;
    }

    inventory = calloc(productCount,sizeof(Product));

    for(int i=0;i<productCount;i++){
        inventory[i].name = malloc(NAME_BUFFER_SIZE * sizeof(char));

        printf("Product %d ID: ", i+1);
        scanf("%d",&inventory[i].id);

        printf("Product %d Name: ", i+1);
        scanf("%s",inventory[i].name);

        printf("Product %d Price: ", i+1);
        scanf("%f",&inventory[i].price);

        printf("Product %d Quantity: ", i+1);
        scanf("%d",&inventory[i].quantity);

    }

    while(1){
        printf("\nInventory MENU\n");
        printf("1. Add Product\n");
        printf("2. View All Products\n");
        printf("3. Update Quantity\n");
        printf("4. Search by ID\n");
        printf("5. Search by Name\n");
        printf("6. Search by Price\n");
        printf("7. Delete by ID\n");
        printf("8. Exit\n");
        printf("Enter your choice: ");
        scanf("%d",&choice);

        switch(choice){
            case 1:
                addProduct(&inventory,&productCount);
                break;
            case 2:
                viewProducts(inventory,productCount);
                break;
            case 3:
                updateQuantity(inventory,productCount);
                break;
            case 4:
                searchByID(inventory,productCount);
                break;
            case 5:
                searchByName(inventory,productCount);
                break;
            case 6:
                searchByPriceRange(inventory,productCount);
                break;
            case 7:
                deleteById(&inventory,&productCount);
                break;
            case 8:
                freeInventory(inventory,&productCount);
                printf("Exiting program..\n");
                
        return 0;

            default:
                printf("Invalid Choice\n\n");
        }
    }

    
}
    
    