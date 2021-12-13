#include <string.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

char strTest[100] = "{\"Code\":\"test\",\"Valeur\":[\"Bonjour\",\"Bonsoir\"]}";
char strTest2[100] = "{\"Code\":\"test\",\"Valeur\":[\"Bonjour\"]}";
char strTest3[100] = "{\"Code\":\"test\",\"Valeur\":[38,45]}";
char strTest4[100] = "{\"Code\":\"test\",\"Valeur\":[38]}";
char strTest5[100] = "{\"Code\":\"test\",\"Valeur\":[Bonjour]}";
char strTest6[100] = "{\"Code\":\"test\",\"Valeur\":[\"38\"]}";
char strTest7[100] = "{\"Code\":\"test\",\"Valeur\":[\"38\",45]}";
char strTest8[100] = "{\"Code\":\"test\",\"Valeur\":\"38\"]}";
char strTest9[100] = "{\"Code\":\"test\",\"Valeur\":[38]";
char strTest10[100] = "{\"Code\":\"test\",\"Valeur\":[\"Comment ca va vous ?\"]}";
char strTest11[100] = "{\"Code\":test\",\"Valeur\":[\"Comment ca va vous ?\"]}";
char strTest12[100] = "{Code\":\"test\",\"Valeur\":[\"Comment ca va vous ?\"]}";
char strTest13[100] = "";
char strTest14[100] = "{\"Code\":\"test\",\"Valeur\":[45,\"38\"]}";

void deleteSpace(char *data)
{
    int j;
    j = strlen(data);
    while(j--)
    {
        if(data[j] == 32)
            data[j] = '\0';

    }
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
        printf("\nMauvais format de JSON, manque des crochets ou autres.\n");
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
    }

    printf("\nMauvais format de JSON, manque des crochets ou autres.\n");
    return 0;

}

void validation(char * data) {

    deleteSpace(data);

    printf("\nData : %s\n", data);

    printf("\nResult : %d\n", testJson(data));

    
}

/* int main(int argc, char ** argv) {
    validation(strTest);
    validation(strTest2);
    validation(strTest3);
    validation(strTest4);
    validation(strTest5);
    validation(strTest6);
    validation(strTest7);
    validation(strTest8);
    validation(strTest9);
    validation(strTest10);
    validation(strTest11);
    validation(strTest12);
    validation(strTest13);
    validation(strTest14);
} */