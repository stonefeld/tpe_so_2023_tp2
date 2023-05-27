#include <pong.h>
#include <stdlib.h>
#include <syscalls.h>

#define TICK_RATE 18

uint32_t start_game(){
    main_menu();
    main_loop();

}

void main_menu(){
    clear();
    puts("\t \t   PONG - MAIN MENU \n \n \n \n \n");
    uint8_t c;
    puts("\t \t PRESS ENTER TO START \n");
    while( (c = getchar()) != ' ' );

    return;
}

void main_loop(){
    clear();
    int64_t ref_tick = get_ticks();

    uint8_t input;
    int8_t exit_flag = 0;
    asm_draw_rectangle(0, 10,90, 10,100);  // testing todo magic numbers

    while(!exit_flag){
        if(ref_tick % TICK_RATE == 0){
            input = getchar();
            if(input != 0) process_key(input);
        }
    } 

    
}

void process_key(uint8_t key){
    switch (key)
    {
    case 'w':
        puts("ARRIBA  1\t");
        break;
    case 's':
        puts("ABAJO  1\t");

        break;
    case 'k':  // UP KEY
        puts("ARRIBA  2\t");

        break;
    case 'i': // DOWN KEY
        puts("ABAJO  2\t");

        break;
    }
}
