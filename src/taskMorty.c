#include "syscall.h"
#include "i386.h"
#include "colors.h"
#include "screen.h"

#include "game.h"
#include "mmu.h"
#include "prng.h"
#include "sched.h"
#include "stdint.h"
#include "types.h"


void meeseks1_func(void);


// void task(void) {

//   syscall_meeseeks((uint32_t)&meeseks1_func, 79 , 0); // 20
//   syscall_meeseeks((uint32_t)&meeseks1_func, 1 , 24); // 22
//   syscall_meeseeks((uint32_t)&meeseks1_func, 11, 25); // 24
//   syscall_meeseeks((uint32_t)&meeseks1_func, 21, 26); // 26
//   syscall_meeseeks((uint32_t)&meeseks1_func, 31, 27); // 28

//   syscall_meeseeks((uint32_t)&meeseks1_func, 51, 28); // 30
//   syscall_meeseeks((uint32_t)&meeseks1_func, 61, 29); // 32
//   syscall_meeseeks((uint32_t)&meeseks1_func, 71, 25); // 34
//   syscall_meeseeks((uint32_t)&meeseks1_func, 56, 11); // 36
//   syscall_meeseeks((uint32_t)&meeseks1_func, 23 , 8); // 38
  

//   syscall_meeseeks((uint32_t)&meeseks1_func, 79 , 0); // 20
//   syscall_meeseeks((uint32_t)&meeseks1_func, 1 , 24); // 22
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 11, 25); // 24
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 21, 26); // 26
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 31, 27); // 28

//   // syscall_meeseeks((uint32_t)&meeseks1_func, 51, 28); // 30
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 61, 29); // 32
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 71, 25); // 34
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 56, 11); // 36
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 23 , 8); // 38
  

//   // syscall_meeseeks((uint32_t)&meeseks1_func, 79 , 0); // 20
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 1 , 24); // 22
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 11, 25); // 24
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 21, 26); // 26
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 31, 27); // 28

//   // syscall_meeseeks((uint32_t)&meeseks1_func, 51, 28); // 30
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 61, 29); // 32
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 71, 25); // 34
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 56, 11); // 36
//   // syscall_meeseeks((uint32_t)&meeseks1_func, 23 , 8); // 38
  

//   while (1) {
//     __asm volatile("nop");
//     // syscall_meeseeks((uint32_t)&meeseks1_func,79,0);
//       // syscall_meeseeks((uint32_t)&meeseks1_func, 23 , 8); // 38

//   }
// }




void task(void) {
  for (int row = 0; row < 40; row++) {
    for (int col = 0; col < 80; col++) {
      uint32_t res = 0;
      while (res == 0) {
        res = syscall_meeseeks((uint32_t)&meeseks1_func, col, row);
        // breakpoint();
      }
    }
  }

  while (1) {
    __asm volatile("nop");
  }
}



void meeseks1_func(void){
  while (0) {
    // __asm volatile("nop");
      for (int i = 0; i < 20; i++) {
        syscall_move(1,0);
        // int a = 8;
        // a=a;
      }
      // syscall_move(1, 1);
      // syscall_move(0, 1);
      // syscall_move(0, 1);
      // syscall_move(1, -1);
  }


  int8_t deltax, deltay;

  while (1) {

    
    syscall_look(&deltax, &deltay);

    while (deltax < 0) {
      syscall_move(-1, 0);
      deltax++;
    }
    while (deltay < 0) {
      syscall_move(0, -1);
      deltay++;
    }
    while (deltax > 0) {
      syscall_move(1, 0);
      deltax--;
    }
    while (deltay > 0) {
      syscall_move(0, 1);
      deltay--;
    }

    // if(tareaActual == 0x22){
     // breakpoint();
    // }

    syscall_use_portal_gun();

  }

} 















































// void meeseeks_quieto_func(void);

// void task(void) {
//   for (int row = 0; row < 40; row++) {
//     for (int col = 0; col < 80; col++) {
//       uint32_t res = 0;
//       while (res == 0) {
//         res = syscall_meeseeks((uint32_t)&meeseeks_quieto_func, col, row);
//       }
//     }
//   }

//   while (1) {
//     __asm volatile("nop");
//   }
// }

// void meeseeks_quieto_func(void) {
//   int8_t x, y;
//   syscall_look(&x, &y);
//   syscall_look(&x, &y);
//   syscall_look(&x, &y);
//   syscall_look(&x, &y);
//   syscall_look(&x, &y);
//   syscall_look(&x, &y);
//   syscall_look(&x, &y);
//   syscall_look(&x, &y);
//   syscall_look(&x, &y);
//   syscall_look(&x, &y);
//   syscall_look(&x, &y);
//   __builtin_trap();
// }




























// void task(void) {
//   while (1) {
//     __asm volatile("nop");
//   }
// }
