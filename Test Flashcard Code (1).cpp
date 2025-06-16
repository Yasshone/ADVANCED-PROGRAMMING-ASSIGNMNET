#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// FlashCard class
class FlashCard {
public:
    string question;
    string answer;
    int score;

    FlashCard() : question(""), answer(""), score(0) {}

    FlashCard(string q, string a) : question(q), answer(a), score(0) {}

    FlashCard(string q, string a, int s) : question(q), answer(a), score(s) {}
};

// Deck class
class Deck {
private:
    vector<FlashCard> cards;

public:
    void addCard(string q, string a) {
        cards.push_back(FlashCard(q, a));
        cout << "Flashcard added.\n";
    }

    vector<FlashCard>& getCards() {
        return cards;
    }

    void saveToFile(string filename) {
        ofstream fout(filename, ios::binary);
        if (!fout) {
            cout << "Cannot open file for saving!\n";
            return;
        }
        int size = cards.size();
        fout.write((char*)&size, sizeof(int));
        for (auto& card : cards) {
            int qLen = card.question.size();
            int aLen = card.answer.size();
            fout.write((char*)&qLen, sizeof(int));
            fout.write(card.question.c_str(), qLen);
            fout.write((char*)&aLen, sizeof(int));
            fout.write(card.answer.c_str(), aLen);
            fout.write((char*)&card.score, sizeof(int));
        }
        fout.close();
        cout << "Saved to file.\n";
    }

    void loadFromFile(string filename) {
        ifstream fin(filename, ios::binary);
        if (!fin) {
            cout << "Cannot open file for loading!\n";
            return;
        }
        cards.clear();
        int size;
        fin.read((char*)&size, sizeof(int));
        for (int i = 0; i < size; i++) {
            int qLen, aLen;
            fin.read((char*)&qLen, sizeof(int));
            char* qbuf = new char[qLen + 1];
            fin.read(qbuf, qLen);
            qbuf[qLen] = '\0';
            string q(qbuf);
            delete[] qbuf;

            fin.read((char*)&aLen, sizeof(int));
            char* abuf = new char[aLen + 1];
            fin.read(abuf, aLen);
            abuf[aLen] = '\0';
            string a(abuf);
            delete[] abuf;

            int score;
            fin.read((char*)&score, sizeof(int));

            cards.push_back(FlashCard(q, a, score));
        }
        fin.close();
        cout << "Loaded from file.\n";
    }

    bool isEmpty() {
        return cards.empty();
    }
};

// User class
class User {
private:
    string name;
    int totalScore = 0;

public:
    void setName(string n) {
        name = n;
    }

    void updateScore(int s) {
        totalScore += s;
    }

    int getTotalScore() {
        return totalScore;
    }

    string getName() {
        return name;
    }
};

// Spaced Repetition Manager
class SpacedRepetitionManager {
public:
    FlashCard* getNextCard(vector<FlashCard>& cards) {
        if (cards.empty()) return nullptr;

        sort(cards.begin(), cards.end(), [](FlashCard& a, FlashCard& b) {
            return a.score < b.score;
        });

        int range = min(3, (int)cards.size());
        return &cards[rand() % range];
    }
};

// App class
class App {
private:
    Deck deck;
    User user;
    SpacedRepetitionManager manager;
    string filename;

public:
    App(string fname = "flashcards.dat") : filename(fname) {}

    void run() {
        srand(time(0));
        int choice;
        cout << "Enter your name: ";
        string uname;
        getline(cin, uname);
        user.setName(uname);

        do {
            cout << "\n--- Flashcard Menu ---\n";
            cout << "1. Add Flashcard\n";
            cout << "2. Review Flashcards\n";
            cout << "3. Save Flashcards\n";
            cout << "4. Load Flashcards\n";
            cout << "5. Show Total Score\n";
            cout << "6. Exit\n";
            cout << "Choice: ";
            cin >> choice;
            cin.ignore();
            system("CLS");

            switch (choice) {
                case 1: addFlashcard(); break;
                case 2: reviewFlashcards(); break;
                case 3: deck.saveToFile(filename); break;
                case 4: deck.loadFromFile(filename); break;
                case 5: cout << "Total Score: " << user.getTotalScore() << "\n"; break;
                case 6: cout << "Goodbye!\n"; break;
                default: cout << "Invalid choice.\n";
            }
        } while (choice != 6);
    }

    void addFlashcard() {
        string q, a;
        cout << "Enter question: ";
        getline(cin, q);
        cout << "Enter answer: ";
        getline(cin, a);
        deck.addCard(q, a);
    }

    void reviewFlashcards() {
        if (deck.isEmpty()) {
            cout << "No flashcards available.\n";
            return;
        }

        FlashCard* card = manager.getNextCard(deck.getCards());
        cout << "\nQuestion: " << card->question << "\n";
        cout << "Your Answer: ";
        string userAnswer;
        getline(cin, userAnswer);

        if (userAnswer == card->answer) {
            cout << "Correct!\n";
            card->score++;
            user.updateScore(1);
        } else {
            cout << "Wrong. Correct answer: " << card->answer << "\n";
            card->score = max(0, card->score - 1);
        }
    }
};

// Main function
int main() {
    App app;
    app.run();
    return 0;
}
