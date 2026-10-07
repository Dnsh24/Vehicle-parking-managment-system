const button = document.getElementById("testButton");
const result = document.getElementById("result");

button.addEventListener("click", async () => {
    try {
        const response = await fetch("http://localhost:3000/api/hello");
        const data = await response.json();

        result.textContent = data.message;
    } catch (error) {
        result.textContent = "Backend is not running";
        console.error(error);
    }
});