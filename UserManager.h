#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

class User {
public:
    enum class Role { Admin = 1, Reader = 2, Guest = 3 };

    // ���캯��
    User(const string& username, const string& password, Role role)
        : username_(username), password_(password), role_(role) {}
    User() {}

    // ��ȡ
    string GetUsername() const { return username_; }
    string GetPassword() const { return password_; }
    Role GetRole() const { return role_; }

    // ����
    void SetPassword(const string& password) { password_ = password; }
    void SetRole(Role newRole) { role_ = newRole; }

private:
    string username_;
    string password_;
    Role role_;
};

class UserManager {
public:
    // ������û�
    bool AddUser(const string& username, const string& password, User::Role role) {
        if (users_.find(username) != users_.end()) {
            return false; // �û��Ѵ���
        }
        users_[username] = User(username, password, role);
        return true;
    }

    // �û���֤
    bool Authenticate(const string& username, const string& password) {
        auto it = users_.find(username);
        if (it == users_.end()) {
            return false; // �û�δ�ҵ�
        }
        return it->second.GetPassword() == password;
    }
    //�����û�
    void LoadUsersFromFile(const string& filename) {
        ifstream file(filename);
        string line;

        if (!file.is_open()) {
            cerr << "Error opening file: " << filename << endl;
            return;
        }

        try {
            while (getline(file, line)) {
                stringstream ss(line);
                string username, password, roleStr;
                getline(ss, username, ',');
                getline(ss, password, ',');
                getline(ss, roleStr);

                // ������ɫ
                User::Role role = User::Role::Guest; // Ĭ��ֵ
                if (roleStr == "����Ա") {
                    role = User::Role::Admin;
                }
                else if (roleStr == "����") {
                    role = User::Role::Reader;
                }

                // ����û�
                AddUser(username, password, role);
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
    //�洢�û�
    void SaveUsers(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            cerr << "Error opening file for writing: " << filename << endl;
            return;
        }

        try {
            for (const auto& userPair : users_) {
                const User& user = userPair.second;
                file << user.GetUsername() << ","
                    << user.GetPassword() << ","
                    << RoleToString(user.GetRole()) << "\n"; // ʹ���ַ�����ʾ�Ľ�ɫ
            }
        }
        catch (const ofstream::failure& e) {
            cerr << "Exception writing to file: " << e.what() << endl;
        }

        file.close();
    }



    // �����û�����
    bool ChangePassword(const string& username, const string& oldPassword, const string& newPassword) {
        auto it = users_.find(username);
        if (it == users_.end() || it->second.GetPassword() != oldPassword) {
            return false; // �û�δ�ҵ�������벻��ȷ
        }
        it->second.SetPassword(newPassword);
        return true;
    }

    // ɾ���û�
    bool DeleteUser(const string& username, const string& filename) {
        // ����û��Ƿ����
        auto it = users_.find(username);
        if (it == users_.end()) {
            return false; // �û�������
        }

        // ���ڴ���ɾ���û�
        users_.erase(it);

        // �����ļ�
        ofstream file(filename, ios::trunc); // ���ļ���ɾ��ԭ������
        if (!file.is_open()) {
            cerr << "Error opening file for writing: " << filename << endl;
            return false;
        }

        try {
            // ��ʣ���û�д���ļ�
            for (const auto& userPair : users_) {
                const User& user = userPair.second;
                file << user.GetUsername() << ","
                    << user.GetPassword() << ","
                    << RoleToString(user.GetRole()) << "\n";
            }
        }
        catch (const ofstream::failure& e) {
            cerr << "Exception writing to file: " << e.what() << endl;
            file.close();
            return false;
        }

        file.close();
        return true;
    }

    // �޸��û���ɫ
    bool ModifyUserRole(const string& username, User::Role newRole) {
        auto it = users_.find(username);
        if (it != users_.end()) {
            it->second.SetRole(newRole);
            return true;
        }
        return false;
    }

    // �����û�����
    bool ResetPassword(const string& username, const string& filename) {
        auto it = users_.find(username);
        if (it != users_.end()) {
            it->second.SetPassword("123456"); // ����������ΪĬ��ֵ
            SaveUsers(filename); // ������ĵ��ļ�
            return true;
        }
        return false;
    }

    // �����Ը����û�����
    bool ChangeUserPassword(const string& username, const string& newPassword, const string& filename) {
        auto it = users_.find(username);
        if (it != users_.end()) {
            it->second.SetPassword(newPassword);
            SaveUsers(filename); // ������ĵ��ļ�
            return true;
        }
        return false;
    }

    // ����Ա�г������û�
    void ListUsers() const {
    vector<string> readers;
    vector<string> admins;

    // ���û����ݽ�ɫ����
    for (const auto& userPair : users_) {
        if (userPair.second.GetRole() == User::Role::Admin) {
            admins.push_back(userPair.first);
        } else if (userPair.second.GetRole() == User::Role::Reader) {
            readers.push_back(userPair.first);
        }
    }

    // �г����й���Ա
    cout << "����Ա�б�:" << endl;
    for (const auto& username : admins) {
        cout << "�û���: " << username << endl;
    }

    // �г����ж���
    cout << "\n�����б�:" << endl;
    for (const auto& username : readers) {
        cout << "�û���: " << username << endl;
    }
}

    // ��ȡ�û��Ľ�ɫ
    User::Role GetUserRole(const string& username) const {
        auto it = users_.find(username);
        if (it != users_.end()) {
            return it->second.GetRole();
        }
        return User::Role::Guest; // ����û������ڣ�����Ĭ�Ͻ�ɫ
    }

    // ��ȡ�û���ϸ��Ϣ�ķ���
    void GetUserDetails(const string& username, const LibrarySystem& librarySystem) const {
        auto userIt = users_.find(username);
        if (userIt != users_.end()) {
            const User& user = userIt->second;
            cout << "�û���: " << username << endl;
            cout << "��ɫ: " << RoleToString(user.GetRole()) << endl;
            
            // ����Ƕ��ߣ���ʾ����ļ�¼
            if (user.GetRole() == User::Role::Reader) {
                librarySystem.DisplayUserBorrowHistory(username);
            }
        } else {
            cout << "�û� " << username << " ������" << endl;
        }
    }

private:
    unordered_map<string, User> users_; // �û����� User �����ӳ��
    string RoleToString(User::Role role) const {
        switch (role) {
        case User::Role::Admin:
            return "����Ա";
        case User::Role::Reader:
            return "����";
        case User::Role::Guest:
            return "�ο�";
        default:
            return "δ֪��ɫ";
        }
    }
};
