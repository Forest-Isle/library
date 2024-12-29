#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
using namespace std;

class User {
public:
    enum class Role { Admin = 1, Reader = 2, Guest = 3 };

    // 构造函数
    User(const string& username, const string& password, Role role)
        : username_(username), password_(password), role_(role) {}
    User() {}

    // 获取
    string GetUsername() const { return username_; }
    string GetPassword() const { return password_; }
    Role GetRole() const { return role_; }

    // 设置
    void SetPassword(const string& password) { password_ = password; }
    void SetRole(Role newRole) { role_ = newRole; }

private:
    string username_;
    string password_;
    Role role_;
};

class UserManager {
public:
    // 添加新用户
    bool AddUser(const string& username, const string& password, User::Role role) {
        if (users_.find(username) != users_.end()) {
            return false; // 用户已存在
        }
        users_[username] = User(username, password, role);
        return true;
    }

    // 用户认证
    bool Authenticate(const string& username, const string& password) {
        auto it = users_.find(username);
        if (it == users_.end()) {
            return false; // 用户未找到
        }
        return it->second.GetPassword() == password;
    }
    //加载用户
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

                // 解析角色
                User::Role role = User::Role::Guest; // 默认值
                if (roleStr == "管理员") {
                    role = User::Role::Admin;
                }
                else if (roleStr == "读者") {
                    role = User::Role::Reader;
                }

                // 添加用户
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
    //存储用户
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
                    << RoleToString(user.GetRole()) << "\n"; // 使用字符串表示的角色
            }
        }
        catch (const ofstream::failure& e) {
            cerr << "Exception writing to file: " << e.what() << endl;
        }

        file.close();
    }



    // 更改用户密码
    bool ChangePassword(const string& username, const string& oldPassword, const string& newPassword) {
        auto it = users_.find(username);
        if (it == users_.end() || it->second.GetPassword() != oldPassword) {
            return false; // 用户未找到或旧密码不正确
        }
        it->second.SetPassword(newPassword);
        return true;
    }

    // 删除用户
    bool DeleteUser(const string& username, const string& filename) {
        // 检查用户是否存在
        auto it = users_.find(username);
        if (it == users_.end()) {
            return false; // 用户不存在
        }

        // 从内存中删除用户
        users_.erase(it);

        // 更新文件
        ofstream file(filename, ios::trunc); // 打开文件，删除原有内容
        if (!file.is_open()) {
            cerr << "Error opening file for writing: " << filename << endl;
            return false;
        }

        try {
            // 将剩余用户写回文件
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

    // 修改用户角色
    bool ModifyUserRole(const string& username, User::Role newRole) {
        auto it = users_.find(username);
        if (it != users_.end()) {
            it->second.SetRole(newRole);
            return true;
        }
        return false;
    }

    // 重置用户密码
    bool ResetPassword(const string& username, const string& filename) {
        auto it = users_.find(username);
        if (it != users_.end()) {
            it->second.SetPassword("123456"); // 将密码重置为默认值
            SaveUsers(filename); // 保存更改到文件
            return true;
        }
        return false;
    }

    // 方法以更改用户密码
    bool ChangeUserPassword(const string& username, const string& newPassword, const string& filename) {
        auto it = users_.find(username);
        if (it != users_.end()) {
            it->second.SetPassword(newPassword);
            SaveUsers(filename); // 保存更改到文件
            return true;
        }
        return false;
    }

    // 管理员列出所有用户
    void ListUsers() const {
    vector<string> readers;
    vector<string> admins;

    // 将用户根据角色分组
    for (const auto& userPair : users_) {
        if (userPair.second.GetRole() == User::Role::Admin) {
            admins.push_back(userPair.first);
        } else if (userPair.second.GetRole() == User::Role::Reader) {
            readers.push_back(userPair.first);
        }
    }

    // 列出所有管理员
    cout << "管理员列表:" << endl;
    for (const auto& username : admins) {
        cout << "用户名: " << username << endl;
    }

    // 列出所有读者
    cout << "\n读者列表:" << endl;
    for (const auto& username : readers) {
        cout << "用户名: " << username << endl;
    }
}

    // 获取用户的角色
    User::Role GetUserRole(const string& username) const {
        auto it = users_.find(username);
        if (it != users_.end()) {
            return it->second.GetRole();
        }
        return User::Role::Guest; // 如果用户不存在，返回默认角色
    }

    // 获取用户详细信息的方法
    void GetUserDetails(const string& username, const LibrarySystem& librarySystem) const {
        auto userIt = users_.find(username);
        if (userIt != users_.end()) {
            const User& user = userIt->second;
            cout << "用户名: " << username << endl;
            cout << "角色: " << RoleToString(user.GetRole()) << endl;
            
            // 如果是读者，显示其借阅记录
            if (user.GetRole() == User::Role::Reader) {
                librarySystem.DisplayUserBorrowHistory(username);
            }
        } else {
            cout << "用户 " << username << " 不存在" << endl;
        }
    }

private:
    unordered_map<string, User> users_; // 用户名到 User 对象的映射
    string RoleToString(User::Role role) const {
        switch (role) {
        case User::Role::Admin:
            return "管理员";
        case User::Role::Reader:
            return "读者";
        case User::Role::Guest:
            return "游客";
        default:
            return "未知角色";
        }
    }
};
