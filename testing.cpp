#include <iostream>
#include <vector>
#include <string>
#include <algorithm> // For shuffle
#include <random>    // For default_random_engine, random_device
#include <chrono>    // For seeding the random engine

// --- USING DECLARATION (This makes "std::" disappear) ---
using namespace std;

// --- 1. DATA STRUCTURE ---

struct Question {
    string text;
    vector<string> choices; // Options for Multiple Choice
    int correctAnswerIndex;        // 0=A, 1=B, 2=C, 3=D
    string subject;
    int gradeLevel;
    string format;           // Used to dictate how the question is presented (e.g., "MCQ", "TF")
};

// --- GLOBAL DATA STORE (Simulated Database) ---
// In a real application, this would be loaded from a file.
vector<Question> allQuestions = {
    {"What is the square root of 64?", {"6", "8", "16", "32"}, 1, "Math", 7, "MCQ"},
    {"Who wrote 'Romeo and Juliet'?", {"Dickens", "Shakespeare", "Hemingway", "Austen"}, 1, "English", 8, "MCQ"},
    {"What is the chemical symbol for water?", {"O2", "H2O", "CO2", "HO"}, 1, "Science", 7, "MCQ"},
    {"The sun is a planet. (True/False)", {"True", "False"}, 1, "Science", 9, "TF"},
    {"What is the capital of France?", {"London", "Berlin", "Paris", "Rome"}, 2, "History", 10, "MCQ"},
    {"If x=5, what is 3x + 1?", {"15", "16", "17", "18"}, 1, "Math", 8, "MCQ"}
};

// --- FUNCTION PROTOTYPES ---
void clientReviewerMode();
void adminMode();
void showQuestion(const Question& q, int questionNumber);
void shuffleQuestions(vector<Question>& questions);
vector<Question> filterQuestions(int level, const string& subject);

// =========================================================

int main() {
    cout << "--- Reviewer Program Main Menu ---\n";
    cout << "1. Client Reviewer Mode\n";
    cout << "2. Admin CRUD Mode\n";
    cout << "3. Exit\n";
    cout << "Enter choice: ";

    int choice;
    cin >> choice;

    if (choice == 1) {
        clientReviewerMode();
    } else if (choice == 2) {
        adminMode();
    } else {
        cout << "Exiting program. Goodbye!\n";
    }

    return 0;
}

// =========================================================
// --- CLIENT REVIEWER MODE FUNCTIONS ---
// =========================================================

/**
 * @brief Filters the global question list based on grade level and subject.
 */
vector<Question> filterQuestions(int level, const string& subject) {
    vector<Question> result;
    // Iterate through all questions and keep only the ones that match
    for (const auto& q : allQuestions) {
        if (q.gradeLevel == level && q.subject == subject) {
            result.push_back(q);
        }
    }
    return result;
}

/**
 * @brief Shuffles the questions to randomize the order.
 */
void shuffleQuestions(vector<Question>& questions) {
    // Get a time-based seed for true randomization
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine rng(seed);
    shuffle(questions.begin(), questions.end(), rng);
}

/**
 * @brief Displays a single question and handles user input.
 */
void showQuestion(const Question& q, int questionNumber) {
    cout << "\n--- Question " << questionNumber << " (" << q.subject << " - G" << q.gradeLevel << ") --- \n";
    cout << q.text << "\n";

    if (q.format == "MCQ") {
        char optionChar = 'A';
        for (const auto& choice : q.choices) {
            cout << optionChar++ << ") " << choice << "\n";
        }
    } else if (q.format == "TF") {
        cout << "Options: T/F \n";
    }
}

/**
 * @brief Main logic for the client quiz session.
 */
