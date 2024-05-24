#include "tk.h"

static inline tk_base_t levl_entry(int l, int n, int sh, int w, tk_base_t *ni) {
  tk_base_t shift;
  tk_base_t size;
  tk_base_t mask;
  tk_base_t lni;

  shift = LEVL_BITS_SHIFT(n, sh, l, w);
  size = BIT(shift);
  mask = size - 1;
  lni = *ni >> shift;
  *ni &= mask;

  return lni;
}
static inline tk_word_t nitem_to_order(tk_word_t *nr_items) {
  tk_word_t order;
  tk_word_t max_order;
  tk_word_t max_bits = sizeof(tk_word_t) * 8;
  tk_word_t nitems = *nr_items;

  if (!nitems)
    return 0;

  max_order = fls(nitems);

  order = max_order - 1;
  *nr_items = 0;
  if (max_order == max_bits)
    return order;
  if (nitems > BIT(order))
    return max_order;
  return order;
}

static inline int nitem_to_orders(tk_word_t *nr_items) {
  int x = (int)fls(*nr_items);

  if (x)
    CB(*nr_items, x - 1);
  return x ? x - 1 : 0;
}

/* input: s,e,n,sh,w,ni
 * output: l,lni,ni
 */
#define foreach_levl_entry_(s, e, n, sh, w, ni, l, lni)                        \
  for (l = s, lni = levl_entry(l, n, sh, w, &ni); l < e;                       \
       ++l, lni = levl_entry(l, n, sh, w, &ni))

#define foreach_levl_entry(se, ni, l, lni)                                     \
  foreach_levl_entry_(0, (se->gn), (se->gn), (se->gs), (se->gw), ni, l, lni)

static inline bool levl_forward_entry(int s, int l, int *lo) {
  if (s != l)
    *lo = 0;

  return s == l ? false : true;
}

static inline bool levl_backward_entry_ni(int s, int l, int w, int *ni) {
  if (s != l)
    *ni <<= w;

  return s == l ? false : true;
}

static inline bool levl_backward_entry_lo(int l, int e, int *lo, int olo) {
  *lo = (e != l) ? 0 : olo;
  return e == l ? false : true;
}

#define foreach_levl_forward(s, e, l, lo, b)                                   \
  for (l = s, b = levl_forward_entry(s, l, &lo); l >= e;                       \
       --l, b = levl_forward_entry(s, l, &lo))

#define foreach_levl_backward_ni(s, e, w, ni, b)                               \
  for (int l = s, b = levl_backward_entry_ni(s, l, w, &ni); l < e;             \
       ++l, b = levl_backward_entry_ni(s, l, w, &ni))

#define foreach_levl_backward_lo(e, l, lo, olo, b)                             \
  for (++l, b = levl_backward_entry_lo(l, e, &lo, olo); l <= e;                \
       ++l, b = levl_backward_entry_lo(l, e, &lo, olo))

#define foreach_levl_available_node(nd, se, ni)

struct node_para {
  uint8_t combined : 1;
  uint8_t stream : 1;
  uint8_t thread : 1;
};

tk_stat_t node_struct_alloc(struct node_struct *const self, tk_base_t nitems,
                            void *const p, struct item_struct **iptr) {
  struct group_struct *gptr;
  tk_stat_t ret;
  tk_base_t levl_nitems;
  int levl_order;
  int levl;
  struct node_para *const para = (struct node_para *)p;
  tk_base_t tmp_nitems = nitems;
  bool stream, enabled, combined, thread;
  stream = para->stream;
  thread = para->thread;

do_traverse_group:
  foreach_levl_entry(self, tmp_nitems, levl, levl_nitems) {
    if (!levl_nitems)
      continue;

    gptr = &self->groups[levl];

    if (stream) {
      ret = gptr->vptr->refill_item(gptr);
      if (!ret) {
      } else {
      }
      if (thread)
        goto do_group_migrate;
      goto do_node_stream;
    } else {
      combined = para->combined;
      levl_order = combined ? nitem_to_orders(&levl_nitems)
                            : nitem_to_order(&levl_nitems);
      break;
    }
  }

do_group_forward_dp:
  // Based on available free capacity
  // 1) try to forward decompose
  int tmp_order = levl_order;
  int tmp_levl;

  foreach_levl_forward(levl, 0, tmp_levl, tmp_order, enabled) {
    gptr = &self->groups[tmp_levl];
    ret = gptr->vptr->forward_dp(gptr, p, tmp_order, iptr, enabled);
    if (!ret && tmp_levl != levl) {
      // forward the item
      int ttmp_order;
      foreach_levl_backward_lo(tmp_levl, levl, ttmp_order, tmp_order, enabled) {
        // fill item
        gptr = &self->groups[tmp_levl];
        ret = gptr->vptr->refill_item(gptr);
        assert(ret == 0);

        ret = gptr->vptr->forward_dp(gptr, p, ttmp_order, iptr, enabled);
        assert(ret == 0);
      }
    }

    if (!ret) // current the item
      goto group_forward_dp_done;
  }

do_group_backward_cp:
  // 2) try to backward compound
  if (combined) {
    int ttmp_nitems = BIT(levl_order);

    // TODO: Precondition satisfied.
    foreach_levl_backward_ni(levl, self->gn, self->gw, ttmp_nitems, enabled) {
      ret = gptr->vptr->backward_cp(gptr, p, &ttmp_nitems, iptr, enabled);
      if (!ret)
        goto group_backward_cp_done;
    }
  }

do_group_migrate:
  // 3) try to migrate from other groups(different migrate type)
  // TODO: Precondition satisfied.
  ret = gptr->vptr->migrate(gptr, p, nitems, iptr);
  if (!ret)
    goto group_migrate_done;

do_node_migrate:
  // 4) try to migrate from other node of groups
  struct node_struct *tmp_node;
  // TODO: Precondition satisfied.
  foreach_levl_available_node(tmp_node, self, nitems) {
    ret = self->vptr->migrate(tmp_node, p, nitems, iptr);
    if (!ret)
      goto node_migrate_done;
  }

do_node_stream:
  // 5) try to stream
  if (stream) {
    ret = self->vptr->stream(self, p, tmp_nitems, iptr);
    if (!ret)
      goto node_stream_done;
  }

  // 6) compound each levl
  if (combined) {
    ret = self->vptr->compound(self, iptr);
    if (ret) {
    } else {
    }
    tmp_nitems = levl_nitems;
    if (tmp_nitems)
      goto do_traverse_group;
  }

  // TODO: Fail Step

  // TODO: Postcondition
group_forward_dp_done:
group_backward_cp_done:
group_migrate_done:
node_migrate_done:
node_stream_done:

  return ret;
}
