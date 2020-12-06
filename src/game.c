/* ** por compatibilidad se omiten tildes **
================================================================================
 TRABAJO PRACTICO 3 - System Programming - ORGANIZACION DE COMPUTADOR II - FCEN
================================================================================
*/

#include "game.h"
#include "prng.h"
#include "types.h"
#include "colors.h"
#include "screen.h"
#include "sched.h"
#include "mmu.h"


#include "stdint.h"
extern void pantalla_negra_debug();

seed semillas[MAX_CANT_SEMILLAS];
uint8_t indexSemilla;
uint16_t cant_semillas;
uint32_t score[PLAYERS];
const bool ADD = true;
const bool DELETE = false;

// void update_map(void){
//     update_seed();
// }

void clean_cell(coordenadas coord){
    print("X", coord.x, coord.y, GREEN_GREEN);
}

void update_map_seed(coordenadas coord){
    // en dicha coord voy a pisar el lugar con el fondo del mapa
    clean_cell(coord);
}

void update_meeseek_map(player_t player, coordenadas coord,bool reason){
    
    if (reason){                            // ADD
        uint8_t PLAYER_MEESEEK_COLOR;
        if (player == RICK){   
            PLAYER_MEESEEK_COLOR = RICK_MEESEEK_COLOR; 
        } else{
            PLAYER_MEESEEK_COLOR = MORTY_MEESEEK_COLOR;
        }
        //breakpoint();
        print("M", coord.x, coord.y, PLAYER_MEESEEK_COLOR);
    } else{                                 // reason == DELETE
        //breakpoint();
        clean_cell(coord);
    }
    
}

void game_init(void) {
    tss_creator(1,0);
    tss_creator(0,0);
    indexSemilla = 0;
    score[MORTY] = 0;
    score[RICK]  = 0;
    print_dec(score[RICK], 8, 10, 43, WHITE_RED);
    print_dec(score[MORTY], 8, 62, 43, WHITE_BLUE);
    // print("00000000",10, 43, WHITE_RED);  // puntajes rojos en 0
    // print("00000000",62, 43, WHITE_BLUE); // puntajes azules en 0
    print("R  00 01 02 03 04 05 06 07 08 09 ",23, 41,BLACK_RED); // letras rojas
    print("M",57, 41, BLACK_BLUE); // M azul
    print("00 01 02 03 04 05 06 07 08 09 ",26, 45,BLACK_BLUE); // letras azules


    // print semillas al azar
    for(int i=1; i < MAX_CANT_SEMILLAS; i++){
        uint8_t x = rand();
        uint8_t y = rand();
        print("s", x % 79, y % 40, GREEN_YELLOW);
        coordenadas coord;
        coord.x = x % 79;
        coord.y = y % 40;
        indexSemilla++;
        semillas[indexSemilla].p = true;
        semillas[indexSemilla].coord = coord;
    }
    

    // setear todos los meeseeks como inactivos
    for (player_t player = 0; player < 2; player++){
        for (player_t i = 0; i < MAX_CANT_MEESEEKS; i++){
            meeseeks[player][i].p = 0;
        }
    }
}



void end_game(void){
    // breakpoint();
    //pantalla_negra_debug();
    print("FIN DEL JUEGO", 35, 5, C_FG_WHITE); // ver si quead centrado
}

void add_update_score(player_t player){
    score[player] += 425;
    print_dec(score[RICK], 8, 10, 43, WHITE_RED);
    print_dec(score[MORTY], 8, 62, 43, WHITE_BLUE);
}

bool same(coordenadas a, coordenadas b){
    // print_dec(a.x, 2 ,2,  i - 1, WHITE_RED);
    // print_dec(a.y, 2 ,6,  i - 1, WHITE_RED);
    // print_dec(b.x, 2 ,12, i - 1, WHITE_RED);
    // print_dec(b.y, 2 ,18, i - 1, WHITE_RED);
    return (a.x == b.x && a.y == b.y);
}

