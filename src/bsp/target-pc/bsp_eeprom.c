#include <stdio.h>
#include "bsp_eeprom.h"


#define SIMULATED_EEPROM_FILE "./simulated_eeprom.txt"


eeprom_result_t bsp_eeprom_init(){
    return EEPROM_OK;

}

eeprom_result_t bsp_eeprom_read(uint16_t address, uint32_t data){


    FILE *fp;
    char buff[255];

    fp = fopen("/tmp/test.txt", "r");
    fscanf(fp, "%s", buff);
    printf("1 : %s\n", buff );

    fgets(buff, 255, (FILE*)fp);
    printf("2: %s\n", buff );

    fgets(buff, 255, (FILE*)fp);
    printf("3: %s\n", buff );
    fclose(fp);

    return EEPROM_OK;

}


eeprom_result_t bsp_eeprom_write(uint16_t address, uint32_t data){

    FILE *fp;

    fp = fopen(SIMULATED_EEPROM_FILE, "w+");
    fprintf(fp, "This is testing for fprintf...\n");
    fputs("This is testing for fputs...\n", fp);
    fclose(fp);

    return EEPROM_OK;



}
