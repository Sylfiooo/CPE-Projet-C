#include <string.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

void deleteSpace(char * data) {
    char* tempData = data;
    do {
        while (*tempData == ' ') {
            ++tempData;
        }
    } while (*data++ = *tempData++);
}

int testNum(char * data, int i) {
    int j = strlen(data) - 1;
    while(1) {
        i++;
        if (i == j) {
            return 0;
        } else if (data[i] >= '0' && data[i] <= '9') {
            continue;
        } else if (data[i] == ',' || data[i] == ']') {
            break;
        } else if (data[i] <= '0' || data[i] >= '9') {
            return 0;
        }
    }

    return i;
}

//check
int testJson(char * data) {

    deleteSpace(data);

    //check {" x.... }
    int i = 0;
    int j = strlen(data) - 1;
    if (data[0] != '{' || data[1] != '\"' || data[j] != '}') {
        printf("\nMauvais format de JSON, manque des crochets ou autres. 1\n");
        return 0;
    }

    i++;

    //check  xxxx"
    while(1) {
        i++;
        if (i == j) {
            printf("\nMauvais format de JSON, manque des guillemets ou autres.\n");
            return 0;
        } else if (data[i] == '\"') {
            break;
        }
    }

    i++;

    //check :"
    if (data[i] != ':' || data[i+1] != '\"') {
        printf("\nMauvais format de JSON, manque des : ou autres.\n");
        return 0;
    }

    i++;
    i++;

    //check  xxxx"
    while(1) {
        i++;
        if (i == j) {
            printf("\nMauvais format de JSON, manque des guillemets ou autres.\n");
            return 0;
        } else if (data[i] == '\"') {
            break;
        }
    }

    i++;

    //check ,"
    if (data[i] != ',' || data[i+1] != '\"'){
        printf("\nMauvais format de JSON, manque des guillemets, des virgules ou autres.\n");
        return 0;
    }

    i++;
    i++;

    //check  xxxx"
    while(1) {
        i++;
        if (i == j) {
            printf("\nMauvais format de JSON, manque des guillemets ou autres.\n");
            return 0;
        } else if (data[i] == '\"') {
            break;
        }
    }

    i++;

    //check :[
    if (data[i] != ':' || data[i+1] != '[') {
        printf("\nMauvais format de JSON, manque des :, crochets ou autres.\n");
        return 0;
    }

    i++;
    i++;

    //ON EN EST lA
    //{"Code":"test","Valeur":[   ICI  -->   "Bonjour","Bonsoir"]}
    //Test si Num ou string
    
    while (i != j) {
        //Si pas de " devrait etre que des digits jusqu'a prochaine , ou ]
        if (data[i] != '\"') {
            int tNum = testNum(data, i-1);
            if (tNum == 0){
                printf("\nMauvais format de JSON, seul les numeros n'ont pas de guillemets\n");
                return 0;
            } else {
                i = tNum;
                // i = , ou ]
            } 
         
        //Si " donc devrait avoir un " apres
        } else if (data[i] == '\"'){
            int balise = i + 2;
            int countInt = 0;
            while(1) {
                i++;
                if (i == j) {
                    printf("\nMauvais format de JSON, les strings ont des guillemets\n");
                    return 0;
                } else if (data[i] == '\"') {
                    i++;
                    break;
                    // i = , ou ]
                }
                if (data[i] >= '0' && data[i] <= '9') {
                    countInt++;
                }
            }
            if (countInt == i - balise) {
                printf("\nMauvais format de JSON, seul les numeros n'ont pas de guillemets\n");
                return 0;
            }   
        }

        if (data[i] == ']') {
            i++;
            break;
        }
        
        i++;
    }

    //i = }
    if (data[i] == '}' && i == j) {
        return 1;
    } else {
        printf("\nMauvais format de JSON, manque des crochets ou autres.\n");
        return 0;
    }


}

void validationJson(char * data) {

    int validate = testJson(data);

    if (validate) {
        printf("Good Json\n");
    } else {
        printf("Bad Json\n");
    }
    
    
}