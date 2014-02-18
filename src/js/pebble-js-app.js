Pebble.addEventListener("showConfiguration", function() {
	console.log("showing config");
	Pebble.openURL('http://akrs.github.io/moontiles/');
});

var handleACK = function (e) {
	console.log("Successfully delivered message with transactionId=" + e.data.transactionId);
};

var handleNACK = function (e) {
	console.log("Unable to deliver message with transactionId=" + e.data.transactionId + " Error is: " + e.error.message);
};

Pebble.addEventListener("webviewclosed", function(e) {
	console.log("configuration closed");
	// webview closed
	var options = JSON.parse(decodeURIComponent(e.response));
	console.log("Options = " + JSON.stringify(options));
	Pebble.sendAppMessage({"color": 0}, handleACK, handleNACK);
});