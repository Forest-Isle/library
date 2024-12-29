#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib> // 引入 system 函数所在的头文件
#include "Book.h"          
#include "LibrarySystem.h" 
#include "UserManager.h"   
using namespace std;

string trim(const std::string& str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}
//导入数据集中的图书
void InitializeLibraryFromDataset(const string& filename, LibrarySystem& library) {
    ifstream file(filename);  // 使用默认编码打开文件
    string line;

    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

   

    while (getline(file, line)) {
        stringstream ss(line);
        string id, isbn, name, author, publishing, publishedDate, priceStr, pagesStr, description;
        double price;
        int pages;

        getline(ss, id, ',');
        isbn = trim(isbn);
        getline(ss, isbn, ',');
        isbn = trim(isbn);
        getline(ss, name, ',');
        isbn = trim(isbn);
        getline(ss, author, ',');
        isbn = trim(isbn);
        getline(ss, publishing, ',');
        isbn = trim(isbn);
        getline(ss, publishedDate, ',');
        isbn = trim(isbn);
        getline(ss, priceStr, ',');
        isbn = trim(isbn);
        getline(ss, pagesStr, ',');
        isbn = trim(isbn);
        getline(ss, description);

        try {
            if (!priceStr.empty()) {
                price = stod(priceStr);
            }
            else {
                price = 0.0; // or an appropriate default value
            }
            pages = stoi(pagesStr);
        }
        catch (const exception& e) {
            cerr << "Error parsing line: " << line << "\nError: " << e.what() << endl;
            continue;
        }

        Book book(id, isbn, name, author, publishing, publishedDate, price, pages, description);
        library.AddBook(book);
    }
   
}

// 分页显示函数
void DisplayBooksInPages(const vector<Book>& books, const LibrarySystem& library, int booksPerPage, int currentPage) {
    int totalBooks = books.size();
    int startIndex = (currentPage - 1) * booksPerPage;
    int endIndex = min(startIndex + booksPerPage, totalBooks);

    for (int i = startIndex; i < endIndex; ++i) {
        const Book& book = books[i];
        book.Display();
        cout << " - 借阅状态: " << (library.IsBookAvailable(book.GetISBN()) ? "可借" : "已借出") << endl;
    }

    // 显示分页信息
    cout << "Page " << currentPage << " of " << (totalBooks / booksPerPage + (totalBooks % booksPerPage != 0)) << endl;
}



void DisplayMenu(User::Role currentRole) {
    if (currentRole == User::Role::Admin) {
        cout << "管理员菜单栏:\n";
        cout << "1. 添加书籍\n";
        cout << "2. 删除书籍\n";
        cout << "3. 列出所有读者\n";
        cout << "4. 修改用户角色\n";
        cout << "5. 查看借阅排行榜\n";
        cout << "6. 列出所有书籍\n";
        cout << "7. 重置读者密码\n";
        cout << "8. 添加用户\n";
        cout << "9. 删除用户\n";
        cout << "10. 更改用户密码\n";
        cout << "11. 查看用户\n";
        cout << "12. 退出登录\n";
    }
    else if (currentRole == User::Role::Reader) {
        cout << "读者菜单栏:\n";
        cout << "1. 找书\n";
        cout << "2. 借书\n";
        cout << "3. 还书\n";
        cout << "4. 查看借阅历史\n";
        cout << "5. 修改密码\n";
        cout << "6. 退出\n";
    }
    else if (currentRole == User::Role::Guest) {
        cout << "游客菜单栏:\n";
        cout << "1. 注册\n";
        cout << "2. 登录\n";
        cout << "3. 找书\n";
        cout << "4. 退出\n";
    }
}

//清屏
void ClearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}




