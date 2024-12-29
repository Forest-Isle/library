#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <cstdlib> // ���� system �������ڵ�ͷ�ļ�
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
//�������ݼ��е�ͼ��
void InitializeLibraryFromDataset(const string& filename, LibrarySystem& library) {
    ifstream file(filename);  // ʹ��Ĭ�ϱ�����ļ�
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

// ��ҳ��ʾ����
void DisplayBooksInPages(const vector<Book>& books, const LibrarySystem& library, int booksPerPage, int currentPage) {
    int totalBooks = books.size();
    int startIndex = (currentPage - 1) * booksPerPage;
    int endIndex = min(startIndex + booksPerPage, totalBooks);

    for (int i = startIndex; i < endIndex; ++i) {
        const Book& book = books[i];
        book.Display();
        cout << " - ����״̬: " << (library.IsBookAvailable(book.GetISBN()) ? "�ɽ�" : "�ѽ��") << endl;
    }

    // ��ʾ��ҳ��Ϣ
    cout << "Page " << currentPage << " of " << (totalBooks / booksPerPage + (totalBooks % booksPerPage != 0)) << endl;
}



void DisplayMenu(User::Role currentRole) {
    if (currentRole == User::Role::Admin) {
        cout << "����Ա�˵���:\n";
        cout << "1. ����鼮\n";
        cout << "2. ɾ���鼮\n";
        cout << "3. �г����ж���\n";
        cout << "4. �޸��û���ɫ\n";
        cout << "5. �鿴�������а�\n";
        cout << "6. �г������鼮\n";
        cout << "7. ���ö�������\n";
        cout << "8. ����û�\n";
        cout << "9. ɾ���û�\n";
        cout << "10. �����û�����\n";
        cout << "11. �鿴�û�\n";
        cout << "12. �˳���¼\n";
    }
    else if (currentRole == User::Role::Reader) {
        cout << "���߲˵���:\n";
        cout << "1. ����\n";
        cout << "2. ����\n";
        cout << "3. ����\n";
        cout << "4. �鿴������ʷ\n";
        cout << "5. �޸�����\n";
        cout << "6. �˳�\n";
    }
    else if (currentRole == User::Role::Guest) {
        cout << "�οͲ˵���:\n";
        cout << "1. ע��\n";
        cout << "2. ��¼\n";
        cout << "3. ����\n";
        cout << "4. �˳�\n";
    }
}

//����
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
    User::Role currentRole = User::Role::Guest; // Ĭ�Ͻ�ɫΪ�ο�

    // ����һ������Ա�˻�
    string adminUsername = "admin";
    string adminPassword = "123"; // Ӧ��ѡ��һ������ȫ������
    User::Role adminRole = User::Role::Admin;

    // ������Ա�˻��Ƿ��Ѵ���
    if (!userManager.Authenticate(adminUsername, adminPassword)) {
        // ��������ڣ�����ӹ���Ա�˻�
        userManager.AddUser(adminUsername, adminPassword, adminRole);
        cout << "Admin account created." << endl;
    }
    else {
        cout << "Admin account already exists." << endl;
    }

    // �������ݼ�
    InitializeLibraryFromDataset("bookdata.txt", library);
    library.PrintBookCount();

    // ���ؽ��ļ�¼
    library.LoadBorrowRecords("borrowRecords.txt");

    // �����û�����
    userManager.LoadUsersFromFile("userData.txt");

    while (true) {
        cout << "��ӭ����ͼ��ݹ���ϵͳ����ѡ���¼��ݣ�" << endl;
        cout << "1. �ο͵�¼" << endl;
        cout << "2. ���ߵ�¼" << endl;
        cout << "3. ����Ա��¼" << endl;
        cout << "4. �˳�ϵͳ" << endl;
        cout << "��ѡ��1-4��: ";
        cin >> command;
        ClearScreen(); // ������������

        if (command == "1") {
            currentRole = User::Role::Guest;
        }
        else if (command == "2") {
            cout << "����������û���: ";
            cin >> username;
            cout << "����������: ";
            cin >> password;
            if (userManager.Authenticate(username, password)) {
                cout << "��¼�ɹ���" << endl;
                currentRole = userManager.GetUserRole(username);
            }
            else {
                cout << "�û������������" << endl;
                cout << "\n����������ز˵�..." << endl;
                cin.ignore();
                cin.get(); // �ȴ��û�����
                ClearScreen(); // ������������
                continue;
            }
            cout << "\n����������ز˵�..." << endl;
            cin.ignore();
            cin.get(); // �ȴ��û�����
            ClearScreen(); // ������������
        }
        else if (command == "3") {
            cout << "���������Ա�û���: ";
            cin >> username;
            cout << "����������: ";
            cin >> password;
            if (userManager.Authenticate(username, password) && userManager.GetUserRole(username) == User::Role::Admin) {
                cout << "����Ա��¼�ɹ���" << endl;
                currentRole = User::Role::Admin;
            }
            else {
                cout << "�û�����������󣬻��ǹ���Ա��" << endl;
                cout << "\n����������ز˵�..." << endl;
                cin.ignore();
                cin.get(); // �ȴ��û�����
                ClearScreen(); // ������������
                continue;
            }
            cout << "\n����������ز˵�..." << endl;
            cin.ignore();
            cin.get(); // �ȴ��û�����
            ClearScreen(); // ������������
        }
        else if (command == "4") {
            cout << "��лʹ��ͼ��ݹ���ϵͳ���ټ���" << endl;
            break;
        }
        else {
            cout << "��Ч��ѡ�����������룡" << endl;
            cout << "\n����������ز˵�..." << endl;
            cin.ignore();
            cin.get(); // �ȴ��û�����
            ClearScreen(); // ������������
            continue;
        }
        while (true) {
            ClearScreen(); // ������������
            DisplayMenu(currentRole);
            cout << "��ѡ����������: ";
            cin >> command;
            ClearScreen(); // ������������
            if (currentRole == User::Role::Guest) {

                if (command == "1") {
                    // ע���߼�
                    string username, password;
                    cout << "�������û���: ";
                    cin >> username;
                    cout << "����������: ";
                    cin >> password;
                    if (userManager.AddUser(username, password, User::Role::Reader)) {
                        cout << "��ϲ��ɹ�ע�ᣡ" << endl;
                    }
                    else {
                        cout << "ע��ʧ�ܣ��Ѿ�������û��ˡ�" << endl;
                    }
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.get(); // �ȴ��û�����

                }
                else if (command == "2") {
                    // ��¼�߼�
                    string username, password;
                    cout << "�������û���: ";
                    cin >> username;
                    cout << "����������: ";
                    cin >> password;
                    if (userManager.Authenticate(username, password)) {
                        cout << "��ӭ�ص�ͼ���" << endl;
                        // ���õ�ǰ�û���ɫ
                        currentRole = userManager.GetUserRole(username);
                    }
                    else {
                        cout << "��¼ʧ�ܡ�" << endl;
                    }
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.get(); // �ȴ��û�����

                }
                else if (command == "3") {
                    // �����鼮�߼�
                    string choice, searchTerm;
                    cout << "ѡ���������� (1: ����, 2: ISBN/ISSN, 3: ����, 4: ������): ";
                    cin >> choice;
                    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ���Ի��з�
                    cout << "����������: ";
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
                        // ��һ����������ģ�������ķ���
                        foundBooks = library.SearchBooksByPublisherFuzzy(searchTerm);
                    }

                    // ��ҳ��ʾ�߼�
                    int currentPage = 1;
                    int booksPerPage = 3; // ��������Ϊ��ϣ����ÿҳ��ʾ���鼮����
                    char nextPage;

                    do {
                        ClearScreen(); // ����������ʾ�µ�һҳ����
                        DisplayBooksInPages(foundBooks, library, booksPerPage, currentPage);
                        cout << "��ʾ��һҳ? (y/n): ";
                        cin >> nextPage;
                        if (nextPage == 'y' || nextPage == 'Y') {
                            if ((static_cast<unsigned long long>(currentPage) - 1) * booksPerPage < foundBooks.size()) {
                                currentPage++;
                            }
                            else {
                                cout << "�Ѿ������һҳ�ˡ�" << endl;
                            }
                        }
                    } while ((nextPage == 'y' || nextPage == 'Y') && (static_cast<unsigned long long>(currentPage) - 1) * booksPerPage < foundBooks.size());

                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                
                    cin.get(); // �ȴ��û�����


                }
                else if (command == "4") {
                    break;
                }
                else {
                    cout << "δ֪�����Ȩ�޲��㡣" << endl;
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.get(); // �ȴ��û�����

                }
                cin.get(); // �ȴ��û�����

            }

            //����Ա����
            else if (currentRole == User::Role::Admin) {

                if (command == "1") {
                    // ����Ա����鼮�߼�
                    string id, isbn, name, author, publisher, publishedDate, description;
                    double price;
                    int pages;

                    cout << "������ ID: ";
                    cin >> id;
                    cin.ignore(); // Ignore newline character
                    cout << "����������: ";
                    getline(cin, name);
                    cout << "������ ISBN: ";
                    getline(cin, isbn);
                    cout << "����������: ";
                    getline(cin, author);
                    cout << "�����������: ";
                    getline(cin, publisher);
                    cout << "�����뷢������: ";
                    getline(cin, publishedDate);
                    cout << "�������鼮�۸�: ";
                    cin >> price;
                    cin.ignore();
                    cout << "�������鼮ҳ��: ";
                    cin >> pages;
                    cin.ignore();
                    //cin.ignore(); // Ignore newline character
                    cout << "�������鼮��������: ";
                    getline(cin, description);

                    Book newBook(id, isbn, name, author, publisher, publishedDate, price, pages, description);
                    // ����鼮��ͼ��ݺ��ļ�
                    library.AddBookToFile(newBook, "bookdata.txt"); // �����������������ļ�·��
                    
                    // �ȴ��û������������
                    //cout << "\n����������ز˵�..." << endl;
                    //cin.get(); // �ȴ��û�����
                    system("pause");
                }

                else if (command == "2") {
                    // ����Աɾ���鼮�߼�
                    string isbn;
                    cout << "����������Ҫɾ�����鼮��ISBN: ";
                    cin >> isbn;
                    if (library.DeleteBookByISBNAndUpdateFileDirectly(isbn, "bookdata.txt")) { // ɾ���鼮�������ļ�
                        cout << "�鼮�ѳɹ�ɾ����" << endl;
                    }
                    else {
                        cout << "��Ŷ��ɾ����������ͼ��ݺ���û���Ȿ�顣" << endl;
                    }
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.ignore();
                    cin.get(); // �ȴ��û�����
                }

                //��ʾ�������а�

                else if (command == "5") {

                    library.DisplayTopBorrowedBooks();


                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.ignore();
                    cin.get(); // �ȴ��û�����

                }
                else if (command == "8") {
                    // ����û��߼�
                    string newUsername, newPassword;
                    int role;
                    cout << "�������û����û���: ";
                    cin >> newUsername;
                    cout << "�������û�������: ";
                    cin >> newPassword;
                    cout << "�������û��Ľ�ɫ (1: ����Ա, 2: ����): ";
                    cin >> role;
                    if (userManager.AddUser(newUsername, newPassword, static_cast<User::Role>(role))) {
                        cout << "�û���ӳɹ���\n";
                    }
                    else {
                        cout << "�û����ʧ�ܣ������û����Ѵ��ڡ�\n";
                    }
                    // �����û�����
                    userManager.SaveUsers("userData.txt");
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.ignore();
                    cin.get(); // �ȴ��û�����
                }
                else if (command == "9") {
                    // ɾ���û��߼�
                    string username;
                    cout << "����Ҫɾ�����û����û���: ";
                    cin >> username;
                    if (userManager.DeleteUser(username, "userData.txt")) {
                        cout << "�û�ɾ���ɹ���\n";
                    }
                    else {
                        cout << "�û�ɾ��ʧ�ܣ������û������ڡ�\n";
                    }
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.ignore();
                    cin.get(); // �ȴ��û�����
                }
                else if (command == "3") {
                    // ����Ա�г������û�
                    userManager.ListUsers();
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.ignore();
                    cin.get(); // �ȴ��û�����

                }
                else if (command == "4") {
                    // ����Ա�޸��û���ɫ�߼�
                    string targetUsername;
                    int newRole;
                    cout << "�������޸Ľ�ɫ���û���: ";
                    cin >> targetUsername;
                    cout << "�������µĽ�ɫ (1 for Admin, 2 for Reader, 3 for Guest): ";
                    cin >> newRole;

                    if (userManager.ModifyUserRole(targetUsername, static_cast<User::Role>(newRole))) {
                        cout << "�û���ɫ���³ɹ���" << endl;
                    }
                    else {
                        cout << "�޷������û���ɫ���û����ܲ����ڡ�" << endl;
                    }
                    // �����û�����
                    userManager.SaveUsers("userData.txt");
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.ignore();
                    cin.get(); // �ȴ��û�����

                }

                else if (command == "7") {
                    string targetUsername;
                    cout << "��������Ҫ����������û���: ";
                    cin >> targetUsername;
                    cin.ignore();
                    if (userManager.ResetPassword(targetUsername, "userData.txt")) {
                        cout << "����������ΪĬ��ֵ��123456����" << endl;
                    }
                    else {
                        cout << "��������ʧ�ܣ��Ҳ����û���" << endl;
                    }
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.get(); // �ȴ��û�����
                }
                else if (command == "10") {
                    // �����û������߼�
                    string username, newPassword;
                    cout << "����Ҫ����������û����û���: ";
                    cin >> username;
                    cout << "�����µ�����: ";
                    cin >> newPassword;
                    if (userManager.ChangeUserPassword(username, newPassword, "userData.txt")) {
                        cout << "������ĳɹ���\n";
                    }
                    else {
                        cout << "�������ʧ�ܣ������û������ڡ�\n";
                    }
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.ignore();
                    cin.get(); // �ȴ��û�����
                }
                else if (command == "11") {
                    // ��ȡ�û���ϸ��Ϣ���߼�
                    string username;
                    cout << "����Ҫ��ѯ���û���: ";
                    cin >> username;
                    userManager.GetUserDetails(username, library); // �����·���
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.ignore();
                    cin.get(); // �ȴ��û�����
                }
                else if (command == "12") {
                    //�˳���¼
                    break;
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.get(); // �ȴ��û�����

                }
                //����Ա�г������鼮
                else if (command == "6") {
                    library.ListBooks();
                }
                else {
                    cout << "δ֪�����Ȩ�޲��㡣" << endl;
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.get(); // �ȴ��û�����

                }
            }

            else if (currentRole == User::Role::Reader) {
                //�����޸�����

                if (command == "5") {
                    string oldPassword, newPassword;
                    cout << "�����뵱ǰ����: ";
                    cin >> oldPassword;
                    cout << "������������: ";
                    cin >> newPassword;

                    if (userManager.ChangePassword(username, oldPassword, newPassword)) {
                        cout << "�����޸ĳɹ���" << endl;
                    }
                    else {
                        cout << "�����޸�ʧ�ܣ���ȷ����������ȷ��" << endl;
                    }
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.ignore();
                    cin.get(); // �ȴ��û�����
                }
                else if (command == "2") {
                    // �����߼�
                    string isbn;
                    cout << "��������ISBN: ";
                    cin >> isbn;
                    // ���ȼ���鼮�Ƿ񱻽��
                    if (!library.IsBookAvailable(isbn)) {
                        cout << "��Ǹ���Ȿ���Ѿ��������ˡ�" << endl;
                    }
                    else {
                        // ����鼮δ����������Խ���
                        if (library.BorrowBook(isbn, username)) {
                            cout << "ͼ����ĳɹ���" << endl;
                        }
                        else {
                            cout << "Ŷ���Ȿ��Ŀǰ���ɽ��ġ�" << endl;
                        }
                    }
                    //������ļ�¼
                    library.SaveBorrowRecords("borrowRecords.txt");
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.ignore();
                    cin.get(); // �ȴ��û�����

                }

                else if (command == "3") {
                    // �����߼�
                    string isbn;
                    cout << "��������ISBN: ";
                    cin >> isbn;
                    cin.ignore();
                    if (library.ReturnBook(isbn, username)) {
                        cout << "����ɹ���" << endl;
                    }
                    else {
                        cout << "����ʧ�ܡ�" << endl;
                    }
                    //������ļ�¼
                    library.SaveBorrowRecords("borrowRecords.txt");
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.get(); // �ȴ��û�����

                }
                else if (command == "1") {
                    // �����鼮�߼�
                    string choice, searchTerm;
                    cout << "ѡ���������� (1: ����, 2: ISBN/ISSN, 3: ����, 4: ������): ";
                    cin >> choice;
                    cin.ignore();
                    cout << "����������: ";
                    
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
                        // ��һ����������ģ�������ķ���
                        foundBooks = library.SearchBooksByPublisherFuzzy(searchTerm);
                    }

                    // ��ҳ��ʾ�߼�
                    int currentPage = 1;
                    int booksPerPage = 3; // ��������Ϊ��ϣ����ÿҳ��ʾ���鼮����
                    char nextPage;

                    do {
                        ClearScreen(); // ����������ʾ�µ�һҳ����
                        DisplayBooksInPages(foundBooks, library, booksPerPage, currentPage);
                        cout << "��ʾ��һҳ? (y/n): ";
                        cin >> nextPage;
                        if (nextPage == 'y' || nextPage == 'Y') {
                            if ((static_cast<unsigned long long>(currentPage) - 1) * booksPerPage < foundBooks.size()) {
                                currentPage++;
                            }
                            else {
                                cout << "�Ѿ������һҳ�ˡ�" << endl;
                            }
                        }
                    } while ((nextPage == 'y' || nextPage == 'Y') && (static_cast<unsigned long long>(currentPage) - 1) * booksPerPage < foundBooks.size());


                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    
                    cin.get(); // �ȴ��û�����


                }

                else if (command == "4") {
                    // �鿴������ʷ�߼�
                    library.DisplayUserBorrowHistory(username);
                    // ���ﲻ����Ҫ�ȴ��û����������������Ϊ��ҳ�߼��Ѱ����� DisplayUserBorrowHistory ��
                    cout << "�������˵�..." << endl;
                    }


                else if (command == "6") {
                    //�˳���¼
                    break;
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.get(); // �ȴ��û�����

                }
                else {
                    cout << "δ֪�����Ȩ�޲��㡣" << endl;
                    // �ȴ��û������������
                    cout << "\n����������ز˵�..." << endl;
                    cin.get(); // �ȴ��û�����

                }
            }
        }
    }
    
    

    return 0;
}
