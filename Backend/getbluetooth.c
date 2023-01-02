#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int hexToDec (char hex);

int main()
{
    FILE *fpipe;
    FILE *fp;
    char *filename = "bluetooth_output.txt";
    char *command = "gatttool -b 4C:EB:D6:4C:AE:26 --char-read -a 0x000c";
    //char *command = "gatttool -b 03:81:56:96:83:09 --char-read -a 0x000c";// old arduino
    //char *command = "gatttool -b 4C:EB:D6:4E:58:CE --char-read -a 0x000c";
    char c = 0;
    int i = 0;
    char output[36] = "";
    char valueHex[3] = "";
    int valueDec = 0;
    char valueDecStr[3] = "";

    while (1 == 1)
    {
        valueDec = 0;
        if (0 == (fpipe = (FILE *)popen(command, "r")))
        {
            perror("popen() failed.");
            exit(EXIT_FAILURE);
        }

        i = 0;
        while (fread(&c, sizeof c, 1, fpipe))
        {
            output[i] = c;
            i++;
        }
        pclose(fpipe);

        memcpy(valueHex, &output[33], 2);
        valueHex[2] = '\0';

        valueDec = hexToDec(valueHex[1]) + hexToDec(valueHex[0]) * 16;\

        fp = fopen(filename, "w+");
        sprintf(valueDecStr, "%i\n", valueDec);
        fputs(valueDecStr, fp);
        fclose(fp);

        printf("%i\n", valueDec);
    }



    return EXIT_SUCCESS;
}

int hexToDec (char hex) {
    int output = 0;
    switch (hex){
        case '0':
            output = 0;
            break;
        case '1':
            output = 1;
            break;
        case '2':
            output = 2;
            break;
        case '3':
            output = 3;
            break;
        case '4':
            output = 4;
            break;
        case '5':
            output = 5;
            break;
        case '6':
            output = 6;
            break;
        case '7':
            output = 7;
            break;
        case '8':
            output = 8;
            break;
        case '9':
            output = 9;
            break;
        case 'a':
            output = 10;
            break;
        case 'b':
            output = 11;
            break;
        case 'c':
            output = 12;
            break;
        case 'd':
            output = 13;
            break;
        case 'e':
            output = 14;
            break;
        case 'f':
            output = 15;
            break;
    }
    return output;
}
