#include <stdio.h>
#include <stdbool.h>
#include "bsp_eeprom.h"


#define SIMULATED_EEPROM_FILE "./simulated_eeprom.txt"
#define MAX_ADDRESSES          128

eeprom_result_t bsp_eeprom_init(){
    return EEPROM_OK;

}

eeprom_result_t bsp_eeprom_read(uint16_t address, uint32_t* data){


    // FILE *fp;
    // char buff[255];

    // fp = fopen("/tmp/test.txt", "r");
    // fscanf(fp, "%s", buff);
    // printf("1 : %s\n", buff );

    // fgets(buff, 255, (FILE*)fp);
    // printf("2: %s\n", buff );

    // fgets(buff, 255, (FILE*)fp);
    // printf("3: %s\n", buff );
    // fclose(fp);

    return EEPROM_OK;

}


eeprom_result_t bsp_eeprom_write(uint16_t address, uint32_t data){

    if (address > MAX_ADDRESSES){
        return EEPROM_ERROR;
    }

    printf("EEPROM Write %d in address %d\r\n", data, address);

    FILE *fp, *fp_aux;
    fp = fopen(SIMULATED_EEPROM_FILE, "r+");
    fp_aux = fopen("./aux.txt", "w");

    bool address_in_file = false;
    char line[MAX_ADDRESSES]; 

    // find address to write
    while (fgets(line, MAX_ADDRESSES, fp) != NULL){
        int read_address;
        sscanf(line, "%d: ", &read_address);
        if (read_address == address){
            uint8_t buf[32];
            sprintf((char * restrict)buf, "%d : %d\r\n", address, data);
            address_in_file = true;
            fputs((char * restrict)buf, fp_aux);
        } else {
            fputs(line, fp_aux);
        }
    }


    if (!address_in_file){
        uint8_t buf[32];
        sprintf((char * restrict)buf, "\r\n%d : %d", address, data);
        fputs((char * restrict)buf, fp_aux);
    }

    fclose(fp);
    fclose(fp_aux);


    remove(SIMULATED_EEPROM_FILE);
    rename("./aux.txt", SIMULATED_EEPROM_FILE);


    return EEPROM_OK;



}
