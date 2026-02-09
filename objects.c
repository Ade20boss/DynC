#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct Object object_t;
void object_free(object_t *obj);

typedef enum {
    INTEGER,
    FLOAT,
    STRING,
    COLLECTION
} object_kind_t;

typedef struct{
    size_t  length;
    object_t **data;
    size_t capacity;
} collection;


typedef union {
    int v_int;
    float v_float;
    char * v_string;
    collection v_collection;
} object_data_t;


typedef struct Object{
    object_kind_t kind;
    object_data_t data;
} object_t;


object_t *new_object_integer(int value){
   object_t *new_obj = (object_t *)malloc(sizeof(object_t));
   if (new_obj == NULL){
        return NULL;
   }
   new_obj -> kind = INTEGER;
   new_obj -> data.v_int = value;

   return new_obj;
}

object_t *new_object_float(float value){
   object_t *new_obj = (object_t *)malloc(sizeof(object_t));
   if (new_obj == NULL){
        return NULL;
   }
   new_obj -> kind = FLOAT;
   new_obj -> data.v_float = value;

   return new_obj;
}


object_t *new_object_string(char *value){
   object_t *new_obj = (object_t *)malloc(sizeof(object_t));
   if (new_obj == NULL){
        return NULL;
   }
   new_obj -> kind = STRING;
   new_obj -> data.v_string = malloc(strlen(value) + 1);
   if(new_obj -> data.v_string == NULL){
        free(new_obj);
        return NULL;
   }
   strcpy(new_obj -> data.v_string, value);

   return new_obj;
}

object_t *new_object_collection(size_t capacity) {
    if (capacity == 0){
        fprintf(stderr, "Cannot initialize collection kind with 0 capacity\n");
        return NULL;
    }

    object_t *new_obj = (object_t *)malloc(sizeof(object_t));

    if(new_obj == NULL){
        return NULL;
    }

    new_obj -> kind = COLLECTION;
    new_obj -> data.v_collection.length = 0;
    new_obj -> data.v_collection.capacity = capacity;
    new_obj -> data.v_collection.data = calloc(capacity, sizeof(object_t *));

    if (new_obj -> data.v_collection.data == NULL){
        free(new_obj);
        return NULL;
    }
    return new_obj;

}

int object_length(object_t *obj){
    if (obj == NULL){
        fprintf(stderr, "Cannot perform operation on null parameters\n");
        return -1;
    }
    switch (obj -> kind){
        case INTEGER:
            fprintf(stderr, "Cannot perform operation on Object of kind INTEGER\n");
            return -1;
        case FLOAT:
            fprintf(stderr, "Cannot perform operation on Object of kind FLOAT\n");
            return -1;
        case STRING:
            return strlen(obj -> data.v_string);
        case COLLECTION:
            return  obj -> data.v_collection.length;
        default:
            fprintf(stderr, "Error: Unknown object kind detected\n");
            return -1;
    }

}



int collection_append(object_t *collection, object_t *item){
    if(collection == NULL || item == NULL){
        fprintf(stderr, "Cannot perform operation on null object\n");
        return -1;
    }
    if (collection -> kind != COLLECTION){
        fprintf(stderr, "Error: Can't perform append operation on non_collection kind\n");
        return -1;
    }
    if (collection -> data.v_collection.capacity == collection -> data.v_collection.length){
        size_t new_cap = collection -> data.v_collection.capacity * 2;
        object_t **temp = realloc(collection -> data.v_collection.data, sizeof(object_t *) * new_cap);

        if (temp == NULL){
            return -1;
        }

        collection -> data.v_collection.data = temp;
        collection -> data.v_collection.capacity = new_cap;

    }
    collection -> data.v_collection.data[collection -> data.v_collection.length] = item;

    collection -> data.v_collection.length++;
    

    return 0;
}

int collection_set(object_t *collection, size_t index, object_t *value){
    if (collection == NULL || value == NULL){
        fprintf(stderr, "Unable to perform operation with null values\n");
        return -1;
    }

    if (collection -> kind != COLLECTION){
        fprintf(stderr, "Cannot perform operation on non_collection kind\n");
        return -1;
    }

    if (index >= collection -> data.v_collection.length){
        fprintf(stderr, "Index specified is out of bounds\n");
        return -1;
    }

    object_free(collection -> data.v_collection.data[index]);

    collection -> data.v_collection.data[index] = value;
    return 0;
}

object_t *collection_access(object_t *collection, size_t index){
    if (collection == NULL){
        fprintf(stderr, "Unable to perform operation with null values\n");
        return NULL;
    }

    if (collection -> kind != COLLECTION){
        fprintf(stderr, "Cannot perform operation on non_collection kind\n");
        return NULL;
    }

    if (index >= collection -> data.v_collection.length){
        fprintf(stderr, "Index specified is out of bounds\n");
        return NULL;
    }

    return collection -> data.v_collection.data[index];

}

