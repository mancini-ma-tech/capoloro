document.addEventListener("DOMContentLoaded", () => {
  const tasksContainer = document.getElementById("tasksContainer");
  const addTaskBtn = document.getElementById("addTaskBtn");
  const taskTitleInput = document.getElementById("taskTitle");
  const taskDescriptionInput = document.getElementById("taskDescription");
  const taskCategoryInput = document.getElementById("taskCategory");
  const taskPrioritySelect = document.getElementById("taskPriority");
  const taskDueDateInput = document.getElementById("taskDueDate");
  const filterPrioritySelect = document.getElementById("filterPriority");

  // Funzione per eseguire il programma C++ e ottenere l'output
  async function executeCpp(command, args = []) {
    try {
      // Qui facciamo una chiamata al nostro piccolo server Node.js
      // che a sua volta eseguirà il programma C++.
      const response = await fetch("/execute-cpp", {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
        },
        body: JSON.stringify({ command: command, args: args }),
      });
      const data = await response.json();
      if (data.error) {
        console.error("Errore C++:", data.error);
        alert("Errore nell'esecuzione del comando: " + data.error);
        return null;
      }
      return data.output.trim();
    } catch (error) {
      console.error("Errore nella comunicazione con il server:", error);
      alert(
        "Errore di comunicazione con l'applicazione. Assicurati che il server Node.js sia in esecuzione."
      );
      return null;
    }
  }

  // Funzione per visualizzare i task
  async function renderTasks(filterPriority = "") {
    tasksContainer.innerHTML = ""; // Pulisce la lista
    let output;
    if (filterPriority) {
      output = await executeCpp("filter_priority", [filterPriority]);
    } else {
      output = await executeCpp("list");
    }

    if (output) {
      const lines = output.split("\n").filter((line) => line.trim() !== "");
      if (lines.length === 0) {
        tasksContainer.innerHTML =
          '<li class="no-tasks">Nessuna attività da visualizzare con questo filtro.</li>';
        return;
      }

      lines.forEach((line) => {
        const parts = line.split("|");
        if (parts.length === 6) {
          // Assicurati che ci siano tutti i 6 campi
          const [id, title, description, category, priority, dueDate] = parts;
          const listItem = document.createElement("li");
          listItem.innerHTML = `
                        <div class="task-details">
                            <h3>${title} (ID: ${id})</h3>
                            <p><strong>Descrizione:</strong> ${description}</p>
                            <p><strong>Categoria:</strong> ${category}</p>
                            <p><strong>Priorità:</strong> <span class="priority priority-${priority}">${
            priority == 1 ? "Alta" : priority == 2 ? "Media" : "Bassa"
          }</span></p>
                            <p><strong>Scadenza:</strong> ${dueDate}</p>
                        </div>
                        <button data-id="${id}">Rimuovi</button>
                    `;
          tasksContainer.appendChild(listItem);
        }
      });
    } else {
      tasksContainer.innerHTML =
        '<li class="no-tasks">Nessuna attività da caricare o errore.</li>';
    }

    // Aggiungi event listener per i bottoni "Rimuovi"
    document.querySelectorAll("#tasksContainer button").forEach((button) => {
      button.addEventListener("click", async (event) => {
        const taskId = event.target.dataset.id;
        const successOutput = await executeCpp("remove", [taskId]);
        if (
          successOutput &&
          successOutput.includes("Attivita' rimossa con successo")
        ) {
          alert("Attività rimossa!");
          renderTasks(filterPrioritySelect.value); // Ricarica la lista
        } else {
          alert("Errore durante la rimozione dell'attività: " + successOutput);
        }
      });
    });
  }

  // Event Listener per aggiungere un task
  addTaskBtn.addEventListener("click", async () => {
    const title = taskTitleInput.value.trim();
    const description = taskDescriptionInput.value.trim();
    const category = taskCategoryInput.value.trim();
    const priority = taskPrioritySelect.value;
    const dueDate = taskDueDateInput.value.trim();

    if (!title || !description || !category || !dueDate) {
      alert("Per favore, compila tutti i campi.");
      return;
    }

    // Passiamo i parametri come array di stringhe
    const successOutput = await executeCpp("add", [
      title,
      description,
      category,
      priority,
      dueDate,
    ]);
    if (
      successOutput &&
      successOutput.includes("Attivita' aggiunta con successo")
    ) {
      alert("Attività aggiunta!");
      // Pulisci i campi
      taskTitleInput.value = "";
      taskDescriptionInput.value = "";
      taskCategoryInput.value = "";
      taskPrioritySelect.value = "1";
      taskDueDateInput.value = "";
      renderTasks(filterPrioritySelect.value); // Ricarica la lista
    } else {
      alert("Errore durante l'aggiunta dell'attività: " + successOutput);
    }
  });

  // Event Listener per filtrare i task
  filterPrioritySelect.addEventListener("change", () => {
    renderTasks(filterPrioritySelect.value);
  });

  // Carica i task all'avvio
  renderTasks();
});
