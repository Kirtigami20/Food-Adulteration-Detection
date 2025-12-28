const URL = "./my_model/";

let model, webcam, labelContainer, maxPredictions;
let isPredicting = false;
let loopRequestId = null;

async function init() {
  const modelURL = URL + "model.json";
  const metadataURL = URL + "metadata.json";

  model = await tmImage.load(modelURL, metadataURL);
  maxPredictions = model.getTotalClasses();

  const flip = true; 
  webcam = new tmImage.Webcam(400, 400, flip); 
  await webcam.setup(); 
  await webcam.play();
  window.requestAnimationFrame(loop);

  document.getElementById("webcam-container").appendChild(webcam.canvas);
  labelContainer = document.getElementById("label-container");

  for (let i = 0; i < maxPredictions; i++) {
      const labelDiv = document.createElement("div");
      labelDiv.className = "label";
    
        
        labelContainer.appendChild(labelDiv);
   
  }
}

async function start() {
  if (!isPredicting) {
    await webcam.play();
    isPredicting = true;
    loop();
  }
}

function stop() {
  if (isPredicting) {
    webcam.stop();
    isPredicting = false;
    if (loopRequestId) {
      window.cancelAnimationFrame(loopRequestId);
      loopRequestId = null;
    }
  }
}

async function loop() {
  webcam.update();
  await predict();
  if (isPredicting) {
    loopRequestId = window.requestAnimationFrame(loop);
  }
}

let lastUpdate = 0;
async function predict() {
  const prediction = await model.predict(webcam.canvas);


    for (let i = 0; i < maxPredictions; i++) {
      const classPrediction = prediction[i].className;
      const probability = Math.round(prediction[i].probability * 100);
      const labelDiv = labelContainer.childNodes[i];

      labelDiv.innerHTML = `${classPrediction}: ${probability}%`;

      // Create or update the progress bar
      let progressBar = labelDiv.querySelector('.progress-bar');
      if (!progressBar) {
        const progressDiv = document.createElement('div');
        progressDiv.className = 'progress';
        progressBar = document.createElement('div');
        progressBar.className = 'progress-bar';
        progressDiv.appendChild(progressBar);
        labelDiv.appendChild(progressDiv);
      }

      progressBar.style.width = `${probability}%`;
    }

}

let button = document.getElementById("aibutton");
button.style.background = "linear-gradient(to right, purple, blue)";
let webcamcont = document.getElementById("webcam-container");
let labelcont = document.getElementById("label-container");
let webcamsection = document.getElementsByClassName("webcamsection")[0];
document.getElementById("aibutton").addEventListener("click", function() {
  if (isPredicting) {
    stop();
    button.innerHTML = "Start"; 
    button.style.background = "linear-gradient(to right, purple, blue)"; 
    webcamcont.innerHTML = "";
    labelcont.innerHTML = "";
    webcamsection.classList.add("CameraOff")
  } else {
    init().then(start);
    button.innerHTML = "Stop";
    webcamsection.classList.remove("CameraOff");
    button.style.background = "linear-gradient(to right, crimson, purple)";
  }
});

if (navigator.mediaDevices && navigator.mediaDevices.getUserMedia) {
  console.log("WebRTC is supported");
} else {
  console.error("Your browser does not support WebRTC.");
}
