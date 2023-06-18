// Import the functions you need from the SDKs you need
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.17.1/firebase-app.js";

// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries ====================== use /firebase-database to import realtime Database
import { getDatabase, ref, onValue, update } from "https://www.gstatic.com/firebasejs/9.17.1/firebase-database.js";

// Your web app's Firebase configuration
const firebaseConfig = {
  // apiKey: "AIzaSyDESjsyGhzvfx0UyD0KJHU25T26u1fGnzQ",
  // authDomain: "tt-iot-tuan1.firebaseapp.com",
  // databaseURL: "https://tt-iot-tuan1-default-rtdb.firebaseio.com",
  // projectId: "tt-iot-tuan1",
  // storageBucket: "tt-iot-tuan1.appspot.com",
  // messagingSenderId: "908073094535",
  // appId: "1:908073094535:web:262cb36ce67daeb5551722"
  apiKey: "AIzaSyD9VXVhHwHSqQ3JwCFSO7IsvefIiHXMroc",
  authDomain: "doan-1-1c412.firebaseapp.com",
  databaseURL: "https://doan-1-1c412-default-rtdb.asia-southeast1.firebasedatabase.app",
  projectId: "doan-1-1c412",
  storageBucket: "doan-1-1c412.appspot.com",
  messagingSenderId: "1040475811034",
  appId: "1:1040475811034:web:de203e337be63b63a689e3"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);

// Initialize Realtime Database and get a reference to the service
const database = getDatabase(app);

onValue(ref(database, "soxe"), (snapshot) => {
    const xe = snapshot.val();
    document.getElementById("soxe").innerHTML = xe;
})

onValue(ref(database, "max"), (snapshot) => {
  const max = snapshot.val();
  document.getElementById("max").innerHTML = max;
})