int main() {
    LibrarySystem library;
    UserManager userManager;
    string command, username, password;
    User::Role currentRole = User::Role::Guest; // 默认角色为游客

    // 创建一个管理员账户
    string adminUsername = "admin";
    string adminPassword = "123"; // 应该选择一个更安全的密码
    User::Role adminRole = User::Role::Admin;

    // 检查管理员账户是否已存在
    if (!userManager.Authenticate(adminUsername, adminPassword)) {
        // 如果不存在，则添加管理员账户
        userManager.AddUser(adminUsername, adminPassword, adminRole);
        cout << "Admin account created." << endl;
    }
    else {
        cout << "Admin account already exists." << endl;
    }

    // 加载数据集
    InitializeLibraryFromDataset("bookdata.txt", library);
    library.PrintBookCount();

    // 加载借阅记录
    library.LoadBorrowRecords("borrowRecords.txt");

    // 加载用户数据
    userManager.LoadUsersFromFile("userData.txt");

    while (true) {
        cout << "欢迎来到图书馆管理系统！请选择登录身份：" << endl;
        cout << "1. 游客登录" << endl;
        cout << "2. 读者登录" << endl;
        cout << "3. 管理员登录" << endl;
        cout << "4. 退出系统" << endl;
        cout << "请选择（1-4）: ";
        cin >> command;
        ClearScreen(); // 调用清屏函数

        if (command == "1") {
            currentRole = User::Role::Guest;
        }
        else if (command == "2") {
            cout << "请输入读者用户名: ";
            cin >> username;
            cout << "请输入密码: ";
            cin >> password;
            if (userManager.Authenticate(username, password)) {
                cout << "登录成功！" << endl;
                currentRole = userManager.GetUserRole(username);
            }
            else {
                cout << "用户名或密码错误！" << endl;
                cout << "\n按任意键返回菜单..." << endl;
                cin.ignore();
                cin.get(); // 等待用户输入
                ClearScreen(); // 调用清屏函数
                continue;
            }
            cout << "\n按任意键返回菜单..." << endl;
            cin.ignore();
            cin.get(); // 等待用户输入
            ClearScreen(); // 调用清屏函数
        }
        else if (command == "3") {
            cout << "请输入管理员用户名: ";
            cin >> username;
            cout << "请输入密码: ";
            cin >> password;
            if (userManager.Authenticate(username, password) && userManager.GetUserRole(username) == User::Role::Admin) {
                cout << "管理员登录成功！" << endl;
                currentRole = User::Role::Admin;
            }
            else {
                cout << "用户名或密码错误，或不是管理员！" << endl;
                cout << "\n按任意键返回菜单..." << endl;
                cin.ignore();
                cin.get(); // 等待用户输入
                ClearScreen(); // 调用清屏函数
                continue;
            }
            cout << "\n按任意键返回菜单..." << endl;
            cin.ignore();
            cin.get(); // 等待用户输入
            ClearScreen(); // 调用清屏函数
        }
        else if (command == "4") {
            cout << "感谢使用图书馆管理系统！再见！" << endl;
            break;
        }
        else {
            cout << "无效的选择，请重新输入！" << endl;
            cout << "\n按任意键返回菜单..." << endl;
            cin.ignore();
            cin.get(); // 等待用户输入
            ClearScreen(); // 调用清屏函数
            continue;
        }
        while (true) {
            ClearScreen(); // 调用清屏函数
            DisplayMenu(currentRole);
            cout << "请选择您的需求: ";
            cin >> command;
            ClearScreen(); // 调用清屏函数
            if (currentRole == User::Role::Guest) {

                if (command == "1") {
                    // 注册逻辑
                    string username, password;
                    cout << "请输入用户名: ";
                    cin >> username;
                    cout << "请输入密码: ";
                    cin >> password;
                    if (userManager.AddUser(username, password, User::Role::Reader)) {
                        cout << "恭喜你成功注册！" << endl;
                    }
                    else {
                        cout << "注册失败，已经有这个用户了。" << endl;
                    }
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.get(); // 等待用户输入

                }
                else if (command == "2") {
                    // 登录逻辑
                    string username, password;
                    cout << "请输入用户名: ";
                    cin >> username;
                    cout << "请输入密码: ";
                    cin >> password;
                    if (userManager.Authenticate(username, password)) {
                        cout << "欢迎回到图书馆" << endl;
                        // 设置当前用户角色
                        currentRole = userManager.GetUserRole(username);
                    }
                    else {
                        cout << "登录失败。" << endl;
                    }
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.get(); // 等待用户输入

                }
                else if (command == "3") {
                    // 搜索书籍逻辑
                    string choice, searchTerm;
                    cout << "选择搜索类型 (1: 题名, 2: ISBN/ISSN, 3: 作者, 4: 出版社): ";
                    cin >> choice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 忽略换行符
                    cout << "输入搜索词: ";
                    //cin.ignore();
                    getline(cin, searchTerm);
                    vector<Book> foundBooks;
                    if (choice == "1") {
                        foundBooks = library.SearchBooksByName(searchTerm);
                    }
                    else if (choice == "2") {
                        foundBooks = library.SearchBooksByISBN(searchTerm);
                    }
                    else if (choice == "3") {
                        foundBooks = library.SearchBooksByAuthorFuzzy(searchTerm);
                    }
                    else if (choice == "4") {
                        // 有一个按出版社模糊搜索的方法
                        foundBooks = library.SearchBooksByPublisherFuzzy(searchTerm);
                    }

                    // 分页显示逻辑
                    int currentPage = 1;
                    int booksPerPage = 3; // 可以设置为您希望的每页显示的书籍数量
                    char nextPage;

                    do {
                        ClearScreen(); // 清屏，以显示新的一页内容
                        DisplayBooksInPages(foundBooks, library, booksPerPage, currentPage);
                        cout << "显示下一页? (y/n): ";
                        cin >> nextPage;
                        if (nextPage == 'y' || nextPage == 'Y') {
                            if ((static_cast<unsigned long long>(currentPage) - 1) * booksPerPage < foundBooks.size()) {
                                currentPage++;
                            }
                            else {
                                cout << "已经是最后一页了。" << endl;
                            }
                        }
                    } while ((nextPage == 'y' || nextPage == 'Y') && (static_cast<unsigned long long>(currentPage) - 1) * booksPerPage < foundBooks.size());

                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                
                    cin.get(); // 等待用户输入


                }
                else if (command == "4") {
                    break;
                }
                else {
                    cout << "未知命令或权限不足。" << endl;
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.get(); // 等待用户输入

                }
                cin.get(); // 等待用户输入

            }

            //管理员功能
            else if (currentRole == User::Role::Admin) {

                if (command == "1") {
                    // 管理员添加书籍逻辑
                    string id, isbn, name, author, publisher, publishedDate, description;
                    double price;
                    int pages;

                    cout << "请输入 ID: ";
                    cin >> id;
                    cin.ignore(); // Ignore newline character
                    cout << "请输入书名: ";
                    getline(cin, name);
                    cout << "请输入 ISBN: ";
                    getline(cin, isbn);
                    cout << "请输入作者: ";
                    getline(cin, author);
                    cout << "请输入出版社: ";
                    getline(cin, publisher);
                    cout << "请输入发行日期: ";
                    getline(cin, publishedDate);
                    cout << "请输入书籍价格: ";
                    cin >> price;
                    cin.ignore();
                    cout << "请输入书籍页数: ";
                    cin >> pages;
                    cin.ignore();
                    //cin.ignore(); // Ignore newline character
                    cout << "请输入书籍基本概述: ";
                    getline(cin, description);

                    Book newBook(id, isbn, name, author, publisher, publishedDate, price, pages, description);
                    // 添加书籍到图书馆和文件
                    library.AddBookToFile(newBook, "bookdata.txt"); // 假设这是您的数据文件路径
                    
                    // 等待用户按任意键继续
                    //cout << "\n按任意键返回菜单..." << endl;
                    //cin.get(); // 等待用户输入
                    system("pause");
                }

                else if (command == "2") {
                    // 管理员删除书籍逻辑
                    string isbn;
                    cout << "请输入您想要删除的书籍的ISBN: ";
                    cin >> isbn;
                    if (library.DeleteBookByISBNAndUpdateFileDirectly(isbn, "bookdata.txt")) { // 删除书籍并更新文件
                        cout << "书籍已成功删除。" << endl;
                    }
                    else {
                        cout << "啊哦，删不掉，咱们图书馆好像没有这本书。" << endl;
                    }
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.ignore();
                    cin.get(); // 等待用户输入
                }

                //显示借阅排行榜

                else if (command == "5") {

                    library.DisplayTopBorrowedBooks();


                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.ignore();
                    cin.get(); // 等待用户输入

                }
                else if (command == "8") {
                    // 添加用户逻辑
                    string newUsername, newPassword;
                    int role;
                    cout << "输入新用户的用户名: ";
                    cin >> newUsername;
                    cout << "输入新用户的密码: ";
                    cin >> newPassword;
                    cout << "输入新用户的角色 (1: 管理员, 2: 读者): ";
                    cin >> role;
                    if (userManager.AddUser(newUsername, newPassword, static_cast<User::Role>(role))) {
                        cout << "用户添加成功。\n";
                    }
                    else {
                        cout << "用户添加失败，可能用户名已存在。\n";
                    }
                    // 保存用户数据
                    userManager.SaveUsers("userData.txt");
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.ignore();
                    cin.get(); // 等待用户输入
                }
                else if (command == "9") {
                    // 删除用户逻辑
                    string username;
                    cout << "输入要删除的用户的用户名: ";
                    cin >> username;
                    if (userManager.DeleteUser(username, "userData.txt")) {
                        cout << "用户删除成功。\n";
                    }
                    else {
                        cout << "用户删除失败，可能用户不存在。\n";
                    }
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.ignore();
                    cin.get(); // 等待用户输入
                }
                else if (command == "3") {
                    // 管理员列出所有用户
                    userManager.ListUsers();
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.ignore();
                    cin.get(); // 等待用户输入

                }
                else if (command == "4") {
                    // 管理员修改用户角色逻辑
                    string targetUsername;
                    int newRole;
                    cout << "请输入修改角色的用户名: ";
                    cin >> targetUsername;
                    cout << "请输入新的角色 (1 for Admin, 2 for Reader, 3 for Guest): ";
                    cin >> newRole;

                    if (userManager.ModifyUserRole(targetUsername, static_cast<User::Role>(newRole))) {
                        cout << "用户角色更新成功。" << endl;
                    }
                    else {
                        cout << "无法更新用户角色，用户可能不存在。" << endl;
                    }
                    // 保存用户数据
                    userManager.SaveUsers("userData.txt");
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.ignore();
                    cin.get(); // 等待用户输入

                }

                else if (command == "7") {
                    string targetUsername;
                    cout << "请输入需要重置密码的用户名: ";
                    cin >> targetUsername;
                    cin.ignore();
                    if (userManager.ResetPassword(targetUsername, "userData.txt")) {
                        cout << "密码已重置为默认值（123456）。" << endl;
                    }
                    else {
                        cout << "密码重置失败，找不到用户。" << endl;
                    }
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.get(); // 等待用户输入
                }
                else if (command == "10") {
                    // 更改用户密码逻辑
                    string username, newPassword;
                    cout << "输入要更改密码的用户的用户名: ";
                    cin >> username;
                    cout << "输入新的密码: ";
                    cin >> newPassword;
                    if (userManager.ChangeUserPassword(username, newPassword, "userData.txt")) {
                        cout << "密码更改成功。\n";
                    }
                    else {
                        cout << "密码更改失败，可能用户不存在。\n";
                    }
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.ignore();
                    cin.get(); // 等待用户输入
                }
                else if (command == "11") {
                    // 获取用户详细信息的逻辑
                    string username;
                    cout << "输入要查询的用户名: ";
                    cin >> username;
                    userManager.GetUserDetails(username, library); // 调用新方法
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.ignore();
                    cin.get(); // 等待用户输入
                }
                else if (command == "12") {
                    //退出登录
                    break;
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.get(); // 等待用户输入

                }
                //管理员列出所有书籍
                else if (command == "6") {
                    library.ListBooks();
                }
                else {
                    cout << "未知命令或权限不足。" << endl;
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.get(); // 等待用户输入

                }
            }

            else if (currentRole == User::Role::Reader) {
                //读者修改密码

                if (command == "5") {
                    string oldPassword, newPassword;
                    cout << "请输入当前密码: ";
                    cin >> oldPassword;
                    cout << "请输入新密码: ";
                    cin >> newPassword;

                    if (userManager.ChangePassword(username, oldPassword, newPassword)) {
                        cout << "密码修改成功！" << endl;
                    }
                    else {
                        cout << "密码修改失败，请确保旧密码正确。" << endl;
                    }
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.ignore();
                    cin.get(); // 等待用户输入
                }
                else if (command == "2") {
                    // 借书逻辑
                    string isbn;
                    cout << "输入该书的ISBN: ";
                    cin >> isbn;
                    // 首先检查书籍是否被借出
                    if (!library.IsBookAvailable(isbn)) {
                        cout << "抱歉，这本书已经被借走了。" << endl;
                    }
                    else {
                        // 如果书籍未被借出，尝试借书
                        if (library.BorrowBook(isbn, username)) {
                            cout << "图书借阅成功。" << endl;
                        }
                        else {
                            cout << "哦吼，这本书目前不可借阅。" << endl;
                        }
                    }
                    //保存借阅记录
                    library.SaveBorrowRecords("borrowRecords.txt");
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.ignore();
                    cin.get(); // 等待用户输入

                }

                else if (command == "3") {
                    // 还书逻辑
                    string isbn;
                    cout << "输入该书的ISBN: ";
                    cin >> isbn;
                    cin.ignore();
                    if (library.ReturnBook(isbn, username)) {
                        cout << "还书成功。" << endl;
                    }
                    else {
                        cout << "还书失败。" << endl;
                    }
                    //保存借阅记录
                    library.SaveBorrowRecords("borrowRecords.txt");
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.get(); // 等待用户输入

                }
                else if (command == "1") {
                    // 搜索书籍逻辑
                    string choice, searchTerm;
                    cout << "选择搜索类型 (1: 题名, 2: ISBN/ISSN, 3: 作者, 4: 出版社): ";
                    cin >> choice;
                    cin.ignore();
                    cout << "输入搜索词: ";
                    
                    getline(cin, searchTerm);

                    vector<Book> foundBooks;
                    if (choice == "1") {
                        foundBooks = library.SearchBooksByName(searchTerm);
                    }
                    else if (choice == "2") {
                        foundBooks = library.SearchBooksByISBN(searchTerm);
                    }
                    else if (choice == "3") {
                        foundBooks = library.SearchBooksByAuthorFuzzy(searchTerm);
                    }
                    else if (choice == "4") {
                        // 有一个按出版社模糊搜索的方法
                        foundBooks = library.SearchBooksByPublisherFuzzy(searchTerm);
                    }

                    // 分页显示逻辑
                    int currentPage = 1;
                    int booksPerPage = 3; // 可以设置为您希望的每页显示的书籍数量
                    char nextPage;

                    do {
                        ClearScreen(); // 清屏，以显示新的一页内容
                        DisplayBooksInPages(foundBooks, library, booksPerPage, currentPage);
                        cout << "显示下一页? (y/n): ";
                        cin >> nextPage;
                        if (nextPage == 'y' || nextPage == 'Y') {
                            if ((static_cast<unsigned long long>(currentPage) - 1) * booksPerPage < foundBooks.size()) {
                                currentPage++;
                            }
                            else {
                                cout << "已经是最后一页了。" << endl;
                            }
                        }
                    } while ((nextPage == 'y' || nextPage == 'Y') && (static_cast<unsigned long long>(currentPage) - 1) * booksPerPage < foundBooks.size());


                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    
                    cin.get(); // 等待用户输入


                }

                else if (command == "4") {
                    // 查看借阅历史逻辑
                    library.DisplayUserBorrowHistory(username);
                    // 这里不再需要等待用户按任意键继续，因为分页逻辑已包含在 DisplayUserBorrowHistory 中
                    cout << "返回主菜单..." << endl;
                    }


                else if (command == "6") {
                    //退出登录
                    break;
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.get(); // 等待用户输入

                }
                else {
                    cout << "未知命令或权限不足。" << endl;
                    // 等待用户按任意键继续
                    cout << "\n按任意键返回菜单..." << endl;
                    cin.get(); // 等待用户输入

                }
            }
        }
    }
    
    

    return 0;
}
