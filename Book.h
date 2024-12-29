#pragma once
#include <iostream>
#include <string>
using namespace std;

class Book {
public:
    // Constructors
    Book() = default;
    Book(const string& id, const string& isbn, const string& name,
        const string& author, const string& publishing,
        const string& publishedDate, double price, int pages, const string& description)
        : id_(id), isbn_(isbn), name_(name), author_(author),
        publishing_(publishing), publishedDate_(publishedDate),
        price_(price), pages_(pages), description_(description) {}

    // Getters
    string GetID() const { return id_; }
    string GetISBN() const { return isbn_; }
    string GetName() const { return name_; }
    string GetAuthor() const { return author_; }
    string GetPublishing() const { return publishing_; }
    string GetPublishedDate() const { return publishedDate_; }
    double GetPrice() const { return price_; }
    int GetPages() const { return pages_; }
    string GetDescription() const { return description_; }

    // Setters
    void SetID(const string& id) { id_ = id; }
    void SetISBN(const string& isbn) { isbn_ = isbn; }
    void SetName(const string& name) { name_ = name; }
    void SetAuthor(const string& author) { author_ = author; }
    void SetPublishing(const string& publishing) { publishing_ = publishing; }
    void SetPublishedDate(const string& publishedDate) { publishedDate_ = publishedDate; }
    void SetPrice(double price) { price_ = price; }
    void SetPages(int pages) { pages_ = pages; }
    void SetDescription(const string& description) { description_ = description; }

    // Method to display book information
    void Display() const {
        cout << "ID: " << id_ << endl;
        cout << "ISBN: " << isbn_ << endl;
        cout << "Name: " << name_ << endl;
        cout << "Author: " << author_ << endl;
        cout << "Publishing: " << publishing_ << endl;
        cout << "Published Date: " << publishedDate_ << endl;
        cout << "Price: " << price_ << endl;
        cout << "Pages: " << pages_ << endl;
        cout << "Description: " << description_ << endl;
    }

    // 将书籍信息转换为字符串
    string ToString() const {
        return id_ + "," + isbn_ + "," + name_ + "," + author_ + "," +
            publishing_ + "," + publishedDate_ + "," +
            to_string(price_) + "," + to_string(pages_) + "," +
            description_;
    }

private:
    string id_;
    string isbn_;
    string name_;
    string author_;
    string publishing_;
    string publishedDate_;
    double price_; // Changed from string to double
    int pages_;
    string description_;
};