void object_free(object_t *obj){
    if (obj == NULL){
        return;
    }
    
    else if (obj -> kind == STRING){
        free(obj -> data.v_string);
    }
    else if(obj -> kind == COLLECTION){
        for (size_t i = 0; i < obj -> data.v_collection.length; i++){
            object_free(obj -> data.v_collection.data[i]);
        }
        free(obj -> data.v_collection.data);
    }

    free(obj);

}


object_t *object_add(object_t *a, object_t *b){
        /**
     * @brief Performs a polymorphic addition or collection merge.
     *
     * @warning **DESTRUCTIVE OPERATION FOR COLLECTIONS (MOVE SEMANTICS)**
     * When adding two objects of kind COLLECTION, this function takes OWNERSHIP
     * of the internal data of both 'a' and 'b'.
     *
     * @details
     * - **Primitives (INT/FLOAT):** Returns a new object. 'a' and 'b' remain valid.
     * - **Strings:** Returns a new string. 'a' and 'b' remain valid.
     * - **Collections:** * 1. A new collection is created containing pointers to ALL items from 'a' and 'b'.
     * 2. The items are **moved**, not copied. 
     * 3. The input shells 'a' and 'b' are **FREED** and invalid after return.
     *
     * @param a The first operand. Consumed if COLLECTION.
     * @param b The second operand. Consumed if COLLECTION.
     * @return object_t* A new object containing the result, or NULL on failure.
     */
    if (a == NULL || b == NULL){
        fprintf(stderr, "Cannot perform operation on Null data\n");
        return NULL;
    }
    
    switch (a -> kind){
        case INTEGER:
            switch (b -> kind){
                case INTEGER:
                    return new_object_integer(a -> data.v_int + b -> data.v_int);
                case FLOAT:
                    return new_object_float((float)a -> data.v_int + b -> data.v_float);
                default:
                    fprintf(stderr, "Cannot perform operation on incompatible types\n");
                    return NULL;
            }
        case FLOAT:
            switch (b -> kind){
                case INTEGER:
                    return new_object_float(a -> data.v_float + (float)b -> data.v_int);
                case FLOAT:
                    return new_object_float(a -> data.v_float + b -> data.v_float);
                default:
                    fprintf(stderr, "Cannot perform operation on incompatible types\n");
                    return NULL;
            }

        case STRING:
            if (b -> kind != STRING){
                fprintf(stderr, "Cannot perform operation on incompatible types\n");
                return NULL;  
            }
            size_t length = strlen(a -> data.v_string) + strlen(b -> data.v_string) + 1;

            char *new_string = calloc(sizeof(char), length);
            if(new_string == NULL){
                return NULL;
            }

            strcat(new_string, a -> data.v_string);
            strcat(new_string, b -> data.v_string);

            object_t *newstring = new_object_string(new_string);
            free(new_string);
            return newstring;
            
        case COLLECTION:
            if (b -> kind != COLLECTION){
               fprintf(stderr, "Cannot perform operation on incompatible types\n");
               return NULL;          
            }

            size_t total_length = a -> data.v_collection.length + b -> data.v_collection.length;

            size_t capacity = (total_length > 0) ? total_length : 1;
            object_t *new_collection = new_object_collection(capacity);
            // [CRITICAL] TRANSFER OWNERSHIP
            // We are moving the pointers from a/b to new_collection.
            // We do NOT deep copy the items.
            for (size_t i = 0; i < a -> data.v_collection.length; i++){
                collection_append(new_collection, a -> data.v_collection.data[i]);
            }
            for (size_t j = 0; j < b -> data.v_collection.length; j++){
                collection_append(new_collection, b -> data.v_collection.data[j]);
            }
            // [DO NOT TOUCH] PREVENT RECURSIVE DESTRUCTION
            // We set lengths to 0 so object_free() destroys the array wrapper
            // but does NOT touch the items (which now belong to new_collection).
            // Removing this line causes Double Free errors.
            a -> data.v_collection.length = 0;
            b -> data.v_collection.length = 0;
            object_free(a);
            object_free(b);
            return new_collection;
        default: return NULL;
    }


}


void print_object(object_t *obj){
    if (obj == NULL){
        printf("NULL");
        return;
    }

    switch(obj -> kind){
        case INTEGER:
            printf("%d", obj -> data.v_int);
            break;
        case FLOAT:
            printf("%f", obj -> data.v_float);
            break;
        case STRING:
            printf("%s", obj -> data.v_string);
            break;
        case COLLECTION:
            printf("[");
            for (size_t i = 0; i < obj -> data.v_collection.length; i++){
                print_object(obj -> data.v_collection.data[i]);
                if (i < obj -> data.v_collection.length - 1){
                      printf(", ");
                }

            }
            printf("]");
            break;
    }

}

void print_collection_data(object_t *obj){
    if (obj -> kind != COLLECTION){
        fprintf(stderr, "Cannot print data of non_collection kind");
        return;
    }

    printf("Collection capacity: %zu\n", obj -> data.v_collection.capacity);
    printf("Number of items in collection: %zu\n", obj -> data.v_collection.length);

}

