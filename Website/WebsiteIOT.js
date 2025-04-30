function login() {
    // Get email and password values from the inputs
    const email = document.getElementById("femail").value;
    const password = document.getElementById("spassword").value;
  
    // Define the correct email and password
    const correctEmail = "ernestoalonsop6@gmail.com";
    const correctPassword = "Ernesto1234";
  
    // Check if email and password match
    if (email === correctEmail && password === correctPassword) {
      // If they match, show a success message and redirect
      window.location.href = "WebsiteIOT2.html"; // Replace with the actual URL you want to redirect to
    } else {
      // If they don't match, show an error message
      document.getElementById("error-message").style.display = "block";
    }
  }