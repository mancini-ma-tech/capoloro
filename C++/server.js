const express = require("express");
const { spawn } = require("child_process");
const path = require("path");
const app = express();
const port = 3000; // Puoi cambiare la porta se vuoi

app.use(express.json()); // Per parsare il body delle richieste JSON
app.use(express.static(__dirname)); // Serve i file statici dalla directory corrente

// Endpoint per eseguire il programma C++
app.post("/execute-cpp", (req, res) => {
  const { command, args } = req.body;
  const executablePath = path.join(__dirname, "task_manager.exe"); // Assicurati che task_manager.exe sia qui

  console.log(
    `Esecuzione comando C++: ${executablePath} ${command} ${args.join(" ")}`
  );

  // spawn esegue un comando esterno
  const cppProcess = spawn(executablePath, [command, ...args]);

  let output = "";
  let errorOutput = "";

  cppProcess.stdout.on("data", (data) => {
    output += data.toString();
  });

  cppProcess.stderr.on("data", (data) => {
    errorOutput += data.toString();
  });

  cppProcess.on("close", (code) => {
    if (code === 0) {
      // Successo
      console.log("Output C++:", output);
      res.json({ success: true, output: output });
    } else {
      // Errore
      console.error(`Processo C++ terminato con codice ${code}`);
      console.error("Errore C++:", errorOutput);
      res.json({
        success: false,
        error: errorOutput || `Processo C++ terminato con codice ${code}`,
        output: output,
      });
    }
  });

  cppProcess.on("error", (err) => {
    console.error("Errore nell'avvio del processo C++:", err);
    res.json({
      success: false,
      error: `Impossibile avviare il programma C++. Errore: ${err.message}`,
    });
  });
});

app.listen(port, () => {
  console.log(`Server Node.js in ascolto su http://localhost:${port}`);
  console.log(`Apri http://localhost:${port}/index.html nel tuo browser.`);
});