int is_empty(object_t *obj){
    if (obj -> kind != COLLECTION){
        fprintf(stderr, "Object of non_collection kind cannot be empty");
        return -1;
    }

    if (obj -> data.v_collection.length == 0){
        return 1;
    }

    return 0;

}

int is_full(object_t *obj){
    if (obj -> kind != COLLECTION){
        fprintf(stderr, "Object of non_collection kind cannot be empty");
        return -1;
    }

    if(obj -> data.v_collection.length == obj -> data.v_collection.capacity){
        return 1;
    }
    return 0;

}



int main(void){
    printf("=== TEST SUITE START ===\n\n");

    // --- TEST 1: Primitive Arithmetic ---
    printf("--- Test 1: Primitives & Math ---\n");
    object_t *i1 = new_object_integer(10);
    object_t *i2 = new_object_integer(20);
    object_t *f1 = new_object_float(5.5);
    
    object_t *sum_int = object_add(i1, i2);
    object_t *sum_mixed = object_add(i1, f1); // Int + Float

    printf("10 + 20 = "); print_object(sum_int); printf("\n");
    printf("10 + 5.5 = "); print_object(sum_mixed); printf("\n");

    // Cleanup primitives (We don't need them anymore)
    object_free(i1); object_free(i2); object_free(f1);
    object_free(sum_int); object_free(sum_mixed);


    // --- TEST 2: String Concatenation ---
    printf("\n--- Test 2: Strings ---\n");
    object_t *s1 = new_object_string("Hello, ");
    object_t *s2 = new_object_string("World!");
    
    object_t *s_combined = object_add(s1, s2);
    printf("String Result: "); print_object(s_combined); printf("\n");
    
    object_free(s1); object_free(s2);
    object_free(s_combined);


    // --- TEST 3: Collections & Auto-Resizing ---
    printf("\n--- Test 3: Collection Auto-Resize ---\n");
    // Start with small capacity (2) to force a resize when we add 3 items
    object_t *col = new_object_collection(2); 
    
    printf("Initial Capacity: "); print_collection_data(col);

    collection_append(col, new_object_integer(1));
    collection_append(col, new_object_integer(2));
    collection_append(col, new_object_integer(3)); // Triggers realloc
    
    printf("After Appends:    "); print_collection_data(col);
    printf("Content: "); print_object(col); printf("\n");


    // --- TEST 4: Nested Collections (Polymorphism) ---
    printf("\n--- Test 4: Nested Lists ---\n");
    object_t *nested_parent = new_object_collection(2);
    // We append the list from Test 3 into this new list
    // Note: 'col' is now owned by 'nested_parent'. We don't free 'col' manually anymore.
    collection_append(nested_parent, col); 
    collection_append(nested_parent, new_object_string("Sibling"));

    printf("Nested Structure: "); print_object(nested_parent); printf("\n");


    // --- TEST 5: Destructive Merge (The Big Logic) ---
    printf("\n--- Test 5: Destructive Merge (Move Semantics) ---\n");
    object_t *list_a = new_object_collection(5);
    collection_append(list_a, new_object_integer(100));
    collection_append(list_a, new_object_integer(200));

    object_t *list_b = new_object_collection(5);
    collection_append(list_b, new_object_integer(300));
    collection_append(list_b, new_object_integer(400));

    printf("Merging [100, 200] + [300, 400]...\n");
    
    // list_a and list_b are DESTROYED here. Their items move to 'merged'.
    object_t *merged = object_add(list_a, list_b);
    
    printf("Merged Result: "); print_object(merged); printf("\n");


    // --- TEST 6: Collection Set (Replacement) ---
    printf("\n--- Test 6: Collection Set/Replace ---\n");
    // Replace index 1 (value 200) with a String
    printf("Replacing index 1...\n");
    collection_set(merged, 1, new_object_string("REPLACED"));
    
    printf("Final List: "); print_object(merged); printf("\n");


    // --- TEST 7: The Edge Case (Empty + Empty) ---
    printf("\n--- Test 7: Empty List Edge Case ---\n");
    object_t *empty1 = new_object_collection(5); // Capacity 5, Length 0
    object_t *empty2 = new_object_collection(5); // Capacity 5, Length 0
    
    // This used to crash. Now it should return a valid empty list.
    object_t *empty_result = object_add(empty1, empty2);
    
    if (empty_result != NULL) {
        printf("Success! Result: "); print_object(empty_result); 
        printf(" (Length: %zu)\n", empty_result->data.v_collection.length);
        object_free(empty_result);
    } else {
        printf("FAILED: Result was NULL.\n");
    }

    // --- CLEANUP ---
    // We only need to free the "roots" of our data structures.
    // 'col' is inside 'nested_parent'.
    // 'list_a' and 'list_b' were already freed by object_add.
    
    object_free(nested_parent);
    object_free(merged);

    printf("\n=== ALL TESTS PASSED (If no crashes/leaks) ===\n");
    return 0;
}