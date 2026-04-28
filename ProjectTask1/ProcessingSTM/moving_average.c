#include <stdio.h>
#include <stdlib.h>

int get_average(int array[], int len);
void change_array(int array[], int len, int value);

int main(){



    return 0;
}

int get_average(int array[], int len){
    int sum = 0;
    for (int i = 0; i < len; i++){
        sum += array[i];
    }
    int average = sum/len;
    return average;

}

void change_array(int array[], int len, int value){
    for(int i = 1; i < len; i++){
        array[i-1] = array[i];
    }
    array[len-1] = value;

    
}
