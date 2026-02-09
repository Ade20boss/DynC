# 🧬 DynC: A Polymorphic Object System in C

> **"Why use Python when you can reinvent the wheel in C with 10x the lines of code?"**

**DynC** is a lightweight, header-only style implementation of dynamic typing in standard C99. It brings high-level concepts like **heterogeneous collections**, **runtime polymorphism**, and **move semantics** to a statically typed world.

Designed for educational exploration of memory management, unions, and v-table-less polymorphism.

---

## ⚡ Features

* **Runtime Polymorphism:** Store Integers, Floats, Strings, and Collections in a single `object_t*` type.
* **Dynamic Collections:** Auto-resizing arrays (Vectors) that function like Python Lists or JS Arrays.
* **Recursive Structures:** Lists can contain other lists (nested complexity).
* **Smart Memory Management:** Implements **Move Semantics** for collection merging (Zero-Copy Transfer).
* **Type-Safe Arithmetic:** `object_add` handles `Int+Int`, `Float+Int`, `String+String` and `List+List` automatically.

---

## 🛠️ Quick Start

### 1. The Basics

Everything is an `object_t`. You create it, use it, and `object_free` it.

```c
// Create primitives
object_t *num = new_object_integer(42);
object_t *str = new_object_string("Hello World");

// Create a polymorphic list
object_t *list = new_object_collection(2);

// Append ANY type to the list
collection_append(list, num);
collection_append(list, str);

// Print the whole structure recursively
print_object(list); 
// Output: [42, Hello World]

```

### 2. The "Magic" (Polymorphic Addition)

The `object_add` function checks types at runtime and behaves accordingly.

```c
object_t *a = new_object_integer(10);
object_t *b = new_object_float(5.5);
object_t *c = new_object_string("Foo");
object_t *d = new_object_string("Bar");

object_t *sum = object_add(a, b); // Result: Float(15.5)
object_t *cat = object_add(c, d); // Result: String("FooBar")

```

---

## 🧠 The Architecture (Deep Dive)

### The Struct

We use a tagged union to achieve dynamic typing without void pointer chaos.

```c
typedef enum { INTEGER, FLOAT, STRING, COLLECTION } object_kind_t;

typedef union {
    int v_int;
    float v_float;
    char *v_string;
    collection v_collection;
} object_data_t;

typedef struct Object {
    object_kind_t kind;
    object_data_t data;
} object_t;

```

### ⚠️ Ownership & Move Semantics

This is the coolest (and most dangerous) part of the codebase.

When you merge two collections using `object_add`, the operation follows **Move Semantics**.

1. It creates a new Collection shell.
2. It **steals the pointers** from the internal arrays of the two input lists.
3. It **destroys the input shells** to prevent double-frees.

**Why?** Performance. Merging two massive lists is  (pointer copying) rather than  (deep allocation and data copying).

```c
object_t *list1 = ...;
object_t *list2 = ...;

// list1 and list2 are FREED inside this function.
// Their data now lives inside 'merged'.
object_t *merged = object_add(list1, list2); 

```

---

## 🚀 Building & Testing

No Makefiles, no CMake hell. It's pure C.

```bash
# Compile
gcc -o dyn_test objects.c -Wall -Wextra

# Run
./dyn_test

```

### Expected Output

```text
=== TEST SUITE START ===

--- Test 1: Primitives & Math ---
10 + 20 = 30
10 + 5.5 = 15.500000

--- Test 2: Strings ---
String Result: Hello, World!

--- Test 3: Collection Auto-Resize ---
Initial Capacity: Collection capacity: 2
After Appends:    Collection capacity: 4
Content: [1, 2, 3]

--- Test 5: Destructive Merge (Move Semantics) ---
Merging [100, 200] + [300, 400]...
Merged Result: [100, 200, 300, 400]

=== ALL TESTS PASSED ===

```

---

## 🤝 Contributing / Roasting

Found a memory leak? Want to implement `object_remove` or `object_deep_copy`?
Feel free to open a PR or ping me on Discord!

License: MIT (Go wild).
