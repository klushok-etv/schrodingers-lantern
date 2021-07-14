// // get rgb values on first visit
// async function getInitData() {
//     const solidColor = document.getElementById("solidColor");
//     let response = await ajaxGet(`/api/get?color`)
//     console.log(response, solidColor)
// }



function setRGB(red, green, blue) {

    ajaxGet(`/api/set?state=1&red=${red}&green=${green}&blue=${blue}`);
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

window.onload = async function() {
    const colorPicker = document.getElementById("solidColor");
    console.log(colorPicker)
    colorPicker.addEventListener("change", watchColorPicker, false);
    let response = JSON.parse(await ajaxGet(`/api/get?color`));
    console.log(response);
    colorPicker.value = rgbToHex(
        response.rgb[0],
        response.rgb[1],
        response.rgb[2]);
}

function watchColorPicker(event) {
    const RGB = hexToRgb(event.target.value)
        // console.log(RGB);
    const response = setRGB(RGB.r, RGB.b, RGB.g);
    console.log(response);
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