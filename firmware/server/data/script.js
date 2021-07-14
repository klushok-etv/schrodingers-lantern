function setRGB(red, green, blue) {

    ajaxGet(`/api/set?state=1&red=${red}&green=${green}&blue=${blue}`);
}

function setState(newState) {
    ajaxGet(`/api/toggle?state=${newState}`);
}

function ajaxGet(url) {
    const xhttp = new XMLHttpRequest();
    xhttp.onload = function() {
        // document.getElementById("alert").innerHTML = this.responseText;
        return this.responseText;
    }
    xhttp.open("GET", url);
    xhttp.send();
}

window.onload = function() {
    let colorPicker = document.getElementById("solidColor");
    console.log(colorPicker)
    colorPicker.addEventListener("change", watchColorPicker, false);
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