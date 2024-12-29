#pragma once
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include <string>
#include <unordered_map>
#include "Book.h" 

using namespace std;
//定义一个借阅记录的结构
struct BorrowRecord {
    string username;
    string bookISBN;
    bool isReturned;
    BorrowRecord() {};
    BorrowRecord(const string& username, const string& bookISBN, bool isReturned = false)
        : username(username), bookISBN(bookISBN), isReturned(isReturned) {}
};


//清屏
void ClearScreen();
class LibrarySystem {
public:
    // 新增的公共成员函数，用于打印已加载的书籍数量
    void PrintBookCount() const {
        cout << "Currently loaded books: " << books_.size() << endl;
    }

    // 添加书籍到图书馆
    void AddBook(const Book& book) {
        books_[book.GetISBN()] = book; // 使用 ISBN 作为键
        bookAvailability_[book.GetISBN()] = true; // 书籍初始可用
        }

    // 添加书籍到图书馆和文件
    void AddBookToFile(const Book& book, const string& filename) {
        if (books_.find(book.GetISBN()) == books_.end()) { // 检查书籍是否已存在
            AddBook(book); // 添加到图书馆

            // 将书籍信息写入文件
            ofstream file(filename, ios::app); // 以追加模式打开文件
            if (file.is_open()) {
                file << book.ToString() << endl; // ToString 方法应返回完整的书籍信息字符串
                file.close(); // 关闭文件
                cout << "恭喜你，您成功的又将一本书添加进了图书馆。" << endl;
            }
            else {
                cerr << "无法打开文件：" << filename << endl;
            }
        }
        else {
            cout << "书籍已存在，不会重复添加。" << endl;
        }
    }

