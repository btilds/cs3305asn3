#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h> 
#include <pthread.h>

// Account structure
struct Account{
    int accNo;
    int type; //0 for personal 1 for business
    int depFee;
    int withFee;
    int transferFee;
    int numTransactBeforeFee; // Number of transactions before a fee occurs
    int numTransact; // Number of transactions done on the account
    int transactFee;
    int overdraft; //0 for no 1 for yes
    int overdraftFee;
    int balance;
};

// Position of above input 2D array
int numberOfWords = 0;

// Put all words/numbers from file into above input array
void count_words (FILE *inputFile){
    char word[12];
    while (fscanf(inputFile, " %11s", word) == 1){
        numberOfWords++;
    }
}

// Put all words/numbers from file into above input array
void read_words (FILE *inputFile, char input[][13]){
    char word[12];
    int i = 0;
    while (fscanf(inputFile, " %11s", word) == 1){
        strcpy(input[i], word);
        i++;
    }
}

// Current account
int currentAccount = -1;

// Number of accounts
int numAccount = 0;

// Create bank account, each one as an Account struct, and add them to the accounts list
void create_accounts(char input[][13], struct Account accounts[]){
    for (int i = 0; i < numberOfWords; i++){
        if((strcmp(input[i], "personal") == 0) || (strcmp(input[i], "business")==0)){
            if(strcmp(input[i], "personal")==0){
                accounts[currentAccount].accNo = currentAccount+1;
                accounts[currentAccount].type = 0;
                accounts[currentAccount].depFee = atoi(input[i+2]);
                accounts[currentAccount].withFee = atoi(input[i+4]);
                accounts[currentAccount].transferFee = atoi(input[i+6]);
            }
            else if(strcmp(input[i], "business")==0){
                accounts[currentAccount].accNo = currentAccount+1;
                accounts[currentAccount].type = 1;
                accounts[currentAccount].depFee = atoi(input[i+2]);
                accounts[currentAccount].withFee = atoi(input[i+4]);
                accounts[currentAccount].transferFee = atoi(input[i+6]);
            }
        }
        else if(strcmp(input[i], "transactions")==0){
            accounts[currentAccount].numTransactBeforeFee = atoi(input[i+1]);
            accounts[currentAccount].transactFee = atoi(input[i+2]);
        }
        else if(strcmp(input[i], "Y")==0){
            accounts[currentAccount].overdraft = 1;
            accounts[currentAccount].overdraftFee = atoi(input[i+1]);
        }
        else if(strcmp(input[i], "N")==0){
            accounts[currentAccount].overdraft = 0;
        }
    }
}

// Alter account data after deposit
void deposit_account_change(int accountNumber, int depositAmmount, struct Account accounts[]){
    // Increase account balance
    accounts[accountNumber].balance += depositAmmount;
    // Subtract deposit fee
    accounts[accountNumber].balance -= accounts[accountNumber].depFee;
    // Increase number of transactions
    accounts[accountNumber].numTransact += 1;
    // If number of transactions is over transaction limit, overage transaction fee
    if((accounts[accountNumber].numTransact) > (accounts[accountNumber].numTransactBeforeFee)){
        accounts[accountNumber].balance -= accounts[accountNumber].transactFee;
    }
}

// Add deposits to accounts
void deposit(char input[][13], struct Account accounts[]){
    int accountNumber;
    int done = 0;
    // Iterate through the array of words
    for (int i = 0; i < numberOfWords; i++){
        if(done == 0){
            // Check if we are at a new depositor
            if(strncmp(input[i], "dep", 3) == 0){
                sscanf(input[i+2], "a%d", &accountNumber);
                deposit_account_change(accountNumber-1, atoi(input[i+3]), accounts);
                // Iterate through all deposits from the depositor
                for (int j = i+3; j < numberOfWords; j+=3){
                    // Check if no more deposits from this depositor and there is another depositor
                    if(strncmp(input[j+1], "dep", 3) == 0){
                        break;
                    }
                    // Check if there are no more depositors
                    else if(strncmp(input[j+1], "c", 1) == 0){
                        done = 1;
                        break;
                    }
                    else{
                        sscanf(input[j+2], "a%d", &accountNumber);
                        deposit_account_change(accountNumber-1, atoi(input[j+3]), accounts);
                    }
                }
            }
        }
        else{
            break;
        }
    }
}

