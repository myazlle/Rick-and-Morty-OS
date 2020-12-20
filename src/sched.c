/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================

  Definicion de funciones del scheduler
*/

#include "sched.h"

player_t ultimoJugador;  // 0001 0011
uint16_t jugadorActual = 1;
uint16_t index;
uint16_t tareaActual;
tss_t* tssActual;
uint16_t tareaActualAnterior;
uint8_t clocks[PLAYERS][MAX_CANT_MEESEEKS];
const char* CLOCK[4] = {"| ", "/ ", "- ", "\\ "};

extern void pantalla_negra_debug();
extern void init_pantalla();
extern void registrosActuales();

sched_t sched[PLAYERS][11];

bool modoDebug;

uint32_t backup_map[80*41];


void sched_init(void)
{
  index = 16;
  tssActual = TSSs[16];
  tareaActual = index;
  tareaActualAnterior = tareaActual;
  modoDebug = false;

  for (player_t player = MORTY; player < PLAYERS ; player++){
    for (int i = 0; i < 11; i++){
      sched[player][i].p_loop_sched = 0;
      sched[player][i].info_task->flag_loop = 0;
      sched[player][i].info_task->active = false;
    }
    for (int i = 0; i < MAX_CANT_MEESEEKS; i++){
      clocks[player][i] = 0;
    }
  }
}



void reset_p_loop_task(player_t player){
  for (uint8_t i = 0; i < 11; i++){
    if(sched[player][i].info_task->active){
      sched[player][i].info_task->flag_loop = false;
    }
  }
}



info_task_t* next(player_t player){

  for (uint8_t i = 0; i < 11; i++){
    bool active = sched[player][i].info_task->active;
    bool p_loop_task = sched[player][i].info_task->flag_loop;

    if( active && !p_loop_task ){
        sched[player][i].info_task->flag_loop = true;
        sched[player][i].p_loop_sched = true;

        return sched[player][i].info_task;
    }
  }

  reset_p_loop_task(player);

  for (uint8_t i = 0; i < 11; i++){
    bool active = sched[player][i].info_task->active;
    bool p_loop_task = sched[player][i].info_task->flag_loop;
    if( active && !p_loop_task ){
        sched[player][i].info_task->flag_loop = true;
        sched[player][i].p_loop_sched = true;
        return sched[player][i].info_task;
    }
  }

  print( "LLEGO HASTA ACA BRO, ANDA TODO MAL :(", 1,2,WHITE_RED); // dejarlo porque sirve de advertencia
  breakpoint();
  //! ACA NO TIENE QUE LLEGAR, POR ENDE, SI LLEGA ACA HAY QUE LLAMAR A ENDGAME
  info_task_t* task_que_nunca_va_llegar = sched[player][0].info_task; // pero al compilar me jode que ponga un return aca
  return task_que_nunca_va_llegar;
}

bool end_loop_sched(){
  bool ret = true;
  for (player_t player = 0; player < PLAYERS; player++){
    for (uint8_t i = 0; i < 11; i++){
      if(sched[player][i].info_task->active){        
        ret = ret && sched[player][i].p_loop_sched; 
      }
    }
  }
  return ret;
}

void reset_sched_p(){
  for (player_t player = 0; player < PLAYERS; player++){
    for (size_t i = 0; i < 11; i++){
      sched[player][i].p_loop_sched = false;
    }
  }
}

void update_msk_clocks(info_task_t* task){
  task->clock = (task->clock + 1) %4;
  uint8_t i = task->clock;
  coordenadas coord; 
  coord.x = 26 + 3 * task->idx_msk;
  coord.y = 49 - 4 * task->player;
  print("C", coord.x, coord.y, BLACK_BLACK);
  print(CLOCK[i], coord.x, coord.y, WHITE_BLACK);
}


// //! SCHED NAJU
uint16_t sched_next_task(void){

  if (modoDebug){
    tssActual = TSSs[IDLE];
    return (IDLE << 3);
  }

  player_t new_player;
  tareaActual = tareaActualAnterior;

  if(tareaActualAnterior == IDLE){ 
    tareaActual++;
    new_player = RICK; // ver si 
  }else{
    new_player = info_task[tareaActual].player ? MORTY : RICK;
  }

  info_task_t* task = next(new_player);
  index = task->idx_gdt;

  bool msk_task = index > 18 ;
  if (msk_task){
    update_msk_clocks(task);                   
  }

  bool end_loop= end_loop_sched(); 
  if(end_loop){
    reset_sched_p();
  }

  tareaActual = index;
  tareaActualAnterior = index;

  tssActual = TSSs[task->idx_gdt];
  return (task->idx_gdt << 3);

}
    


void desactivar_tarea(){
  if(tareaActual == 17 || tareaActual == 18){
    end_game();
  }
  //tareasActivas[tareaActual] = false;
  info_task[tareaActual].active = false;

  info_task[tareaActual].clock = 0;
  coordenadas coord; 
  coord.x = 26 + 3 * info_task[tareaActual].idx_msk;
  coord.y = 49 - 4 * info_task[tareaActual].player;
  print("C ", coord.x, coord.y, BLACK_BLACK);
  print("X ", coord.x, coord.y, WHITE_BLACK);


}

