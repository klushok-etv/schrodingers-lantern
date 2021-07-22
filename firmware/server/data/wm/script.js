window.onload = async function() {
    let response = JSON.parse(await ajaxGet(`/scan`));
    const select = document.getElementById("ssid");

    if (response.length == 0) {
        select.options[select.options.length] = new Option("No networks found", "");
    } else {
        const sorted = response.sort((a, b) => (a.rssi < b.rssi) ? 1 : (a.rssi === b.rssi) ? ((a.ssid > b.ssid) ? 1 : -1) : -1)
        console.log(sorted)
        for (i in sorted) {
            select.options[select.options.length] = new Option(sorted[i].ssid, sorted[i].ssid);
        }
    }
}


function ajaxGet(url) {
    return new Promise(function(resolve, reject) {
        const xhttp = new XMLHttpRequest();
        xhttp.open("GET", url);
        xhttp.timeout = 2000; // time in milliseconds
        xhttp.onload = function() {
            if (this.status >= 200 && this.status < 300) {
                resolve(xhttp.response);
            } else {
                setErrorInfo()
                reject({
                    status: this.status,
                    statusText: xhttp.statusText
                });
            }
        };
        xhttp.onerror = function() {
            setErrorInfo()
            reject({
                status: this.status,
                statusText: xhttp.statusText
            });
        };
        xhttp.ontimeout = function() {
            setErrorInfo()
        }
        xhttp.send();
    });
}