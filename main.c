
#include "global.h"
#include "sqlite3.h"


/* .schema of table
CREATE TABLE account_info(
id integer UNIQUE DEFAULT(1) PRIMARY KEY,
first_name VARCHAR(40),
last_name VARCHAR(40),
password VARCHAR(100),
balance integer,
upload DATETIME DEFAULT CURRENT_TIMESTAMP);
*/

typedef struct{
    int id;
    char first_name[first_name_max];
    char last_name[last_name_max];
    char password[password_max];
    int  balance; // maximum balance is 2147483647;
} account;

account active_accounts = {0};



int callback(void *notused, int argc,char ** argv, char ** argcolumn)
{
    for(int i = 0; i < argc; i++)
    {
        printf("%s = %s \n", argcolumn[i], argv[i]);
    }
    printf("\n");
    return 0;
}

int callback_1(void *notused, int argc, char **argv, char **argcolumn)
{
    // Your code here
    return 0;
}

int callback_active_index(void *notused, int argc, char ** argv, char ** argcolumn)
{
    active_accounts.id = atoi(argv[0]);
    strcpy(active_accounts.first_name, argv[1]);
    strcpy(active_accounts.last_name, argv[2]);
    strcpy(active_accounts.password, argv[3]);
    active_accounts.balance = atoi(argv[4]);

    return 0;
}

int update(sqlite3 *db);
int updateinfo(sqlite3 *db);

int main()
{
    sqlite3 *db;
    int choice;
    restart:
    printf("**************************************\n");
    printf("1. Login \n2. register \n3. Exit\n");
    printf("Enter your choice : ");
    scanf("%d", &choice);

    int sql_exit = sqlite3_open("account.db", &db);
    char *errormessage = NULL;
    char sql_query[400] ;
    switch(choice)
    {
        case 1:
            // login page
            char first_name_login[first_name_max], last_name_login[last_name_max], password_login[password_max];
            printf("Enter the First Name : ");
            scanf("%s", first_name_login);
            printf("Enter the Last Name : ");
            scanf("%s", last_name_login);
            printf("Enter the Password : ");
            scanf("%s", password_login);
            sprintf(sql_query,"select id, first_name, last_name, password, balance  from account_info where first_name = '%s' and last_name = '%s' and password = '%s'", first_name_login, last_name_login, password_login );
            sql_exit = sqlite3_exec(db, sql_query, callback_active_index, 0, &errormessage);
            if(sql_exit != 0)
            {
                printf("Error in sql_exit || sql_exit != 0\n");
            }
            printf("Error : %s\n", (errormessage != NULL)?errormessage:"");
            printf("\n **** You are now Logedin ****\n");
            printf("%d\n\n", active_accounts.id);


            break;
        case 2:
            // 1. check weter the username is alreddy taken or not 
            // and if takend then promp the user to enter another username/
            // or use username as @email so that the user only inpust a single string and not multiple or even breakes the string

            char first_name[first_name_max], last_name[last_name_max], password[password_max], re_password[password_max];
            printf("Enter the user's First name : ");
            scanf("%s", first_name);
            printf("Enter the user's Last name : ");
            scanf("%s", last_name);
            printf("Enter the password : ");
            scanf("%s", password);
            printf("Re-enter the password : ");
            scanf("%s", re_password);
            if(strcmp(password, re_password) != 0)
            {
                printf(" *** Please enter a matching password *** \n");
                printf("Exiting \n");
                return 1;
            }
            sprintf(sql_query, "insert into account_info (first_name, last_name, password, balance) values ('%s', '%s', '%s', %d)", first_name, last_name, password, 0); 
            // sql_query = "insert into account_info (first_name, last_name, password, balance) values ('sanatan', 'mishra', 'abc@123', 0);";
            sql_exit = sqlite3_exec(db, sql_query, callback, 0, &errormessage);
            if(sql_exit != 0)
            {
                printf("Error in registering user \n");
                return 1;
            }
            if(errormessage == NULL)
            {
                printf("\n **** The New user is now registered **** \n\n");
            }
            
            // register
            break;
        case 3:
            sqlite3_close(db);
            return 0;
            break;
            
        default:
            printf("\n    **** The New user is now registered **** \n\n");
            goto restart;
            break;
    }
    logedin:
    printf("\n\n\n");
    printf("********************************************\n");
    printf("Name : %s %s************** balance : %d\n", active_accounts.first_name, active_accounts.last_name, active_accounts.balance);
    printf("********************************************\n");

    // options print
    int task, amount = 0;
    printf("1. Add Money \n2. withdraw money \n3. Logout\n4. Update Info\n" );
    printf("Enter your Choice : ");
    scanf("%d", &task);
    
    switch(task)
    {
        case 1:
            // input filter
            do{
                printf("Enter the amount you want to Deposite : ");
                scanf("%d", &amount);
                if(amount < 0)
                {   printf(" !! can not Deposite -ve amount !!\n");}
            }while(amount < 0);

            // deposit_money();
            active_accounts.balance += amount;
            amount = 0;
            update(db);

            goto logedin;
            break;
        case 2:
            // input filter;
            do{
                printf("Enter the amount you want to Withdraw : ");
                scanf("%d", &amount);
                if(amount < 0)
                {   printf(" !! can not Withdraw -ve amount !!\n");}
            }while(amount < 0);
            if(active_accounts.balance < amount)
            {
                printf(" !!!! **** Sorry you dont have enoughf balance **** !!!!");
                goto logedin;
            }

            // withdraw_money();
            active_accounts.balance -= amount;
            amount = 0;

            update(db);

            goto logedin;
            break;

        case 3:

            // clearing data
            active_accounts.id = 0;
            strcpy(active_accounts.first_name, "");
            strcpy(active_accounts.last_name, "");
            strcpy(active_accounts.password, "");
            active_accounts.balance = 0;
            // clearing data

            return 0;
        case 4:
            int choice123;
            if(updateinfo(db) != 0)
            {
                printf("An Error has ocured \n");
            }
            else
            {
                printf(" **** Update Done successfully **** \n");
            }
            goto logedin;

        default:
            printf("Please Enter a valid input : ");
            goto logedin;
        
    }

    

    sqlite3_close(db);
}

