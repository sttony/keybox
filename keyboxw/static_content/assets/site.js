const form = document.getElementById("publicKeyForm");
const emailInput = document.getElementById("emailInput");
const result = document.getElementById("result");

form.addEventListener("submit", async (event) => {
    event.preventDefault();

    const email = emailInput.value.trim();
    if (!email) {
        return;
    }

    result.className = "result";
    result.textContent = "Searching...";

    try {
        const response = await fetch(`/get_public_key?email=${encodeURIComponent(email)}`);
        const data = await response.json();

        if (!response.ok) {
            throw new Error(data.message || "Failed to retrieve public key");
        }

        result.textContent = `Public Key for ${data.email}:\n\n${data.public_key}`;
    } catch (error) {
        result.className = "result error";
        result.textContent = `Error: ${error.message}`;
    }
});