    // 通过 ISBN 删除书籍并直接在原文件上进行修改
    bool DeleteBookByISBNAndUpdateFileDirectly(const string& isbn, const string& filename) {
        auto it = books_.find(isbn);
        if (it != books_.end()) {
            // 从内存中删除书籍
            books_.erase(it);
            bookAvailability_.erase(isbn);

            // 读取整个文件内容到内存中
            vector<string> lines;
            string line;
            ifstream inFile(filename);
            if (!inFile.is_open()) {
                cerr << "无法打开文件：" << filename << endl;
                return false;
            }

            while (getline(inFile, line)) {
                if (line.find(isbn) == string::npos) {
                    lines.push_back(line);
                }
            }
            inFile.close();

            // 重新以写入模式打开文件
            ofstream outFile(filename, ios::trunc);
            if (!outFile.is_open()) {
                cerr << "无法写入文件：" << filename << endl;
                return false;
            }

            // 将除了被删除书籍的所有内容写回文件
            for (const auto& ln : lines) {
                outFile << ln << endl;
            }

            outFile.close();
            cout << "书籍已删除，并更新了文件。" << endl;
            return true;
        }
        else {
            cout << "书籍未找到，无法删除。" << endl;
            return false;
        }
    }
       //存储借阅记录
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
                    << bookName << "," // 添加书名
                    << (record.isReturned ? "已归还" : "未归还") << "\n";
            }
        }
        catch (const ofstream::failure& e) {
            cerr << "Exception writing to file: " << e.what() << endl;
        }

        file.close();
    }

    //加载借阅记录
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
                getline(ss, bookName, ','); // 获取书名
                getline(ss, returnedStr);

                bool isReturned = (returnedStr == "已归还");
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


    // 借出书籍
    bool BorrowBook(const string& isbn, const string& username) {
        // 检查书籍是否存在并且是可借的
        if (books_.find(isbn) != books_.end() && bookAvailability_[isbn]) {
            bookAvailability_[isbn] = false; // 设置书籍为不可借

            // 更新用户的借阅记录
            if (userBorrowRecords_.find(username) == userBorrowRecords_.end()) {
                // 如果用户不存在于借阅记录中，则为其创建一个新条目
                userBorrowRecords_[username] = vector<string>();
            }
            userBorrowRecords_[username].push_back(isbn);

            // 添加借阅记录到 borrowRecords_
            BorrowRecord newRecord(username, isbn, false);
            borrowRecords_.push_back(newRecord);

            return true;
        }
        return false; // 如果书籍不存在或不可借，则返回 false
    }


    // 归还书籍
    bool ReturnBook(const string& isbn, const string& username) {
        if (books_.find(isbn) == books_.end()) {
            cout << "书籍不存在: ISBN=" << isbn << endl;
            return false;
        }

        if (userBorrowRecords_.find(username) == userBorrowRecords_.end()) {
            cout << "用户未找到: " << username << endl;
            return false;
        }

        auto& borrowedBooks = userBorrowRecords_[username];
        auto it = find(borrowedBooks.begin(), borrowedBooks.end(), isbn);

        if (it == borrowedBooks.end()) {
            cout << "用户未借阅此书: ISBN=" << isbn << endl;
            return false;
        }

        // 找到最近一次尚未归还的借阅记录
        auto recordIt = find_if(borrowRecords_.rbegin(), borrowRecords_.rend(),
            [&username, &isbn](const BorrowRecord& record) {
                return record.username == username && record.bookISBN == isbn && !record.isReturned;
            });

        if (recordIt != borrowRecords_.rend()) {
            recordIt->isReturned = true; // 标记为已归还
            bookAvailability_[isbn] = true; // 书籍变为可借
            borrowedBooks.erase(it); // 移除用户当前借阅列表中的这本书
            cout << "书籍已归还: ISBN=" << isbn << endl;
            return true;
        }
        else {
            cout << "未找到未归还的借阅记录: ISBN=" << isbn << endl;
            return false;
        }
    }





    // 展示图书馆中的所有书籍
    void DisplayAllBooks() const {
        for (const auto& bookPair : books_) {
            const Book& book = bookPair.second;
            book.Display(); //  Book 类有一个更新后的展示方法
            cout << (bookAvailability_.at(book.GetID()) ? " (可借)" : " (已借出)") << endl;
        }
    }

    // 通过书名搜索书籍
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

    // 通过作者搜索书籍
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

    // 通过 ISBN 搜索书籍
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

    // 作者模糊搜索
    vector<Book> SearchBooksByAuthorFuzzy(const string& author) const {
        vector<Book> foundBooks;
        for (const auto& bookPair : books_) {
            const Book& book = bookPair.second;
            if (book.GetAuthor().find(author) != string::npos) {
                foundBooks.push_back(book);
            }
        }
        // 对结果进行字典序排序
        sort(foundBooks.begin(), foundBooks.end(),
            [](const Book& a, const Book& b) {
                return a.GetAuthor() < b.GetAuthor();
            });
        return foundBooks;
    }
    //出版社模糊搜索
    vector<Book> SearchBooksByPublisherFuzzy(const string& publisher) const {
        vector<Book> foundBooks;
        for (const auto& bookPair : books_) {
            const Book& book = bookPair.second;
            if (book.GetPublishing().find(publisher) != string::npos) {
                foundBooks.push_back(book);
            }
        }
        sort(foundBooks.begin(), foundBooks.end(), [](const Book& a, const Book& b) {
            return a.GetPublishing() < b.GetPublishing(); // 字典序排序
            });
        return foundBooks;
    }

    // 检查书籍是否可用
    bool IsBookAvailable(const string& isbn) const {
        auto it = bookAvailability_.find(isbn);
        if (it != bookAvailability_.end()) {
            return it->second;
        }
        return false; // 如果书籍不存在
    }

    


    // 展示用户的借阅历史
    void DisplayUserBorrowHistory(const string& username, int recordsPerPage = 5) const {
        vector<BorrowRecord> userRecords;
        for (const auto& record : borrowRecords_) {
            if (record.username == username) {
                userRecords.push_back(record);
            }
        }

        int totalRecords = userRecords.size();
        if (totalRecords == 0) {
            cout << "该用户目前没有借阅记录。" << endl;
            system("pause");
            return;
        }

        int totalPages = totalRecords / recordsPerPage + (totalRecords % recordsPerPage != 0);
        int currentPage = 1;
        char choice;

        do {
            int startIndex = (currentPage - 1) * recordsPerPage;
            int endIndex = min(startIndex + recordsPerPage, totalRecords);

            cout << "借阅历史 - 第 " << currentPage << " 页 / 共 " << totalPages << " 页" << endl;
            for (int i = startIndex; i < endIndex; ++i) {
                const auto& record = userRecords[i];
                auto bookIt = books_.find(record.bookISBN);
                if (bookIt != books_.end()) {
                    const Book& book = bookIt->second;
                    book.Display();
                    cout << " - 书籍状态: " << (record.isReturned ? "已归还" : "未归还") << endl;
                }
            }

            // 分页控制
            cout << "\n[N] 下一页, [P] 上一页, [R] 返回菜单: ";
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



    // 展示最受欢迎的借阅书籍
    void DisplayTopBorrowedBooks() {
        const int BOOKS_PER_PAGE = 5;
        map<string, int> borrowCount;

        // 使用 borrowRecords_ 来统计每本书的借阅次数
        for (const auto& record : borrowRecords_) {
            borrowCount[record.bookISBN]++;
        }

        vector<pair<string, int>> borrowVec(borrowCount.begin(), borrowCount.end());
        sort(borrowVec.begin(), borrowVec.end(), [](const pair<string, int>& a, const pair<string, int>& b) {
            return a.second > b.second; // 按借阅次数降序排列
            });

        // 计算总页数
        int totalBooks = borrowVec.size();
        int totalPages = totalBooks / BOOKS_PER_PAGE + (totalBooks % BOOKS_PER_PAGE == 0 ? 0 : 1);

        auto currentPageStart = borrowVec.begin();
        int currentPage = 1;
        while (true) {
            cout << "当前页: " << currentPage << " / " << totalPages << endl;

            auto& it = currentPageStart;
            for (int count = 0; count < BOOKS_PER_PAGE && it != borrowVec.end(); ++count, ++it) {
                auto bookIt = books_.find(it->first);
                if (bookIt != books_.end()) {
                    const Book& book = bookIt->second;
                    book.Display();
                    cout << " - 借阅次数: " << it->second << " 次" << endl;
                }
            }

            char choice;
            cout << "\n[N] 下一页, [P] 上一页, [Q] 退出: ";
            cin >> choice;

            if (choice == 'Q' || choice == 'q') {
                break;
            }
            else if (choice == 'P' || choice == 'p') {
                if (currentPage > 1) {
                    --currentPage;
                    advance(currentPageStart, -BOOKS_PER_PAGE);
                    ClearScreen(); // 清屏
                }
                else {
                    
                    ClearScreen(); // 清屏
                    cout << "已经是第一页了" << '\n';
                }
            }
            else if (choice == 'N' || choice == 'n') {
                if (currentPage < totalPages) {
                    ++currentPage;
                    currentPageStart = it;
                    ClearScreen(); // 清屏
                }
                else {
                    
                    ClearScreen(); // 清屏
                    cout << "已经是最后一页了" << '\n';
                }
            }
           
        }
    }





    // 管理员列出所有书籍
    void ListBooks() const {
        vector<pair<string, Book>> sortedBooks(books_.begin(), books_.end());

        // 按 ID 排序
        sort(sortedBooks.begin(), sortedBooks.end(), [](const auto& a, const auto& b) {
            return a.first < b.first; // a.first 和 b.first 分别是 map 的键，即书籍的 ID
            });

        cout << "图书馆书籍清单（按 ID 排序）:" << endl;

        // 以下是分页显示逻辑
        int totalBooks = sortedBooks.size();
        int totalPages = totalBooks / BOOKS_PER_PAGE + (totalBooks % BOOKS_PER_PAGE == 0 ? 0 : 1);

        auto currentPageStart = sortedBooks.begin();

        int currentPage = 1;
        while (true) {
            cout << "当前页: " << currentPage << " / " << totalPages << endl;

            auto& it = currentPageStart;
            for (int count = 0; count < BOOKS_PER_PAGE && it != sortedBooks.end(); ++count, ++it) {
                const Book& book = it->second;
                book.Display();
                if (bookAvailability_.find(book.GetISBN()) != bookAvailability_.end()) {
                    cout << (bookAvailability_.at(book.GetISBN()) ? " (可借)" : " (已借出)") << endl;
                }
                else {
                    cout << " (信息不可用)" << endl;
                }
            }

            char choice;
            cout << "\n[N] 下一页, [P] 上一页, [Q] 退出: ";
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
            ClearScreen(); // 清屏
        }
    }



    


    
    



private:
    unordered_map<string, Book> books_; // 使用书籍的 ISBN 来映射 Book 对象
    unordered_map<string, bool> bookAvailability_; // 使用书籍的 ISBN 来映射其可借性状态
    unordered_map<string, vector<string>> userBorrowRecords_; // 使用用户名来映射其借阅的书籍 ISBN 列表
    vector<BorrowRecord> borrowRecords_; // 存储借阅记录
    static const int BOOKS_PER_PAGE = 3;  // 每页显示的书籍数量
};
