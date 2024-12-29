#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <unordered_map>
#include "Book.h" 

using namespace std;
//����һ�����ļ�¼�Ľṹ
struct BorrowRecord {
    string username;
    string bookISBN;
    bool isReturned;
    BorrowRecord() {};
    BorrowRecord(const string& username, const string& bookISBN, bool isReturned = false)
        : username(username), bookISBN(bookISBN), isReturned(isReturned) {}
};


//����
void ClearScreen();
class LibrarySystem {
public:
    // �����Ĺ�����Ա���������ڴ�ӡ�Ѽ��ص��鼮����
    void PrintBookCount() const {
        cout << "Currently loaded books: " << books_.size() << endl;
    }

    // ����鼮��ͼ���
    void AddBook(const Book& book) {
        books_[book.GetISBN()] = book; // ʹ�� ISBN ��Ϊ��
        bookAvailability_[book.GetISBN()] = true; // �鼮��ʼ����
        }

    // ����鼮��ͼ��ݺ��ļ�
    void AddBookToFile(const Book& book, const string& filename) {
        if (books_.find(book.GetISBN()) == books_.end()) { // ����鼮�Ƿ��Ѵ���
            AddBook(book); // ��ӵ�ͼ���

            // ���鼮��Ϣд���ļ�
            ofstream file(filename, ios::app); // ��׷��ģʽ���ļ�
            if (file.is_open()) {
                file << book.ToString() << endl; // ToString ����Ӧ�����������鼮��Ϣ�ַ���
                file.close(); // �ر��ļ�
                cout << "��ϲ�㣬���ɹ����ֽ�һ������ӽ���ͼ��ݡ�" << endl;
            }
            else {
                cerr << "�޷����ļ���" << filename << endl;
            }
        }
        else {
            cout << "�鼮�Ѵ��ڣ������ظ���ӡ�" << endl;
        }
    }

    // ͨ�� ISBN ɾ���鼮��ֱ����ԭ�ļ��Ͻ����޸�
    bool DeleteBookByISBNAndUpdateFileDirectly(const string& isbn, const string& filename) {
        auto it = books_.find(isbn);
        if (it != books_.end()) {
            // ���ڴ���ɾ���鼮
            books_.erase(it);
            bookAvailability_.erase(isbn);

            // ��ȡ�����ļ����ݵ��ڴ���
            vector<string> lines;
            string line;
            ifstream inFile(filename);
            if (!inFile.is_open()) {
                cerr << "�޷����ļ���" << filename << endl;
                return false;
            }

            while (getline(inFile, line)) {
                if (line.find(isbn) == string::npos) {
                    lines.push_back(line);
                }
            }
            inFile.close();

            // ������д��ģʽ���ļ�
            ofstream outFile(filename, ios::trunc);
            if (!outFile.is_open()) {
                cerr << "�޷�д���ļ���" << filename << endl;
                return false;
            }

            // �����˱�ɾ���鼮����������д���ļ�
            for (const auto& ln : lines) {
                outFile << ln << endl;
            }

            outFile.close();
            cout << "�鼮��ɾ�������������ļ���" << endl;
            return true;
        }
        else {
            cout << "�鼮δ�ҵ����޷�ɾ����" << endl;
            return false;
        }
    }
       //�洢���ļ�¼
    void SaveBorrowRecords(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }

        try {
            for (const auto& record : borrowRecords_) {
                string isbn = record.bookISBN;
                if (books_.find(isbn) == books_.end()) {
                    cout << "Warning: ISBN not found in books_: " << isbn << endl;
                }
                string bookName = (books_.find(isbn) != books_.end()) ? books_.at(isbn).GetName() : "Unknown";
                file << record.username << ","
                    << isbn << ","
                    << bookName << "," // �������
                    << (record.isReturned ? "�ѹ黹" : "δ�黹") << "\n";
            }
        }
        catch (const ofstream::failure& e) {
            cerr << "Exception writing to file: " << e.what() << endl;
        }