int update(sqlite3 *db)
{
    char sql_query[400];
    char * errormessage;
    sprintf(sql_query, "update account_info set  first_name = '%s' , last_name = '%s', password = '%s', balance = %d where id = %d", active_accounts.first_name, active_accounts.last_name, active_accounts.password, active_accounts.balance, active_accounts.id );
    int sql_exit = sqlite3_exec(db, sql_query, callback_1, 0, &errormessage);
    if(sql_exit == 0){printf("Updates SuccessFull\n");}
    printf("%s", (errormessage != NULL)?errormessage:"");
    return 0;
}

int updateinfo(sqlite3 *db)
{
    int choice;
    updateform:
    printf("WHat info you want to update ?? \n");
    printf("1. First Name\n2. Last Name\n3. Password\n4. Exit \n");
    printf("Enter your choice : ");
    scanf("%d", &choice);
    switch(choice)
    {
        case 1:
            // First name
            char first_name[first_name_max];
            printf("Enter your new first name : ");
            scanf("%s", first_name);
            strcpy(active_accounts.first_name, first_name);
            update(db);
            break;
        case 2:
            // last name
            char last_name[last_name_max];
            printf("Enter your new Lase name : ");
            scanf("%s", last_name);
            strcpy(active_accounts.last_name, last_name);
            update(db);
            break;
        case 3:
            // password
            char old_password[password_max], new_password[password_max], re_new_password[password_max];
            int chance = 3;
            old_password:
            if(chance < 0)
            {
                printf("Sorry please try again later. You have Used all your chances To enter the correct password \n");
            }
            printf("Enter your Old Password : ");
            scanf("%s", old_password);
            if(strcmp(old_password, active_accounts.password) != 0)
            {
                printf("Please enter correct password \n");
                chance -= 1;
                goto old_password;
            }
            new_password:
            printf("Enter your New Password : ");
            scanf("%s", new_password);
            printf("Re-Enter yout New Password : ");
            if(strcpy(new_password, re_new_password) != 0)
            {
                printf(" **** Please enter a matching password **** \n");
                goto new_password;
            }
            strcpy(active_accounts.password, new_password);
            update(db); 
            break;
        
        case 4:
            // exit
            return 0;
            break;
        default:
            printf(" **** Please enter a valid choice **** \n");
            goto updateform;
    }
    return 0;
}