void clientReviewerMode() {
    int level;
    string subject;
    
    cout << "\n\n--- Client Reviewer Start ---\n";
    cout << "Enter Grade Level (7-10): ";
    cin >> level;
    cout << "Enter Subject (e.g., Math, Science, English, History): ";
    cin >> subject;

    // Step 1: Filter Questions
    vector<Question> quizQuestions = filterQuestions(level, subject);

    if (quizQuestions.empty()) {
        cout << "No questions found for Grade " << level << " in " << subject << ".\n";
        return;
    }

    // Step 2: Implement the "Plot Twist" - Randomize Format and Shuffle
    // For simplicity, we randomly pick a format and apply it to ALL filtered questions
    vector<string> formats = {"MCQ", "TF"};
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, formats.size() - 1);
    
    string chosenFormat = formats[distrib(gen)];
    cout << "\n[PLOT TWIST]: This session's question format is **" << chosenFormat << "**.\n";

    // Apply the chosen format and shuffle
    for(auto& q : quizQuestions) {
        q.format = chosenFormat; 
    }
    shuffleQuestions(quizQuestions);
    
    // Step 3: Run the Quiz
    int score = 0;
    int totalQuestions = quizQuestions.size();

    for (size_t i = 0; i < totalQuestions; ++i) {
        showQuestion(quizQuestions[i], i + 1);
        string userAnswer;
        cout << "Your Answer: ";
        cin >> userAnswer;
        
        // Basic check (Need robust error handling for real use)
        char correctLetter = 'A' + quizQuestions[i].correctAnswerIndex;
        string correctAnswer;
        correctAnswer += correctLetter; 

        // Crude True/False check if format is TF
        if (chosenFormat == "TF") {
            correctAnswer = (quizQuestions[i].correctAnswerIndex == 1) ? "F" : "T";
        }
        
        // Compare user answer (convert to uppercase for consistent checking)
        transform(userAnswer.begin(), userAnswer.end(), userAnswer.begin(), ::toupper);
        
        if (userAnswer == correctAnswer) {
            cout << "Correct!\n";
            score++;
        } else {
            cout << "Incorrect. The correct answer was " << correctAnswer << ".\n";
        }
    }

    cout << "\n--- Quiz Finished ---\n";
    cout << "You scored " << score << " out of " << totalQuestions << ".\n";
}

// =========================================================
// --- ADMIN CRUD MODE FUNCTIONS ---
// =========================================================

/**
 * @brief Simple Admin Menu for managing questions (CRUD).
 */
void adminMode() {
    cout << "\n\n--- Admin Mode (Basic CRUD) ---\n";
    
    int adminChoice;
    
    cout << "1. Add New Question\n";
    cout << "2. View All Questions\n";
    cout << "3. Back to Main Menu\n";
    cout << "Enter choice: ";
    cin >> adminChoice;
    
    if (adminChoice == 1) {
        // --- CREATE (Add) ---
        Question newQ;
        cout << "Enter Question Text: ";
        // Clear buffer and use getline for text that might have spaces
        cin.ignore(1000, '\n'); 
        getline(cin, newQ.text);
        
        cout << "Enter Grade Level (7-10): ";
        cin >> newQ.gradeLevel;
        cout << "Enter Subject: ";
        cin >> newQ.subject;
        cout << "Enter Format (MCQ/TF): ";
        cin >> newQ.format;
        
        newQ.choices.resize(4); // Pre-allocate 4 slots for choices
        for (int i = 0; i < 4; ++i) {
            cout << "Enter Choice " << (char)('A' + i) << ": ";
            cin.ignore(1000, '\n');
            getline(cin, newQ.choices[i]);
        }
        
        cout << "Enter Correct Answer Index (0=A, 1=B, 2=C, 3=D): ";
        cin >> newQ.correctAnswerIndex;
        
        allQuestions.push_back(newQ);
        cout << "\nQuestion added successfully!\n";
        
    } else if (adminChoice == 2) {
        // --- READ (View) ---
        cout << "\n--- Current Questions in Database ---\n";
        if (allQuestions.empty()) {
            cout << "Database is empty.\n";
            return;
        }
        for (size_t i = 0; i < allQuestions.size(); ++i) {
            cout << "[" << i << "] G" << allQuestions[i].gradeLevel 
                      << " | " << allQuestions[i].subject 
                      << " | " << allQuestions[i].text.substr(0, 30) << "...\n";
        }
    }
    // Simple way to loop back to the main menu after the operation
    main(); 
}