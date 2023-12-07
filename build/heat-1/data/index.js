var statusData = null;
let interval = null;
let started = false;
let previousStatus = "OFF";
const baseUrl = `${location.protocol}//${location.host}`;

// Functions for /

/**
 * Get the status from the API
 *
 * @returns {Promise<{internalTemp: number, externalTemp: number, desiredTemperature: number, status: string}>}
 */
function getStatus() {
  return fetch(`${baseUrl}/status`, {
    headers: {
      "Content-Type": "application/json",
    },
  })
    .then((response) => response.json())
    .catch((error) => console.error("Error fetching data:", error));
}

/**
 * Initialize the device with the desired temperature and the sampling frequency
 *
 * @param {number} desiredTemp
 * @param {number} samplingFrequency
 * @returns {Promise<void>}
 */
function initializeDevice(desiredTemp, samplingFrequency) {
  return fetch(`${baseUrl}`, {
    method: "POST",
    body: JSON.stringify({
      samplingFrequency,
      desiredTemp,
    }),
    headers: {
      "Content-Type": "application/json",
    },
  }).then((response) => {
    if (!response.ok) {
      return response.json().then((data) => {
        throw { status: response.status, error: data.error };
      });
    }
    return response.json();
  });
}

/**
 * Setup the initial configuration for the main page
 */
function setupMainPage() {
  setupForm();
  setupCurrentStatus();
}

/**
 * Setup the form with the current internal temperature as the minimum value
 */
async function setupCurrentStatus() {
  const statusData = await getStatus();
  const { internalTemp } = statusData;
  const desiredTemperature = document.getElementById("desiredTemp");
  desiredTemperature.min = internalTemp;
}

/**
 * Setup the initial configuration for the prototype form page
 */
function setupForm() {
  localStorage.clear();
  const prototypeForm = document.getElementById("prototypeForm");

  prototypeForm?.addEventListener("submit", function (event) {
    event.preventDefault();
    // Comparar la temperatura ingresada con la internalTemp
    const desiredTemperature = +document.getElementById("desiredTemp").value;
    const samplingFrequency =
      +document.getElementById("samplingFrequency").value;

    initializeDevice(desiredTemperature, samplingFrequency)
      .then(() => {
        window.location.href = "on";
      })
      .catch((error) => {
        if (error.status === 400 && error.error) {
          alert(error.error);
        }
      });
  });
}

// Functions for /on

/**
 * Setup the interval to fetch data from the API
 */
function setupOnPage() {
  getStatusData();
  setDataFetchInterval();
}

/**
 * Clear the interval to fetch data from the API
 */
function clearDataFetchInterval() {
  clearInterval(interval);
}

/**
 * Set the interval to fetch data from the API every 3 seconds
 */
function setDataFetchInterval() {
  interval = setInterval(() => {
    getStatusData();
  }, 5000);
}

/**
 * Get the status from the API
 */
async function getStatusData() {
  const statusData = await getStatus();
  const { internalTemp, externalTemp, desiredTemp, status } = statusData;
  const internalTempElement = document.getElementById("internalTemp");
  const externalTempElement = document.getElementById("externalTemp");
  const desiredTempElement = document.getElementById("desiredTemp");
  const statusElement = document.getElementById("deviceStatus");

  if (!internalTempElement.innerHTML.includes(internalTemp)) {
    addDataIntoCell(internalTempElement, internalTemp);
  }
  if (!externalTempElement.innerHTML.includes(externalTemp)) {
    addDataIntoCell(externalTempElement, externalTemp);
  }
  if (!desiredTempElement.innerHTML.includes(desiredTemp)) {
    desiredTempElement.innerHTML = `${desiredTemp ?? "--"} °C`;
  }
  if (previousStatus !== status) {
    statusElement.innerHTML = status;
  }
  if (previousStatus !== "OFF") {
    if (!started && status === "COOLING_DOWN") {
      started = true;
      localStorage.setItem("startTime", new Date().toISOString());
    } else if (status === "END") {
      clearDataFetchInterval();
      localStorage.setItem("endTime", new Date().toISOString());
      location.href = "/report";
    }
  }
  previousStatus = status;
}

// Functions for /report

/**
 * Get the results from the API
 *
 * @returns {Promise<Array<{internalTemp: number, externalTemp: number, date: string}>>}
 */
function getResults() {
  return fetch(`${baseUrl}/results`, {
    headers: {
      "Content-Type": "application/json",
    },
  })
    .then((response) => response.json())
    .catch((error) => console.error("Error fetching data:", error));
}

/**
 * Fill the table with the results from the API
 */
async function fillTable() {
  const { data } = await getResults();
  const table = document.getElementById("dataTable");
  data.forEach((medicion) => {
    const row = table.insertRow();
    addDataIntoCell(row.insertCell(0), medicion.externalTemp);
    addDataIntoCell(row.insertCell(1), medicion.internalTemp);
    addDataIntoCell(
      row.insertCell(2),
      medicion.externalTemp - medicion.internalTemp,
    );
  });
}

/**
 * Setup the initial configuration for the report page
 */
function setupReportPage() {
  fillTable();
  const endTime = localStorage.getItem("endTime");
  const startTime = localStorage.getItem("startTime");
  const timeDifference = getTimeDifference(startTime, endTime);
  const timeElement = document.getElementById("time");
  timeElement.innerHTML = `${timeDifference} minutes`;
}

/**
 * Get the time difference between two dates
 *
 * @param {string} startTime 
 * @param {string} endTime 
 * @returns {number}
 */
function getTimeDifference(startTime, endTime) {
  const start = new Date(startTime);
  const end = new Date(endTime);
  const difference = end.getTime() - start.getTime();
  const minutes = Math.floor(difference / 1000 / 60);
  return minutes;
}

/**
 * Add data into a cell
 *
 * @param {HTMLElement} cell
 * @param {string|number} data
 */
function addDataIntoCell(cell, data) {
  cell.innerHTML = `${data || "--"} °C`;
}
