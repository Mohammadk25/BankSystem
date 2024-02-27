#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream> 
#include <iomanip>
#include <ctime>
#include <time.h>
#include <limits>

using namespace std;

class AccountType {
private:
    int accountClass;
    std::string designation;

public:
    AccountType(int ac, std::string des) : accountClass(ac), designation(des) {}

    int getAccountClass() const { return accountClass; }
    std::string getDesignation() const { return designation; }
    // int getFirstDigit() const = 0;
    
    static std::vector<AccountType> loadAccountTypesFromFile() {
        std::vector<AccountType> accountTypes;
        std::ifstream file("account_types.csv");
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            std::vector<std::string> tokens;
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            if (tokens.size() >= 2) { // Ensure that there are enough tokens to form an AccountType
                int ac = std::stoi(tokens[0]);
                std::string des = tokens[1];
                accountTypes.push_back(AccountType(ac, des));
            }
        }
        file.close();
        return accountTypes;
    }


    void saveToFile() const {
        std::ofstream file("account_types.csv", std::ios::app);
        file << accountClass << "," << designation << std::endl;
        file.close();
    }
};
class PostingAccount : public AccountType{
private:
    int number;
    std::string name;

public:
    PostingAccount(int num, const std::string& nm) 
        : AccountType(-1, "Undefined"), number(num), name(nm) {}

    PostingAccount(int accClass, const std::string& desig, int num, const std::string& accName)
        : AccountType(accClass, desig), number(num), name(accName) {}
    int getNumber() const { return number; }
    std::string getName() const { return name; }

    static std::vector<PostingAccount> loadPostingAccountsFromFile() {
        std::vector<PostingAccount> postingAccounts;
        std::ifstream file("posting_accounts.csv");
        std::string line;
        while (std::getline(file, line)) {
            int commaPos = line.find(",");
            int num = std::stoi(line.substr(0, commaPos));
            std::string nm = line.substr(commaPos + 1);
            postingAccounts.push_back(PostingAccount(num, nm));
        }
        file.close();
        return postingAccounts;
    }

    void saveToFile() const {
        std::ofstream file("posting_accounts.csv", std::ios::app);
        file << number << "," << name << std::endl;
        file.close();
    }
};

// Derived class for Cash Account
class CashAccount : public AccountType {
public:
    CashAccount() : AccountType(1, "Cash") {}

    // int getFirstDigit() const { return 1; }
};

// Derived class for Credit Card Account
class CreditCardAccount : public AccountType {
public:
    CreditCardAccount() : AccountType(6, "Credit Card") {}

    // int getFirstDigit() const { return 6; }
};

class PostingRecord {
private:
    int bookingNumber;
    std::string date;
    std::string postingText;
    int targetAccount;
    int originalAccount;
    int postingAmount; // In cents

public:
    PostingRecord(int bn, std::string dt, std::string text, int ta, int oa, int amount)
        : bookingNumber(bn), date(dt), postingText(text), targetAccount(ta), originalAccount(oa), postingAmount(amount) {}

    int getBookingNumber() const { return bookingNumber; }
    std::string getDate() const { return date; }
    std::string getPostingText() const { return postingText; }
    int getTargetAccount() const { return targetAccount; }
    int getOriginalAccount() const { return originalAccount; }
    int getPostingAmount() const { return postingAmount; }

    static std::vector<PostingRecord> loadPostingRecordsFromFile() {
        std::vector<PostingRecord> records;
        std::ifstream file("journal.csv");
        std::string line;
        while (std::getline(file, line)) {
            // Parse CSV line
            std::stringstream ss(line);
            std::vector<std::string> tokens;
            std::string token;
            while (std::getline(ss, token, ',')) {
                tokens.push_back(token);
            }
            // Create PostingRecord from parsed data
            int bn = std::stoi(tokens[0]);
            int ta = std::stoi(tokens[3]);
            int oa = std::stoi(tokens[4]);
            int amount = std::stoi(tokens[5]);
            records.push_back(PostingRecord(bn, tokens[1], tokens[2], ta, oa, amount));
        }
        file.close();
        return records;
    }

    void saveToFile() const {
        std::ofstream file("journal.csv", std::ios::app);
        file << bookingNumber << "," << date << "," << postingText << "," << targetAccount << ","
            << originalAccount << "," << postingAmount << std::endl;
        file.close();
    }
};

