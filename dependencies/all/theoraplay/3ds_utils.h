#if defined _3DS

#ifndef _3DS_UTILS_H
#define _3DS_UTILS_H

#include <stdio.h>
#include <stdbool.h>
#include <3ds.h>

#ifdef OS_HEAP_AREA_BEGIN // defined in libctru 2.0+
#define USE_CTRULIB_2 1
#endif

#define MEMOP_PROT      6
#define MEMOP_MAP       4
#define MEMOP_UNMAP     5

#define DEBUG_HOLD() do{printf("%s@%s:%d.\n",__FUNCTION__, __FILE__, __LINE__);fflush(stdout);wait_for_input();}while(0)

void wait_for_input(void);

extern __attribute__((weak)) int  __ctr_svchax;

static bool has_rosalina;

static void check_rosalina() {
  int64_t version;
  uint32_t major;

  has_rosalina = false;

  if (!svcGetSystemInfo(&version, 0x10000, 0)) {
     major = GET_VERSION_MAJOR(version);

     if (major >= 8)
       has_rosalina = true;
  }
}

void ctr_clear_cache(void);

typedef int32_t (*ctr_callback_type)(void);

static inline void ctr_invalidate_ICache_kernel(void)
{
   __asm__ volatile(
      "cpsid aif\n\t"
      "mov r0, #0\n\t"
      "mcr p15, 0, r0, c7, c5, 0\n\t");
}

static inline void ctr_flush_DCache_kernel(void)
{
   __asm__ volatile(
      "cpsid aif\n\t"
      "mov r0, #0\n\t"
      "mcr p15, 0, r0, c7, c10, 0\n\t");
}

static inline void ctr_invalidate_ICache(void)
{
   svcBackdoor((ctr_callback_type)ctr_invalidate_ICache_kernel);
}

static inline void ctr_flush_DCache(void)
{
   svcBackdoor((ctr_callback_type)ctr_flush_DCache_kernel);
}

static inline void ctr_flush_invalidate_cache(void)
{
   if (has_rosalina) {
      ctr_clear_cache();
   } else {
      ctr_flush_DCache();
      ctr_invalidate_ICache();
   }
}

#endif // _3DS_UTILS_H
#endif // RETRO_PLATFORM == RETRO_3DS
