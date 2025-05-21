/*
"Task Tracker Dinamico": Un'Applicazione Web per la Gestione delle Attività.

    L'idea:
        Sviluppare un'applicazione web che permetta agli utenti di
        creare, modificare, eliminare e organizzare le proprie attività.
        Potresti usare HTML per la struttura, CSS per lo stile e JavaScript
        per la dinamicità (aggiungere/rimuovere task senza ricaricare la
        pagina, ordinare, filtrare).
        Per mostrare le tue qualità, potresti implementare funzionalità
        avanzate come l'assegnazione di priorità, date di scadenza,
        categorie e magari anche la persistenza dei dati
        (anche solo usando il localStorage del browser all'inizio).

    Tocco C++ (pensando al "dietro le quinte"):
        Potresti immaginare di creare una libreria in C++ per la
        gestione efficiente dei dati delle attività (aggiunta,
        ricerca, ordinamento) e poi esporla tramite un'interfaccia
        (magari usando qualcosa come FastCGI o un framework web C++)
        a cui la tua applicazione JavaScript potrebbe fare delle chiamate.
*/
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
using namespace std;

// Classe che rappresenta un'attività
class Task
{
private:
    int id;
    string title;
    string description;
    string category;
    int priority;
    string dueDate;

public:
    Task(int id, const string &title, const string &description, const string &category, int priority, const string &dueDate)
        : id(id), title(title), description(description), category(category), priority(priority), dueDate(dueDate) {}

    int getId() const { return id; }
    string getTitle() const { return title; }
    string getDescription() const { return description; }
    string getCategory() const { return category; }
    int getPriority() const { return priority; }
    string getDueDate() const { return dueDate; }

    void setTitle(const string &newTitle) { title = newTitle; }
    void setDescription(const string &newDescription) { description = newDescription; }
    void setCategory(const string &newCategory) { category = newCategory; }
    void setPriority(int newPriority) { priority = newPriority; }
    void setDueDate(const string &newDueDate) { dueDate = newDueDate; }

    void printTask() const
    {
        cout << left << setw(5) << id
             << setw(20) << title
             << setw(30) << description
             << setw(15) << category
             << setw(10) << priority
             << setw(15) << dueDate << endl;
    }
};

// Classe che gestisce un insieme di attività
class TaskManager
{
private:
    vector<Task> tasks;
    int nextId = 1;

public:
    Task addTask(const string &title, const string &description, const string &category, int priority, const string &dueDate)
    {
        Task newTask(nextId++, title, description, category, priority, dueDate);
        tasks.push_back(newTask);
        return newTask;
    }

    bool removeTask(int id)
    {
        auto it = remove_if(tasks.begin(), tasks.end(), [id](const Task &task)
                            { return task.getId() == id; });
        if (it != tasks.end())
        {
            tasks.erase(it, tasks.end());
            return true;
        }
        return false;
    }

    vector<Task> getTasksByPriority(int priority) const
    {
        vector<Task> filteredTasks;
        copy_if(tasks.begin(), tasks.end(), back_inserter(filteredTasks), [priority](const Task &task)
                { return task.getPriority() == priority; });
        return filteredTasks;
    }

    void printAllTasks() const
    {
        cout << left << setw(5) << "ID"
             << setw(20) << "Title"
             << setw(30) << "Description"
             << setw(15) << "Category"
             << setw(10) << "Priority"
             << setw(15) << "Due Date" << endl;
        cout << string(95, '-') << endl;
        for (const auto &task : tasks)
        {
            task.printTask();
        }
    }
};

int main()
{
    TaskManager manager;

    while (true)
    {
        cout << "\n--- Task Tracker Dinamico ---\n";
        cout << "1. Aggiungi attività\n";
        cout << "2. Rimuovi attività\n";
        cout << "3. Visualizza tutte le attività\n";
        cout << "4. Filtra attività per priorità\n";
        cout << "5. Esci\n";
        cout << "Seleziona un'opzione: ";

        int choice;
        cin >> choice;

        if (choice == 1)
        {
            string title, description, category, dueDate;
            int priority;

            cout << "Titolo: ";
            cin.ignore();
            getline(cin, title);
            cout << "Descrizione: ";
            getline(cin, description);
            cout << "Categoria: ";
            getline(cin, category);
            cout << "Priorità (1 = Alta, 2 = Media, 3 = Bassa): ";
            cin >> priority;
            cout << "Data di scadenza (YYYY-MM-DD): ";
            cin >> dueDate;

            manager.addTask(title, description, category, priority, dueDate);
            cout << "Attività aggiunta con successo!\n";
        }
        else if (choice == 2)
        {
            int id;
            cout << "Inserisci l'ID dell'attività da rimuovere: ";
            cin >> id;

            if (manager.removeTask(id))
                cout << "Attività rimossa con successo!\n";
            else
                cout << "Attività non trovata.\n";
        }
        else if (choice == 3)
        {
            cout << "\n--- Tutte le attività ---\n";
            manager.printAllTasks();
        }
        else if (choice == 4)
        {
            int priority;
            cout << "Inserisci la priorità da filtrare (1 = Alta, 2 = Media, 3 = Bassa): ";
            cin >> priority;

            auto filteredTasks = manager.getTasksByPriority(priority);
            cout << "\n--- Attività con priorità " << priority << " ---\n";
            cout << left << setw(5) << "ID"
                 << setw(20) << "Title"
                 << setw(30) << "Description"
                 << setw(15) << "Category"
                 << setw(10) << "Priority"
                 << setw(15) << "Due Date" << endl;
            cout << string(95, '-') << endl;
            for (const auto &task : filteredTasks)
            {
                task.printTask();
            }
        }
        else if (choice == 5)
        {
            cout << "Uscita dal programma. Arrivederci!\n";
            break;
        }
        else
        {
            cout << "Opzione non valida. Riprova.\n";
        }
    }

    return 0;
}