int getNumericInput(int maxDigits = -1) {
    int value;
    while (true) { // Keep asking until valid input is provided
        while (!(std::cin >> value)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number: ";
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (maxDigits == -1) {
            break; // If no maxDigits limit is set, break out of the loop
        }

        int maxValue = 1;
        for (int i = 0; i < maxDigits; i++) {
            maxValue *= 10;
        }
        if (value < maxValue) {
            break; // If the value is within the allowed limit, break out of the loop
        }
        std::cout << "Please enter a number with at most " << maxDigits << " digits: ";
    }
    return value;
}


void createNewAccountType() {
    std::cout << "Enter account class (one-digit numeric): ";
    int accountClass = getNumericInput(1);

    // Create instances of account type classes
    CashAccount cashAccount;
    CreditCardAccount creditCardAccount;
    // Create instances for other account types as needed

    std::cout << "1. " << cashAccount.getDesignation() << std::endl;
    std::cout << "2. " << creditCardAccount.getDesignation() << std::endl;
    // Display other account types

    std::cout << "Enter the number corresponding to the account type: ";
    int selectedAccountType;
    std::cin >> selectedAccountType;

    // Use polymorphism to get the first digit based on the selected account type
    AccountType* selectedAccount = nullptr;
    switch (selectedAccountType) {
    case 1:
        selectedAccount = &cashAccount;
        break;
    case 2:
        selectedAccount = &creditCardAccount;
        break;
    // Handle other account types
    default:
        std::cout << "Invalid account type selection." << std::endl;
        return;
    }

    int firstDigit = 0;

    // Check uniqueness of account class
    std::vector<AccountType> existingTypes = AccountType::loadAccountTypesFromFile();
    for (const AccountType& type : existingTypes) {
        if (type.getAccountClass() == accountClass) {
            std::cout << "Error: Account class already exists!" << std::endl;
            return; 
        }
    }
    std::cout << "Enter designation: ";
    std::string designation;
    std::getline(std::cin, designation);

    // Check uniqueness of designation
    for (const AccountType& type : existingTypes) {
        if (type.getDesignation() == designation) {
            std::cout << "Error: Designation already exists!" << std::endl;
            return; 
        }
    }

    AccountType newType(accountClass, designation);
    newType.saveToFile();
    std::cout << "New account type created successfully!" << std::endl;
}

void listAllAccountTypes() {
    std::vector<AccountType> accountTypes = AccountType::loadAccountTypesFromFile();
    std::cout << "Account Class | Designation" << std::endl;
    std::cout << "---------------------------" << std::endl;
    for (const AccountType& type : accountTypes) {
        std::cout << type.getAccountClass() << "             | " << type.getDesignation() << std::endl;
    }
}
void createNewAccount() {
    std::cout << "Select a type of account:" << std::endl;
    listAllAccountTypes();

    std::cout << "Enter the number corresponding to the type of account: ";
    int selectedAccountType;
    std::cin >> selectedAccountType;

    // Check if the selected account type exists
    std::vector<AccountType> accountTypes = AccountType::loadAccountTypesFromFile();
    bool accountTypeExists = false;
    for (const AccountType& type : accountTypes) {
        if (type.getAccountClass() == selectedAccountType) {
            accountTypeExists = true;
            break;
        }
    }

    if (!accountTypeExists) {
        std::cout << "Error: Selected account type does not exist!" << std::endl;
        return;
    }

    // Automatically determine the first digit based on the selected account type
    int firstDigit = selectedAccountType;

    std::cout << "Enter the remaining 3 digits of the account number (e.g., 001): ";
    int remainingDigits = getNumericInput(3);

    int accountNumber = (firstDigit * 1000) + remainingDigits;

    int accountClass = accountNumber / 1000; // Get the first digit

    if (accountClass != 1 && accountClass != 6 && accountClass != 8) {
        std::cout << "Error: Invalid account number for the account class." << std::endl;
        return;
    }

    std::cout << "Enter account name: ";
    std::string accountName;
    std::cin.ignore(); // Clear the newline character from previous input
    std::getline(std::cin, accountName);

    std::vector<PostingAccount> existingAccounts = PostingAccount::loadPostingAccountsFromFile();
    for (const PostingAccount& account : existingAccounts) {
        if (account.getNumber() == accountNumber) {
            std::cout << "Error: Account number already exists!" << std::endl;
            return;
        }
        if (account.getName() == accountName) {
            std::cout << "Error: Account name already exists!" << std::endl;
            return;
        }
    }

    // Save the new account
    PostingAccount newAccount(accountNumber, accountName);
    newAccount.saveToFile();
    std::cout << "New account created successfully!" << std::endl;
}


void listAllAccounts() {
    std::vector<PostingAccount> postingAccounts = PostingAccount::loadPostingAccountsFromFile();
    std::cout << "Account Number | Account Name" << std::endl;
    std::cout << "-------------------------------" << std::endl;
    for (const PostingAccount& account : postingAccounts) {
        std::cout << account.getNumber() << "            | " << account.getName() << std::endl;
    }
}
void listAllPostingRecords() {
    std::vector<PostingRecord> records = PostingRecord::loadPostingRecordsFromFile();
    std::vector<PostingAccount> accounts = PostingAccount::loadPostingAccountsFromFile();

    std::cout << std::left << std::setw(15) << "Booking Number"
        << std::setw(15) << "Date"
        << std::setw(25) << "Text"
        << std::setw(25) << "Target Account"
        << std::setw(25) << "Original Account"
        << "Amount ( )" << std::endl;
    std::cout << "----------------------------------------------------------------------------------------------------" << std::endl;

    for (const PostingRecord& record : records) {
        std::string targetAccountName, originalAccountName;
        for (const PostingAccount& account : accounts) {
            if (account.getNumber() == record.getTargetAccount()) {
                targetAccountName = account.getName();
            }
            if (account.getNumber() == record.getOriginalAccount()) {
                originalAccountName = account.getName();
            }
        }
        std::cout << std::left << std::setw(15) << record.getBookingNumber()
            << std::setw(15) << record.getDate()
            << std::setw(25) << record.getPostingText().substr(0, 24) 
            << std::setw(25) << targetAccountName
            << std::setw(25) << originalAccountName
            << std::fixed << std::setprecision(2) << record.getPostingAmount() / 100.0 << " Euro " << std::endl;
    }
}

void searchPostingRecords() {
    std::cout << "Enter term to search for in posting texts: ";
    std::string searchTerm;
    std::getline(std::cin, searchTerm); // fetch the term

    std::vector<PostingRecord> records = PostingRecord::loadPostingRecordsFromFile();
    bool found = false;
    for (const PostingRecord& record : records) {
        if (record.getPostingText().find(searchTerm) != std::string::npos) {
            std::cout << "Found record with Booking Number: " << record.getBookingNumber() << std::endl;
            std::cout << "Date: " << record.getDate() << std::endl;
            std::cout << "Text: " << record.getPostingText() << std::endl;
            std::cout << "Target Account: " << record.getTargetAccount() << std::endl;
            std::cout << "Original Account: " << record.getOriginalAccount() << std::endl;
            std::cout << "Amount (cents): " << record.getPostingAmount() << std::endl;
            found = true;
        }
    }
    if (!found) {
        std::cout << "No records found for the term " << searchTerm << "." << std::endl;
    }
}



void saveBookingToAccountFile(int accountNumber, int bookingNumber) {
    std::string filename = "account_" + std::to_string(accountNumber) + ".csv";
    std::ofstream file(filename, std::ios::app);
    file << bookingNumber << std::endl;
    file.close();
}

int getBalanceForAccount(int accountNumber) {
    int balance = 0;
    std::string filename = "account_" + std::to_string(accountNumber) + ".csv";
    std::ifstream file(filename);
    if (!file) {
        return balance; // Return 0 balance if the file doesn't exist
    }

    std::vector<int> bookingNumbers;
    int bookingNum;
    while (file >> bookingNum) {
        bookingNumbers.push_back(bookingNum);
    }
    file.close();

    // Load all posting records
    std::vector<PostingRecord> allRecords = PostingRecord::loadPostingRecordsFromFile();

    // Calculate the balance based on the booking numbers
    for (const int& num : bookingNumbers) {
        for (const PostingRecord& record : allRecords) {
            if (record.getBookingNumber() == num) {
                // Determine whether the account is the target or original account in the record
                if (record.getTargetAccount() == accountNumber) {
                    balance += record.getPostingAmount();
                } else if (record.getOriginalAccount() == accountNumber) {
                    balance -= record.getPostingAmount();
                }
                break;
            }
        }
    }

    return balance;
}

void createNewPostingRecord() {
    std::vector<PostingRecord> existingRecords = PostingRecord::loadPostingRecordsFromFile();
    time_t now = time(0);
    // struct tm ltm;
    tm ltm;
    #ifdef _WIN32
    localtime_s(&ltm, &now);
    #else
    localtime_r(&now, &ltm);
    #endif
    // std::string currentDate = std::to_string(1900 + ltm.tm_year) + "-"
    //     + std::to_string(1 + ltm.tm_mon) + "-"
    //     + std::to_string(ltm.tm_mday);

    std::stringstream dateStream;
    dateStream << std::setw(2) << std::setfill('0') << ltm.tm_mday << "-"
            << std::setw(2) << std::setfill('0') << (1 + ltm.tm_mon) << "-"
            << (1900 + ltm.tm_year);
    std::string currentDate = dateStream.str();

    int newBookingNumber = 1;  // default value
    if (!existingRecords.empty()) {
        int latestBookingNumber = existingRecords.back().getBookingNumber();
        newBookingNumber = latestBookingNumber + 1;
    }
    std::cout << "Enter booking number: ";
    int bookingNumber = getNumericInput();

    // Check uniqueness of booking number
    for (const PostingRecord& record : existingRecords) {
        if (record.getBookingNumber() == bookingNumber) {
            std::cout << "Error: Booking number already exists!" << std::endl;
            return;
        }
    }

    std::cout << "Enter date (YYYY-MM-DD format): ";
    std::string date;
    std::getline(std::cin, date);

    std::cout << "Enter posting text: ";
    std::string postingText;
    std::getline(std::cin, postingText);

    std::cout << "Enter target account number: ";
    int targetAccount = getNumericInput();

    std::cout << "Enter original account number: ";
    int originalAccount = getNumericInput();

    std::cout << "Enter posting amount (in cents): ";
    int postingAmount = getNumericInput();

    // Check balance before proceeding with the transaction
    std::vector<PostingAccount> postingAccounts = PostingAccount::loadPostingAccountsFromFile();
    int targetAccountBalance = 0;
    for (const PostingAccount& account : postingAccounts) {
        if (account.getNumber() == targetAccount) {
            targetAccountBalance = getBalanceForAccount(targetAccount);
            break;
        }
    }

    // Check if the transaction would result in a negative balance
    if (targetAccountBalance + postingAmount < 0) {
        std::cout << "Error: Transaction would result in a negative balance for the target account!" << std::endl;
        return;
    }

    PostingRecord newRecord(bookingNumber, date, postingText, targetAccount, originalAccount, postingAmount);
    newRecord.saveToFile();
    std::cout << "New posting record created successfully!" << std::endl;
    saveBookingToAccountFile(targetAccount, bookingNumber);
    saveBookingToAccountFile(originalAccount, bookingNumber);

}
void displayDetailedView() {
    std::cout << "Enter booking number to view details: ";
    int bookingNumber = getNumericInput();

    std::vector<PostingRecord> records = PostingRecord::loadPostingRecordsFromFile();
    bool found = false;
    for (const PostingRecord& record : records) {
        if (record.getBookingNumber() == bookingNumber) {
            // Display all details of the record
            std::cout << "Booking Number: " << record.getBookingNumber() << std::endl;
            std::cout << "Date: " << record.getDate() << std::endl;
            std::cout << "Text: " << record.getPostingText() << std::endl;
            std::cout << "Target Account: " << record.getTargetAccount() << std::endl;
            std::cout << "Original Account: " << record.getOriginalAccount() << std::endl;
            std::cout << "Amount: " << record.getPostingAmount() << std::endl;
            found = true;
            break;
        }
    }
    if (!found) {
        std::cout << "No records found for booking number " << bookingNumber << "." << std::endl;
    }
}
// Expansion of listAllPostingRecordsForAccount
void listAllPostingRecordsForAccount(int accountNumber) {
    std::string filename = "account_" + std::to_string(accountNumber) + ".csv";
    std::ifstream file(filename);
    if (!file) {
        std::cout << "Error reading account file." << std::endl;
        return;
    }

    std::vector<int> bookingNumbers;
    int bookingNum;
    while (file >> bookingNum) {
        bookingNumbers.push_back(bookingNum);
    }
    file.close();

    std::vector<PostingRecord> allRecords = PostingRecord::loadPostingRecordsFromFile();
    std::vector<PostingRecord> relevantRecords;
    for (const int& num : bookingNumbers) {
        for (const PostingRecord& record : allRecords) {
            if (record.getBookingNumber() == num) {
                relevantRecords.push_back(record);
                break;
            }
        }
    }

    // Now display these relevantRecords in tabular form
    for (const PostingRecord& record : relevantRecords) {
        std::cout << record.getBookingNumber() << " - "
            << record.getDate() << " - "
            << record.getPostingText() << " - "
            << record.getPostingAmount() << std::endl;
    }
}

void initializeDefaultAccountTypes() {
    std::ofstream file("account_types.csv", std::ios::app); // Append mode
    file << "1,Stock" << std::endl;
    file << "6,Expense" << std::endl;
    file << "8,EBK" << std::endl;
    file.close();
}
void initializeDefaultPostingAccounts() {
    std::ofstream file("posting_accounts.csv", std::ios::app); 
    file << "1000,Cash" << std::endl;
    file << "1001,Credit card" << std::endl;
    file << "6000,Hotel" << std::endl;
    file << "6001,Food" << std::endl;
    file << "6002,Fuel" << std::endl;
    file << "6003,Souvenirs" << std::endl;
    file << "6004,Clothing" << std::endl;
    file << "6005,Entrance fee" << std::endl;
    file.close();
}
void initializeJournal() {
    std::ofstream file("journal.csv", std::ios::app); // Append mode
    file << "1, ,Opening entry cash,0,0,500" << std::endl;
    file << "2, ,Opening entry EC card,0,0,5000" << std::endl;
    file.close();
}

void displayMainMenu() {
    std::cout << std::endl;
    std::cout << "Welcome to the Banking System" << std::endl;
    std::cout << "1. Create new account type" << std::endl;
    std::cout << "2. List of all account types" << std::endl;
    std::cout << "3. Create new account" << std::endl;
    std::cout << "4. List of all accounts" << std::endl;
    std::cout << "5. Create new posting record" << std::endl;
    std::cout << "6. List of all posting records" << std::endl;
    std::cout << "7. Search for posting records" << std::endl;
    std::cout << "8. View details of a specific posting record" << std::endl;
    std::cout << "9. List all posting records for an account" << std::endl;
    std::cout << "10. Exit" << std::endl;
    std::cout << "Enter your choice: ";
}
int main() {
    std::ifstream ifile("journal.csv");
    if (!ifile) { // If file doesn't exist
        initializeJournal();
    }
    std::vector<AccountType> accountTypes = AccountType::loadAccountTypesFromFile();
    std::vector<PostingAccount> postingAccounts = PostingAccount::loadPostingAccountsFromFile();

    if (accountTypes.size() < 3) {
        std::cout << "Initializing default account types..." << std::endl;
        initializeDefaultAccountTypes();
        accountTypes = AccountType::loadAccountTypesFromFile();  // reload the account types
    }

    if (postingAccounts.size() < 8) {
        std::cout << "Initializing default posting accounts..." << std::endl;
        initializeDefaultPostingAccounts();
        postingAccounts = PostingAccount::loadPostingAccountsFromFile();  // reload the posting accounts
    }


    int choice;
    do {
        displayMainMenu();
        choice = getNumericInput(); // Use the getNumericInput function to ensure valid input
        switch (choice) {
        case 1:
            createNewAccountType();
            break;
        case 2:
            listAllAccountTypes();
            break;
        case 3:
            createNewAccount();
            break;
        case 4:
            listAllAccounts();
            break;
        case 5:
            createNewPostingRecord();
            break;
        case 6:
            listAllPostingRecords();
            break;
        case 7:
            searchPostingRecords();
            break;
        case 8:
            displayDetailedView();
            break;
        case 9:
            std::cout << "Enter the account number for which you want to view all posting records: ";
            int accNum;
            std::cin >> accNum; 
            listAllPostingRecordsForAccount(accNum);
            break;

            break;
        case 10:
            std::cout << "Exiting..." << std::endl;
            break;
        default:
            std::cout << "Invalid choice. Please try again." << std::endl;
            break;
        }
    } while (choice != 10);

    return 0;
}
