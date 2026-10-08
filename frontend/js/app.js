const helloButton = document.getElementById("helloButton");
const helloResult = document.getElementById("helloResult");
const databaseButton = document.getElementById("databaseButton");
const databaseResult = document.getElementById("databaseResult");

async function callApi(endpoint, result, loadingMessage) {
    result.textContent = loadingMessage;
    try {
        const response = await fetch(`http://localhost:3000${endpoint}`);
        const data = await response.json();

        if (!response.ok || data.status === "error") {
            result.textContent = data.message || "Database connection failed";
            return;
        }

        result.textContent = data.database
            ? `${data.message} (Database: ${data.database})`
            : data.message;
    } catch (error) {
        result.textContent = "Could not reach the backend. Make sure it is running.";
        console.error(error);
    }
}

helloButton.addEventListener("click", () => {
    callApi("/api/hello", helloResult, "Checking backend...");
});

databaseButton.addEventListener("click", () => {
    callApi("/api/database", databaseResult, "Checking database connection...");
});