#include <list.h>
#include <stdint.h>
#include <types.h>

typedef enum
{
        NO_ERROR,
        ERR_NOT_FOUND,
        ERR_NO_MEMORY,
        ERR_INVALID_ARGS,
        ERR_OUT_OF_RANGE
} tk_stat_t;

typedef enum
{
        GROUPS_LEVL_NUM = 4,
        ITEM_SIZE_SHIFT = 12,
        NODE_NUM_SHIFT = 4,
        NODE_DIM_NUM_SHIFT = 4,

} tk_macro_t;

typedef uintptr_t tk_base_t;
typedef uintptr_t tk_word_t;

typedef enum
{

} item_macro_t;

typedef enum
{

} group_macro_t;

typedef enum
{
        // migrate
        MIGRATION_MOVABLE = 0,
        MIGRATION_SPARSE_MOVABLE = 0, // Response action, event-driven
        MIGRATION_UNMOVABLE = 1,      // Dead, event-driven
        MIGRATION_RECLAIMABLE = 2,    // Blocking wait, event-driven
        MIGRATION_LRUTYPE = 3,        // Frequent use, event-driven
        MIGRATION_NUM = 4,
        //

} node_macro_t;

/* n=4, s=12, w=9 ->
 *  l=0, bits=39
 *  l=1, bits=30
 *  l=2, bits=21
 *  l=3, bits=12
 */

#define LEVL_BITS(n, s, l, w) ((n - l) * w + s - w)
#define LEVL_BITS_SHIFT(n, s, l, w) (LEVL_BITS(n, s, l, w) - s)

// For different types of resources and algorithms,
// the structure is different.
struct item_struct
{
        // private:
        union
        {
                struct list_node node;
                struct
                {
                        struct item_struct *compound_node;
                        tk_word_t compound_head : 1;
                };
        };
        struct item_struct *next;
        struct item_struct *prev;

        // protected:
};
struct group_struct_cell
{
};

//
struct item_struct_vtable
{
};

struct group_struct_vtable
{
        // struct group_struct_cell cells[MIGRATION_NUM];
        tk_stat_t (*init)(struct group_struct *const self);
        tk_stat_t (*dump)(struct group_struct *const self);
        tk_stat_t (*backward_cp)(struct group_struct *const self);
        tk_stat_t (*forward_dp)(struct group_struct *const self);
        tk_stat_t (*migrate)(struct group_struct *const self);
        tk_stat_t (*refill_item)(struct group_struct *const self);
};

struct node_struct_vtable
{
        tk_stat_t (*init)(struct node_struct *const self);
        tk_stat_t (*dump)(struct node_struct *const self);
        tk_stat_t (*migrate)(struct node_struct *const self);
        tk_stat_t (*compound)(struct node_struct *const self);
        tk_stat_t (*stream)(struct group_struct *const self);
        tk_stat_t (*alloc)(struct node_struct *const self);
        tk_stat_t (*free)(struct node_struct *const self);        
};

struct group_struct
{
        // private:
        struct group_struct_vtable const *vptr;
        // protected:
        struct node_struct *const nptr;
};

// for dim, we need to dynamic request.

struct node_struct
{
        // protected:
        void *const iroot; // i->item_struct
        struct node_struct_vtable const *vptr;

        // private:
        int gn;
        int gw;
        int gs;
        struct group_struct *groups; // gn
        struct object_struct iarray; // i->item_struct
};

struct object_struct
{
};

// derive, such as mem_node_struct, time_node_struct
struct node_struct_attr
{
        uint8_t node_id;
        uint8_t node_dim;
        tk_base_t off;
        size_t size;
        uint16_t stat;
        size_t frees;
        uint64_t stream_id;
};

struct entity_struct
{
        // private:

        // protected:
};

struct assets_struct_vtable
{
        tk_stat_t (*alloc)(struct assets_struct *const self);
        tk_stat_t (*free)(struct assets_struct *const self);
        tk_stat_t (*dispatch)(struct assets_struct *const self);
};

struct assets_struct
{
        // public
        struct assets_struct_vtable const *vptr;
        // private:

        // protected:
};