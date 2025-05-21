/*
"Task Tracker Dinamico": Un'Applicazione Web per la Gestione delle Attivita'.

    L'idea:
        Sviluppare un'applicazione web che permetta agli utenti di
        creare, modificare, eliminare e organizzare le proprie attivita'.
        Potresti usare HTML per la struttura, CSS per lo stile e JavaScript
        per la dinamicita' (aggiungere/rimuovere task senza ricaricare la
        pagina, ordinare, filtrare).
        Per mostrare le tue qualita', potresti implementare funzionalita'
        avanzate come l'assegnazione di priorita', date di scadenza,
        categorie e magari anche la persistenza dei dati
        (anche solo usando il localStorage del browser all'inizio).

    Tocco C++ (pensando al "dietro le quinte"):
        Potresti immaginare di creare una libreria in C++ per la
        gestione efficiente dei dati delle attivita' (aggiunta,
        ricerca, ordinamento) e poi esporla tramite un'interfaccia
        (magari usando qualcosa come FastCGI o un framework web C++)
        a cui la tua applicazione JavaScript potrebbe fare delle chiamate.
*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <limits>
#include <fstream> // Per la gestione dei file
#include <sstream> // Per la gestione delle stringhe in input/output

using namespace std;

// Classe che rappresenta un'attivita'
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

    // Funzione per stampare il task in un formato "parsabile" (per JavaScript)
    // Formato: ID|TITOLO|DESCRIZIONE|CATEGORIA|PRIORITA|DATA_SCADENZA
    string toString() const
    {
        return to_string(id) + "|" + title + "|" + description + "|" + category + "|" + to_string(priority) + "|" + dueDate;
    }

    // Funzione statica per creare un Task da una stringa (per caricamento da file)
    static Task fromString(const string &data)
    {
        stringstream ss(data);
        string segment;
        vector<string> seglist;

        while (getline(ss, segment, '|'))
        {
            seglist.push_back(segment);
        }

        return Task(stoi(seglist[0]), seglist[1], seglist[2], seglist[3], stoi(seglist[4]), seglist[5]);
    }

    // Per la stampa a console (utile per il debug)
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

// Classe che gestisce un insieme di attivita'
class TaskManager
{
private:
    vector<Task> tasks;
    int nextId = 1;
    const string DATA_FILE = "tasks.txt"; // Nome del file per la persistenza

    void saveTasks()
    {
        ofstream outFile(DATA_FILE);
        if (outFile.is_open())
        {
            for (const auto &task : tasks)
            {
                outFile << task.toString() << endl;
            }
            outFile.close();
        }
        else
        {
            cerr << "Errore: Impossibile salvare i dati nel file " << DATA_FILE << endl;
        }
    }

    void loadTasks()
    {
        ifstream inFile(DATA_FILE);
        if (inFile.is_open())
        {
            string line;
            tasks.clear(); // Pulisce le attività correnti
            int maxId = 0;
            while (getline(inFile, line))
            {
                Task loadedTask = Task::fromString(line);
                tasks.push_back(loadedTask);
                if (loadedTask.getId() > maxId)
                {
                    maxId = loadedTask.getId();
                }
            }
            inFile.close();
            nextId = maxId + 1; // Imposta il nextId correttamente
        }
        // Se il file non esiste o non può essere aperto, iniziamo con un task manager vuoto
    }

public:
    TaskManager()
    {
        loadTasks(); // Carica i task all'avvio del TaskManager
    }

    Task addTask(const string &title, const string &description, const string &category, int priority, const string &dueDate)
    {
        Task newTask(nextId++, title, description, category, priority, dueDate);
        tasks.push_back(newTask);
        saveTasks(); // Salva i task dopo l'aggiunta
        return newTask;
    }

    bool removeTask(int id)
    {
        auto it = remove_if(tasks.begin(), tasks.end(), [id](const Task &task)
                            { return task.getId() == id; });
        if (it != tasks.end())
        {
            tasks.erase(it, tasks.end());
            saveTasks(); // Salva i task dopo la rimozione
            return true;
        }
        return false;
    }

    vector<Task> getAllTasks() const
    {
        return tasks;
    }

    vector<Task> getTasksByPriority(int priority) const
    {
        vector<Task> filteredTasks;
        copy_if(tasks.begin(), tasks.end(), back_inserter(filteredTasks), [priority](const Task &task)
                { return task.getPriority() == priority; });
        return filteredTasks;
    }
};

int main(int argc, char *argv[])
{
    TaskManager manager;

    if (argc < 2)
    {
        cout << "Errore: Comando non specificato. Usare: list, add, remove, filter_priority." << endl;
        return 1;
    }

    string command = argv[1];

    if (command == "list")
    {
        for (const auto &task : manager.getAllTasks())
        {
            cout << task.toString() << endl;
        }
    }
    else if (command == "add")
    {
        if (argc < 7)
        {
            cout << "Errore: Parametri insufficienti per 'add'. Usare: add <title> <description> <category> <priority> <dueDate>" << endl;
            return 1;
        }
        // Nota: i parametri della riga di comando sono trattati come singole parole.
        // Se titolo, descrizione o categoria contengono spazi, dovranno essere racchiusi tra virgolette nel comando
        // e poi gestiti correttamente. Per semplicità, qui assumiamo che siano una singola stringa o che gli spazi siano gestiti da chi chiama.
        string title = argv[2];
        string description = argv[3];
        string category = argv[4];
        int priority = stoi(argv[5]);
        string dueDate = argv[6];

        manager.addTask(title, description, category, priority, dueDate);
        cout << "Attivita' aggiunta con successo." << endl;
    }
    else if (command == "remove")
    {
        if (argc < 3)
        {
            cout << "Errore: ID non specificato per 'remove'. Usare: remove <id>" << endl;
            return 1;
        }
        int id = stoi(argv[2]);
        if (manager.removeTask(id))
            cout << "Attivita' rimossa con successo." << endl;
        else
            cout << "Attivita' non trovata." << endl;
    }
    else if (command == "filter_priority")
    {
        if (argc < 3)
        {
            cout << "Errore: Priorita' non specificata per 'filter_priority'. Usare: filter_priority <priority>" << endl;
            return 1;
        }
        int priority = stoi(argv[2]);
        auto filteredTasks = manager.getTasksByPriority(priority);
        for (const auto &task : filteredTasks)
        {
            cout << task.toString() << endl;
        }
    }
    else
    {
        cout << "Comando non riconosciuto: " << command << endl;
        return 1;
    }

    return 0;
}