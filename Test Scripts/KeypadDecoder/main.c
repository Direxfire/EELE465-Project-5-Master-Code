#include <stdio.h>

//Testing the keypad decode to ASCII function

char Decode_Input(int);
int Test_Input[17] = {0x84,0x18,0x14,0x12,0x28,0x24,0x22, 0x48, 0x44, 0x42,0x11,0x21,0x41,0x81,0x88,0x82, 0x90};
int main() {
    char Input_ASCII;
    int i = 0;
    for(i = 0; i < 16; i++) {
        Input_ASCII = Decode_Input(Test_Input[i]);
        printf("The input was: %d, with an output of: %c as a char, with ASCII value of: %d.\n", Test_Input[i], Input_ASCII, (int) Input_ASCII);
    }
    return 0;
}


char Decode_Input(int Input) {

    switch (Input) {
        case 0x84:
            return ('0');
            break;

        case 0x18:
            return ('1');
            break;

        case 0x14:
            return ('2');
            break;

        case 0x12:
            return ('3');
            break;

        case 0x28:
            return ('4');
            break;

        case 0x24:
            return ('5');
            break;

        case 0x22:
            return ('6');
            break;

        case 0x48:
            return ('7');
            break;

        case 0x44:
            return ('8');
            break;

        case 0x42:
            return ('9');
            break;

        case 0x11:
            return ('A');
            break;

        case 0x21:
            return ('B');
            break;

        case 0x41:
            return ('C');
            break;

        case 0x81:
            return ('D');
            break;

        case 0x88:
            return ('*');
            break;

        case 0x82:
            return ('#');
            break;
        default:
            return ('\0');
            break;
    }
}