var encodeAndClose = function () {
    var dropdown = Document.getElementById("chooser");
    var response;
    if (dropdown.options[e.selectedIndex].text == reversed) {
        response = {"reversed": 1};
    } else {
        response = {"reversed": 0}; 
    }
    window.location.replace("pebblejs://close#" + encodeURIComponent(JSON.stringify(response)));
};

document.getElementById("close").onclick = encodeAndClose;
