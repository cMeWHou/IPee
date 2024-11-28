#include <utils.h>

#include <stdlib.h>

/***********************************************************************************************
 * TYPEDEFS
 **********************************************************************************************/

/** Address type. */
typedef unsigned long long address_t;


/***********************************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 **********************************************************************************************/

/**
 * @brief Check if an address is a character.
 * 
 * @param address Address.
 * @return 1 if it is a character, 0 otherwise.
 */
static int is_char(address_t address);

/**
 * @brief Check if an address contains a string.
 * 
 * @param address Address.
 * @return 1 if it is a string, 0 otherwise.
 */
static int is_string(address_t address);


/***********************************************************************************************
 * STATIC VARIABLES
 **********************************************************************************************/

static address_t local_base = NULL;
static address_t global_base = NULL;
static address_t heap_base = NULL;


/***********************************************************************************************
 * FUNCTIONS DEFINITIONS
 **********************************************************************************************/

void init_memory_config() {
    local_base = (address_t)&(int){0};
    global_base = (address_t)&init_memory_config;
    heap_base = (address_t)malloc(1);

    if (heap_base)
        free((void *)heap_base);
}

int get_allocation_type(address_t address) {
    if (address < (address_t)10000) {
        if (address == (address_t)0)
            return ALLOCATION_TYPE_NUMBER | ALLOCATION_TYPE_NULLPTR;
        if (address >= 32 && address <= 127)
            return ALLOCATION_TYPE_NUMBER | ALLOCATION_TYPE_CHARACTER;
        return ALLOCATION_TYPE_NUMBER;
    }
    if (address <= local_base) {
        if (is_string(address))
            return ALLOCATION_TYPE_LOCAL | ALLOCATION_TYPE_CHARACTER;
        else if (is_char(address))
            return ALLOCATION_TYPE_LOCAL | ALLOCATION_TYPE_NUMBER | ALLOCATION_TYPE_CHARACTER;
        return ALLOCATION_TYPE_LOCAL;
    }
    if (address >= heap_base && address < global_base) {
        if (is_string(address))
            return ALLOCATION_TYPE_HEAP | ALLOCATION_TYPE_CHARACTER;
        else if (is_char(address))
            return ALLOCATION_TYPE_HEAP | ALLOCATION_TYPE_NUMBER | ALLOCATION_TYPE_CHARACTER;
        return ALLOCATION_TYPE_HEAP;
    }
    if (address >= global_base) {
        if (is_string(address))
            return ALLOCATION_TYPE_GLOBAL | ALLOCATION_TYPE_CHARACTER;
        else if (is_char(address))
            return ALLOCATION_TYPE_GLOBAL | ALLOCATION_TYPE_NUMBER | ALLOCATION_TYPE_CHARACTER;
        return ALLOCATION_TYPE_GLOBAL;
    }
    else
        return ALLOCATION_TYPE_UNKNOWN;
}

static int is_char(address_t address) {
    return *(char *)address >= 32 && *(char *)address <= 127;
}

static int is_string(address_t address) {
    return is_char(address) && (*(char *)(address + 1) == 0 || is_char(address + 1));
}