// Transaction Struct
struct Transaction {
    int type; // 0 for withdrawl, 1 for deposit, 2 for transfer
    int withAccountNo;
    int withdrawalAmount;
    int depAccountNo;
    int depositAmmount;
    int fromAccountNo;
    int toAccountNo;
    int transferAmount;
};

// Client structure
struct Client{
    int clientNo;
    int numTransactions;
    struct Transaction transactions[100];
};

// Create an array of clients (1000 max)
struct Client clients[100];
// The number of clients
int numberOfClients = 0;

// Add transactions to clients linked list; each client will have a linked list of transactions
void manage_clients(char input[][13]){
    int clientNumber;
    // Iterate through the array of words
    for (int i = 0; i < numberOfWords; i++){
        if((sscanf(input[i], "c%d", &clientNumber)) == 1){
            // Increase number of clients
            numberOfClients++;
            // Create client and set number of transactions to 0
            clients[clientNumber-1].clientNo = clientNumber;
            clients[clientNumber-1].numTransactions = 0;
            int j = i;
            // Temp account information
            int depositAccount;
            int withdrawalAccount;
            int transferFromAccount;
            int transferToAccount;
            int transferAmount;
            // While we have not reached the end of the input array
            for(int j = i; j < numberOfWords; j++){
                // Temp variable for client number position in clients array
                int x = clientNumber-1;
                if(strcmp(input[j+1], "d") == 0){
                    sscanf(input[j+2], "a%d", &depositAccount);
                    clients[x].transactions[clients[x].numTransactions].depAccountNo = depositAccount;
                    clients[x].transactions[clients[x].numTransactions].depositAmmount = atoi(input[j+3]);
                    clients[x].numTransactions++;
                }
                else if(strcmp(input[j+1], "w") == 0){
                    sscanf(input[j+2], "a%d", &withdrawalAccount);
                    clients[x].transactions[clients[x].numTransactions].withAccountNo = withdrawalAccount;
                    clients[x].transactions[clients[x].numTransactions].withdrawalAmount = atoi(input[j+3]);
                    clients[x].numTransactions++;
                }
                else if(strcmp(input[j+1], "t") == 0){
                    sscanf(input[j+2], "a%d", &transferFromAccount);
                    sscanf(input[j+3], "a%d", &transferToAccount);
                    clients[x].transactions[clients[x].numTransactions].fromAccountNo = transferFromAccount;
                    clients[x].transactions[clients[x].numTransactions].toAccountNo = transferToAccount;
                    clients[x].transactions[clients[x].numTransactions].transferAmount = atoi(input[j+4]);
                    clients[x].numTransactions++;
                }
                else if(strncmp(input[j+1], "c", 1) == 0){
                    break;
                }
            }
        }
    }
}

pthread_mutex_t lock;

void *manage_transactions(void *input){

    printf("\n\n * From thread manage_transactions entering lock region * \n");
    
}

int main (int args, char* argv[]){
    // Check if 2 arguments have been passed
    if(args != 2){
    printf("\n Please enter a text file named 'assignment_3_input_file.txt'\n");
    exit(0);
    }

    // Open input file
    FILE *inputFile;
    inputFile = fopen(argv[1], "r");
    if (inputFile == NULL){
        printf("Error opening file\n");
        exit(1);
    }

    count_words(inputFile);
    char input[numberOfWords][13];
    fclose(inputFile);

    inputFile = fopen(argv[1], "r");
    read_words(inputFile, input);
    fclose(inputFile);

    // Array of accounts
    struct Account accounts[numberOfWords];

    create_accounts(input, accounts);
    deposit(input, accounts);
    manage_clients(input);

    pthread_t threads[numberOfClients];

    if(pthread_mutex_init(&lock, NULL) != 0) {
        printf("mutex init failed\n");
        return 1;
    }

    for(int i = 0; i < numberOfClients; i++){
        pthread_create(&threads[i], NULL, manage_transactions, (void *)&clients[i]);
    }
    for(int i = 0; i < numberOfClients; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

}