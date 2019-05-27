#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TOKEN_COUNT 50

typedef enum
{
    UNDEFINED = 0,
    OBJECT = 1,
    ARRAY = 2,
    STRING = 3,
    PRIMITIVE = 4
} TYPE_T;

typedef struct token
{
    TYPE_T type;
    int start;
    int end;
    int size;
    int flag; // token sizes
    struct token *value;
    struct token *next; // point next node
} TOKEN_T;

int getFileSize(char *filename);
void JsonParser(char *allContent, int contentSize, TOKEN_T *list, int base);
void Pushtoken(TOKEN_T *head, TOKEN_T *data);
void printToken(TOKEN_T *head, char* allContent);
int numOfToken = 0;

int main(int argc, char **argv)
{
    int sizeOfFile = -1;
    char buffer[64];

    FILE *fp = NULL;
    //tokenList의 첫 node는 next를 제외하고 모두 null입니다.
    TOKEN_T *tokenList = malloc(sizeof(TOKEN_T));

    tokenList->value = NULL;
    tokenList->next = NULL;

    if (argc < 2)
    {
        printf("usage: ./out <filename>\n");
        return -1;
    }

    fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("fail to open file %s\n", argv[1]);
        return -1;
    }

    sizeOfFile = getFileSize(argv[1]);
    printf("size of file %d \n", sizeOfFile);
    char allContent[sizeOfFile];

    while (fscanf(fp, "%s", buffer) != EOF)
    {
        strcat(allContent, buffer);
        strcat(allContent, " ");
    }

    JsonParser(allContent, sizeOfFile, tokenList, 0);

    return 0;
}

//get length of file content
int getFileSize(char *filename)
{
    FILE *fp = fopen(filename, "r");
    int len = -1;
    if (fp == NULL)
    {
        printf("fail to open file\n");
    }
    else
    {
        fseek(fp, 0, SEEK_END);
        len = (int)ftell(fp);
    }

    return len;
}

void Pushtoken(TOKEN_T *head, TOKEN_T *data)
{
    TOKEN_T *tail;

    //fine end of list
    while (tail->next)
        tail = tail->next;

    //data is key
    if (data->size == 1){
        tail->next = data;
    }
    //data is value
    else if (data->size == 0){
        switch (data->type){
            case OBJECT:{
                tail->value = data;
                break;
            }

            case ARRAY:{
                //find end of array
                while(tail->value) tail = tail->value;
                tail->value = data;
                break;
            }

            case STRING:{
                tail->value = data;
                break;
            }
            default:{
                printf("What is the token's type?\n");
            }
        }
    }
    else
        printf("fail to add token in list\n");
}
void printToken(TOKEN_T *head, char* allContent){
    TOKEN_T *temp = head;

    do{
        if(temp->value == NULL) continue;
        else if(temp->value->type == OBJECT){
            printToken(temp->value, allContent);
        }
        else if(temp->value->type == ARRAY){
            while(temp->value){
                printf("\n");
                temp = temp->value;
            }
        }
        else {
            printf("\n");
        }
        temp = temp->next;
    }while(temp->next);
}


void JsonParser(char *allContent, int contentSize, TOKEN_T *list, int base)
{
    int cur = base;
    //int numOfToken = 0;
    // 이거 있으면 ARRAY에 원소 2개 이상일 때 동작 안함.
    // if(allContent[cur] != '{'){
    //     return ;
    // }

    cur++;

    while(cur < contentSize){
        switch (allContent[cur]) { //doc[pos]

            //string
            case '"':
            {
                char *begin = allContent + cur + 1;
                char *end = strchr(begin, '"');
                char temp[contentSize];

                if(end == NULL) break;

                int wordLen = end - begin;

                strncpy(temp, begin, wordLen);
                temp[wordLen] = '\0';
                numOfToken++;

                //print info of each token
                printf("[%2d] %s (size=1, %lu~%lu)\n", numOfToken, temp, strlen(allContent) - strlen(begin), strlen(allContent) - strlen(end));

                cur = cur + wordLen + 1;
                //tokenList에 저장하는거
                //token size
            }
                break;

            //array
            case '[':
            {
                cur++;
                char temp[contentSize];

                while( allContent[cur] != ']' ){// doc[pos]
                  cur++;
                    if( allContent[cur] == '"')
                    {
                        char *begin = allContent + cur + 1;
                        char *end = strchr(begin, '"');// '"'로 시작하는 문자열을 end에
                        if( end==NULL ) break;

                        int wordLen = end - begin;
                        strncpy(temp, begin, wordLen);
                        temp[wordLen] = '\0';
                        cur = cur + wordLen + 1;
                        numOfToken++;
                        printf("[%2d] %s (size=1, %lu~%lu)\n", numOfToken, temp, strlen(allContent) - strlen(begin), strlen(allContent) - strlen(end));
                    }
                    //print info of each token
                }

            break;
            }
            
            //object
            case '{':
            {
                char temp[contentSize]; // 객체 저장
                char *begin = allContent + cur;

                // if( allContent[cur] == '{') { begin = allContent + cur; }

                char *end = strchr(begin+1, '}');
                int wordLen = end - begin + 1;
                strncpy(temp, begin, wordLen);
                temp[wordLen] = '\0';
                numOfToken++;
                //printf("[%2d] %s (size=1, %lu~%lu)\n", numOfToken, temp, strlen(allContent) - strlen(begin), strlen(allContent) - strlen(end));

                JsonParser(allContent, wordLen + cur, list, cur);
                cur = cur + wordLen + 1;

                break;
            }
            case '}':
                return;

            //numbers
            case '0': case '1': case '2': case '3': case '4': case '5': case '6': case'7': case '8': case '9': case '-':
            {
              char *begin = allContent + cur;
              char *end;
              char *buffer;
              int num = 0;

              end = strchr( allContent + cur, ',' );
              if( end  == NULL ) {
                end = strchr( allContent + cur, '}');
                if ( end == NULL ) break;
              }

              int stringLength = end - begin;
              buffer = malloc(stringLength + 1);

              strncpy(buffer, begin, stringLength);
              num = atoi(buffer);
              numOfToken++;
              printf("[%2d] %s (size=1, %lu~%lu)\n", numOfToken, buffer, strlen(allContent) - strlen(begin), strlen(allContent) - strlen(end));
              cur = cur + stringLength + 1;
              break;
            }
            default:
                break;
        }
        cur++;
    }
}
