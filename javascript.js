var options = {};
var reversedDropdown = document.getElementById("chooser");

var load = function () {
    options = JSON.parse(decodeURIComponent(window.location.href.split("?")[1]));
    reversedDropdown.selectedIndex = options.reversed;
};

var encodeAndClose = function () {
    if (reversedDropdown.options[reversedDropdown.selectedIndex].value == "reversed") {
        options.reversed = 1;
    }
    window.location.href = "pebblejs://close#" + encodeURIComponent(JSON.stringify(options));
};

document.getElementById("close").onclick = encodeAndClose;
window.onload = load;
