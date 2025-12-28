let phsensor, color, turb, methane, alcohol;
const buttons = document.querySelectorAll('.item-button');
let phSensorDisplay = document.querySelector("#phsensor")
let turbiditySensorDisplay = document.querySelector("#turbidity")
let OutputScreen = document.querySelector(".right-stat")
const dotenv = require('dotenv')
dotenv.config({path:`./.env.${process.env.APP_ENV}`});

// Add click event listener to each button
buttons.forEach(button => {
  button.addEventListener('click', () => {
      // Remove 'activate' class from all buttons
      buttons.forEach(btn => btn.classList.remove('activate'));

      // Add 'activate' class to the clicked button
      button.classList.add('activate');

      // Log specific value based on button text
      if (button.textContent.trim() === 'Water') {
          console.log('You selected Water');
          if(phsensor < 6.5 || phsensor>8.5)
          {
             OutputScreen.innerHTML = "The Water is not drinkable"
          }
          else{
            OutputScreen.innerHTML = "The Water is drinkable"
          }
       
  
      } else if (button.textContent.trim() === 'Vegetables') {
          console.log('You selected Vegetables');
          if(alcohol>3000 || methane>2000){
            OutputScreen.innerHTML = "The vegetable is Bad"
          }
          else
          {
            OutputScreen.innerHTML = "The vegetable is fresh"
          }
        } else if (button.textContent.trim() === 'Milk') {
          console.log('You selected Milk');
          if(phsensor < 6.4 || phsensor>9.5){
             OutputScreen.innerHTML = "The Milk is Bad"
          }
          else
          {
            OutputScreen.innerHTML = "The Milk is fresh"
          }
      } else if (button.textContent.trim() === 'Fruits') {
          console.log('You selected Fruits');
          if(alcohol>3000 || methane>2000){
            OutputScreen.innerHTML = "The fruit is Bad"
          }
          else
          {
            OutputScreen.innerHTML = "The fruit is fresh"
          }
      } else {
          console.log('Unknown selection');
      }
  });
});



(function() {
  const channelId = process.env.CHANNEL_ID;
  const apiKey = process.env.API_KEY;

  async function fetchSensorData() {
    const url = process.env.URL;

    try {
      const response = await fetch(url);
      const data = await response.json();
      phsensor = data.v1;
      color = data.v3;
      turb = data.v0;
      methane = data.v32;
      alcohol = data.v2;
      updateSensorValues(); // Call a function to update the DOM
    } catch (error) {
      console.error("Error fetching data:", error);
    }
  }

  function updateSensorValues() {
    document.querySelector("#turbidity").innerHTML = `${turb}`;
    document.querySelector("#phsensor").innerHTML = `${phsensor}`;
    document.querySelector("#colorsensor").innerHTML = `${color}`;
    document.querySelector("#methane").innerHTML = `${methane}`;
    document.querySelector("#alcohol").innerHTML = `${alcohol}`;
  }

  // Expose a function to access sensor data from another script
  window.getSensorData = function() {
    return {
      turbidity: turb,
      ph: phsensor,
      color,
      methane,
      alcohol
    };
  };

  setInterval(fetchSensorData, 1000);
  fetchSensorData(); // Initial call
})();