uint16_t sched_idle(){
  tareaActualAnterior = tareaActual;
  tareaActual = 16;;
  return 0x80;
}


uint32_t proximoStack(int i){
  // Agarrar el esp del tssActual y devolver la proxima instrucción
  char* stack = (char*) tssActual->esp + (i-1);
  return (uint32_t) stack;
}

int codigoError(void){
  // Buscar la x posicion en la pila para devolver el err tssActual-> esp + 
  return 0;
}

//uint32_t registros[15];

//register uint8_t valor_scancode asm("al");

void imprimirRegistros(uint32_t eax, uint32_t ebx, uint32_t ecx, uint32_t edx, uint32_t esi, uint32_t edi, uint32_t ebp, uint32_t esp, /*uint32_t eip,*/ uint32_t cs, uint32_t ds, uint32_t es, uint32_t fs, uint32_t gs,uint32_t ss){
  char registrosNombre[15][10] = {"eax", "ebx","ecx","edx","esi","edi","ebp","esp","eip","cs","ds","es","fs","gs","ss"};
  uint32_t registros[15] = {eax,ebx,ecx,edx,esi,edi,ebp,esp,0,cs,ds,es,fs,gs,ss};
  for (size_t i = 0; i < 30; i+=2){
    print((char*)(registrosNombre + (i/2)), 21, i+4, C_FG_WHITE);
    print_hex(registros[i/2],8, 25, i+4, C_FG_LIGHT_GREEN);
  }
}

void modo_debug(void){
  breakpoint();
  if(modoDebug){

    modoDebug=false;
    uint32_t* video = (uint32_t*) VIDEO;
	  for(uint32_t i = 0; i< 80*41; i++) {
   		video[i] = backup_map[i];
	  }

    //cambiar a la tarea que le toca  //! ME PARECE QUE POR COMO FUNCA EL SCHED, NO HACE FALTA ESTO

    //init_pantalla();  // recuperar pantalla

  } else{

    modoDebug=true;
    // hacer backup_pantalla

    uint32_t* video = (uint32_t*) VIDEO;
	  for(uint32_t i = 0; i< 80*41; i++) {
   		backup_map[i] = video[i];
	  }

    pantalla_negra_debug();
    // registros[0]  = tssActual->eax;
    // registros[1]  = tssActual->ebx;
    // registros[2]  = tssActual->ecx;
    // registros[3]  = tssActual->edx;
    // registros[4]  = tssActual->esi;
    // registros[5]  = tssActual->edi;
    // registros[6]  = tssActual->ebp;
    // registros[7]  = tssActual->esp;
    // registros[8]  = tssActual->eip;
    // registros[9]  = tssActual->cs;
    // registros[10] = tssActual->ds;
    // registros[11] = tssActual->es;
    // registros[12] = tssActual->fs;
    // registros[13] = tssActual->gs;
    // registros[14] = tssActual->ss;

    if(ultExcepcion!=260){
      imprimir_excepcion(ultExcepcion);
    }
    registrosActuales();
    // for(int i=0; i<30; i = i + 2){
    //   print((char*)(registrosNombre + (i/2)), 21, i+4, C_FG_WHITE);
    //   print_hex(registros[i/2],8, 25, i+4, C_FG_LIGHT_GREEN);
    // }
    print("eflags",21,37,C_FG_WHITE);
    print_hex(tssActual->eflags,8, 28, 37, C_FG_LIGHT_GREEN);

    print("stack", 36, 17, C_FG_WHITE);
    for(int i=0; i<6; i = i + 2){
      print_hex(/*proximoStack(i/2)*/ tssActual->esp, 8, 36, i+19, C_FG_LIGHT_GREEN);
    }
    
    print("backtrace", 36, 25, C_FG_WHITE);
    print("00000000" , 36, 27, C_FG_LIGHT_GREEN);
    print("00000000" , 36, 29, C_FG_LIGHT_GREEN);
    print("00000000" , 36, 31, C_FG_LIGHT_GREEN);
    print("00000000" , 36, 33, C_FG_LIGHT_GREEN);

    print("cr0", 45, 5, C_FG_WHITE);
    print_hex(rcr0(), 8, 49, 5, C_FG_LIGHT_GREEN);

    print("cr2", 45, 7, C_FG_WHITE);
    print_hex(rcr2(), 8, 49, 7, C_FG_LIGHT_GREEN);

    print("cr3", 45, 9, C_FG_WHITE);
    print_hex(rcr3(),8 , 49, 9, C_FG_LIGHT_GREEN);

    print("cr4", 45, 11, C_FG_WHITE);
    print_hex(rcr4(),8 , 49, 11, C_FG_LIGHT_GREEN);
    
    print_hex(tareaQueRompio, 2, 56,2, C_FG_LIGHT_GREEN);


    print("err", 45, 13, C_FG_WHITE);
    print_hex(codigoError(),8 , 49, 13, C_FG_LIGHT_GREEN);
  }
  breakpoint();
}

