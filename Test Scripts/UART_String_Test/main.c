#include <stdio.h>
char UART_Message_Global[64];
char *UART_Message_Global_ptr;
int Raw_Temp;
int main() {
    Raw_Temp = 1938;
    snprintf(UART_Message_Global, 100, "Current temperature raw data is: %d. \n", Raw_Temp);
    printf("%s",UART_Message_Global);
    return 0;
}
