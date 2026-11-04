#pragma once
// Host stub: tests are single threaded, so the critical section is a no-op.
typedef struct { int dummy; } critical_section_t;
static inline void critical_section_init(critical_section_t *) {}
static inline void critical_section_enter_blocking(critical_section_t *) {}
static inline void critical_section_exit(critical_section_t *) {}