        file.close();
    }

    //���ؽ��ļ�¼
    void LoadBorrowRecords(const string& filename) {
        ifstream file(filename);
        string line;

        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        try {
            while (getline(file, line)) {
                stringstream ss(line);
                string username, isbn, bookName, returnedStr;
                getline(ss, username, ',');
                getline(ss, isbn, ',');
                getline(ss, bookName, ','); // ��ȡ����
                getline(ss, returnedStr);

                bool isReturned = (returnedStr == "�ѹ黹");
                BorrowRecord record{ username, isbn, isReturned };
                borrowRecords_.push_back(record);

                if (!isReturned) {
                    userBorrowRecords_[username].push_back(isbn);
                }
            }
        }
        catch (const ifstream::failure& e) {
            cerr << "Exception reading file: " << e.what() << endl;
        }
        catch (const exception& e) {
            cerr << "Exception: " << e.what() << endl;
        }

        file.close();
    }


    // ����鼮
    bool BorrowBook(const string& isbn, const string& username) {
        // ����鼮�Ƿ���ڲ����ǿɽ��
        if (books_.find(isbn) != books_.end() && bookAvailability_[isbn]) {
            bookAvailability_[isbn] = false; // �����鼮Ϊ���ɽ�

            // �����û��Ľ��ļ�¼
            if (userBorrowRecords_.find(username) == userBorrowRecords_.end()) {
                // ����û��������ڽ��ļ�¼�У���Ϊ�䴴��һ������Ŀ
                userBorrowRecords_[username] = vector<string>();
            }
            userBorrowRecords_[username].push_back(isbn);

            // ��ӽ��ļ�¼�� borrowRecords_
            BorrowRecord newRecord(username, isbn, false);
            borrowRecords_.push_back(newRecord);

            return true;
        }
        return false; // ����鼮�����ڻ򲻿ɽ裬�򷵻� false
    }


    // �黹�鼮
    bool ReturnBook(const string& isbn, const string& username) {
        if (books_.find(isbn) == books_.end()) {
            cout << "�鼮������: ISBN=" << isbn << endl;
            return false;
        }

        if (userBorrowRecords_.find(username) == userBorrowRecords_.end()) {
            cout << "�û�δ�ҵ�: " << username << endl;
            return false;
        }

        auto& borrowedBooks = userBorrowRecords_[username];
        auto it = find(borrowedBooks.begin(), borrowedBooks.end(), isbn);

        if (it == borrowedBooks.end()) {
            cout << "�û�δ���Ĵ���: ISBN=" << isbn << endl;
            return false;
        }

        // �ҵ����һ����δ�黹�Ľ��ļ�¼
        auto recordIt = find_if(borrowRecords_.rbegin(), borrowRecords_.rend(),
            [&username, &isbn](const BorrowRecord& record) {
                return record.username == username && record.bookISBN == isbn && !record.isReturned;
            });

        if (recordIt != borrowRecords_.rend()) {
            recordIt->isReturned = true; // ���Ϊ�ѹ黹
            bookAvailability_[isbn] = true; // �鼮��Ϊ�ɽ�
            borrowedBooks.erase(it); // �Ƴ��û���ǰ�����б��е��Ȿ��
            cout << "�鼮�ѹ黹: ISBN=" << isbn << endl;
            return true;
        }
        else {
            cout << "δ�ҵ�δ�黹�Ľ��ļ�¼: ISBN=" << isbn << endl;
            return false;
        }
    }





    // չʾͼ����е������鼮
    void DisplayAllBooks() const {
        for (const auto& bookPair : books_) {
            const Book& book = bookPair.second;
            book.Display(); //  Book ����һ�����º��չʾ����
            cout << (bookAvailability_.at(book.GetID()) ? " (�ɽ�)" : " (�ѽ��)") << endl;
        }
    }

    // ͨ�����������鼮
    vector<Book> SearchBooksByName(const string& name) const {
        vector<Book> foundBooks;
        for (const auto& bookPair : books_) {
            const Book& book = bookPair.second;
            if (book.GetName() == name) {
                foundBooks.push_back(book);
            }
        }
        return foundBooks;
    }

    // ͨ�����������鼮
    vector<Book> SearchBooksByAuthor(const string& author) const {
        vector<Book> foundBooks;
        for (const auto& bookPair : books_) {
            const Book& book = bookPair.second;
            if (book.GetAuthor() == author) {
                foundBooks.push_back(book);
            }
        }
        return foundBooks;
    }

    // ͨ�� ISBN �����鼮
    vector<Book> SearchBooksByISBN(const string& isbn) const {
        vector<Book> foundBooks;
        for (const auto& bookPair : books_) {
            const Book& book = bookPair.second;
            if (book.GetISBN() == isbn) {
                foundBooks.push_back(book);
            }
        }
        return foundBooks;
    }

    // ����ģ������
    vector<Book> SearchBooksByAuthorFuzzy(const string& author) const {
        vector<Book> foundBooks;
        for (const auto& bookPair : books_) {
            const Book& book = bookPair.second;
            if (book.GetAuthor().find(author) != string::npos) {
                foundBooks.push_back(book);
            }
        }
        // �Խ�������ֵ�������
        sort(foundBooks.begin(), foundBooks.end(),
            [](const Book& a, const Book& b) {
                return a.GetAuthor() < b.GetAuthor();
            });
        return foundBooks;
    }
    //������ģ������
    vector<Book> SearchBooksByPublisherFuzzy(const string& publisher) const {
        vector<Book> foundBooks;
        for (const auto& bookPair : books_) {
            const Book& book = bookPair.second;
            if (book.GetPublishing().find(publisher) != string::npos) {
                foundBooks.push_back(book);
            }
        }
        sort(foundBooks.begin(), foundBooks.end(), [](const Book& a, const Book& b) {
            return a.GetPublishing() < b.GetPublishing(); // �ֵ�������
            });
        return foundBooks;
    }

    // ����鼮�Ƿ����
    bool IsBookAvailable(const string& isbn) const {
        auto it = bookAvailability_.find(isbn);
        if (it != bookAvailability_.end()) {
            return it->second;
        }
        return false; // ����鼮������
    }

    


    // չʾ�û��Ľ�����ʷ
    void DisplayUserBorrowHistory(const string& username, int recordsPerPage = 5) const {
        vector<BorrowRecord> userRecords;
        for (const auto& record : borrowRecords_) {
            if (record.username == username) {
                userRecords.push_back(record);
            }
        }

        int totalRecords = userRecords.size();
        if (totalRecords == 0) {
            cout << "���û�Ŀǰû�н��ļ�¼��" << endl;
            system("pause");
            return;
        }

        int totalPages = totalRecords / recordsPerPage + (totalRecords % recordsPerPage != 0);
        int currentPage = 1;
        char choice;

        do {
            int startIndex = (currentPage - 1) * recordsPerPage;
            int endIndex = min(startIndex + recordsPerPage, totalRecords);

            cout << "������ʷ - �� " << currentPage << " ҳ / �� " << totalPages << " ҳ" << endl;
            for (int i = startIndex; i < endIndex; ++i) {
                const auto& record = userRecords[i];
                auto bookIt = books_.find(record.bookISBN);
                if (bookIt != books_.end()) {
                    const Book& book = bookIt->second;
                    book.Display();
                    cout << " - �鼮״̬: " << (record.isReturned ? "�ѹ黹" : "δ�黹") << endl;
                }
            }

            // ��ҳ����
            cout << "\n[N] ��һҳ, [P] ��һҳ, [R] ���ز˵�: ";
            cin >> choice;

            if (choice == 'N' || choice == 'n') {
                if (currentPage < totalPages) currentPage++;
            }
            else if (choice == 'P' || choice == 'p') {
                if (currentPage > 1) currentPage--;
            }
            ClearScreen();
            
        } while (choice != 'R' && choice != 'r');
    }



    // չʾ���ܻ�ӭ�Ľ����鼮
    void DisplayTopBorrowedBooks() {
        const int BOOKS_PER_PAGE = 5;
        map<string, int> borrowCount;

        // ʹ�� borrowRecords_ ��ͳ��ÿ����Ľ��Ĵ���
        for (const auto& record : borrowRecords_) {
            borrowCount[record.bookISBN]++;
        }

        vector<pair<string, int>> borrowVec(borrowCount.begin(), borrowCount.end());
        sort(borrowVec.begin(), borrowVec.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second; // �����Ĵ�����������
            });

        // ������ҳ��
        int totalBooks = borrowVec.size();
        int totalPages = totalBooks / BOOKS_PER_PAGE + (totalBooks % BOOKS_PER_PAGE == 0 ? 0 : 1);

        auto currentPageStart = borrowVec.begin();
        int currentPage = 1;
        while (true) {
            cout << "��ǰҳ: " << currentPage << " / " << totalPages << endl;

            auto& it = currentPageStart;
            for (int count = 0; count < BOOKS_PER_PAGE && it != borrowVec.end(); ++count, ++it) {
                auto bookIt = books_.find(it->first);
                if (bookIt != books_.end()) {
                    const Book& book = bookIt->second;
                    book.Display();
                    cout << " - ���Ĵ���: " << it->second << " ��" << endl;
                }
            }

            char choice;
            cout << "\n[N] ��һҳ, [P] ��һҳ, [Q] �˳�: ";
            cin >> choice;

            if (choice == 'Q' || choice == 'q') {
                break;
            }
            else if (choice == 'P' || choice == 'p') {
                if (currentPage > 1) {
                    --currentPage;
                    advance(currentPageStart, -BOOKS_PER_PAGE);
                    ClearScreen(); // ����
                }
                else {
                    
                    ClearScreen(); // ����
                    cout << "�Ѿ��ǵ�һҳ��" << '\n';
                }
            }
            else if (choice == 'N' || choice == 'n') {
                if (currentPage < totalPages) {
                    ++currentPage;
                    currentPageStart = it;
                    ClearScreen(); // ����
                }
                else {
                    
                    ClearScreen(); // ����
                    cout << "�Ѿ������һҳ��" << '\n';
                }
            }
           
        }
    }





    // ����Ա�г������鼮
    void ListBooks() const {
        vector<pair<string, Book>> sortedBooks(books_.begin(), books_.end());

        // �� ID ����
        sort(sortedBooks.begin(), sortedBooks.end(), [](const auto& a, const auto& b) {
            return a.first < b.first; // a.first �� b.first �ֱ��� map �ļ������鼮�� ID
            });

        cout << "ͼ����鼮�嵥���� ID ����:" << endl;

        // �����Ƿ�ҳ��ʾ�߼�
        int totalBooks = sortedBooks.size();
        int totalPages = totalBooks / BOOKS_PER_PAGE + (totalBooks % BOOKS_PER_PAGE == 0 ? 0 : 1);

        auto currentPageStart = sortedBooks.begin();

        int currentPage = 1;
        while (true) {
            cout << "��ǰҳ: " << currentPage << " / " << totalPages << endl;

            auto& it = currentPageStart;
            for (int count = 0; count < BOOKS_PER_PAGE && it != sortedBooks.end(); ++count, ++it) {
                const Book& book = it->second;
                book.Display();
                if (bookAvailability_.find(book.GetISBN()) != bookAvailability_.end()) {
                    cout << (bookAvailability_.at(book.GetISBN()) ? " (�ɽ�)" : " (�ѽ��)") << endl;
                }
                else {
                    cout << " (��Ϣ������)" << endl;
                }
            }

            char choice;
            cout << "\n[N] ��һҳ, [P] ��һҳ, [Q] �˳�: ";
            cin >> choice;

            if (choice == 'Q' || choice == 'q') {
                break;
            }
            else if (choice == 'P' || choice == 'p') {
                if (currentPage > 1) {
                    --currentPage;
                    advance(currentPageStart, -BOOKS_PER_PAGE);
                }
            }
            else if (choice == 'N' || choice == 'n') {
                if (currentPage < totalPages) {
                    ++currentPage;
                    currentPageStart = it;
                }
            }
            ClearScreen(); // ����
        }
    }



    


    
    



private:
    unordered_map<string, Book> books_; // ʹ���鼮�� ISBN ��ӳ�� Book ����
    unordered_map<string, bool> bookAvailability_; // ʹ���鼮�� ISBN ��ӳ����ɽ���״̬
    unordered_map<string, vector<string>> userBorrowRecords_; // ʹ���û�����ӳ������ĵ��鼮 ISBN �б�
    vector<BorrowRecord> borrowRecords_; // �洢���ļ�¼
    static const int BOOKS_PER_PAGE = 3;  // ÿҳ��ʾ���鼮����
};
