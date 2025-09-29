#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define MAX 100

//Remove White spaces
void RW(char *s){
    int i=0,j=0;
    while(s[i]){
        if(!isspace(s[i])){
            s[j++]=s[i];
        }
        i++;
    }
    s[j]='\0';
}

int VE(const char *s){
    for(int i=0;s[i];i++){
        if(!(isdigit(s[i]) || s[i]=='+' || s[i]=='-' || s[i]=='*' || s[i]=='/')){
            return 0;
        }
    }
    return 1;
}

int calculate(char *expr , int *error){
    int nums[MAX];
    int ops[MAX];
    int n=0, o=0,i=0;

    while(expr[i]){
        if(isdigit(expr[i])){
            int num=0;
            while(isdigit(expr[i])){
                num=num*10+(expr[i]-'0');
                i++;
            }
            nums[n++]=num;
        }else{
            if(expr[i]=='+' || expr[i]=='-' || expr[i]=='*' || expr[i]=='/'){
                ops[o++]=expr[i];
                i++;
            }
        }
    }
    for(int j=0;j<o;){
        if(ops[j]=='*' || ops[j]=='/'){
            if(ops[j]=='*'){
                nums[j]=nums[j]*nums[j+1];
            }else{
                if(nums[j+1]==0){
                    *error = 2; 
                    return 0;
                }
                nums[j]=nums[j]/nums[j+1];
            }
            for(int k=j+1;k<n-1;k++)
                nums[k]=nums[k+1];
            for(int k=j;k<o-1;k++)
                ops[k]=ops[k+1];
            n--;
            o--;
        }
        else{
            j++;
        }
    }
    int result=nums[0];
    for(int j=0;j<o;j++){
        if(ops[j]=='+')
            result+=nums[j+1];
        else
            result-=nums[j+1];
    }
    return result;
}

int main(){
    char input[MAX];
    fgets(input,sizeof(input),stdin);

    input[strcspn(input,"\n")]=0;

    RW(input);

    if(!VE(input) || strlen(input)==0){
        printf("Error: INvalid expression. \n");
        return 0;
    }
    int error=0;
    int result=calculate(input,&error);
    if(error==2){
        printf("Error: Division by zero.\n");
        return 0;
    }
    printf("%d\n",result);
    return 0;
}