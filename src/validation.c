#include <string.h>

#include <stdio.h>

#include <stdlib.h>

#include <unistd.h>

char strTest[100] = "{\"Code\":\"test\",\"Valeur\":[\"Bonjour\",\"Bonsoir\"]}";

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

//check {"xx":"yy",
int testUn(char * data, int i) {

    int j;
    j = strlen(data);

    printf("%d", j);
    
    if (data[i] != '{' || data[i+1] != '\"') {
        printf("Fail1");
        return 0;
    }

    i = i + 2;
    
    while (i < j && data[i] != '\"') {
        i++;
        if (i-1 == j) {
            printf("Fail2");
            return 0;
        }
    }

    i++;

    if (i == j || data[i] != ':' || data[i+1] != '\"') {
        printf("Fail3");
        return 0;
    }

    i = i + 2;

    while (i < j && data[i] != '\"') {
        i++;
        if (i-1 == j) {
            printf("Fail4");
            return 0;
        }
    }

    i++;

    if (i == j || data[i] != ',') {
        printf("Fail5");
        return 0;
    }

    i++;

    return i;

}

//check "xx":"y1", "y2", "y3", ... }, attention y peut etre int donc sans ""
int testDeux(char * data, int i) {

    int j;
    j = strlen(data);

    if (data[i] != '\"') {
        printf("Fail6");
        return 0;
    }

    i++;

    while (i < j && data[i] != '\"') {
        i++;
        if (i-1 == j) {
            printf("Fail7");
            return 0;
        }
    }

    i++;

    if (i+2 >= j || data[i] != ':' || data[i+1] != '[' || data[i+2] != '\"') {
        printf("Fail8");
        return 0;
    }

    i = i + 3;

    //on est a :[


}

void validation(char * data) {

    deleteSpace(data);

    int i;
    int result;

    i = testUn(data, 0);

    result = testDeux(data, i);

    printf("\n%d", result);
}

int main(int argc, char ** argv) {
    validation(strTest);
}