int8_t next_index_meeseek_free(player_t player){
    for (int8_t i = 0; i < MAX_CANT_MEESEEKS; i++){
        if (!meeseeks[player][i].p){
            return i;
        }   
    }
    return -1;  //no entra nunca, pero sino tira warning      // 255 = no existe lugar libre para otro meeseek
}
 
    // busca si la coordenada esta sobre una semilla, ret -1 = no hay semilla en tal cordenada, ret x =  en semillas[x] esta la semilla encontrada
int index_in_seed(coordenadas coord){        //! cambiar mañána que devuelva -1 si es false, y el index en caso de true asi queda mejor
    for (int i = 0; i < MAX_CANT_SEMILLAS; i++)
    {
        if (semillas[i].p){
            if ( same(coord, semillas[i].coord) ){
                return i;
            }
        }
    }
    return -1;
}



void remove_seed(int idx){
    update_map_seed(semillas[idx].coord);
    semillas[idx].p = false; // chau semillas                 //! FALTA ACUTALIZAR EL MAPA(SACAR LA SEMILLA)
}

void msk_found_seed(player_t player, int idx_msk, int idx_seed){

    // delete msk
    meeseeks[player][idx_msk].p = false;

    // delete seed
    remove_seed(idx_seed);

    // update
    update_meeseek_map(player, meeseeks[player][idx_msk].coord, DELETE);
    add_update_score(player);
    cant_meeseeks[player] --;

    // flag_off  recycling msk memory
    backup_meeseks[player][idx_msk].p = false;

    // clean_stack_level_0 para reciclar
    char* ptr_virt_page = (char*) backup_meeseks[player][idx_msk].stack_level_0;

    for (int i = 0; i < PAGE_SIZE; i++){
        ptr_virt_page[i] = 0;
    }

    // unmap msk
    uint32_t cr32 = rcr3();

    paddr_t virt2 =backup_meeseks[player][idx_msk].virt;
    
    for (int i = 0; i < 2; i++){
        mmu_unmap_page(cr32, virt2);
        virt2 += PAGE_SIZE;    
    }           

}

    // ;
    // in EAX = code Código de la tarea Mr Meeseeks a ser ejecutada.;
    // in EBX = x Columna en el mapa donde crear el Mr Meeseeks.;
    // in ECX = y Fila en el mapa donde crear el Mr Meeseeks

    //code 4KB  Tener en cuenta que este código debe estar declarado dentro del espacio de memoria de usuario de la tarea.
    
uint32_t create_meeseek(uint32_t code, uint8_t x, uint8_t y){
    // breakpoint();

    player_t player = player_idx_gdt[tareaActual];

    // filtro jugador                               VEER SI NO ES MEJOR FILTRAR ANTES (juan dice creo que con filtar aca esta bien)
    if (player != RICK && player != MORTY){
        return 0;
    }

    // filtro coordenadas validas
    if (x > 80 || y > 39){
        return 0;
    }

    // filtramos que los meeseeks del jugador no esten en el limite de capacidad
    if (cant_meeseeks[player] == MAX_CANT_MEESEEKS){
        return 0;
    }

    coordenadas coord_actual;
    coord_actual.x = x;
    coord_actual.y = y;

    // si meeseek justo cae en semilla, suma puntos y chau semilla
    int index_aux = index_in_seed(coord_actual);
    bool in_seed = index_aux != -1;

    //print_dec(in_seed, 8, 10, 7, WHITE_RED);

    if (in_seed){
        remove_seed(index_aux);
        add_update_score(player);
        return 0;
    }

    // crear Meeseek

    int8_t index_meeseek = next_index_meeseek_free(player);

    // mapear
    paddr_t virt_res;
    virt_res = tss_meeseeks_creator(player, index_meeseek + 1, code, coord_actual);

    meeseeks[player][index_meeseek].p = 1;
    meeseeks[player][index_meeseek].coord = coord_actual;
    update_meeseek_map(player, coord_actual, ADD); // 1 = ADD

    cant_meeseeks[player]++;

    // breakpoint();

    return virt_res;
    }
