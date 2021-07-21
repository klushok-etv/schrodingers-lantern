// const dummydata = `[{"rssi":-83,"ssid":"SilenceoftheLANs","secure":3},{"rssi":-53,"ssid":"Deze is het Oma","secure":3},{"rssi":-86,"ssid":"TP-LINK_E170","secure":3},{"rssi":-86,"ssid":"Duckies","secure":3},{"rssi":-88,"ssid":"ZygorXS3","secure":3},{"rssi":-89,"ssid":"Nick","secure":3},{"rssi":-90,"ssid":"Mike's 1094","secure":4},{"rssi":-91,"ssid":"Mike's 1094-gast","secure":0},{"rssi":-94,"ssid":"TellUrWiFiLoveHer","secure":4}]`

window.onload = async function() {
    let response = JSON.parse(await ajaxGet(`/scan`));
    // response = JSON.parse(dummydata);
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