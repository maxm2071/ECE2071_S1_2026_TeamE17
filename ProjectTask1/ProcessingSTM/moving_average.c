#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t get_average(uint8_t array[], int len);
void change_array(uint8_t array[], int len, uint8_t value);

int main(){



    return 0;
}

uint8_t get_average(uint8_t array[], int len){
    uint8_t sum = 0;
    for (int i = 0; i < len; i++){
        sum += array[i];
    }
    uint8_t average = sum/len;
    return average;

}

void change_array(uint8_t array[], int len, uint8_t value){
    for(int i = 1; i < len; i++){
        array[i-1] = array[i];
    }
    array[len-1] = value;


}
