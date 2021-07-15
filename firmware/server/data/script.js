// // get rgb values on first visit
// async function getInitData() {
//     const solidColor = document.getElementById("solidColor");
//     let response = await ajaxGet(`/api/get?color`)
//     console.log(response, solidColor)
// }

let color = { r: 180, b: 31, g: 33 };
let intensity = 100;

window.onload = async function() {
    // todo implement heartbeat

    const brightnessEl = document.getElementById("brightness")

    // slider percentage update
    brightnessEl.oninput = function(event) {
        const span = document.getElementById("brightness_percentage");
        let newVal = event.target.value
        span.innerText = parseInt(newVal);
    }

    // send updates when slider is released
    brightnessEl.addEventListener('change', function(event) {
        // todo validation
        intensity = event.target.value;
        setRGB();
    }, false);

    // make request to obtain color and intensity
    const colorPicker = document.getElementById("solidColor");
    colorPicker.addEventListener("change", watchColorPicker, false);
    let response = JSON.parse(await ajaxGet(`/api/get?color`));
    intensity = response.intensity;
    brightnessEl.value = intensity;
    document.getElementById("brightness_percentage").innerHTML = intensity;


    color_arr = response.rgb; // this is a 
    color.r = parseInt(color_arr[0] / (intensity / 100));
    color.g = parseInt(color_arr[1] / (intensity / 100));
    color.b = parseInt(color_arr[2] / (intensity / 100));
    
    colorPicker.value = rgbToHex(
        color.r,
        color.g,
        color.b);
}


function setRGB() {
    // todo validate input
    console.log(color)
    const i = intensity / 100
    const red = parseInt(color.r * i);
    const green = parseInt(color.g * i);
    const blue = parseInt(color.b * i);

    ajaxGet(`/api/set?state=1&red=${red}&green=${green}&blue=${blue}&intensity=${intensity}`);
}

function setState(newState) {
    ajaxGet(`/api/toggle?state=${newState}`);
}

function ajaxGet(url) {
    return new Promise(function(resolve, reject) {
        const xhttp = new XMLHttpRequest();
        xhttp.open("GET", url);
        xhttp.onload = function() {
            if (this.status >= 200 && this.status < 300) {
                resolve(xhttp.response);
            } else {
                reject({
                    status: this.status,
                    statusText: xhttp.statusText
                });
            }
        };
        xhttp.onerror = function() {
            reject({
                status: this.status,
                statusText: xhttp.statusText
            });
        };
        xhttp.send();
    });
}

function watchColorPicker(event) {
    const RGB = hexToRgb(event.target.value)
    color = RGB;
    const response = setRGB();
}

function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}

function rgbToHex(r, g, b) {
    return "#" + componentToHex(r) + componentToHex(g) + componentToHex(b);
}

function componentToHex(c) {
    var hex = c.toString(16);
    return hex.length == 1 ? "0" + hex : hex;
}